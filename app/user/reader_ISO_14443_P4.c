/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) Philips Semiconductors
//
//                       (C)PHILIPS Electronics N.V.2004
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    Philips reserves the right to make changes without notice at any time.
//   Philips makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. Philips must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */


/*! \file Iso14443_4_ReaderC.cpp
 *
 * $Author: mha $
 * $Revision: 1.34 $
 * $Date: Fri Jun 30 11:02:44 2006 $
 *
 * Comment:
 *  Example code for using the ISO14443-4 Reader protocol.
 *
 */

/* Include all necessary component headers for Mifare Reader operation and additional definitions.
   (BAL, RegisterControl, Iso14443Part3, OperationControl, RcIo, Mifare)                 */

/* For debugging purposes (printf) */
//#include <stdio.h>
//#include <string.h>
#include "ets_sys.h"
#include "osapi.h"

/* Common headers */
#include <phcsBflRefDefs.h>
#include <phcsBflHw1Reg.h>

/* Needed for any Operating Mode */
#include <phcsBflBal.h>
#include <phcsBflRegCtl.h>
#include <phcsBflIo.h>
#include <phcsBflOpCtl.h>
#include <phcsBflOpCtl_Hw1Ordinals.h>

/* Needed for this Operating Mode */
#include <phcsBflI3P3A.h>
#include <phcsBflI3P4.h>
#include <phcsBflI3P4AAct.h>
#include "reader_Mifare.h"
#include "reader_ISO_14443_P4.h"
//#include "includes.h"

/*************************************************************************/
//#include "timer_tick.h"
#define get_pre_timer_tick()
#define get_timer_tick_offset()         0

uint8_t UartDebug = 0;

extern void TestScPubuf(uint8_t *buf,int len,char *ch);

/*************************************************************************/
/*
 * This function sets the PN51x timer
 */

/*
   Callback definition for WTX timeout handling.
   The user interactions is done separated from the main routine which performs only data operations.
 */
phcsBfl_Status_t ICACHE_FLASH_ATTR
WTXCallback(phcsBflI3P4_CbSetWtxParam_t* wtx_param)
{
#define         PRESCALER_100_US    0x02A5
    phcsBfl_Status_t  status     = PH_ERR_BFL_SUCCESS;
    uint32_t   timeout           = 302;
    uint8_t   _buffer[4];

    /* Operation control parameters */
    phcsBflOpCtl_AttribParam_t      opp;

    /* change timeout value according to received TO value */
    timeout   = (timeout << wtx_param->fwi) / 100;

    /* The value of wtxm is only valid if this value is set to 1, otherwise reset to the default */
    if(wtx_param->set)
    {
        /* multiply the timeout value with the wtxm value */
        timeout = timeout * wtx_param->wtxm;
    }
    /* prepare buffer for Timer settings (Prescaler and Reload value) in any case */
    _buffer[0] = (uint8_t) ( PRESCALER_100_US & 0xFF );        /* Low value */
    _buffer[1] = (uint8_t) ( (PRESCALER_100_US >> 8) & 0x0F ); /* High value (max 0x0F) */
    _buffer[2] = (uint8_t) ( timeout & 0xFF );          /* Low value */
    _buffer[3] = (uint8_t) ( (timeout >> 8) & 0xFF );   /* High value (max 0xFF) */

    /* do adjustments of timer with preset values */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR;
    opp.param_a          = 1;                   /* Timer Auto mode ON */
    opp.param_b          = 0;                   /* Timer auto reload OFF */
    opp.buffer           = _buffer;              /* Buffer used for Prescaler and Reload value */
    opp.self             = ((phcsBflOpCtl_t*)(((phcsBflI3P4_t*)(wtx_param->object))->mp_UserRef));
    status = ((phcsBflOpCtl_t*)(((phcsBflI3P4_t*)(wtx_param->object))->mp_UserRef))->SetAttrib(&opp);
    /* Check status and display error if something went wrong */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        if (UartDebug)
        os_printf("[E] Error setting timer! Status = %04X \r\n", status);
        return status;
    }
    if (UartDebug)
        os_printf("[E] setting ok timeout  = %ld\r\n",timeout);

    return status;
#undef PRESCALER_100_US
}


/*
typedef uint32_t    DWORD;
typedef int8_t      BYTE;
typedef struct
{
    DWORD u32PanelType;
    DWORD bpp;
    DWORD u32PanelW;
    DWORD u32PanelH;

    DWORD u32ImgBufAddr;
    DWORD u32ColLUTAddr;
    DWORD u32RowLUTAddr;
    DWORD u32SegLUTAddr;

    DWORD u32PeriodDataAddr;
    DWORD u32MaxTranSize;
    DWORD u32Voltage;
    BYTE FWVersion[4];
} PanelInfo;
*/
/* Declaration of RCL Component Structures (auto), C variant: */
static phcsBflBal_t            bal;
static phcsBflRegCtl_t         rc_reg_ctl;
static phcsBflIo_t             rcio;
static phcsBflOpCtl_t          rc_op_ctl;
static phcsBflI3P3A_t    iso14443_3;
static phcsBflI3P4_t     iso14443_4;
static phcsBflI3P4AAct_t iso14443_4A;
static phcsBflRegCtl_SerHw1Params_t        rc_reg_ctl_params;
static phcsBflIo_Hw1Params_t               rc_io_params;
static phcsBflOpCtl_Hw1Params_t            rc_op_ctl_params;
static phcsBflI3P3A_Hw1Params_t           iso_14443_3_params;
/*
 * Declaration of used parameter structures
 */
/* Register control parameters */
static phcsBflRegCtl_SetRegParam_t         srp;
static phcsBflRegCtl_GetRegParam_t         grp;
static phcsBflRegCtl_ModRegParam_t         mrp;

/* Operation control parameters */
static phcsBflOpCtl_AttribParam_t          opp;

/* ISO14443-3A parameters       */
static phcsBflI3P3A_ReqAParam_t          req_p;
static phcsBflI3P3A_AnticollSelectParam_t    sel_p;
/* ISO14443-4A activation parameters */
static phcsBflI3P4_ProtParam_t           pro_p;
static phcsBflI3P4AAct_RatsParam_t           rat_p;
static phcsBflI3P4AAct_PpsParam_t            pps_p;
/* Transparent protocol operations */
static phcsBflI3P4_SetCbParam_t              scb_p;
static phcsBflI3P4_ExchangeParam_t           exc_p;
static phcsBflI3P4_CommParam_t      com_p;
static phcsBflI3P4_PresCheckParam_t      prs_p;
static phcsBflI3P4_DeselectParam_t           dsl_p;
static phcsBflI3P4_SetProtParam_t        spr_p;
static phcsBflI3P4_ResetProtParam_t      rst_p;
static uint8_t buffer[EXAMPLE_TRX_BUFFER_SIZE];
/* Buffer serial number */
static uint8_t serial[12];
static uint8_t  ats[64];




static void ICACHE_FLASH_ATTR
show_data(uint8_t *buf, uint_fast8_t len)
{
    uint_fast8_t i;

 #if defined (DEBUGMODE)
    if (UartDebug == 0) return;
    os_printf("recv:");
    for (i=0; i<len; i++)
    {
        os_printf("%02X ", buf[i]);
    }
    os_printf("\r\n");
#endif
}

int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader_init(int argc, char *argv[])
{
    /* Status variable */
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Counter variable */
    uint_fast32_t loop_cnt;

    get_pre_timer_tick();

    /* Initialisation of self parameters */
    srp.self = &rc_reg_ctl;
    grp.self = &rc_reg_ctl;
    mrp.self = &rc_reg_ctl;
    opp.self = &rc_op_ctl;
    req_p.self = &iso14443_3;
    sel_p.self = &iso14443_3;
    rat_p.self = &iso14443_4A;
    pps_p.self = &iso14443_4A;
    scb_p.self = &iso14443_4;
    exc_p.self = &iso14443_4;
    prs_p.self = &iso14443_4;
    dsl_p.self = &iso14443_4;
    spr_p.self = &iso14443_4;
    rst_p.self = &iso14443_4;
    /*
     * Build up stack for serial communication.
     * Start with the lowest layer, so that the upper ones may refer to this already.
     */
    phcsBflRegCtl_SpiHw1Init(&rc_reg_ctl, &rc_reg_ctl_params, &bal);
    /* Initialise RcIo component */
    phcsBflIo_Hw1Init(&rcio, &rc_io_params, &rc_reg_ctl, PHCS_BFLIO_INITIATOR);
    /* Initialise Operation Control component */
    phcsBflOpCtl_Hw1Init(&rc_op_ctl, &rc_op_ctl_params, &rc_reg_ctl);
    /* Initialise ISO14443-3 component */
    phcsBflI3P3A_Hw1Initialise(&iso14443_3, &iso_14443_3_params, &rc_reg_ctl, PHCS_BFLI3P3A_INITIATOR);
    /* Initialise ISO14443-4A Activation */
    phcsBflI3P4AAct_Init(&iso14443_4A, &pro_p, buffer, EXAMPLE_TRX_BUFFER_SIZE, &rcio);
    /* Initialise ISO14443-4 */
    phcsBflI3P4_Init(&iso14443_4, &com_p, &rcio);

    /*
     * Configure PN51x to act as a Mifare Reader. All register which are set in other modes
     * are configured to the right values.
     * Remark: Operation control function do not perform a softreset and does not know anything
     *         about the antanne configuration, so these things have to be handled here!
     */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_MFRD_A;
    opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
    opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
    status = rc_op_ctl.SetAttrib(&opp);
    /* Set Initiator bit of PN51x to act as a reader using register control functions. */
    srp.address = PHCS_BFL_JREG_CONTROL;
    srp.reg_data = PHCS_BFL_JBIT_INITIATOR;
    status = rc_reg_ctl.SetRegister(&srp);
    /*
     * Set the timer to auto mode, 5ms using operation control commands before HF is
     * switched on to guarantee Iso14443-3 compliance of the polling procedure
     */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;         /* Set operation control group parameter to Timer */
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_AUTO;     /* Set function for Timer automatic mode          */
    opp.param_a          = PHCS_BFLOPCTL_VAL_ON;              /* Use parameter a to switch it on                */
    status = rc_op_ctl.SetAttrib(&opp);
    /*
     * Set prescaler steps to 100us
     */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_PRESCALER;
    /* Calculation of the values: 100us = 6.78MHz/678 = 6.78MHz/0x2A6 */
    opp.param_a          = 0xA5;                        /* Low value  */
    opp.param_b          = 0x02;                        /* High value */
    status = rc_op_ctl.SetAttrib(&opp);

    /* set reload value
       (group parameter is set from above and not modified during operation) */
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_RELOAD;   /* set to 50 for 5 ms    */
    opp.param_a          = 50;                      /* Low value             */
    opp.param_b          = 0;                       /* High value (max 0xFF) */
    status = rc_op_ctl.SetAttrib(&opp);

    /* Activate the field (automatically if there is no external field detected) */
    mrp.address = PHCS_BFL_JREG_TXAUTO;
    mrp.mask    = PHCS_BFL_JBIT_INITIALRFON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
    mrp.set     = 1;
    status = rc_reg_ctl.ModifyRegister(&mrp);
    /*
     * After switching on the drivers wait until the timer interrupt occures, so that
     * the field is on and the 5ms delay have been passed.
     */
    loop_cnt = 50000;
    grp.address = PHCS_BFL_JREG_COMMIRQ;
    do
    {
        //IWDG_ReloadCounter();
        status = rc_reg_ctl.GetRegister(&grp);
    }
    while(!(grp.reg_data & PHCS_BFL_JBIT_TIMERI) && (status == PH_ERR_BFL_SUCCESS) && (--loop_cnt));

    /* Check status and diplay error if something went wrong */
    if ((status != PH_ERR_BFL_SUCCESS) || (loop_cnt == 0))
    {
        if (UartDebug)
            os_printf("*** ERROR! GetReg%02X,Dat=%02X,Sta=%X,loop=%u\r\n",
               grp.address, grp.reg_data, status, loop_cnt);
    }
    else
    {
        if (UartDebug)
            os_printf("GetRegister OK %ums \r\n", get_timer_tick_offset());
    }

    /* Clear the status flag afterwards */
    srp.address  = PHCS_BFL_JREG_COMMIRQ;
    srp.reg_data = PHCS_BFL_JBIT_TIMERI;
    status = rc_reg_ctl.SetRegister(&srp);
    /*
     * Reset timer 1 ms using operation control commands (AutoMode and Prescaler are the same)
     * set reload value
     */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;         /* set operation control group parameter to Timer */
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_RELOAD;   /* set to 10 for 1 ms */
    opp.param_a          = 10;                      /* Low value */
    opp.param_b          = 0;                       /* High value (max 0xFF) */
    status = rc_op_ctl.SetAttrib(&opp);
    /* Activate receiver for communication
       The RcvOff bit and the PowerDown bit are cleared, the command is not changed. */
    srp.address  = PHCS_BFL_JREG_COMMAND;
    srp.reg_data = PHCS_BFL_JCMD_NOCMDCHANGE;
    status = rc_reg_ctl.SetRegister(&srp);
    return 0;
}

int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader_request(int argc, char *argv[])
{
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;

    if (argc & COIN_OPT_INIT_PRE_FLOW)
    {
        ActivateIso14443_4_Reader_init(COIN_OPT_INIT_PRE_FLOW, NULL);
        //SysTimeDly(200);
    }
    /* Set timeout for REQA, ANTICOLL, SELECT to 200us */
    status = SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 50000, 0);

    /*
     * Do activation according to ISO14443A Part3
     * Prepare the Request command
     */
    buffer[0] = 0;
    buffer[1] = 0;
    req_p.req_code = PHCS_BFLI3P3A_REQIDL;     /* Set Request command code (or Wakeup: ISO14443_3_REQALL) */
   // req_p.req_code = PHCS_BFLI3P3A_REQALL;     /* Set Request command code (or Wakeup: ISO14443_3_REQALL) */

    req_p.atq      = buffer;                /* Let Request use the defined return buffer */
    status = iso14443_3.RequestA(&req_p);   /* Start Request command */
    /*
     * In normal operation this command returns one of the following three return codes:
     * - PH_ERR_BFL_SUCCESS             At least one card has responded
     * - PH_ERR_BFL_COLLISION_ERROR     At least two cards have responded
     * - PH_ERR_BFL_IO_TIMEOUT          No response at all
     */
    if(status == PH_ERR_BFL_SUCCESS || (status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
    {
        /* There was at least one response */
        if((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
        {
            if (UartDebug)
                os_printf("[W] Multiple Cards detected!! Collision occured during Request!\r\n");
        }
        else
        {
        }
        if (UartDebug)
            os_printf("[I] ATQA = %02X %02X \r\n", req_p.atq[0],req_p.atq[1]);
    }
    else
    {
        /* No response at all */
        if (UartDebug)
            os_printf("*** ERROR! RequestA: RequestCode=%02X, ATQA=%02X %02X, Status = %04X \r\n",
                   req_p.req_code, req_p.atq[0],req_p.atq[1], status);
    }
    return status;
}
int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader_select(int argc, char *argv[],uint8_t *uid,uint8_t *uid_len)
{
    extern void show_id_sak(uint8_t *id, uint_fast8_t len, uint_fast8_t sak);
    /* Status variable */
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    if (argc&COIN_OPT_INIT_PRE_FLOW)
    {
        ActivateIso14443_4_Reader_request(COIN_OPT_INIT_PRE_FLOW, NULL);
    }
    /* Prepare data for combined anticollision/select command to get one complete ID */
    os_memset(serial, 0, sizeof(serial));
    sel_p.uid          = serial;                 /* Put serial buffer for ID */
    sel_p.uid_length   = 0;                      /* No bits are known from ID, so set to 0 */
    /* Start Anticollision/Select command */
    status = iso14443_3.AnticollSelect(&sel_p);
    /* Check return code: If the command was OK, is always returns PH_ERR_BFL_SUCCESS.
     * If nothing was received, parameter uid_length is 0. */
    if(status == PH_ERR_BFL_SUCCESS)
    {
        show_id_sak(sel_p.uid, sel_p.uid_length>>3, sel_p.sak);
        // UID
        *uid_len = sel_p.uid_length>>3;
        os_memcpy((char *)uid,(char *)sel_p.uid,sel_p.uid_length>>3);
        if ( (sel_p.flags & PHCS_BFLI3P3A_COLL_DETECTED) == PHCS_BFLI3P3A_COLL_DETECTED )
        {
            if (UartDebug)
                os_printf("[I] More than one card detected, collision has been resolved. \r\n");
        }
    }
    else
    {
        if (UartDebug)
            os_printf("*** ERROR! Anticollision/Select: Status = %04X \r\n", status);
    }
    return status;
}
/*
     ATS 对选择请求的应答
     RATS 对选择应答请求
*/

int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader_rats(int argc, char *argv[])
{
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    uint_fast8_t i;
    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 50000, 0);
    /* ---------------------------------------------------------------------------- */
    /* Example how to use the Iso14443 Part4 (T=CL) specific command set of the BFL */
    /* Prepare for RATS command */
    rat_p.cid         = 0;
    rat_p.fsi         = 5;  /* Take care that the real buffer size of the PCD is big enough */
    rat_p.ats         = ats;
    /* Issue the RATS command */
    status = iso14443_4A.Rats(&rat_p);
    if (UartDebug)
    {
       if (status == PH_ERR_BFL_SUCCESS)
       {
           /* Display all received parameters if anything was ok */
           os_printf("RATS: \t ATS Length: %04X, ", rat_p.ats_len);
           /* Display (negotiated) FSI
            * FSI is determined by calculating fsi=min(fsci, fsdi) */
           os_printf("negotiated FSI=%02X, ", rat_p.fsi);
           /* Display TA(1) */
           if(rat_p.ta1)
               os_printf("TA(1)=%02X, ", *rat_p.ta1);
           else
               os_printf("no TA(1), ");
           /* display TB(1) */
           if(rat_p.tb1)
               os_printf("TB(1)=%02X, ", *rat_p.tb1);
           else
               os_printf("no TB(1), ");
           /* display TC(1) if applicable */
           if(rat_p.tc1)
               os_printf("TC(1)=%02X, ", *rat_p.tc1);
           else
               os_printf("no TC(1), ");
           if(rat_p.app_inf_len)
           {
               os_printf("\r\n    Historical characters (%d bytes):  ", rat_p.app_inf_len);
               for(i=0; i<(rat_p.app_inf_len); i++)
                   os_printf("%02X ", rat_p.app_inf[i]);
           }
           else
           {
               os_printf("\r\nNo Historical characters. ");
           }
           os_printf("\r\n");
       }
       else
       {
           os_printf("*** ERROR! ISO14443 Part4 -> RATS: Status = %04X \r\n", status);
       }

      /* Check speed capability of the remote card (stored in TA(1) byte) */
      /* Only relevant if the last command was successful and TA(1) is supported */
      if ( ( !status ) && (rat_p.ta1 != 0) )
      {
          if ((*rat_p.ta1 & 0x80) == 0)
          {
              os_printf("Asymmetric datarates supported!\r\nPossible data rates (besides 106kBit/s) for PCD -> PICC are:\r\n\t");
              if(*rat_p.ta1 & 0x01)
                  os_printf("212kbps, ");
              if(*rat_p.ta1 & 0x02)
                  os_printf("424kbps, ");
              if(*rat_p.ta1 & 0x04)
                  os_printf("848kbps");
              os_printf("\r\n  Possible data rates (besides 106kBit/s) for PICC -> PCD are:\r\n\t");
              if(*rat_p.ta1 & 0x10)
                  os_printf("212kbps, ");
              if(*rat_p.ta1 & 0x20)
                  os_printf("424kbps, ");
              if(*rat_p.ta1 & 0x40)
                  os_printf("848kbps");
          }
          else
          {
              os_printf("Only symmetrical datarates supported!\r\nPossible data rates (besides 106kBit/s) are: \r\n\t");
              if(*rat_p.ta1 & 0x01 && *rat_p.ta1 & 0x10)
                  os_printf("212kbps, ");
              if(*rat_p.ta1 & 0x02 && *rat_p.ta1 & 0x20)
                  os_printf("424kbps, ");
              if(*rat_p.ta1 & 0x04 && *rat_p.ta1 & 0x40)
                  os_printf("848kbps");
          }
          os_printf("\r\n");
      }
    }
    return status;
}


int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader_pre_exchange(int argc, char *argv[])
{
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    if (rat_p.ta1)
    {
        /*
         * Change data rate if TA(1) was valid in ATS
         * PPS can only be done right after the RATS/ATS chain
         */
        pps_p.cid = 0x00;               /* no card identifier used */
        pps_p.dri = PHCS_BFLOPCTL_VAL_RF106K;     /* data rate PCD -> PICC   */
        pps_p.dsi = PHCS_BFLOPCTL_VAL_RF106K;     /* data rate PICC -> PCD   */


        status = iso14443_4A.Pps(&pps_p);
        if(status == PH_ERR_BFL_SUCCESS)
        {
           // os_printf("[I] PPS handled without errors, reconfiguration is possible.\r\n");
            /* call Operation control function with new data rate */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_MFRD_A;


            opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
            opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
            status = rc_op_ctl.SetAttrib(&opp);
           // /*
            if(status != PH_ERR_BFL_SUCCESS)
            {
                if (UartDebug)
                   os_printf("[E] ERROR performing Mifare Reader configuration for higher data rates! Status = %04x \n", status);
            }
           // */
        }
        else
        {
            if (UartDebug)
                os_printf("[E] ERROR! ISO14443 Part4 -> PPS: Status = %04x\r\n", status);
        }

        /* Hand over parameters to transparent protocol */
    }
    else
    {
        if (UartDebug)
            os_printf("[I] No TA(1) has been specified -> no speed adjustment possible.\r\n");
    }
    /* Initalize Callbacks for user interaction during protocol operation */
    iso14443_4.mp_UserRef = &rc_op_ctl;         /* Init user pointer to reach operation control component from callback */
    scb_p.set_reader_params_cb = NULL;          /* no interactive change of bitrate parameters, done in the main function */

    scb_p.set_wtx_cb           = WTXCallback;   /* Callback function to process WTX requests from card side */
    iso14443_4.SetCallbacks(&scb_p);

    /* Hand over parameters generated in activatin part to transparent protocol part of ISO 14443-4 */
    spr_p.max_retry        = 1;
    spr_p.p_protocol_param = &pro_p;

    iso14443_4.SetProtocol(&spr_p);

    return status;
}

int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader_deselect(int argc, char *argv[])
{
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Check if card is still in field */
    status = iso14443_4.PresenceCheck(&prs_p);
    if(status == PH_ERR_BFL_SUCCESS)
    {
        if (UartDebug)
        os_printf("Presence check OK, Card available. \r\n");
    }
    else
    {
        if (UartDebug)
        os_printf("Presence check FAIL, No card available any more! \r\n");
    }

    /* Deselect device at the end of communication */
    status = iso14443_4.Deselect(&dsl_p);
    if(status == PH_ERR_BFL_SUCCESS)
    {
        if (UartDebug)
        os_printf("Deselect OK. \r\n");
    }
    else
    {
        if (UartDebug)
        os_printf("Deselect FAIL, No card available any more! Try error handling. \r\n");
    }
    /* To start a communication with a new device all internal status variables have to be reset.  */
    iso14443_4.ResetProt(&rst_p);     /* This function has no return value. All variables are reset! */
    return status;
}
int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader_Apdu(uint8_t *apdu,int len)
{
     phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
     uint8_t rcvBuf[EXAMPLE_TRX_BUFFER_SIZE];
     uint8_t send_buf[64];
     os_memcpy(send_buf, apdu, len);
     /*
      * This is a typical data exchange pair
      */

     SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 500000, 0);

     TestScPubuf(apdu,len,"CPU Apdu:");

     exc_p.nad_receive = 0;
     exc_p.nad_send    = 0;

     exc_p.snd_buf     = send_buf;
     exc_p.snd_buf_len = len;
     os_memset(rcvBuf, 0, sizeof(rcvBuf));
     exc_p.rec_buf     = rcvBuf;
     exc_p.rec_buf_len = 64;
     status = iso14443_4.Exchange(&exc_p);
     if (status == PH_ERR_BFL_SUCCESS)
     {
         show_data(exc_p.rec_buf, exc_p.rec_buf_len);
         TestScPubuf(NULL,0,"  ");
     }
     else
     {
         if (UartDebug)
         os_printf("[E] ERROR! ISO14443 Part4 -> Exchange: MifareRead: Status = %04x \r\n", status);
     }
     return status;
}


/*
 * This example shows how to act as ISO14443-4 reader
 */
int ICACHE_FLASH_ATTR
ActivateIso14443_4_Reader(uint8_t *send_buf, uint_fast8_t len)
{
    int ret = 1;
    uint8_t uid[16];
    uint8_t uidlen = 0;
    /* Declaration of internal Component Structures: */
//    phcsBflBal_Hw1SerLinInitParams_t    bal_params;
    /* Declaration of variables, buffer, ... */
    /* Status variable */
    /* Buffer for ATS */
    uint_fast32_t delay;
    ActivateIso14443_4_Reader_init(COIN_OPT_INIT_PRE_FLOW, NULL);
    delay = (500*1000);
    while (delay--);
    ret = ActivateIso14443_4_Reader_request(0, NULL);
    if (ret)
    {
        TestScPubuf(NULL,0,"NO CARD");
        return ret;
    }
    ActivateIso14443_4_Reader_select(0, NULL,uid,&uidlen);
    ActivateIso14443_4_Reader_rats(0, NULL);
    ActivateIso14443_4_Reader_pre_exchange(0, NULL);
    ActivateIso14443_4_Reader_Apdu(send_buf,len);
    ActivateIso14443_4_Reader_deselect(0, NULL);
    return 0;
}
/**************************************************************************/
static uint8_t  reset_tmot = 0;
void ICACHE_FLASH_ATTR
rfid_reset_enable(uint_fast16_t _ms)
{
    reset_tmot = (_ms+99)/100;
}
void ICACHE_FLASH_ATTR
rfid_reset_timer(void)
{
    if (reset_tmot)
    {
        --reset_tmot;
        if (0 == reset_tmot)
        {
            TestScPubuf(NULL,0,"PN512 RESET");
            PN512_reset();
        }
    }
}
/**************************************************************************/
extern int ICACHE_FLASH_ATTR
Cpu_Reader_Request(uint8_t *uid,uint8_t *uidlen)
{
    int ret = 1;

    PN512_reset();

    ret = ActivateIso14443_4_Reader_request(COIN_OPT_INIT_PRE_FLOW, NULL);
    if (ret)
    {
        TestScPubuf(NULL,0,"NO CARD");
        return ret;
    }
    ret  = ActivateIso14443_4_Reader_select(0, NULL,uid,uidlen);
    if (ret)
    {
        TestScPubuf(NULL,0,"SELECT ERR");
        return ret;
    }
    ret = ActivateIso14443_4_Reader_rats(0, NULL);
    if (ret)
    {
        TestScPubuf(NULL,0,"RATS ERR");
        return ret;
    }
    ret = ActivateIso14443_4_Reader_pre_exchange(0, NULL);
    if (ret)
    {
        TestScPubuf(NULL,0,"PPS ERR");
        return ret;
    }
    return 0;
}
extern int ICACHE_FLASH_ATTR
Cpu_Reader_Apdu(uint8_t *send_buf, int len)
{
    int ret;

    ret = ActivateIso14443_4_Reader_Apdu(send_buf,len);


#if 0
    if (ret)
    {
        uint8_t  buf[128];
        uint32_t slen;

        slen = Cpu_Reader_Respone(buf);
        TestScPubuf(buf,slen,"CPU ERR:");
        return ret;
    }
    return 0;
 #else
    return ret;

 #endif
}
extern int ICACHE_FLASH_ATTR
Cpu_Reader_Respone(uint8_t *rec)
{
    os_memcpy((char *)rec,(char *)exc_p.rec_buf,exc_p.rec_buf_len);

    return  exc_p.rec_buf_len;
}
extern int ICACHE_FLASH_ATTR
Cpu_Reader_Close(void)
{
    ActivateIso14443_4_Reader_deselect(0, NULL);

    PN512_reset();
    return 0;
}





