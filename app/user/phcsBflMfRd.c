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

/*! \file phcsBflMfRd.c
 *
 * Project: Object Oriented Library Framework Mifare Component.
 *
 *  Source: phcsBflMfRd.c
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 23 07:21:00 2006 $
 *
 * Comment:
 *  Mifare implementation.
 *
 * History:
 *  GK:  Generated 12. March 2003
 *  MHa: Migrated to MoReUse September 2005
 *
 */

#include <phcsBflMfRd.h>
#include "phcsBflMfRd_Int.h"


void ICACHE_FLASH_ATTR
phcsBflMfRd_Init(phcsBflMfRd_t  *cif,
                    void         *mp,
                    phcsBflIo_t    *p_lower,
                    uint8_t      *p_trxbuffer)
{
    /* Glue together and init the operation parameters: */
    ((phcsBflMfRd_InternalParam_t*)mp)->m_TrxBuffer = p_trxbuffer;
    cif->mp_Members                      = mp;
    cif->mp_Lower                        = p_lower;
    /* Initialize the function pointers: */
    cif->Transaction                     = phcsBflMfRd_Transaction;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflMfRd_GenericSingle(phcsBflMfRd_CommandParam_t *cmd_param)
{
    phcsBfl_Status_t            PHCS_BFL_MEMLOC_REM  status;
    phcsBflMfRd_InternalParam_t PHCS_BFL_MEMLOC_REM *intl_params = (phcsBflMfRd_InternalParam_t*) ((phcsBflMfRd_t*)cmd_param->self)->mp_Members;
    phcsBflIo_TransceiveParam_t PHCS_BFL_MEMLOC_REM  transceive_param;
    uint8_t         PHCS_BFL_MEMLOC_COUNT   index       = 0;
    uint8_t         PHCS_BFL_MEMLOC_COUNT   data_count;
    uint8_t         PHCS_BFL_MEMLOC_REM     rec_ack;

    /* Compose Mifare command if desired: */
    if (cmd_param->cmd != PHCS_BFLMFRD_NOCOMMAND)
    {
        intl_params->m_TrxBuffer[index] = cmd_param->cmd;
        index++;
        intl_params->m_TrxBuffer[index] = cmd_param->addr;
        index++;
    }

    /* Append buffer: */
    if (cmd_param->buffer_length <= PHCS_BFLMFRD_STD_BLOCK_SIZE)
    {
        if ((cmd_param->buffer_length != 0) &&
            (cmd_param->buffer != NULL))
        {
            for (data_count = 0; data_count < cmd_param->buffer_length; data_count++)
            {
                intl_params->m_TrxBuffer[index] = cmd_param->buffer[data_count];
                index++;
            }
        }
    } else
    {
        /* Error: Too many bytes: Premature return: */
        cmd_param->buffer_length = 0;
        return PH_ERR_BFL_BUFFER_OVERFLOW | PH_ERR_BFL_MFRD;
    }

    if (index == 0)
    {
        /* Premature return because of no data to send: */
        cmd_param->buffer_length = 0;
        return PH_ERR_BFL_BUF_2_SMALL | PH_ERR_BFL_MFRD;
    }

    /* Transceive: */
    transceive_param.tx_buffer      = intl_params->m_TrxBuffer;
    transceive_param.tx_buffer_size = index;
    transceive_param.rx_buffer      = intl_params->m_TrxBuffer;
    transceive_param.rx_buffer_size = PHCS_BFLMFRD_MAX_DATABUF_SIZE;
    transceive_param.self           = ((phcsBflMfRd_t*)cmd_param->self)->mp_Lower;
    status = ((phcsBflMfRd_t*)cmd_param->self)->mp_Lower->Transceive(&transceive_param);
    
    /*
     * Evaluate reply & status: These stati are returned by I/O Transceive when handling MF and
     * there has most likely been no error (will be checked):
     */
    if (status == PH_ERR_BFL_SUCCESS)
    {
        if ((cmd_param->reply_type & PHCS_BFLMFRD_EXPECT_DATA) != 0)
        {
            /* Data expected and received correctly. */
            if (transceive_param.rx_buffer_size <= PHCS_BFLMFRD_STD_BLOCK_SIZE)
            {
                for (data_count = 0; data_count < transceive_param.rx_buffer_size; data_count++)
                {
                    cmd_param->buffer[data_count] = transceive_param.rx_buffer[data_count];
                }
                cmd_param->buffer_length = (uint8_t)transceive_param.rx_buffer_size;
                /* Status SUCCESS set correctly: */
            } else
            {
                /* Error: More bytes returned than suitable for MF: */
                cmd_param->buffer_length = 0;
                status = PH_ERR_BFL_BUF_2_SMALL;
            }
        } else
        {
            /* Error: We expected something else: */
            cmd_param->buffer_length = 0;
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    }
    else
    if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_ACK_SUPPOSED)
    {
        if ((cmd_param->reply_type & PHCS_BFLMFRD_EXPECT_ACK) != 0)
        {
            /* We don't copy back data: */
            rec_ack = transceive_param.rx_buffer[0];
            cmd_param->buffer_length = 0;
            /* Upper nibble content of (N)ACK must be either 0 or equal to lower nibble: */
            if (((rec_ack >> 4) == 0x00) ||
                ((rec_ack >> 4) == (rec_ack & (uint8_t)0x0F)))
            {
                /* ACK or NACK ? */
                if ((rec_ack & PHCS_BFLMFRD_ACK_MASK) != 0)
                {
                    /* This is an ACK! */
                    status = PH_ERR_BFL_SUCCESS;
                } else 
                {
                    /* Bah - we got a NACK, refine error: */
                    if ( (((rec_ack & (uint8_t)0x01)) != 0x00) || ((rec_ack & (uint8_t)0x04) != 0x00))
                    {
                        /* A CRC or an "Invalid Transfer Buffer" error is the cause: */
                        status = PH_ERR_BFL_NACK_RECEIVED;
                    } else
                    {
						/* This shouldn't occur anyway (undefined NACK frame) */
                        status = PH_ERR_BFL_INVALID_FORMAT;
                    }
                }
            } else
            {
                /* Overall format NOK: */
                status = PH_ERR_BFL_INVALID_FORMAT;
            }
        } else
        {
            /* in any case we got something wrong, set return buffer length to 0 */
            cmd_param->buffer_length = 0;
            rec_ack = transceive_param.rx_buffer[0];
            /* check if an NACK has been received */
            if(((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_ACK_SUPPOSED) && 
                ( ((rec_ack & (uint8_t)0x01) != 0x00) || ((rec_ack & (uint8_t)0x04) != 0x00)))
            {
                /* Bah - we got a NACK, refine error: */
                status = PH_ERR_BFL_NACK_RECEIVED;
            } else
            {
                /* Error: We expected something else: */
                status = PH_ERR_BFL_INVALID_FORMAT;
            }
        }
    }
    else
    if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_IO_TIMEOUT)
    {
        if ((cmd_param->reply_type & PHCS_BFLMFRD_EXPECT_TIMEOUT) != 0)
        {
            /* Timeout expected and received correctly. */
            cmd_param->buffer_length = 0;
            status = PH_ERR_BFL_SUCCESS;
        } else
        {
            /* Error: We expected something else - can return timeout: */
            cmd_param->buffer_length = 0;
        }
    } else
    {
        /* Error: Merely return the status: */
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_MFRD)
    return status;
    #undef rec__ack /* */
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflMfRd_GenericDouble(phcsBflMfRd_CommandParam_t *double_cmd_param)
{
    phcsBfl_Status_t            PHCS_BFL_MEMLOC_REM status;
    phcsBflMfRd_CommandParam_t  PHCS_BFL_MEMLOC_REM single_cmd_param;

    /* First Pass: An ACK is expected to be returned: */
    single_cmd_param.self           = double_cmd_param->self;
    single_cmd_param.buffer         = NULL;
    single_cmd_param.addr           = double_cmd_param->addr;
    single_cmd_param.buffer_length  = 0;
    single_cmd_param.cmd            = double_cmd_param->cmd;
    single_cmd_param.reply_type     = PHCS_BFLMFRD_EXPECT_ACK;
    status = phcsBflMfRd_GenericSingle(&single_cmd_param);
    if (status == PH_ERR_BFL_SUCCESS)
    {
        /* 1st pass OK, step to 2nd pass: */
        single_cmd_param.buffer         = double_cmd_param->buffer;
        single_cmd_param.buffer_length  = double_cmd_param->buffer_length;
        single_cmd_param.self           = double_cmd_param->self;
        single_cmd_param.addr           = 0;
        single_cmd_param.cmd            = PHCS_BFLMFRD_NOCOMMAND;
        single_cmd_param.reply_type     = double_cmd_param->reply_type;
        status = phcsBflMfRd_GenericSingle(&single_cmd_param);
        /* This function yields no data: */
        double_cmd_param->buffer_length = 0;
    } else
    {
        /* Error, no data return. */
        double_cmd_param->buffer_length = 0;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_MFRD)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflMfRd_Transaction(phcsBflMfRd_CommandParam_t *cmd_param)
{
    #define DUMMY_LENGTH                                4                     /* */
    #define MF_KEY_LENGTH                               6                     /* */
    #define MF_SNR_LENGTH                               4                     /* */
    
    phcsBfl_Status_t          PHCS_BFL_MEMLOC_REM   status;
    uint8_t                   PHCS_BFL_MEMLOC_BUF   int_dummy[DUMMY_LENGTH]             = { 0, 0, 0, 0 };
    phcsBflIo_TransmitParam_t PHCS_BFL_MEMLOC_REM   transmit_param;
    uint8_t                   PHCS_BFL_MEMLOC_COUNT index                           = 0;
    uint8_t                   PHCS_BFL_MEMLOC_COUNT loop_count;
    uint8_t                   PHCS_BFL_MEMLOC_REM   *xch_buffer;

    switch (cmd_param->cmd)
    {
        case PHCS_BFLMFRD_AUTHENT_A:
        case PHCS_BFLMFRD_AUTHENT_B:
            /*
             * Hand over the AUTHENT to the RCIO Authent facility, since it is a hardware-
             * depending command.
             * Format: CMD - ADDR - KEY[0]..KEY[5] - SNR[0]..SNR[3].
             */
            xch_buffer = ((phcsBflMfRd_InternalParam_t*)
                            (((phcsBflMfRd_t*)(cmd_param->self))->mp_Members))->m_TrxBuffer;
            xch_buffer[index] = cmd_param->cmd;
            index++;
            xch_buffer[index] = cmd_param->addr;
            index++;
            for (loop_count = 0; loop_count < (MF_KEY_LENGTH + MF_SNR_LENGTH); loop_count++)
            {
                xch_buffer[index] = (cmd_param->buffer)[loop_count];
                index++;
            }
            transmit_param.tx_buffer      = xch_buffer;
            transmit_param.tx_buffer_size = index;
            transmit_param.self           = ((phcsBflMfRd_t*)(cmd_param->self))->mp_Lower;
            status = ((phcsBflMfRd_t*)cmd_param->self)->mp_Lower->MfAuthent(&transmit_param);
            break;

        case PHCS_BFLMFRD_READ:
            cmd_param->buffer_length = 0;
            cmd_param->reply_type    = PHCS_BFLMFRD_EXPECT_DATA;
            status = phcsBflMfRd_GenericSingle(cmd_param);
            break;

        case PHCS_BFLMFRD_WRITE:
            if (cmd_param->buffer_length == PHCS_BFLMFRD_STD_BLOCK_SIZE)
            {
                cmd_param->reply_type = PHCS_BFLMFRD_EXPECT_ACK;
                status = phcsBflMfRd_GenericDouble(cmd_param);
            } else
            {
                /*
                 * We propagate INVALID_P. since only the length equal to the size of a MF
                 * block is valid.
                 */
                status = PH_ERR_BFL_INVALID_PARAMETER;
            }
            break;

        case PHCS_BFLMFRD_WRITE4:
            if (cmd_param->buffer_length == PHCS_BFLMFRD_UL_PAGE_SIZE)
            {
                cmd_param->reply_type = PHCS_BFLMFRD_EXPECT_ACK;
                status = phcsBflMfRd_GenericSingle(cmd_param);
            } else
            {
                /*
                 * We propagate INVALID_P. since only the length equal to the size of a MF
                 * block is valid.
                 */
                status = PH_ERR_BFL_INVALID_PARAMETER;
            }
            break;

        case PHCS_BFLMFRD_INCREMENT:
            if (cmd_param->buffer_length == PHCS_BFLMFRD_VALUE_DATA_SIZE)
            {
                cmd_param->reply_type = PHCS_BFLMFRD_EXPECT_TIMEOUT;
                status = phcsBflMfRd_GenericDouble(cmd_param);
            } else
            {
                /*
                 * We propagate INVALID_P. since only the length equal to the size of a MF
                 * value block is valid.
                 */
                status = PH_ERR_BFL_INVALID_PARAMETER;
            }
            break;

        case PHCS_BFLMFRD_DECREMENT:
            if (cmd_param->buffer_length == PHCS_BFLMFRD_VALUE_DATA_SIZE)
            {
                cmd_param->reply_type = PHCS_BFLMFRD_EXPECT_TIMEOUT | PHCS_BFLMFRD_EXPECT_ACK;
                status = phcsBflMfRd_GenericDouble(cmd_param);
            } else
            {
                /*
                 * We propagate INVALID_P. since only the length equal to the size of a MF
                 * value block is valid.
                 */
                status = PH_ERR_BFL_INVALID_PARAMETER;
            }
            break;

        case PHCS_BFLMFRD_RESTORE:
            cmd_param->reply_type    = PHCS_BFLMFRD_EXPECT_TIMEOUT;
            cmd_param->buffer_length = DUMMY_LENGTH;
            cmd_param->buffer        = int_dummy;
            status = phcsBflMfRd_GenericDouble(cmd_param);
            cmd_param->buffer_length = 0;
            cmd_param->buffer        = NULL;
            break;

        case PHCS_BFLMFRD_TRANSFER:
            cmd_param->reply_type = PHCS_BFLMFRD_EXPECT_ACK;
            cmd_param->buffer_length = 0;
            status = phcsBflMfRd_GenericSingle(cmd_param);
            break;

        default:
            /* Unknown command: */
            cmd_param->buffer_length = 0;
            status = PH_ERR_BFL_UNSUPPORTED_COMMAND;
            break;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_MFRD)
    return status;
    #undef DUMMY_LENGTH  /* */
    #undef MF_KEY_LENGTH /* */
}


/* E.O.F. */
