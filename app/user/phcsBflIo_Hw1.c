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

/*! \file phcsBflIo_Hw1.c
 *
 * Project: Object Oriented Library Framework I/O Component.
 *
 *  Source: phcsBflIo_Hw1.c
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 30 11:01:53 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  PN51x-specific variant of I/O.
 *
 * Functions:
 *      phcsBflIo_Hw1Init
 *      phcsBflIo_Hw1Transceive
 *      phcsBflIo_Hw1Transmit
 *      phcsBflIo_Hw1Receive
 *      phcsBflIo_Hw1MfAuthent
 *
 * History:
 *  GK:     Generated 12. March 2003
 *  MHa:    Functions implemented 2. May 2003
 *  MHa:    Migrated to MoReUse September 2005
 *
 */


#include <phcsBflHw1Reg.h>
#include <phcsBflAux.h>

#include "phcsBflIo_Hw1Int.h"


void ICACHE_FLASH_ATTR
phcsBflIo_Hw1Init(phcsBflIo_t        *cif,
                          void             *rp,
                          phcsBflRegCtl_t    *p_lower,
                          uint8_t           initiator__not_target)
{
    /* Glue together and init the operation parameters: */
    ((phcsBflIo_Hw1Params_t*)rp)->m_InitiatorNotTarget    = initiator__not_target;
    cif->mp_Members             = rp;
    cif->mp_Lower               = p_lower;
    cif->mp_WaitEventCB         = NULL;
    cif->mp_UserRef             = NULL;
    /* Initialize the function pointers: */
    cif->MfAuthent              = phcsBflIo_Hw1MfAuthent;
    cif->Receive                = phcsBflIo_Hw1Receive;
    cif->Transceive             = phcsBflIo_Hw1Transceive;
    cif->Transmit               = phcsBflIo_Hw1Transmit;
    cif->SetWaitEventCb         = phcsBflIo_Hw1SetWaitEventCb;
}


void ICACHE_FLASH_ATTR
phcsBflIo_Hw1SetWaitEventCb(phcsBflIo_SetWecParam_t *set_wec_param)
{
    phcsBflIo_t *cif        = (phcsBflIo_t*)(set_wec_param->self);
    cif->mp_WaitEventCB   = set_wec_param->wait_event_cb;
    cif->mp_UserRef       = set_wec_param->user_ref;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIo_Hw1Transceive(phcsBflIo_TransceiveParam_t *transceive_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status;
    phcsBflRegCtl_t     PHCS_BFL_MEMLOC_REM *p_lower  = (phcsBflRegCtl_t*)(((phcsBflIo_t*)(transceive_param->self))->mp_Lower);

    if ((((phcsBflIo_Hw1Params_t*)(((phcsBflIo_t*)(transceive_param->self))->mp_Members))->m_InitiatorNotTarget) == PHCS_BFLIO_INITIATOR)
    {
        /* Device is Initiator: We do a "True" TX-RX cycle: */

        phcsBflAux_CommandParam_t PHCS_BFL_MEMLOC_REM command_param;

        command_param.lower          = p_lower;
        command_param.wait_event_cb  = ((phcsBflIo_t*)(transceive_param->self))->mp_WaitEventCB;
        command_param.user_ref       = ((phcsBflIo_t*)(transceive_param->self))->mp_UserRef;
        command_param.initiator      = PHCS_BFLAUX_INITIATOR;
        command_param.cmd            = PHCS_BFL_JCMD_TRANSCEIVE;
        command_param.tx_buffer      = transceive_param->tx_buffer;
        command_param.tx_bytes       = transceive_param->tx_buffer_size;
        command_param.rx_buffer      = transceive_param->rx_buffer;
        command_param.rx_bytes       = transceive_param->rx_buffer_size;

        status = phcsBflAux_Hw1Command(&command_param);

        transceive_param->rx_buffer_size = command_param.rx_bytes;
        transceive_param->rx_bits = command_param.rx_bits;

    } else
    {
        /*
         * Device is Target: The request is invalid (cannot do TX-RX since it is not allowed 
         * to initiate a transfer!
         */
        status = PH_ERR_BFL_INVALID_DEVICE_STATE;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IO)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIo_Hw1Transmit(phcsBflIo_TransmitParam_t *transmit_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status = PH_ERR_BFL_SUCCESS;
    phcsBflRegCtl_t PHCS_BFL_MEMLOC_REM *p_lower = (phcsBflRegCtl_t*)(((phcsBflIo_t*)(transmit_param->self))->mp_Lower);
    
    phcsBflAux_CommandParam_t PHCS_BFL_MEMLOC_REM command_param;
    command_param.lower          = p_lower;
    command_param.wait_event_cb  = ((phcsBflIo_t*)(transmit_param->self))->mp_WaitEventCB;
    command_param.user_ref       = ((phcsBflIo_t*)(transmit_param->self))->mp_UserRef;
    
    if ((((phcsBflIo_Hw1Params_t*)(((phcsBflIo_t*)(transmit_param->self))->mp_Members))->m_InitiatorNotTarget) == PHCS_BFLIO_INITIATOR)
    {
        /* Device is Initiator: A simple TX is done out of context with RX. */
        command_param.initiator = PHCS_BFLAUX_INITIATOR;
        command_param.cmd = PHCS_BFL_JCMD_TRANSMIT;
        command_param.tx_buffer = transmit_param->tx_buffer;
        command_param.tx_bytes = transmit_param->tx_buffer_size;
        command_param.rx_bytes = 0;
        command_param.rx_buffer = NULL;

        status = phcsBflAux_Hw1Command(&command_param);
        
    } else
    {
        /*
         * Device is Target: This enabled data to be returned to the Initiator automatically by the
         * PN51x inverse Transceive routine (StartSend).
         */

        /* do general settings before switching between Initiator and Target */
        phcsBflRegCtl_GetRegParam_t  PHCS_BFL_MEMLOC_REM getreg_param;
        getreg_param.self = p_lower;

        /* check if transceive command is already active
           if not return error because must be activated during receive */
        getreg_param.address  = PHCS_BFL_JREG_COMMAND;
        p_lower->GetRegister(&getreg_param);

        if((getreg_param.reg_data & PHCS_BFL_JMASK_COMMAND) != PHCS_BFL_JCMD_TRANSCEIVE)
        {
            status = PH_ERR_BFL_INVALID_DEVICE_STATE;    /* not allowed state, must be activated during receive */
        }

        if(status == PH_ERR_BFL_SUCCESS)
        {
            command_param.initiator = PHCS_BFLAUX_TARGET;
            command_param.target_send = 1;
            command_param.cmd = PHCS_BFL_JCMD_TRANSCEIVE;
            command_param.tx_buffer = transmit_param->tx_buffer;
            command_param.tx_bytes = transmit_param->tx_buffer_size;
            command_param.rx_bytes = 0;
            command_param.rx_buffer = NULL;

            status = phcsBflAux_Hw1Command(&command_param);
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IO)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIo_Hw1Receive(phcsBflIo_ReceiveParam_t *receive_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status;
    phcsBflRegCtl_t     PHCS_BFL_MEMLOC_REM *p_lower = (phcsBflRegCtl_t*)(((phcsBflIo_t*)(receive_param->self))->mp_Lower);
    
    phcsBflAux_CommandParam_t PHCS_BFL_MEMLOC_REM command_param;
    command_param.lower          = p_lower;
    command_param.wait_event_cb  = ((phcsBflIo_t*)(receive_param->self))->mp_WaitEventCB;
    command_param.user_ref       = ((phcsBflIo_t*)(receive_param->self))->mp_UserRef;

    if ((((phcsBflIo_Hw1Params_t*)(((phcsBflIo_t*)(receive_param->self))->mp_Members))->m_InitiatorNotTarget) == PHCS_BFLIO_INITIATOR)
    {
        /* Device is Initiator: Switch to RX mode and wait for data, out of context with TX: */

        command_param.initiator = PHCS_BFLAUX_INITIATOR;
        command_param.cmd = PHCS_BFL_JCMD_RECEIVE;
        command_param.rx_buffer = receive_param->rx_buffer;
        command_param.rx_bytes = receive_param->rx_buffer_size;
        command_param.rx_bits = 0x00;
        /* set Tx Parameters here. */
        command_param.tx_buffer = NULL;
        command_param.tx_bytes = 0;

        status = phcsBflAux_Hw1Command(&command_param);

        receive_param->rx_buffer_size = command_param.rx_bytes;
        receive_param->rx_bits = command_param.rx_bits;

    } else
    {
        /*
         * Device is Target: This enables data to be received from the Initiator by the PN51x
         * inverse Transceive routine (RX-TX).
         */

        command_param.initiator = PHCS_BFLAUX_TARGET;
        command_param.target_send = 0;
        command_param.cmd = PHCS_BFL_JCMD_TRANSCEIVE;
        command_param.rx_buffer = receive_param->rx_buffer;
        command_param.rx_bytes = receive_param->rx_buffer_size;
        command_param.rx_bits = 0x00;
        /* set Tx Parameters here because Transceive function is used and this parameters are 
         * not initialised by the caller. */
        command_param.tx_buffer = NULL;
        command_param.tx_bytes = 0;

        status = phcsBflAux_Hw1Command(&command_param);

        receive_param->rx_buffer_size = command_param.rx_bytes;
        receive_param->rx_bits = command_param.rx_bits;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IO)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIo_Hw1MfAuthent(phcsBflIo_MfAuthentParam_t *mfauthent_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status;
    phcsBflRegCtl_t     PHCS_BFL_MEMLOC_REM *p_lower = (phcsBflRegCtl_t*)(((phcsBflIo_t*)(mfauthent_param->self))->mp_Lower);

    if ((((phcsBflIo_Hw1Params_t*)(((phcsBflIo_t*)(mfauthent_param->self))->mp_Members))->m_InitiatorNotTarget) == PHCS_BFLIO_INITIATOR)
    {
        /* Device is Initiator: A MF Reader-initiated authentication is performed. ! */

        phcsBflAux_CommandParam_t PHCS_BFL_MEMLOC_REM command_param;
        phcsBflRegCtl_GetRegParam_t      PHCS_BFL_MEMLOC_REM getreg_param;

        command_param.lower          = p_lower;
        command_param.wait_event_cb  = ((phcsBflIo_t*)(mfauthent_param->self))->mp_WaitEventCB;
        command_param.user_ref       = ((phcsBflIo_t*)(mfauthent_param->self))->mp_UserRef;

        getreg_param.self                   = p_lower;

        /* check if incoming parameter length is OK */
        if(mfauthent_param->tx_buffer_size != 12)
        {
            status = PH_ERR_BFL_INVALID_PARAMETER;
        } else
        {
            command_param.cmd       = PHCS_BFL_JCMD_AUTHENT;
            command_param.tx_buffer = mfauthent_param->tx_buffer;
            command_param.tx_bytes  = mfauthent_param->tx_buffer_size;
            command_param.rx_bytes  = 0;
            command_param.rx_bits   = 0;
            command_param.rx_buffer = NULL;
            command_param.initiator = PHCS_BFLAUX_INITIATOR;

            status = phcsBflAux_Hw1Command(&command_param);

            if(status == PH_ERR_BFL_SUCCESS)
            {
                /* check first if the Crypto1 bit is set to 1  */
                getreg_param.address = PHCS_BFL_JREG_STATUS2;
                status = p_lower->GetRegister(&getreg_param);

                if((status == PH_ERR_BFL_SUCCESS) && ((getreg_param.reg_data & PHCS_BFL_JBIT_CRYPTO1ON) == 0x00))
                {
                    status = PH_ERR_BFL_AUTHENT_ERROR;
                } else 
                {   /* Afterwards check if the Autocoll command has terminated correctly (returned to IDLE) */
                    getreg_param.address = PHCS_BFL_JREG_COMMAND;
                    status = p_lower->GetRegister(&getreg_param);
                    if((status == PH_ERR_BFL_SUCCESS) && ((getreg_param.reg_data & PHCS_BFL_JMASK_COMMAND) != PHCS_BFL_JCMD_IDLE))
                    {
                        status = PH_ERR_BFL_AUTHENT_ERROR;
                    }
                }
            }
        }
    } else
    {
        /* Device is Target: The device must refuse the request: */
        status = PH_ERR_BFL_INVALID_DEVICE_STATE;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IO)
    return status;
}


/* E.O.F. */
