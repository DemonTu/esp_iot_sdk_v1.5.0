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


/*! \file MifareReaderC.c
 *
 * Projekt: Object Oriented Reader Library Framework BAL component.
 *
 *  Source: MifareReaderC.c
 * $Author: mha $
 * $Revision: 1.39 $
 * $Date: Fri Jun 30 11:02:45 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

/* For debugging purposes (os_printf) */
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
#include <phcsBflMfRd.h>
#include "reader_Mifare.h"
#include "reader_ISO_14443_P4.h"
//#include "reader_Mifare.h"


/*************************************************************************/
//#include "timer_tick.h"
//#include "includes.h"
/*
 * This function sets the PN51x timer
 */
phcsBfl_Status_t ICACHE_FLASH_ATTR
SetTimeOut(    phcsBflRegCtl_t *rc_reg_ctl,
                                phcsBflOpCtl_t *rc_op_ctl,
                                uint32_t aMicroSeconds,
                                uint8_t aFlags);
#define get_pre_timer_tick()
#define get_timer_tick_offset()         0
#define PDEBUG(format, ...)         os_printf (format, ##__VA_ARGS__)
/*************************************************************************/
/* Declaration of RCL Component Structures (auto), C variant: */
static phcsBflBal_t            bal;
static phcsBflRegCtl_t         rc_reg_ctl;
static phcsBflIo_t             rcio;
static phcsBflOpCtl_t          rc_op_ctl;
static phcsBflI3P3A_t          iso14443_3;
static phcsBflMfRd_t           mfrd;
/* Declaration of internal Component Structures: */
//static phcsBflBal_Hw1SerLinInitParams_t     bal_params;
static phcsBflRegCtl_SerHw1Params_t         rc_reg_ctl_params;
static phcsBflIo_Hw1Params_t                rc_io_params;
static phcsBflOpCtl_Hw1Params_t             rc_op_ctl_params;
static phcsBflI3P3A_Hw1Params_t             iso_14443_3_params;
static phcsBflMfRd_InternalParam_t          mifare_params;
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
static phcsBflI3P3A_ReqAParam_t            req_p;
static phcsBflI3P3A_AnticollSelectParam_t  sel_p;
static phcsBflI3P3A_HaltAParam_t           hlt_p;

/* Mifare parameters            */
static phcsBflMfRd_CommandParam_t          mfr_p;

static uint8_t buffer[EXAMPLE_TRX_BUFFER_SIZE];
static uint8_t serial[12];


void ICACHE_FLASH_ATTR
show_id_sak(uint8_t *id, uint_fast8_t len, uint_fast8_t sak)
{
#if defined (DEBUGMODE)
    uint_fast8_t i;
/* Display UID and SAK if everything was OK */
    os_printf("[I] UID: ");
    for(i=0; i<len; i++)
    {
        os_printf("%02X", id[i]);
    }
    os_printf("\tSAK: %02X \r\n", sak);
#endif
}

void ICACHE_FLASH_ATTR
PN512_reset(void)
{
    phcsBflBal_t            bal;
    phcsBflRegCtl_t         rc_reg_ctl;
    phcsBflRegCtl_SetRegParam_t         srp;
    srp.self = &rc_reg_ctl;
    phcsBflRegCtl_SpiHw1Init(&rc_reg_ctl, NULL, &bal);
    srp.address = PHCS_BFL_JREG_COMMAND;
    srp.reg_data= PHCS_BFL_JCMD_SOFTRESET;
    rc_reg_ctl.SetRegister(&srp);
}

void ICACHE_FLASH_ATTR
PN512_idle(void)
{
    phcsBflBal_t            bal;
    phcsBflRegCtl_t         rc_reg_ctl;
    phcsBflRegCtl_SetRegParam_t         srp;
    srp.self = &rc_reg_ctl;
    phcsBflRegCtl_SpiHw1Init(&rc_reg_ctl, NULL, &bal);
    srp.address = PHCS_BFL_JREG_COMMAND;
    srp.reg_data= PHCS_BFL_JCMD_IDLE;
    rc_reg_ctl.SetRegister(&srp);
}


void ICACHE_FLASH_ATTR
MifareReader_init(void)
{
    /* Initialisation of self parameters */
    srp.self = &rc_reg_ctl;
    grp.self = &rc_reg_ctl;
    mrp.self = &rc_reg_ctl;
    opp.self = &rc_op_ctl;
    req_p.self = &iso14443_3;
    sel_p.self = &iso14443_3;
    hlt_p.self = &iso14443_3;
    mfr_p.self = &mfrd;

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

    /* Initialise Mifare component */
    phcsBflMfRd_Init(&mfrd, &mifare_params, &rcio, buffer+20);

    /* Configure PN51x to act as a Mifare Reader. All register which are set in other modes
     * are configured to the right values.
     * Remark: The operation control function does not perform a soft-reset and does not know anything
     *         about the antenna configuration, so these things have to be handled separately!
     */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_MFRD_A;
    opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
    opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
    rc_op_ctl.SetAttrib(&opp);
    /* Set Initiator bit of PN51x to act as a reader using register control functions. */
    srp.address = PHCS_BFL_JREG_CONTROL;
    srp.reg_data = PHCS_BFL_JBIT_INITIATOR;
    rc_reg_ctl.SetRegister(&srp);
}
static void ICACHE_FLASH_ATTR
fill_auth_buf_by_key_0xFF(uint8_t *buf,  uint8_t *uid)
{
    /* Prepare data for Mifare Authentication */
    /* Copy secret key to buffer */
    os_memset(buf, 0xFF, 6);
    /* Copy UID to Tx buffer */
    os_memcpy(&buf[6], uid, 4);
}

static void ICACHE_FLASH_ATTR
fill_auth_buf_by_key(uint8_t *buf,  uint8_t *uid, uint8_t *key)
{
    /* Prepare data for Mifare Authentication */
    /* Copy secret key to buffer */
    os_memcpy(buf, key, 6);
    /* Copy UID to Tx buffer */
    os_memcpy(&buf[6], uid, 4);
}
static ICACHE_FLASH_ATTR
uint_fast8_t ISO14443_halt(void)
{
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Set timeouts */
    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 1000, 0);
    status = iso14443_3.HaltA(&hlt_p);
    /* Check return code: If timeout occures, PH_ERR_BFL_SUCCESS is returned */
    if(status == PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("Halt probably successful, timeout occured\r\n");
    }
    else
    {
        PDEBUG("*** ERROR! HaltA: Status = %04X \r\n", status);
    }
    return status;
}

static uint_fast8_t ICACHE_FLASH_ATTR
ISO14443_auth(uint_fast8_t block, uint8_t *uid, uint8_t *keyA, uint8_t *keyB)
{
    /* Buffer for data transfer (send/receive) */
    uint8_t tmp_buffer[EXAMPLE_TRX_BUFFER_SIZE];
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Prepare data for Mifare Authentication */
    if (NULL != keyA)
    {
        fill_auth_buf_by_key(tmp_buffer, uid, keyA);
        mfr_p.cmd           = PHCS_BFLMFRD_AUTHENT_A;     // Set Authenticate with KeyA
    }
    else if (NULL != keyB)
    {
        fill_auth_buf_by_key(tmp_buffer, uid, keyB);
        mfr_p.cmd           = PHCS_BFLMFRD_AUTHENT_B;     // Set Authenticate with KeyB
    }
    else
    {
        fill_auth_buf_by_key_0xFF(tmp_buffer, uid);
        mfr_p.cmd           = PHCS_BFLMFRD_AUTHENT_A;
    }
    /* Initialize all the variables of authentication structure */
    mfr_p.addr          = block;                 // Block 7
    mfr_p.buffer        = tmp_buffer;               // Set the mifare buffer to the one initialised
    mfr_p.buffer_length = 10;                   // 6 bytes of key and 4 bytes SNR in buffer
    /* Set timeout for Authentication */
    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 10000, 0);
    /* Start the Mifare authentication */
    status = mfrd.Transaction(&mfr_p);
    /* Check return code and display error number if an error occured */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("*** ERROR! Mifare Authentication: Command=%01X, Block=%02X, Status = %04X\r\n",
               mfr_p.cmd, mfr_p.addr, status);
    }
    return status;
}

static uint_fast8_t ICACHE_FLASH_ATTR
ISO14443_read(uint_fast8_t block, uint8_t *rd_buf)
{
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Buffer serial number                     */
//    uint8_t serial[12];
    /* Counter variable */
    uint_fast8_t i;
    /* Response shall now be sent within 5ms */
    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 5000, 0);
    /* Prepare data for Mifare Read */
    mfr_p.cmd           = PHCS_BFLMFRD_READ;          // set Read command
    mfr_p.addr          = block;                 // Block 5, must be authenticated before
    mfr_p.buffer        = rd_buf;               // set return buffer to buffer
    mfr_p.buffer_length = 0;                    // nothing is handed over in the buffer
    /* Start Mifare Read */
    status = mfrd.Transaction(&mfr_p);
    /* Check return code and display error number if an error occured, else display data */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("*** ERROR! MifareRead: Block=%02X, Status = %04X \r\n", mfr_p.addr, status);
    }
    else
    {
        PDEBUG("Mifare Read OK: Blk=%02X, Dat: ", mfr_p.addr);
        for(i=0; i<mfr_p.buffer_length; i++)
        {
            PDEBUG("%02X", mfr_p.buffer[i]);
        }
        PDEBUG("\r\n");
    }
    return status;
}

static uint_fast8_t ICACHE_FLASH_ATTR
ISO14443_write(uint_fast8_t block, uint8_t *wr_buf)
{
//    phcsBflMfRd_CommandParam_t          mfr_p;
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Response shall now be sent within 5ms */
    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 5000, 0);
    /* Prepare data for Mifare Write */
    mfr_p.cmd           = PHCS_BFLMFRD_WRITE ;          // set Write command
    mfr_p.addr          = block;
    mfr_p.buffer        = wr_buf;
    mfr_p.buffer_length = PHCS_BFLMFRD_STD_BLOCK_SIZE;  // nothing is handed over in the buffer
    /* Start Mifare write */
    status = mfrd.Transaction(&mfr_p);
    /* Check return code and display error number if an error occured, else display data */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("*** ERROR! MifareWrite: Block=%02X, Status = %04x \r\n", mfr_p.addr, status);
    }
    else
    {
        uint_fast8_t i = 0;
        PDEBUG("Mifare write OK: Blk=%02X Dat:", mfr_p.addr);
        for(i=0; i<PHCS_BFLMFRD_STD_BLOCK_SIZE; i++)
        {
            PDEBUG("%02X", mfr_p.buffer[i]);
        }
        PDEBUG("\r\n");
    }
    return status;
}

int ICACHE_FLASH_ATTR 
ISO14443_active(int argc, char *argv[])
{
    /* Counter variable */
    uint_fast16_t loop_cnt;
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Set the timer to auto mode, 5ms using operation control commands before HF is switched on to
     * guarantee Iso14443-3 compliance of Polling procedure
     */
    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 5000, 1);
    /* Activate the field (automatically if there is no external field detected) */
    mrp.address = PHCS_BFL_JREG_TXAUTO;
    mrp.mask    = PHCS_BFL_JBIT_INITIALRFON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
    mrp.set     = 1;
    status = rc_reg_ctl.ModifyRegister(&mrp);
    /*
     * After switching on the drivers wait until the timer interrupt occures, so that
     * the field is on and the 5ms delay have been passed.
     */
    loop_cnt = 5000;
    grp.address = PHCS_BFL_JREG_COMMIRQ;
    do
    {
        status = rc_reg_ctl.GetRegister(&grp);
    }
    while(!(grp.reg_data & PHCS_BFL_JBIT_TIMERI) && (status == PH_ERR_BFL_SUCCESS) && (--loop_cnt));
    /* Check status and diplay error if something went wrong */
    if ((status != PH_ERR_BFL_SUCCESS) || (loop_cnt == 0))
    {
        PDEBUG("*** ERROR! GetReg%02X,Dat=%02X,Sta=%X,loop=%u\r\n",
               grp.address, grp.reg_data, status, loop_cnt);
    }

    /* Clear the status flag afterwards */
    srp.address  = PHCS_BFL_JREG_COMMIRQ;
    srp.reg_data = PHCS_BFL_JBIT_TIMERI;
    status = rc_reg_ctl.SetRegister(&srp);

    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 5000, 0);
    /*
     * Following two register accesses are only needed if the interrupt pin is used
     * It doesn't hurt if polling is used instead.
     */
    /* Set IRQInv to zero -> interrupt signal won't be inverted */
    mrp.address = PHCS_BFL_JREG_COMMIEN;
    mrp.mask    = 0x80;
    mrp.set     = 0x00;
    status = rc_reg_ctl.ModifyRegister(&mrp);

    /* Enable IRQPushPull so that the PN51x actively drives the signal */
    mrp.address = PHCS_BFL_JREG_DIVIEN;
    mrp.mask    = 0x80;
    mrp.set     = 0xFF;
    status = rc_reg_ctl.ModifyRegister(&mrp);

    /* Activate receiver for communication
       The RcvOff bit and the PowerDown bit are cleared, the command is not changed. */
    srp.address  = PHCS_BFL_JREG_COMMAND;
    srp.reg_data = PHCS_BFL_JCMD_NOCMDCHANGE;
    status = rc_reg_ctl.SetRegister(&srp);
    return 0;
}

/* ==========================================================================*/

/*
 * This example shows how to act as Mifare reader
 */
int ICACHE_FLASH_ATTR
MifareReader_requestA(int argc, char *argv[])
{
    /* Status variable */
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Buffer for data transfer (send/receive) */
//    uint8_t buffer[EXAMPLE_TRX_BUFFER_SIZE];
    /* Buffer serial number                     */
    /* Counter variable */
    get_pre_timer_tick();
    if (argc & COIN_OPT_INIT_PRE_FLOW)
    {
        MifareReader_init();
        ISO14443_active(0, NULL);
    }
    /* Set timeout for REQA, ANTICOLL, SELECT to 200us */
    SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 500, 0);
    /*
     * Do activation according to ISO14443A Part3
     * Prepare the Request command
     */
    buffer[0] = 0;
    buffer[1] = 0;
    /* Set Request command code (or Wakeup: ISO14443_3_REQALL) */
    //req_p.req_code = PHCS_BFLI3P3A_REQIDL;
    req_p.req_code = PHCS_BFLI3P3A_REQALL;
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
            PDEBUG("[W] Multiple Cards detected!! Collision occured during Request!\r\n");
        }
        else
        {
        }
//        if (status = PH_ERR_BFL_SUCCESS)
        //os_printf("ATQA=%02X %02X,sta:%X\r\n", req_p.atq[0],req_p.atq[1], status);
        status = PH_ERR_BFL_SUCCESS;
    }
    else
    {
        /* No response at all */
        PDEBUG("*** ERROR! RequestA: RequestCode=%02X, ATQA=%02X %02X, Status = %04X \r\n",
               req_p.req_code, req_p.atq[0],req_p.atq[1], status);
    }
    return status;
}

int ICACHE_FLASH_ATTR
MifareReader_halt(int argc, char *argv[])
{
    /* Buffer for data transfer (send/receive) */
    if (argc & COIN_OPT_INIT_PRE_FLOW)
    {
        MifareReader_select(COIN_OPT_INIT_PRE_FLOW, NULL,NULL);
    }
    return (ISO14443_halt());
}

int ICACHE_FLASH_ATTR
MifareReader_select(int argc, char *argv[],uint8_t *uid)
{
    /* Status variable */
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Buffer for data transfer (send/receive) */
    /* Buffer for data transfer (send/receive) */
    if (argc & COIN_OPT_INIT_PRE_FLOW)
    {
        MifareReader_requestA(COIN_OPT_INIT_PRE_FLOW, NULL);
    }
    /* Prepare data for combined anticollision/select command to get one complete ID */
    os_memset(serial, 0, sizeof(serial));
    sel_p.uid          = serial;                 /* Put serial buffer for ID */
    sel_p.uid_length   = 0;                      /* No bits are known from ID, so set to 0 */
    /* Start Anticollision/Select command */
    status = iso14443_3.AnticollSelect(&sel_p);
    /* Check return code: If the command was OK, is always returns PH_ERR_BFL_SUCCESS.
     * If nothing was received, parameter uid_length is 0. */
    if (status == PH_ERR_BFL_SUCCESS)
    {
        if ((argc&COIN_OPT_OUTPUT_ARGV_PARA) && (NULL != argv))
        {
            os_memcpy((void *)argv, serial, sel_p.uid_length>>3);
        }
        show_id_sak(sel_p.uid, sel_p.uid_length>>3, sel_p.sak);
        if ( (sel_p.flags & PHCS_BFLI3P3A_COLL_DETECTED) == PHCS_BFLI3P3A_COLL_DETECTED )
        {
            //os_printf("[I] More than one card detected, collision has been resolved.\r\n");
        }

        if (0x20 != (sel_p.sak & 0x20))
        {
            //os_printf("The PICC not compliant with ISO/IEC 14443-4\r\n");
        }
        else
        {
            //os_printf("The PICC ISO/IEC 14443-4 card\r\n");
        }
        return status;
    }
    else
    {
        PDEBUG("*** ERROR! Anticollision/Select: Status = %04X \r\n", status);
    }
    return status;
}
extern int ICACHE_FLASH_ATTR  
MifareReader_block_auth(uint_fast8_t block, uint8_t *uid, uint8_t *keyA, uint8_t *keyB)
{
    return (ISO14443_auth(block, uid, keyA, keyB));
}
int ICACHE_FLASH_ATTR
MifareReader_block_read(uint_fast8_t block,  uint8_t *rd_buf)
{
    return (ISO14443_read(block, rd_buf));
}
int ICACHE_FLASH_ATTR
MifareReader_block_write(uint_fast8_t block, uint8_t *wr_buf)
{
    return (ISO14443_write(block, wr_buf));
}
/*******************************************************************************
* Function Name  : PN512RDSerialnum
* Description    : ¶Á¿¨Æ¬ÏµÁÐºÅ
* Input          : none
* Output         : o_snr:¿¨Æ¬Î¨Ò»ºÅ(F08 4¸ö×Ö½Ú)
* Return         : 0:³É¹¦  ·Ç0:´íÎó×´Ì¬
*******************************************************************************/
extern uint_fast8_t ICACHE_FLASH_ATTR
PN512RDSerialnum(uint8_t * o_snr)
{
    if (MifareReader_requestA(COIN_OPT_INIT_PRE_FLOW,NULL) != 0)
    {
        PDEBUG("request\r\n");
        return 1;
    }
    if (MifareReader_select(0,NULL,o_snr) != 0)
    {
        PDEBUG("select\r\n");
        return 1;
    }
    return 0;
}
/*******************************************************************************
* Function Name  : PN512Close
* Description    : ¹Ø±Õ¶Á¿¨IC
* Input          : none
* Output         : none
* Return         : none
*******************************************************************************/
extern void ICACHE_FLASH_ATTR
PN512Close(void)
{
    PN512_reset();
}


//extern phcsBfl_Status_t phcsBflRegCtl_SpiHw1GetReg(phcsBflRegCtl_GetRegParam_t *getreg_param);


/* ===========================================================================*/

/*
 * Sets the timeout value (via PN51x timer).
 */
phcsBfl_Status_t ICACHE_FLASH_ATTR
SetTimeOut(phcsBflRegCtl_t *rc_reg_ctl,
                            phcsBflOpCtl_t *rc_op_ctl,
                            uint32_t aMicroSeconds,
                            uint8_t  aFlags)
{
    phcsBfl_Status_t                    status;
    phcsBflOpCtl_AttribParam_t      opp;
    phcsBflRegCtl_ModRegParam_t     mrp;
    phcsBflRegCtl_GetRegParam_t     grp;
    phcsBflRegCtl_SetRegParam_t     srp;
    uint32_t                        regValues;

    opp.self = rc_op_ctl;
    grp.self = srp.self = mrp.self = rc_reg_ctl;

    if (aFlags == PN51X_STEPS_10US)
    {
        regValues = aMicroSeconds / 10;
    }
    else if ( (aFlags == PN51X_STEPS_100US)  || (aFlags == PN51X_TMR_DEFAULT) )
    {
        aFlags = PN51X_STEPS_100US;
        regValues = aMicroSeconds / 100;
    }
    else if (aFlags == PN51X_STEPS_500US)
    {
        regValues = aMicroSeconds / 500;
    }
    else
    {
        PDEBUG("[E] SetTimeOut: Invalid flags.\r\n");
        return PH_ERR_BFL_INVALID_PARAMETER;
    }


    /* Check if value is within the supported range
     * NOTE: The supported hardware range is bigger, since the prescaler here is always set
     *       to 100us.
     */
    if (regValues >= 0xFFFF)
    {
        PDEBUG("[E] SetTimeOut: Overflow.\r\n");
        return PH_ERR_BFL_INVALID_PARAMETER;
    }

    /*
     * Activate / deactivate timer
     */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_AUTO;
    if (aMicroSeconds == PN51X_TMR_OFF)
    {
        opp.param_a = PHCS_BFLOPCTL_VAL_OFF;
    }
    else
    {
        opp.param_a = PHCS_BFLOPCTL_VAL_ON;
    }
    status = rc_op_ctl->SetAttrib(&opp);
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("[E] Error setting timer! Status = %04x\r\n", status);
        return status;
    }

    if (aMicroSeconds == PN51X_TMR_OFF)
    {
        /* We would just like to switch off the timer --> we are done */
        return status;
    }

    /*
     * Set prescaler steps. A granularity of 100us is defined.
     */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_PRESCALER;
    if (aMicroSeconds == PN51X_TMR_MAX)
    {
        opp.param_a         = 0xFF;
        opp.param_b         = 0x0F;
    }
    else
    {
        if (aFlags == PN51X_STEPS_10US)
        {
            opp.param_a         = 0x44;
            opp.param_b         = 0x00;
        }
        else if (aFlags == PN51X_STEPS_100US)
        {
            /* Calculation of the values: 100us = 6.78MHz/(677+1) = 6.78MHz/(0x2A5+1) */
            opp.param_a          = 0xA5;                        /* Low value  */
            opp.param_b          = 0x02;                        /* High value */
        }
        else if (aFlags == PN51X_STEPS_500US)
        {
            opp.param_a          = 0x3D;
            opp.param_b          = 0x0D;
        }
        else
        {
            PDEBUG("[E] Timer: Invalid step size.\r\n");
            return PH_ERR_BFL_OTHER_ERROR;
        }
    }
    status = rc_op_ctl->SetAttrib(&opp);
    /* Check status and display error if something went wrong */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("[E] Error setting timer! Status = %04x \r\n", status);
        return status;
    }

    /*
     * Set the reload value
     */
    if ( (aMicroSeconds == PN51X_TMR_MAX) || (aFlags == PN51X_TMR_MAX) )
    {
        regValues = 0xFFFF;
    }

    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;         /* set operation control group parameter to timer */
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_RELOAD;   /* set to 50 for 5 ms */
    opp.param_a          = (uint8_t) ( regValues & 0xFF );  /* Low value     */
    opp.param_b          = (uint8_t) ( (regValues >> 8) & 0xFF ); /* High value (max 0xFF) */
    status = rc_op_ctl->SetAttrib(&opp);
    if(status != PH_ERR_BFL_SUCCESS)
    {
       PDEBUG("[E] Error setting timer! Status = %04x\r\n", status);
    }

    // Usually there has to be an event to be occured to start the timer.
    // However, if one wants to start the timer right now, this flag can
    // be set.
    if ((aFlags & PN51X_START_NOW) == PN51X_START_NOW)
    {
        mrp.set = 1;
    }
    else
    {
        mrp.set = 0;
    }

    mrp.address = PHCS_BFL_JREG_CONTROL;
    mrp.mask = PHCS_BFL_JBIT_TSTARTNOW;
    status = rc_reg_ctl->ModifyRegister(&mrp);
    if ((aFlags & PN51X_START_NOW) == PN51X_START_NOW)
    {
        grp.address  = PHCS_BFL_JREG_COMMIRQ;
        grp.reg_data = 0x00;
        while(status == PH_ERR_BFL_SUCCESS && (grp.reg_data & PHCS_BFL_JBIT_TIMERI) != PHCS_BFL_JBIT_TIMERI)
        {
            status = rc_reg_ctl->GetRegister(&grp);
        }
    }
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("[E] Error accessing register! Status = %04x\r\n", status);
    }

    /* stop timer if running */
    mrp.set = 1;
    mrp.address = PHCS_BFL_JREG_CONTROL;
    mrp.mask = PHCS_BFL_JBIT_TSTOPNOW;
    status = rc_reg_ctl->ModifyRegister(&mrp);
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("[E] Error modifying register 0x%02X! Status = %04x\r\n", mrp.address, status);
    }

    /* clear all IRQs if occured */
    srp.address = PHCS_BFL_JREG_COMMIRQ;
    srp.reg_data = 0x7F;
    status = rc_reg_ctl->SetRegister(&srp);
    if(status != PH_ERR_BFL_SUCCESS)
    {
        PDEBUG("[E] Error setting register 0x%02X! Status = %04x \n", srp.address, status);
    }
	else{
		grp.address = PHCS_BFL_JREG_COMMIRQ;
		grp.reg_data = 0xa5;
		//phcsBflRegCtl_SpiHw1GetReg(&srp);
		//PDEBUG("[T] OK get register1 0x%02X \n", srp.reg_data);
		rc_reg_ctl->GetRegister(&grp);
		PDEBUG("[T] OK get register2 0x%02X \n", grp.reg_data);
	}
	
    return status;
}


