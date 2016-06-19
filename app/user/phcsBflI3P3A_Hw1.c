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

/*! \file phcsBflI3P3A_Hw1.c
 *
 * Project: Object Oriented Library Framework phcsBfl_I3P3A component for PN51x.
 *
 *  Source: phcsBflI3P3A_Hw1.c
 * $Author: mha $
 * $Revision: 1.8 $
 * $Date: Fri Jun 30 11:01:48 2006 $
 *
 * Comment:
 *  PN51x-specific variant of Iso14443-3 Type A.
 *
 * History:
 *  MHa: Generated 13. May 2003
 *  MHa: Migrated to MoReUse September 2005
 *
 */


#include <phcsBflHw1Reg.h>
#include <phcsBflAux.h>

#include "phcsBflI3P3A_Hw1Int.h"

//#define YYF_DEBUG
#ifdef YYF_DEBUG
#include <stdio.h>
#define yyf_debug(format, ...) printf (format, __VA_ARGS__)
#else
#define yyf_debug(format, ...)
#endif
void      phcsBflI3P3A_Hw1Initialise(phcsBflI3P3A_t *cif,
                                     void           *rp,
                                     phcsBflRegCtl_t   *p_lower,
                                     uint8_t   initiator__not_target)
{
    /* Glue together and init the operation parameters: */
    ((phcsBflI3P3A_Hw1Params_t*)rp)->m_InitiatorNotTarget = initiator__not_target;
    cif->mp_Members                                     = rp;
    cif->mp_Lower                                       = p_lower;
    cif->mp_WaitEventCB                                 = NULL;
    cif->mp_UserRef                                     = NULL;
    /* Initialize the function pointers: */
    cif->RequestA                                       = phcsBflI3P3A_Hw1RequestA;
    cif->AnticollSelect                                 = phcsBflI3P3A_Hw1AnticollSelect;
    cif->Select                                         = phcsBflI3P3A_Hw1Select;
    cif->HaltA                                          = phcsBflI3P3A_Hw1HaltA;
    cif->SetWaitEventCb                                 = phcsBflI3P3A_Hw1SetWaitEventCb;
}


void phcsBflI3P3A_Hw1SetWaitEventCb(phcsBflI3P3A_SetWecParam_t *set_wec_param)
{
    phcsBflI3P3A_t *cif     = (phcsBflI3P3A_t*)(set_wec_param->self);
    cif->mp_WaitEventCB   = set_wec_param->wait_event_cb;
    cif->mp_UserRef       = set_wec_param->user_ref;
}


/* Definitions for Request command. */
#define REQUEST_BITS    0x07
#define ATQA_LENGTH     0x02

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P3A_Hw1RequestA(phcsBflI3P3A_ReqAParam_t *request_a_param)
{
    phcsBfl_Status_t     PHCS_BFL_MEMLOC_REM  status;
    phcsBfl_Status_t     PHCS_BFL_MEMLOC_REM  istatus;
    phcsBflRegCtl_t  PHCS_BFL_MEMLOC_REM *p_lower   = (phcsBflRegCtl_t*)(((phcsBflI3P3A_t*)(request_a_param->self))->mp_Lower);

    phcsBflAux_CommandParam_t command_param;
    command_param.lower          = p_lower;
    command_param.wait_event_cb  = ((phcsBflI3P3A_t*)(request_a_param->self))->mp_WaitEventCB;
    command_param.user_ref       = ((phcsBflI3P3A_t*)(request_a_param->self))->mp_UserRef;

    if (((phcsBflI3P3A_Hw1Params_t*)(((phcsBflI3P3A_t*)(request_a_param->self))->mp_Members))->m_InitiatorNotTarget == PHCS_BFLI3P3A_INITIATOR)
    {
        /*
         * Device is Initiator: We do a "True" Request cycle:
         */
        uint8_t PHCS_BFL_MEMLOC_REM rx_bytes_int = ATQA_LENGTH;
        uint8_t PHCS_BFL_MEMLOC_REM rx_bits_int  = 0;

        phcsBflRegCtl_ModRegParam_t      modify_param;
        phcsBflRegCtl_SetRegParam_t      setreg_param;
        modify_param.self = p_lower;
        setreg_param.self = p_lower;

        /* disable Crypto if activated before */
        modify_param.set = 0;                   /* clear masked bits */
        modify_param.address = PHCS_BFL_JREG_STATUS2;
        modify_param.mask = PHCS_BFL_JBIT_CRYPTO1ON;
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))

        /* disable DetectSync if activated before */
        modify_param.set = 0;                   /* clear masked bits */
        modify_param.address = PHCS_BFL_JREG_MODE;
        modify_param.mask = PHCS_BFL_JBIT_DETECTSYNC;
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))

        /* activate values after coll */
        setreg_param.address = PHCS_BFL_JREG_COLL;
        setreg_param.reg_data = PHCS_BFL_JBIT_VALUESAFTERCOLL;
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->SetRegister(&setreg_param))

        /* disable TxCRC and RxCRC */
        modify_param.set = 0;                   /* clear masked bits */
        modify_param.address = PHCS_BFL_JREG_TXMODE;     /* TxCRC */
        modify_param.mask = PHCS_BFL_JBIT_CRCEN;
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))

        modify_param.address = PHCS_BFL_JREG_RXMODE;     /* RxCRC */
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))   /* keep .set and .mask parameter like it from above */

        setreg_param.address = PHCS_BFL_JREG_BITFRAMING;
        setreg_param.reg_data = REQUEST_BITS;   /* activate to send only 7 bits */
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->SetRegister(&setreg_param))

        /* set necessary parameters for transmission */
        command_param.initiator = PHCS_BFLAUX_INITIATOR;
        command_param.cmd = PHCS_BFL_JCMD_TRANSCEIVE;
        command_param.tx_buffer = &request_a_param->req_code;
        command_param.tx_bytes = 0x01;
        command_param.rx_buffer = request_a_param->atq;
        command_param.rx_bytes = rx_bytes_int;
        command_param.rx_bits = rx_bits_int;

        istatus = phcsBflAux_Hw1Command(&command_param);

        if((istatus == PH_ERR_BFL_SUCCESS) || ((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR))
        {
            if((command_param.rx_bytes != ATQA_LENGTH) || (command_param.rx_bits != 0x00))
            {
                istatus = PH_ERR_BFL_PROTOCOL_ERROR;
            }
        }
        else
        {
            /* reset atqa parameter */
            request_a_param->atq[0] = 0x00;
            request_a_param->atq[1] = 0x00;
        }
        setreg_param.address = PHCS_BFL_JREG_BITFRAMING;
        setreg_param.reg_data = 0x00;       /* reset all settings done before */
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->SetRegister(&setreg_param))

    }
    else
    {
        /*
         * Device is Target: The request is invalid (use StartIsoActivation instead of individual commands)
         */
        istatus = PH_ERR_BFL_INVALID_DEVICE_STATE;
    }

    PHCS_BFL_ADD_COMPCODE(istatus, PH_ERR_BFL_MFRD)
    return status;
}

/* Command byte definitions for Anticollision/Select functions. */
#define SELECT_CASCADE_LEVEL_2  0x95
#define SELECT_CASCADE_LEVEL_3  0x97
#define MAX_CASCADE_LEVELS      0x03
#define SINGLE_UID_LENGTH       0x20
#define CASCADE_BIT             0x04

phcsBfl_Status_t ICACHE_FLASH_ATTR 
phcsBflI3P3A_Hw1AnticollSelect(phcsBflI3P3A_AnticollSelectParam_t *AnticollSelect_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status = PH_ERR_BFL_SUCCESS;
    phcsBflRegCtl_t PHCS_BFL_MEMLOC_REM *p_lower  = (phcsBflRegCtl_t*)(((phcsBflI3P3A_t*)(AnticollSelect_param->self))->mp_Lower);


    if (((phcsBflI3P3A_Hw1Params_t*)(((phcsBflI3P3A_t*)(AnticollSelect_param->self))->mp_Members))->m_InitiatorNotTarget == PHCS_BFLI3P3A_INITIATOR)
    {
        /*
         * Device is Initiator: We do a "True" Anticollision/Select cycle:
         */
        uint_fast8_t PHCS_BFL_MEMLOC_COUNT  i;       /* cascade level */
        uint_fast8_t PHCS_BFL_MEMLOC_REM len;
        uint_fast8_t PHCS_BFL_MEMLOC_REM casc_code;  /* cascade level code */
        uint_fast8_t PHCS_BFL_MEMLOC_REM length_in;

        length_in = AnticollSelect_param->uid_length;

        /* do loop for max. cascade level */
        for(i=0; i<MAX_CASCADE_LEVELS; i++)
        {
            if(length_in != 0x00)
            {
                if(length_in > SINGLE_UID_LENGTH)
                {
                    len = SINGLE_UID_LENGTH;
                    length_in -= SINGLE_UID_LENGTH;
                }
                else
                {
                    len = length_in;
                    length_in = 0;
                }
            }
            else
            {
                len = 0;
            }

            switch(i)
            {
                case 1:
                    casc_code = SELECT_CASCADE_LEVEL_2;
                    break;
                case 2:
                    casc_code = SELECT_CASCADE_LEVEL_3;
                    break;
                default:
                    casc_code = SELECT_CASCADE_LEVEL_1;
                    break;
            }
            //yyf_debug("select code 0x%02X\n", casc_code);

            if((len != SINGLE_UID_LENGTH) && (status == PH_ERR_BFL_SUCCESS))
            {
                /* do anticollision with selected level */
                status = phcsBflI3P3A_Hw1CascAnticoll((phcsBflI3P3A_t*)(AnticollSelect_param->self),
                                                      p_lower,
                                                      casc_code,
                                                      len,
                                                      AnticollSelect_param->uid + i * 4,
                                                      &(AnticollSelect_param->flags));
            }
            else
            {
            }
            if (status)
            {
                yyf_debug("status=0x%04X\n", status);
            }
            if(status == PH_ERR_BFL_SUCCESS)
            {
                /* select 1st cascade level uid */
                status = phcsBflI3P3A_Hw1IntSelect((phcsBflI3P3A_t*)(AnticollSelect_param->self),
                                                   p_lower,
                                                   casc_code,
                                                   AnticollSelect_param->uid + i * 4,
                                                   &AnticollSelect_param->sak);

                /* check if further cascade level is used */
                if(status == PH_ERR_BFL_SUCCESS)
                {
                    /* increase number of received bits in parameter */
                    AnticollSelect_param->uid_length = (uint8_t)(SINGLE_UID_LENGTH * (i + 1));
                    //yyf_debug("sak=0x%02X\n", AnticollSelect_param->sak);
                    /* check if cascade bit is set */
                    if((AnticollSelect_param->sak & CASCADE_BIT) == 0x00)
                    {
                        i = MAX_CASCADE_LEVELS;
                        continue;
                    }
                    else
                    {
                        /*i++*/
                    }
                }
            }
            else
            {
                i = MAX_CASCADE_LEVELS;
                continue;
            }
        }
    }
    else
    {
        /*
         * Device is Target: The Anticollision/Select is invalid (use StartIsoActivation instead of individual commands)
         */
        status = PH_ERR_BFL_INVALID_DEVICE_STATE;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_MFRD)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P3A_Hw1Select(phcsBflI3P3A_SelectParam_t *select_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status = PH_ERR_BFL_SUCCESS;
    phcsBflRegCtl_t PHCS_BFL_MEMLOC_REM *p_lower  = (phcsBflRegCtl_t*)(((phcsBflI3P3A_t*)(select_param->self))->mp_Lower);


    if (((phcsBflI3P3A_Hw1Params_t*)(((phcsBflI3P3A_t*)(select_param->self))->mp_Members))->m_InitiatorNotTarget == PHCS_BFLI3P3A_INITIATOR)
    {
        /*
         * Device is Initiator: We do a "True" Select cycle:
         */
        uint8_t PHCS_BFL_MEMLOC_COUNT  i;
        uint_fast8_t PHCS_BFL_MEMLOC_REM casc_code;

        if(((select_param->uid_length % SINGLE_UID_LENGTH) != 0) || (select_param->uid_length == 0))
        {
            /* uid_length must be multiple of 4 bytes and must not be 0 */
            status = PH_ERR_BFL_INVALID_PARAMETER;
        }
        else
        {
            for(i=0; i<(select_param->uid_length/SINGLE_UID_LENGTH); i++)
            {
                switch(i)
                {
                    case 1:
                        casc_code = SELECT_CASCADE_LEVEL_2;
                        break;
                    case 2:
                        casc_code = SELECT_CASCADE_LEVEL_3;
                        break;
                    default:
                        casc_code = SELECT_CASCADE_LEVEL_1;
                        break;
                }

                status = phcsBflI3P3A_Hw1IntSelect(((phcsBflI3P3A_t*)(select_param->self)),
                                                   p_lower,
                                                   casc_code,
                                                   select_param->uid + i * 4,
                                                   &select_param->sak);

                /* check if response is correct */
                if(status != PH_ERR_BFL_SUCCESS)
                {
                    i = MAX_CASCADE_LEVELS;
                    continue;
                }
            }
        }
    }
    else
    {
        /*
         * Device is Target: The Select is invalid (use StartIsoActivation instead of individual commands)
         */
        status = PH_ERR_BFL_INVALID_DEVICE_STATE;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_MFRD)
    return status;
}

/* Command and Parameter byte definitions for HaltA function. */
#define HALTA_CMD           0x50
#define HALTA_PARAM         0x00
#define HALTA_CMD_LENGTH    0x02

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P3A_Hw1HaltA(phcsBflI3P3A_HaltAParam_t *halt_a_param)
{
    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM  status;
    phcsBflRegCtl_t     PHCS_BFL_MEMLOC_REM *p_lower  = (phcsBflRegCtl_t*)(((phcsBflI3P3A_t*)(halt_a_param->self))->mp_Lower);

    phcsBflRegCtl_ModRegParam_t  PHCS_BFL_MEMLOC_REM  modify_param = {0};
    modify_param.self = p_lower;

    if (((phcsBflI3P3A_Hw1Params_t*)(((phcsBflI3P3A_t*)(halt_a_param->self))->mp_Members))->m_InitiatorNotTarget == PHCS_BFLI3P3A_INITIATOR)
    {
        /*
         * Device is Initiator: We do a "True" Select cycle:
         */
        uint8_t                     PHCS_BFL_MEMLOC_BUF int_buffer[2];
        phcsBflAux_CommandParam_t   PHCS_BFL_MEMLOC_REM command_param;
        command_param.lower         = p_lower;
        command_param.wait_event_cb = ((phcsBflI3P3A_t*)(halt_a_param->self))->mp_WaitEventCB;
        command_param.user_ref      = ((phcsBflI3P3A_t*)(halt_a_param->self))->mp_UserRef;

        /* Deactivate DetectSync functionality of the hardware */
        modify_param.address = PHCS_BFL_JREG_MODE;
        modify_param.mask = PHCS_BFL_JBIT_DETECTSYNC;
        modify_param.set = 0;
        status = p_lower->ModifyRegister(&modify_param);

        if(status == PH_ERR_BFL_SUCCESS)
        {
            /* initialise data buffer */
            int_buffer[0] = HALTA_CMD;
            int_buffer[1] = HALTA_PARAM;

            /* set single command parameter */
            command_param.initiator = PHCS_BFLAUX_INITIATOR;
            command_param.cmd = PHCS_BFL_JCMD_TRANSCEIVE;
            command_param.tx_bytes = HALTA_CMD_LENGTH;
            command_param.tx_buffer = int_buffer;
            command_param.rx_buffer = int_buffer;
            command_param.rx_bytes = 0;

            if(((status = phcsBflAux_Hw1Command(&command_param)) & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_IO_TIMEOUT)
            {
                status = PH_ERR_BFL_SUCCESS;
            }
        }
    }
    else
    {
        /*
         * Device is Target:
         * The incoming data steam is checked and if according to Halt command, the PN51x's AutoColl command is started.
         */
        if( (halt_a_param->buffer_length == HALTA_CMD_LENGTH) &&
            (halt_a_param->buffer[0] == HALTA_CMD) &&
            (halt_a_param->buffer[1] == HALTA_PARAM) )
        {
            phcsBflRegCtl_SetRegParam_t  PHCS_BFL_MEMLOC_REM  setreg_param;
            setreg_param.self = p_lower;

            /* Set PN51x hardware to Halt mode (response only to WUPA). */
            modify_param.address = PHCS_BFL_JREG_MIFARE;
            modify_param.mask    = PHCS_BFL_JBIT_MFHALTED;
            modify_param.set     = 1;
            status = p_lower->ModifyRegister(&modify_param);

            if(status == PH_ERR_BFL_SUCCESS)
            {
                /* Deactivate DetectSync functionality of the hardware */
                modify_param.address = PHCS_BFL_JREG_MODE;
                modify_param.mask = PHCS_BFL_JBIT_DETECTSYNC;
                modify_param.set = 0;
                status = p_lower->ModifyRegister(&modify_param);
            }

            if(status == PH_ERR_BFL_SUCCESS)
            {
                /* Activate Automatic Collision mode again. */
                setreg_param.address  = PHCS_BFL_JREG_COMMAND;
                setreg_param.reg_data = PHCS_BFL_JCMD_AUTOCOLL;
                status = p_lower->SetRegister(&setreg_param);
            }
        }
        else
        {
            /* Data is not according to Halt, calling function has to handle data. */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_MFRD)
    return status;
}


/* Definitions for lower Anticollision / Select functions. */
#define BITS_PER_BYTE       0x08
#define UPPER_NIBBLE_SHIFT  0x04
#define COMPLETE_UID_BITS   0x28
#define NVB_MIN_PARAMETER   0x20
#define NVB_MAX_PARAMETER   0x70
#define MAX_LOOP_NUM        32
phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P3A_Hw1CascAnticoll(phcsBflI3P3A_t *cif,
        phcsBflRegCtl_t *p_lower,
        uint8_t sel_code,
        uint8_t bitcount,
        uint8_t *snr,
        uint8_t *flags)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status;
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM istatus = PH_ERR_BFL_SUCCESS;

    phcsBflAux_CommandParam_t PHCS_BFL_MEMLOC_REM command_param;

    phcsBflRegCtl_SetRegParam_t      PHCS_BFL_MEMLOC_REM setreg_param;
    phcsBflRegCtl_ModRegParam_t      PHCS_BFL_MEMLOC_REM modify_param;

    uint8_t PHCS_BFL_MEMLOC_COUNT   i;
    uint8_t PHCS_BFL_MEMLOC_REM     complete = 0; /* signs end of anticollision loop */
    uint8_t PHCS_BFL_MEMLOC_REM     rbits;        /* number of total received bits */
    uint8_t PHCS_BFL_MEMLOC_REM     nbits;        /* */
    uint8_t PHCS_BFL_MEMLOC_REM     nbytes;       /* */
    uint8_t PHCS_BFL_MEMLOC_REM     byteOffset;   /* stores offset for ID copy if uncomplete last byte was sent */
    uint8_t PHCS_BFL_MEMLOC_BUF     int_buffer[7];    /* internal IO buffer for communication */
    uint_fast8_t  loop = 0;

    command_param.lower = p_lower;
    command_param.wait_event_cb  = cif->mp_WaitEventCB;
    command_param.user_ref       = cif->mp_UserRef;

    /* self is set only once. Not modified during operation!! */
    setreg_param.self = p_lower;
    modify_param.self = p_lower;


    /* disable TxCRC and RxCRC */
    modify_param.set = 0;                   /* clear masked bits */
    modify_param.address = PHCS_BFL_JREG_TXMODE;     /* TxCRC */
    modify_param.mask = PHCS_BFL_JBIT_CRCEN;
    PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))

    modify_param.address = PHCS_BFL_JREG_RXMODE;     /* RxCRC */
    PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))   /* keep .set and .mask parameter like it from above */

    /* activate deletion of bits after coll */
    setreg_param.address = PHCS_BFL_JREG_COLL;
    setreg_param.reg_data = 0x00;
    PHCS_BFLAUX_CHECK_SUCCESS(p_lower->SetRegister(&setreg_param))

    /* init parameters for anticollision */
    while((complete == 0) && (istatus == PH_ERR_BFL_SUCCESS))
    {
        /* if there is a communication problem on the RF interface, bcnt
           could be larger than 32 - folowing loops will be defective. */
        if(bitcount > SINGLE_UID_LENGTH)
        {
            yyf_debug("bitcount%u > 32", bitcount);
            istatus = PH_ERR_BFL_INVALID_PARAMETER;
            continue;
        }

        if (++loop > MAX_LOOP_NUM)
        {
            yyf_debug("loop%u\n", loop);
            istatus = PH_ERR_BFL_INVALID_PARAMETER;
            continue;
        }

        //yyf_debug("bitcnt=%u\n", bitcount);
        /* prepare data length */
        nbits = (uint8_t)(bitcount % BITS_PER_BYTE);
        nbytes = (uint8_t)(bitcount / BITS_PER_BYTE);
        if(nbits != 0)
        {
            nbytes++;
        }

        /* prepare data buffer */
        int_buffer[0] = sel_code;
        int_buffer[1] = (uint8_t)(NVB_MIN_PARAMETER + ((bitcount / BITS_PER_BYTE) << UPPER_NIBBLE_SHIFT) + nbits);
        if (int_buffer[0] != 0x93)
        {
            yyf_debug("sel_code:%02X %02X\n", int_buffer[0], int_buffer[1]);
        }
        for(i=0; i<nbytes; i++)
        {
            int_buffer[2+i] = snr[i];   /* copy serial number to tranmit buffer */
        }

        /* set TxLastBits and RxAlign to number of bits sent */
        setreg_param.address = PHCS_BFL_JREG_BITFRAMING;
        setreg_param.reg_data = (uint8_t)((nbits << UPPER_NIBBLE_SHIFT) | nbits);
        PHCS_BFLAUX_CHECK_SUCCESS(p_lower->SetRegister(&setreg_param))

        /* prepare data for common transceive */
        command_param.initiator = PHCS_BFLAUX_INITIATOR;
        command_param.cmd = PHCS_BFL_JCMD_TRANSCEIVE;
        command_param.tx_bytes = (uint8_t)(nbytes + 2);
        command_param.tx_buffer = int_buffer;
        command_param.rx_buffer = int_buffer + 2;
        command_param.rx_bytes = 0x05;

        istatus = phcsBflAux_Hw1Command(&command_param);

        if ((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
        {
            /* Specify that a collision has been occurred, the collision will of course be
               resolved, it just provides additional information for the caller */
            *flags |= 0x01;
        }
        else
        {
        }

        if(((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR) || (istatus == PH_ERR_BFL_SUCCESS))
        {
            /* store number of received data bits and bytes internaly */
            rbits = (uint8_t)((command_param.rx_bits + (command_param.rx_bytes << 3)) - nbits);
            if (rbits != 40)
            {
                yyf_debug("rbits:%u\n", rbits);
            }
            if((rbits + bitcount) > COMPLETE_UID_BITS)
            {
                yyf_debug("BITCOUNT_ERROR %u %u\n", rbits, bitcount);
                istatus = PH_ERR_BFL_BITCOUNT_ERROR;
                continue;
            }

            /* increment number of bytes received if also some bits received */
            if(command_param.rx_bits != 0)
            {
                command_param.rx_bytes++;
            }

            /* reset offset for data copying */
            byteOffset = 0;
            /* if number of bits sent are not 0, write first received byte in last of sent */
            if(nbits != 0)
            {
                /* last byte transmitted and first byte received are the same */
                snr[nbytes - 1] |= command_param.rx_buffer[0];
                byteOffset++;
            }

            for(i=0; i<(4-nbytes); i++)
            {
                snr[nbytes + i] = command_param.rx_buffer[i + byteOffset];
            }

            if((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
            {
                /* calculate new bitcount value */
                bitcount = (uint8_t)(bitcount + rbits);
                istatus = PH_ERR_BFL_SUCCESS;
            }
            else
            {
                if((snr[0] ^ snr[1] ^ snr[2] ^ snr[3]) != command_param.rx_buffer[i + byteOffset])
                {
                    istatus = PH_ERR_BFL_WRONG_UID_CHECKBYTE;
                    continue;
                }
                else
                {
                    complete=1;
                }
            }
        }
    }

    /* clear RxAlign and TxLastbits */
    setreg_param.address    = PHCS_BFL_JREG_BITFRAMING;
    setreg_param.reg_data   = 0x00;
    PHCS_BFLAUX_CHECK_SUCCESS(p_lower->SetRegister(&setreg_param))

    /* activate values after coll */
    setreg_param.address    = PHCS_BFL_JREG_COLL;
    setreg_param.reg_data   = PHCS_BFL_JBIT_VALUESAFTERCOLL;
    PHCS_BFLAUX_CHECK_SUCCESS(p_lower->SetRegister(&setreg_param))

    PHCS_BFL_ADD_COMPCODE(istatus, PH_ERR_BFL_MFRD)
    return status;
}

/* Definitions for Select functions. */
#define SAK_LENGTH  0x01

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P3A_Hw1IntSelect(phcsBflI3P3A_t *cif,
        phcsBflRegCtl_t      *p_lower,
        uint8_t               sel_code,
        uint8_t              *snr,
        uint8_t              *sak)
{
    phcsBfl_Status_t                 PHCS_BFL_MEMLOC_REM status;

    phcsBflAux_CommandParam_t        PHCS_BFL_MEMLOC_REM command_param;
    phcsBflRegCtl_ModRegParam_t      PHCS_BFL_MEMLOC_REM modify_param;

    /* define local variables */
    uint8_t              PHCS_BFL_MEMLOC_COUNT  i;
    uint8_t              PHCS_BFL_MEMLOC_BUF    int_buffer[7];

    /* self is set only once. Not modified during operation!! */
    command_param.lower          = p_lower;
    command_param.wait_event_cb  = cif->mp_WaitEventCB;
    command_param.user_ref       = cif->mp_UserRef;
    modify_param.self            = p_lower;

    /* activate CRC */
    modify_param.set = 1;
    modify_param.address = PHCS_BFL_JREG_TXMODE;
    modify_param.mask = PHCS_BFL_JBIT_CRCEN;
    PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))

    modify_param.address = PHCS_BFL_JREG_RXMODE;
    PHCS_BFLAUX_CHECK_SUCCESS(p_lower->ModifyRegister(&modify_param))

    /* prepare data stream */
    int_buffer[0] = sel_code;   /* command code */
    int_buffer[1] = NVB_MAX_PARAMETER;       /* parameter */
    for (i=0; i<4; i++)
    {
        int_buffer[2+i] = snr[i];   /* serial numbner bytes 1 to 4 */
    }
    int_buffer[6] = (uint8_t)(snr[0] ^ snr[1] ^ snr[2] ^ snr[3]);   /* serial number check byte */

    /* prepare data to send */
    command_param.initiator = PHCS_BFLAUX_INITIATOR;
    command_param.cmd = PHCS_BFL_JCMD_TRANSCEIVE;
    command_param.tx_bytes = 0x07;       /* transmit complete ID */
    command_param.tx_buffer = int_buffer;    /* initialised data buffer with command, param and ID */
    command_param.rx_bytes = 0x01;       /* expect only 1 byte (SAK) */
    command_param.rx_buffer = int_buffer;    /* use same buffer for reception of SAK */
    status = phcsBflAux_Hw1Command(&command_param);

    if(status == PH_ERR_BFL_SUCCESS)
    {
        if((command_param.rx_bytes == SAK_LENGTH) && (command_param.rx_bits == 0))
        {
            *sak = command_param.rx_buffer[0];
        }
        else
        {
            status = PH_ERR_BFL_PROTOCOL_ERROR;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_MFRD)
    return status;
}


/* E.O.F. */
