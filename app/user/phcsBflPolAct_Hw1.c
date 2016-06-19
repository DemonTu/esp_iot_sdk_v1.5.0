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

/*! \file phcsBflPolAct_Hw1.c
 *
 * Project: Object Oriented Library Framework phcsBflPolAct_Wrapper Component.
 *
 *  Source: phcsBflPolAct_Hw1.c
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 30 11:01:53 2006 $
 *
 * Comment:
 *  PN51x-specific variant of phcsBflPolAct_Wrapper.
 *
 * Functions:
 *      phcsBflPolAct_Hw1Init
 *      phcsBflPolAct_Hw1Polling
 *
 * History:
 *  MHa: Generated 07. May 2003
 *  MHa: Migrated to MoReUse September 2005
 *
 */

#include <phcsBflRefDefs.h>
#include <phcsBflHw1Reg.h>
#include <phcsBflAux.h>

#include "phcsBflPolAct_Hw1Int.h"


#define POLLING_REQUEST_COMMAND      0x00
#define POLLING_REQUEST_LENGTH       0x06
#define POLLING_RESPONSE_COMMAND     0x01
#define POLLING_RESPONSE_LENGTH_1    0x12
#define POLLING_RESPONSE_LENGTH_2    0x14


void ICACHE_FLASH_ATTR     
phcsBflPolAct_Hw1Init(phcsBflPolAct_t     *cif,
                              void              *rp,
                              phcsBflRegCtl_t     *p_lower,
                              uint8_t            initiator__not_target)
{
    /* Glue together and init the operation parameters: */
    ((phcsBflPolAct_Hw1Params_t*)rp)->m_InitiatorNotTarget    = initiator__not_target;
    cif->mp_Members          = rp;
    cif->mp_Lower            = p_lower;
    cif->mp_WaitEventCB      = NULL;
    cif->mp_UserRef          = NULL;
    /* Initialize the function pointers: */
    cif->Polling             = phcsBflPolAct_Hw1Polling;
    cif->SetWaitEventCb      = phcsBflPolAct_Hw1SetWaitEventCb;
}


void ICACHE_FLASH_ATTR
phcsBflPolAct_Hw1SetWaitEventCb(phcsBflPolAct_SetWecParam_t *set_wec_param)
{
    phcsBflPolAct_t *cif    = (phcsBflPolAct_t*)(set_wec_param->self);
    cif->mp_WaitEventCB   = set_wec_param->wait_event_cb;
    cif->mp_UserRef       = set_wec_param->user_ref;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflPolAct_Hw1Polling(phcsBflPolAct_PollingParam_t *polling_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status;
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  istatus;
    phcsBflRegCtl_t     PHCS_BFL_MEMLOC_REM *lower   = (phcsBflRegCtl_t*)(((phcsBflPolAct_t*)(polling_param->self))->mp_Lower);

    phcsBflAux_CommandParam_t    PHCS_BFL_MEMLOC_REM command_param;
    phcsBflRegCtl_SetRegParam_t  PHCS_BFL_MEMLOC_REM setreg_param;
    phcsBflRegCtl_ModRegParam_t  PHCS_BFL_MEMLOC_REM modify_param;

    /* self is set only once. Not modified during operation!! */
    command_param.lower          = lower;
    command_param.wait_event_cb  = ((phcsBflPolAct_t*)(polling_param->self))->mp_WaitEventCB;
    command_param.user_ref       = ((phcsBflPolAct_t*)(polling_param->self))->mp_UserRef;
    setreg_param.self            = lower;
    modify_param.self            = lower;

    if (((phcsBflPolAct_Hw1Params_t*)(((phcsBflPolAct_t*)(polling_param->self))->mp_Members))->m_InitiatorNotTarget)
    {
        /* Device is Initiator: We act like a FeliCa card Reader and do a full TX/RX cycle. */
        uint8_t  PHCS_BFL_MEMLOC_COUNT  i;              /* running variable to count up the received IDs */
        uint16_t PHCS_BFL_MEMLOC_REM    rx_index;       /* index counter for data coming from RcAux function */
        uint16_t PHCS_BFL_MEMLOC_REM    tx_index;       /* counter for buffer position of outgoing data */
        uint8_t  PHCS_BFL_MEMLOC_REM    OneCardLength;  /* stores length of 1 complete polling response */

        /* check if Time Slot value is in the correct range and
           check if incomming data length is correct and
           check if rx_buffer_size is big enough */
        if(((polling_param->tx_buffer[3] == 0x00) || (polling_param->tx_buffer[3] == 0x01) || 
            (polling_param->tx_buffer[3] == 0x03) || (polling_param->tx_buffer[3] == 0x07) ||
            (polling_param->tx_buffer[3] == 0x0f))
            && (polling_param->tx_buffer_size == 4)
            && (polling_param->rx_buffer_size >= 21))
        {
            /* activate RxMultiple for multiple Target receive */
            modify_param.address = PHCS_BFL_JREG_RXMODE;
            modify_param.set     = 1;
            modify_param.mask    = PHCS_BFL_JBIT_RXMULTIPLE;
            PHCS_BFLAUX_CHECK_SUCCESS(lower->ModifyRegister(&modify_param))

            /* prepare send buffer, first incoming data if buffer are the same, then the command */
            for(i=0; i<4; i++)
                polling_param->rx_buffer[5-i] = polling_param->tx_buffer[3-i];

            polling_param->rx_buffer[1] = POLLING_REQUEST_COMMAND; /* Command */
            polling_param->rx_buffer[0] = POLLING_REQUEST_LENGTH;  /* Length  */


            /* set necessary parameters for transmission */
            command_param.initiator = PHCS_BFLAUX_INITIATOR;
            command_param.cmd       = PHCS_BFL_CMD_TRANSCEIVE_TO;
            command_param.tx_buffer = polling_param->rx_buffer;
            command_param.tx_bytes  = POLLING_REQUEST_LENGTH;
            command_param.rx_buffer = polling_param->rx_buffer;
            command_param.rx_bytes  = polling_param->rx_buffer_size;

            /* start Tx/Rx Procedure */
            istatus = phcsBflAux_Hw1Command(&command_param);

            /* check all possible errors, else skip response 
               Note: PH_ERR_BFL_IO_TIMEOUT never occurs because of PHCS_BFL_CMD_TRANSCEIVE_TO */
            if((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_IO_TIMEOUT || 
               (istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_CRC_ERROR || 
               (istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_BUFFER_OVERFLOW)
            {
                /* read incoming data and if Length and Command are correct store
                    Length, IDm, PMm and optionally the Systemcode in rx buffer of this function. */
                rx_index = 0;
                tx_index = 0;
                while(rx_index < command_param.rx_bytes)
                {
                    /* check length information */
                    if(command_param.rx_buffer[rx_index] == POLLING_RESPONSE_LENGTH_1 ||
                        command_param.rx_buffer[rx_index] == POLLING_RESPONSE_LENGTH_2 )
                    {
                        /* store length information of 1st data stream */
                        OneCardLength = command_param.rx_buffer[rx_index];
                        /* check if received command byte is correct and check byte is 0x00 */
                        if(command_param.rx_buffer[rx_index + 1] == POLLING_RESPONSE_COMMAND && 
                            command_param.rx_buffer[rx_index + OneCardLength] == 0x00 &&
                            command_param.rx_bytes >= (rx_index + OneCardLength))
                        {
                            /* copy all received data (length is stored in 1st byte, command byte stored in 2nd byte) */
                            for(i=0; i < OneCardLength; i++)
                            {
                                polling_param->rx_buffer[tx_index] = command_param.rx_buffer[rx_index];
                                tx_index++;
                                rx_index++;
                            }
                            rx_index++;     /* skip check byte stored in FIFO */

                            istatus = PH_ERR_BFL_SUCCESS;
                        } else
                        {
                            /* skip received data length + 1 and continue (skip also check byte) */
                            rx_index = (uint8_t)(rx_index + OneCardLength + 1);
                        }
                    } else
                    {
						/* rx bytes contradict to protocol specification */
						istatus = PH_ERR_BFL_PROTOCOL_ERROR;
						break;
                    }
                }
                polling_param->rx_buffer_size = tx_index;  /* return length of returned data */
            } else
            {
                polling_param->rx_buffer_size = 0;
            }
            /* reset RxMultiple bit for next communication */
            modify_param.address = PHCS_BFL_JREG_RXMODE;
            modify_param.set     = 0;
            modify_param.mask    = PHCS_BFL_JBIT_RXMULTIPLE;
            PHCS_BFLAUX_CHECK_SUCCESS(lower->ModifyRegister(&modify_param))

            /* Reset RxIrq bit for next communication: PN51x can (re)set individual bits in this reg. */
            setreg_param.address  = PHCS_BFL_JREG_COMMIRQ;
            setreg_param.reg_data = PHCS_BFL_JBIT_RXI;
            PHCS_BFLAUX_CHECK_SUCCESS(lower->SetRegister(&setreg_param))
        } else
        {
           istatus = PH_ERR_BFL_INVALID_PARAMETER;
        }
    } else
    {
        /*
         * Device is Target: Activate automatic polling procedure. 
         * Initialisation of IDm, PMm, systemcode must be done by a configuration command. 
         */
        phcsBflRegCtl_GetRegParam_t      PHCS_BFL_MEMLOC_REM getreg_param;
        getreg_param.self              = lower;

        getreg_param.address = PHCS_BFL_JREG_COMMAND;
        PHCS_BFLAUX_CHECK_SUCCESS(lower->GetRegister(&getreg_param))

        setreg_param.address = PHCS_BFL_JREG_COMMAND;
        setreg_param.reg_data = (uint8_t)((PHCS_BFL_JMASK_COMMAND_INV & getreg_param.reg_data) | PHCS_BFL_JCMD_AUTOCOLL);
        PHCS_BFLAUX_CHECK_SUCCESS(lower->SetRegister(&setreg_param))

        istatus = PH_ERR_BFL_SUCCESS;
    }

    PHCS_BFL_ADD_COMPCODE(istatus, PH_ERR_BFL_POLACT)
    return status;
}


/* E.O.F. */
