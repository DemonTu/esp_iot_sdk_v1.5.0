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

/*! \file phcsBflNfc_Initiator.c
 *
 * Project: NFC Initiator basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflNfc_Initiator.c
 * $Author: bs $
 * $Revision: 1.6 $
 * $Date: Fri Jun 30 09:49:19 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated 25. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 *
 */

#include <phcsBflNfc.h>
#include "phcsBflNfc_InitiatorInt.h"
#include <phcsBflNfcCommon.h>
#include "phcsBflNfc_CommonInt.h"

#define LRI_TABLE_LEN  4  /* */
/*
 *  Table for max. valid length of Transport data:
 */
const uint16_t PHCS_BFL_MEMLOC_BUF gkphcsBflNfc_LRi[LRI_TABLE_LEN] = { 64, 128, 192, 255 };

/*
 *  Define to check if command bytes of target response are valid. */
#define RESPONSE_HEADER_INVALID(cmd0, cmd1, td)                    \
                      (((cmd0) ^ ((td)->mp_TRxBuffer[NFC_CMD0_POS])) ||  \
                       ((cmd1) ^ ((td)->mp_TRxBuffer[NFC_CMD1_POS])))    /* */

/*
 *  Definition used by DEP_REQ to handle an invalid response from the Target.
 */
#define HANDLE_INVALID_RESPONSE(err_state_)               \
        if (((err_state_) & NFC_CURRENT_ERROR_MASK) != NFC_DEP_STATE_ATE)    \
        {                                                 \
            (err_state_) <<= 4;                           \
            (err_state_) |= NFC_DEP_STATE_NAS;            \
        }                                             /* */

/*
 *  Usually, a timeout is followed by an Attention-Request,
 *  except a NACK has been sent before!                    */
#define HANDLE_RESPONSE_TIMEOUT(err_state_)               \
        if (((err_state_) & NFC_CURRENT_ERROR_MASK) == NFC_DEP_STATE_NAE)      \
        {                                                 \
            (err_state_) <<= 4;                           \
            (err_state_) |= NFC_DEP_STATE_NAS;            \
        } else                                            \
        {                                                 \
            (err_state_) <<= 4;                           \
            (err_state_) |= NFC_DEP_STATE_ATS;            \
        }                                             /* */



/* Implementation of"Private" functions, visible from inside this file only:                      */

static phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorCheckOptDidNad(uint8_t *trx_buffer,
                                                       uint8_t  tx_size,
                                                       uint8_t  min_res_length,
                                                       uint8_t  did_i,
                                                       uint8_t *nad_i)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;

    /* OK, receive size is the same as the size of send buffer! */
    /* Applies to */
    if (tx_size > min_res_length)
    {
        if (tx_size == min_res_length + 1)
        {
            /* Only one optional byte was sent. */
            if (did_i == 0)
            {
                /* NAD was sent. */
                if (nad_i != NULL)
                {
                    if (*nad_i != trx_buffer[min_res_length]) status = PH_ERR_BFL_INVALID_FORMAT;
                } else
                {
                    status = PH_ERR_BFL_INVALID_FORMAT;
                }
            } else
            {
                /* DID was sent. */
                if (did_i != trx_buffer[min_res_length]) status = PH_ERR_BFL_INVALID_FORMAT;
            }
        } else if (tx_size == min_res_length + 2)
        {
            /* Both optional bytes were sent. */
            if ((did_i != trx_buffer[min_res_length]) || (*nad_i != trx_buffer[min_res_length + 1]))
            {
                status = PH_ERR_BFL_INVALID_FORMAT;
            }
        } else
        {
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorCheckDepDid(uint8_t *trx_buffer, uint8_t  did_i)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;

    if (did_i != 0)
    {
        /* Check PFB: */
        if ((trx_buffer[NFC_DEP_PFB_POS] & NFC_DEP_DID_PRESENT) == 0)
        {
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
        /* Check if DID of Initiator is equal to the DID of the Target. */
        if (did_i != trx_buffer[NFC_DEP_DID_POS])
        {
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* If no DID was used by the Initiator, the Target must not use DID! */
        if (trx_buffer[NFC_DEP_PFB_POS] & NFC_DEP_DID_PRESENT)
        {
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorCheckDepNad(phcsBflNfc_InitiatorCommParams_t *td,
                                             uint8_t                        *trx_buffer,
                                             uint8_t                        *nad)
{
    phcsBfl_Status_t       PHCS_BFL_MEMLOC_REM status   = PH_ERR_BFL_SUCCESS;
    uint8_t   PHCS_BFL_MEMLOC_REM nad_pos;
    uint8_t   PHCS_BFL_MEMLOC_REM pfb      = trx_buffer[NFC_DEP_PFB_POS];

    if ((td->m_NadUsed) && (nad != NULL))
    {
        /* NAD is used during DEP. */
        if ((pfb & NFC_PFB_7_5_MASK) == NFC_DEP_INFORMATION)
        {
            if (td->m_CtrlWord & NFC_CTRL_NAD_PRESENT)
            {
                /* Yes, we expect a (correct) NAD! Check this: */
                nad_pos = NFC_DEP_PFB_POS + 1;
                if (td->m_DID)
                {
                    /* DID is present, NAD is one pos. after DID. */
                    nad_pos++;
                }

                if (pfb & NFC_PFB_NAD_BIT_SET)
                {
                    /* If NAD differs from original one overwrite parameter in DEP
                        structure to make the NAD also available outside the protocol stack. */
                    if (*nad != trx_buffer[nad_pos])
                    {
                        *nad = trx_buffer[nad_pos];
                    }
                    /* Reset NAD present bit: */
                    td->m_CtrlWord &= NFC_CTRL_NAD_PRESENT_I;
                } else
                {
                    /*
                     * Error: The Target should have answered with a NAD included,
                     * but we don't find one.
                     */
                    status = PH_ERR_BFL_INVALID_FORMAT;
                }
            } else
            {
                if (pfb & NFC_PFB_NAD_BIT_SET)
                {
                    /*
                     * If a NAD is nevertheless received in the PDU it is an error,
                     * the Target misbehaves (NAD must only be sent with the first INF block).
                     */
                    status = PH_ERR_BFL_INVALID_FORMAT;
                } else
                {
                    /* OK, no NAD set ! */
                }
            }
        }
    } else
    {
        if (td->m_CtrlWord & NFC_CTRL_NAD_PRESENT)
        {
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorCheckDid(uint8_t res_length,
                                                 uint8_t rx_size,
                                                 uint8_t did_i,
                                                 uint8_t did_t)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;

    if (res_length == rx_size)
    {
        /* OK, correct number of bytes receive ! */
        if (did_i == did_t)
        {
            /* DID of target matches, no action required! */
        } else
        {
            /* DID of target is not the same as in the request! */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* The frame returned is invalid! */
        status = PH_ERR_BFL_INVALID_FORMAT;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static uint8_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorCheckPfb(uint8_t pfb)
{
    switch (pfb & NFC_PFB_7_5_MASK)
    {
        case NFC_DEP_ACKNOWLEDGE:
            {
                /* PFB specifies a ACK/NACK pdu! */
                if (pfb & NFC_DEP_NACK)
                {
                    /* NACK frame! */
                    return NFC_NACK;
                } else
                {
                    /* ACK frame! */
                    return NFC_ACK;
                }
               // break;
            }
        case NFC_DEP_SUPERVISORY:
            {
                /* PFB specifies a Attention/TimeoutExtension pdu! */
                if (pfb & NFC_DEP_TO_EXT)
                {
                    /* Timeout Extension frame! */
                    return NFC_TO_EXT;
                } else
                {
                    /* Attention frame! */
                    return NFC_ATT;
                }
              //  break;
            }
        case NFC_DEP_INFORMATION:
            {
                return NFC_INF;
               // break;
            }
        default:
            {
                return NFC_ANY;
              //  break;
            }
    }
}


static phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorCheckPni(uint8_t pfb_i, uint8_t pfb_t)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;

    if ((pfb_i & (NFC_DEP_PNI_MASK)) ^ (pfb_t & (NFC_DEP_PNI_MASK)))
    {
        /* PNI sent and PNI received are different. */
        status = PH_ERR_BFL_INVALID_FORMAT;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorTRxAux(phcsBflNfc_Initiator_t *initiator_object,
                                               uint16_t             tx_buffer_size,
                                               uint16_t            *rx_buffer_size)
{
    phcsBfl_Status_t             PHCS_BFL_MEMLOC_REM  status;
    phcsBflIo_TransceiveParam_t PHCS_BFL_MEMLOC_REM  transceive_param;
    uint8_t         PHCS_BFL_MEMLOC_REM  tx_preamble_length;
    uint8_t         PHCS_BFL_MEMLOC_BUF   *trx_buffer;
    uint16_t        PHCS_BFL_MEMLOC_COUNT   loopcount;
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td = (phcsBflNfc_InitiatorCommParams_t*) initiator_object->mp_Members;


    /* Re-use tx_preamble_length for RX (after tx is done !!!) : */
    #define rx__preamble__length tx_preamble_length     /* */

    trx_buffer = td->mp_TRxBuffer;

    /* Move the buffer to [tx_preamble_length] in order to make the preamble fit: */
    tx_preamble_length = (uint8_t)(td->m_TXPreambleType >> 4);
    if ((tx_preamble_length > 0) && (tx_buffer_size > 1))
    {
        loopcount = tx_buffer_size;
        do
        {
            loopcount--;
            trx_buffer[loopcount + tx_preamble_length] = trx_buffer[loopcount];

        } while (loopcount);

        /* Fill in TX preamble: */
        switch (td->m_TXPreambleType)
        {
            case PHCS_BFLNFCCOMMON_PREAM_TYPE1:
                trx_buffer[0] = PHCS_BFLNFCCOMMON_PREAM_1BYTE;
                break;

            case PHCS_BFLNFCCOMMON_PREAM_OMIT:
                /* Intended fallthru: */
            default:
                /* No action required. */
                break;
        }
    } else
    {
        /* Do nothing, no preamble is generated by the protocol. */
    }


    /* Transceive: */
    transceive_param.tx_buffer          = trx_buffer;
    transceive_param.tx_buffer_size     = (uint16_t)(tx_buffer_size + tx_preamble_length);
    transceive_param.rx_buffer          = trx_buffer;
    transceive_param.rx_buffer_size     = td->m_MaxXchBufferSize;
    transceive_param.self               = initiator_object->mp_Lower;
    status = initiator_object->mp_Lower->Transceive(&transceive_param);
    *rx_buffer_size                     = transceive_param.rx_buffer_size;

    /* Throw away RX preamble (don't look at it) by moving the buffer back to [0]. */
    rx__preamble__length = (uint8_t)(td->m_RXPreambleType >> 4);
    if (rx__preamble__length > 0)
    {
        for (loopcount = 0; loopcount < (*rx_buffer_size); loopcount++)
        {
            trx_buffer[loopcount] = trx_buffer[loopcount + rx__preamble__length];
        }
        *rx_buffer_size = (uint16_t)(*rx_buffer_size - rx__preamble__length);
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
    #undef rx__preamble__length /* */
}




/* ////////////////////////////////////////////////////////////////////////////////////////////// */
/* "Public" functions, visible for the user:                                                      */
/* ////////////////////////////////////////////////////////////////////////////////////////////// */

uint8_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorToxRequestValue(phcsBflNfc_InitiatorToxReqParam_t *req_tox_param)
{
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td =
        (phcsBflNfc_InitiatorCommParams_t*)(((phcsBflNfc_Initiator_t*)(req_tox_param->self))->mp_Members);

    /* For safety (ToxValue internally points to a certain location in the TRX buffer)! */
    if (td->m_ToxValue != 0)
    {
        return td->m_ToxValue;
    } else
    {
        return 0;
    }
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorAtrRequest(phcsBflNfc_InitiatorAtrReqParam_t *atr_req_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td  =
        (phcsBflNfc_InitiatorCommParams_t*)(((phcsBflNfc_Initiator_t*)(atr_req_param->self))->mp_Members);
    uint8_t         PHCS_BFL_MEMLOC_COUNT   loopcount;
    uint16_t        PHCS_BFL_MEMLOC_REM  receive_size;
    phcsBfl_Status_t             PHCS_BFL_MEMLOC_REM  status              = PH_ERR_BFL_SUCCESS;
    uint8_t         PHCS_BFL_MEMLOC_COUNT   index;
    uint8_t         PHCS_BFL_MEMLOC_REM *trx_buffer          = td->mp_TRxBuffer;

    /* First byte of buffer is reserved for length-byte! */
    index = 1;
    /* Prepare command:                                  */
    trx_buffer[index] = PHCS_BFLNFCCOMMON_REQ;
    index++;
    trx_buffer[index] = PHCS_BFLNFCCOMMON_ATR_REQ;
    index++;

    /* Fill in NFCIDi: */
    for (loopcount = 0; loopcount < PHCS_BFLNFCCOMMON_ID_LEN; loopcount++)
    {
        trx_buffer[index] = (atr_req_param->nfcid_it)[loopcount];
        index++;
    }

    /* Fill in other params: */
    trx_buffer[index] = atr_req_param->did;
    index++;
    trx_buffer[index] = atr_req_param->bs_it;
    index++;
    trx_buffer[index] = atr_req_param->br_it;
    index++;
    trx_buffer[index] = atr_req_param->pp_it;
    index++;

    /* Copy generics: */
    for (loopcount = 0; loopcount < atr_req_param->glen_i; loopcount++)
    {
        td->mp_TRxBuffer[index] = (atr_req_param->g_it)[loopcount];
        index++;
    }

    /* Write length-byte to the first byte of the buffer, prepare and transceive: */
    trx_buffer[NFC_LEN_POS] = index;
    status = phcsBflNfc_InitiatorTRxAux( (phcsBflNfc_Initiator_t*) atr_req_param->self,
                                            index,
                                            &receive_size);

    if (status == PH_ERR_BFL_SUCCESS)
    {
        if (receive_size >= NFC_MIN_ATR_RES_LEN)
        {
            /* Check both reply header bytes: */
            if (RESPONSE_HEADER_INVALID(PHCS_BFLNFCCOMMON_RES, PHCS_BFLNFCCOMMON_ATR_RES, td))
            {
                status = PH_ERR_BFL_INVALID_FORMAT;
            } else
            {
                /* Get target nfcid: */
                for (index = NFC_ATR_NFCID_POS; index < PHCS_BFLNFCCOMMON_ID_LEN + NFC_ATR_NFCID_POS; index++)
                {
                    (atr_req_param->nfcid_it)[index - NFC_ATR_NFCID_POS] = trx_buffer[index];
                    td->mp_NfcIDt[index - NFC_ATR_NFCID_POS] = trx_buffer[index];
                }

                /* Get all params from the received data. */
                td->m_DID = trx_buffer[index];
                index++;
                atr_req_param->bs_it = trx_buffer[index];
                index++;
                atr_req_param->br_it = trx_buffer[index];
                index++;
                atr_req_param->to_t  = trx_buffer[index];
                index++;
                atr_req_param->pp_it = trx_buffer[index];
                index++;

                if (index != NFC_MIN_ATR_RES_LEN)
                {
                    /* Wrong number of parameters read out (internal function integrity test)! */
                    status = PH_ERR_BFL_INVALID_FORMAT;
                } else
                {
                    /* Check if generic bytes are appended */
                    if (atr_req_param->pp_it & NFC_ATR_GI_PRESENT)
                    {
                        /* Calculate length of generic bytes. */
                        atr_req_param->glen_t = (uint8_t)(receive_size - NFC_MIN_ATR_RES_LEN);

                        /* Read generic bytes until the end of received buffer */
                        do
                        {
                            (atr_req_param->g_it)[index - NFC_MIN_ATR_RES_LEN] = trx_buffer[index];
                            index++;
                        } while (index < (uint8_t)receive_size);
                    } else
                    {
                        /* No Generics: */
                        atr_req_param->glen_t = 0;
                    }
                }
            }
        } else
        {
            /* Wrong number of bytes returned. */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* Unsuccessful: Merely return result of I/O operation */
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorPslRequest(phcsBflNfc_InitiatorPslReqParam_t *psl_req_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td  =
        (phcsBflNfc_InitiatorCommParams_t*)(((phcsBflNfc_Initiator_t*)(psl_req_param->self))->mp_Members);
    uint16_t          PHCS_BFL_MEMLOC_REM  receive_size;
    phcsBfl_Status_t               PHCS_BFL_MEMLOC_REM  status              = PH_ERR_BFL_SUCCESS;
    uint8_t           PHCS_BFL_MEMLOC_COUNT   index;
    uint8_t           PHCS_BFL_MEMLOC_REM *trx_buffer          = td->mp_TRxBuffer;

    /* First byte of buffer is reserved for length-byte! */
    index = 1;
    trx_buffer[index] = PHCS_BFLNFCCOMMON_REQ;
    index++;
    trx_buffer[index] = PHCS_BFLNFCCOMMON_PSL_REQ;
    index++;

    if (td->m_DID)
    {
        trx_buffer[index] = td->m_DID;
        index++;
    }
    trx_buffer[index] = psl_req_param->BRS;
    index++;
    trx_buffer[index] = psl_req_param->FSL;
    index++;

    /* Transceive information: */
    trx_buffer[NFC_LEN_POS] = index;
    status = phcsBflNfc_InitiatorTRxAux( (phcsBflNfc_Initiator_t*) psl_req_param->self,
                                            index,
                                            &receive_size);

    if (status == PH_ERR_BFL_SUCCESS)
    {
        if (RESPONSE_HEADER_INVALID(PHCS_BFLNFCCOMMON_RES, PHCS_BFLNFCCOMMON_PSL_RES, td))
        {
            /* Response header not valid: */
            status = PH_ERR_BFL_INVALID_FORMAT;
        } else
        {
            /* Check received DID and length of response */
            if (td->m_DID)
            {
                status = phcsBflNfc_InitiatorCheckDid(NFC_MAX_PSL_RES_LEN,
                                                                (uint8_t)receive_size,
                                                                td->m_DID,
                                                                trx_buffer[NFC_PSL_DID_POS]);
            } else
            {
                if (receive_size != NFC_MIN_PSL_RES_LEN)
                {
                    /* Response is not valid: */
                    status = PH_ERR_BFL_INVALID_FORMAT;
                }
            }
        }
    } else
    {
        /* Unsuccessful: Merely return result of I/O op. */
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


void ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorWriteDepHeader(phcsBflNfc_InitiatorCommParams_t *td,
                                      uint8_t                         req_type,
                                      uint8_t                        *nad,
                                      uint8_t                        *index)
{
    uint8_t PHCS_BFL_MEMLOC_REM  pfb;
    uint8_t PHCS_BFL_MEMLOC_REM *trx_buffer = td->mp_TRxBuffer;
    /* First byte of buffer is reserved for length-byte! */
    *index = 1;
    trx_buffer[(*index)] = PHCS_BFLNFCCOMMON_REQ;
    (*index)++;
    trx_buffer[(*index)] = PHCS_BFLNFCCOMMON_DEP_REQ;
    (*index)++;

    pfb = req_type;
    /* Advance to the next byte which is DID or NAD. */
    (*index)++;
    if (td->m_DID)
    {
        pfb |= NFC_DEP_DID_PRESENT;
        trx_buffer[(*index)] = td->m_DID;
        (*index)++;
    }

    /* Ask for NAD presence: */
    if (nad != NULL)
    {
        /* Send NAD only for Information pdu's and if it's used.     */
        if ((req_type == NFC_DEP_INFORMATION) && (td->m_NadUsed))
        {
            /* Use NAD only for the first block and in case of no "meta-chaining": */
            if ((!(td->m_CtrlWord & NFC_CTRL_NAD_PRESENT)) &&
                (!(td->m_CtrlWord & NFC_CTRL_CHAINING_CONTD)))
            {
                pfb |= NFC_DEP_NAD_PRESENT;
                trx_buffer[(*index)] = *nad;
                (*index)++;

                /* Set NAD-bit to avoid sending NAD also in the next data-block. */
                td->m_CtrlWord |= NFC_CTRL_NAD_PRESENT;
                /* Store that NAD was used for this block. */
                td->m_CtrlWord |= NFC_CTRL_NAD_PREV_USED;
            } else
            {
                /* Store that NAD was not used for this block. */
                td->m_CtrlWord &= ~NFC_CTRL_NAD_PREV_USED;
            }
        }
    } else
    {
        /* Late NAD usage indicator reset: */
        td->m_NadUsed = 0;
    }

    /* Size of DEP_REQ header + input buffer must not exceed max. buffer size. */
    /* Otherwise chaining is required!                                         */
    if (td->m_CtrlWord & NFC_CTRL_MI_PRESENT)
    {
        /* Chaining necessary! */
        pfb |= NFC_DEP_MI_PRESENT;
    }
    /* Check if PNI is required: */
    if (req_type != NFC_DEP_SUPERVISORY)
    {
        pfb |= (td->m_BlNr & NFC_DEP_PNI_MASK);
    } else
    {
        /* Clear TOX-bit. */
        pfb &= NFC_DEP_TO_EXT_I;
    }

    trx_buffer[NFC_DEP_PFB_POS] = pfb;
}

uint8_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorWriteInputBuffer(phcsBflNfc_InitiatorCommParams_t *td,
                                           uint8_t                        *input_buffer,
                                           uint32_t                        input_buffer_size,
                                           uint8_t                         index)
{
    uint8_t PHCS_BFL_MEMLOC_COUNT   loopcount;
    uint8_t PHCS_BFL_MEMLOC_REM  max_buffer_size;
    uint8_t PHCS_BFL_MEMLOC_REM *trx_buffer       = td->mp_TRxBuffer;

    /* Index has been already set to the size of the prefix: */
    max_buffer_size = (uint8_t)(td->m_MaxPduBufferSize - index);

    for (loopcount = 0; loopcount < max_buffer_size; loopcount++)
    {
        if ((loopcount + td->m_NextStartIndex) < input_buffer_size)
        {
            /* Not all bytes were copied yet. */
            trx_buffer[index + loopcount] = input_buffer[loopcount + td->m_NextStartIndex];
        } else
        {
            /* No bytes left to send. */
            break;
        }
    }

    /* Calculate total length of bytes written to the buffer (header + data bytes). */
    return (uint8_t)(loopcount + index);
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorDepRequest(phcsBflNfc_InitiatorDepReqParam_t *dep_req_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td  =
        (phcsBflNfc_InitiatorCommParams_t*)(((phcsBflNfc_Initiator_t*)(dep_req_param->self))->mp_Members);
    uint8_t           PHCS_BFL_MEMLOC_COUNT  index               = 0;
    phcsBfl_Status_t               PHCS_BFL_MEMLOC_REM status              = PH_ERR_BFL_SUCCESS;
    uint8_t           PHCS_BFL_MEMLOC_REM err_state_ctrl      = 0; /* upper half-byte: previous error, lower half-byte: current error */
    uint8_t           PHCS_BFL_MEMLOC_COUNT  nak_count           = 0;
    uint8_t           PHCS_BFL_MEMLOC_COUNT  att_count           = 0;
    uint16_t          PHCS_BFL_MEMLOC_REM rx_buffer_length    = 0;
    //uint8_t           PHCS_BFL_MEMLOC_REM pfb;
    uint8_t           PHCS_BFL_MEMLOC_REM exit_after_next_cycle = 0;
    uint8_t           PHCS_BFL_MEMLOC_REM TxBufferLength      = 0;    /* stores buffer length during error processing for recovery */

    /* Perform input parameter check: */
    if (dep_req_param->output_buffer_size < td->m_MaxXchBufferSize)
    {
        return PH_ERR_BFL_BUF_2_SMALL | PH_ERR_BFL_NFC;
    }

    /* Reset Ctrl-byte and Buffer Start-Index in case of state unequal to the
     * Timeout-Extension state.
     */
    if ((td->m_CtrlWord & NFC_CTRL_STATE_MASK) != NFC_DEP_STATE_TS)
    {
        if (td->m_CtrlWord & NFC_CTRL_TOX_PRESENT)
        {
            /* Clear TOX-bit but do not reset Request-state! */
            td->m_CtrlWord &= ~NFC_CTRL_TOX_PRESENT;
        } else
        {
            /* Reset Request only in case of state different to TOX-Start and
             * no function reentry due to reset of target TO-value (see above).
             * Also no state reset if the protocol is waiting for more info from
             * the Target after temporary exit due to buffer limit approach (below).
             */
            if (((td->m_CtrlWord & NFC_CTRL_STATE_MASK) == NFC_DEP_STATE_EI) &&
                (dep_req_param->flags & PHCS_BFLNFC_INITIATOR_CONT_CHAIN))
            {
                /* We do not perform a reset. This is because the protocol has quit before during EI
                   due to buffer limit approach. Before the current call the application had the chance
                   to save all data previously returned by this function. The NFCI_CONTINUE_CHAINING flag
                   is to emphasize that EI is really the state to re-start. The application must set it
                   in order to prevent the protocol from resetting itself at re-entry. */
            } else
            {
                td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                td->m_CtrlWord |= NFC_DEP_STATE_RS;
            }
            td->m_NextStartIndex = 0;
            td->m_HeaderSize     = 0;
            td->m_TxBufferLength = 0;
            /* Determine NAD usage: */
            if (dep_req_param->nad != NULL)
            {
                td->m_NadUsed = 1;
            }
        }
    }

    do
    {
        /* Error recovery states: */
        switch (err_state_ctrl & NFC_CURRENT_ERROR_MASK)
        {
            case NFC_DEP_STATE_NAS:
                {
                    if (nak_count < NFC_MAX_RETRY)
                    {
						/* Increment the counter */
						nak_count++;
                        /* Build header for Not-Acknowledge Request. */
                        phcsBflNfc_InitiatorWriteDepHeader(td,
                                                            (uint8_t)(NFC_DEP_ACKNOWLEDGE | NFC_DEP_NACK),
                                                            dep_req_param->nad,
                                                            &index);

                        /* Write length-byte: */
                        td->mp_TRxBuffer[NFC_LEN_POS] = index;

                        /* Make current error to previous error (shift to upper half-byte).
                         * Next state of error recovery is to analyse target response.      */
                        err_state_ctrl <<= 4;
                        err_state_ctrl |= NFC_DEP_STATE_NAE;
                    } else
                    {
						/* Increment the counter */
						nak_count++;

                        /* nak_count exceeded the maximum number of recovery attempts: */
                        td->m_CtrlWord = NFC_DEP_STATE_RE;

                        if (status == PH_ERR_BFL_SUCCESS)
                        {
                            /* Normally we return the status we got from TRx. However if TRx
                             * indicates success the error must have originated an invalid
                             * response (format) from the Target. */
                            status = PH_ERR_BFL_INVALID_FORMAT;
                        }
                        continue;
                    }
                }
                break;
            case NFC_DEP_STATE_NAE:
                {
                     /* Check kind of target response: */
                    switch (phcsBflNfc_InitiatorCheckPfb(td->mp_TRxBuffer[NFC_DEP_PFB_POS]))
                    {
                        case NFC_ACK:
                            {
                                if (phcsBflNfc_InitiatorCheckPni(td->m_BlNr,
                                                                         td->mp_TRxBuffer[NFC_DEP_PFB_POS]) == PH_ERR_BFL_SUCCESS)
                                {
                                    /* Correct PNI was received.
                                     * Continue with sending next data block. */
                                    err_state_ctrl <<= 4;
                                    err_state_ctrl |= NFC_DEP_STATE_ROK;
                                    /* Clear State: */
                                    td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                    /* State = Expect Information: */
                                    td->m_CtrlWord |= NFC_DEP_STATE_EA;
                                    /* Re-store TxLength to global variables after error case */
                                    td->m_TxBufferLength = TxBufferLength;
                                } else
                                {
                                    /* Wrong PNI was received!
                                     * Retransmit last block if Target PNI + 1 = Initiator PNI. */
                                    if ((((td->mp_TRxBuffer[NFC_DEP_PFB_POS] & NFC_DEP_PNI_MASK) + 1) % 4) == td->m_BlNr)
                                    {
                                        err_state_ctrl <<= 4;
                                        err_state_ctrl |= NFC_DEP_STATE_ROK;
                                        /* Clear State: */
                                        td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                        /* Send next block using state Request Start: */
                                        td->m_CtrlWord |= NFC_DEP_STATE_RS;
                                    } else
                                    {
                                        /* Wrong PNI - exit request. */
                                        status = PH_ERR_BFL_INVALID_FORMAT;
                                        td->m_CtrlWord = NFC_DEP_STATE_RE;
                                        continue;
                                    }
                                }
                            }
                            break;

                        case NFC_INF:
                            {
                                if (phcsBflNfc_InitiatorCheckPni(td->m_BlNr,
                                                                         td->mp_TRxBuffer[NFC_DEP_PFB_POS]) == PH_ERR_BFL_SUCCESS)
                                {
                                    /* Correct PNI was received.
                                     * Continue with sending next data block. */
                                    err_state_ctrl <<= 4;
                                    err_state_ctrl |= NFC_DEP_STATE_ROK;
                                    /* Clear State: */
                                    td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                    /* Next state = Expect Information: */
                                    td->m_CtrlWord |= NFC_DEP_STATE_EI;
                                } else
                                {
                                    /* Wrong PNI was received!
                                     * Retransmit last block if Target PNI + 1 = Initiator PNI. */
                                    if ((((td->mp_TRxBuffer[NFC_DEP_PFB_POS] & NFC_DEP_PNI_MASK) + 1) % 4) == td->m_BlNr)
                                    {
                                        err_state_ctrl <<= 4;
                                        err_state_ctrl |= NFC_DEP_STATE_ROK;
                                        /* Clear State: */
                                        td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                        /* State = Send Acknowledge and then Expect Information: */
                                        td->m_CtrlWord |= NFC_DEP_STATE_SA;
                                    } else
                                    {
                                        /* Wrong PNI - exit request. */
                                        status = PH_ERR_BFL_INVALID_FORMAT;
                                        td->m_CtrlWord = NFC_DEP_STATE_RE;
                                        continue;
                                    }
                                }
                            }
                            break;

                        case NFC_TO_EXT:
                            {
                                /* Continue with TOX-handling. */
                                err_state_ctrl <<= 4;
                                err_state_ctrl |= NFC_DEP_STATE_ROK;
                            }
                            break;

                        default:
                            {
                                /* No expected response was received - exit request! */
                                status = PH_ERR_BFL_INVALID_FORMAT;
                                td->m_CtrlWord = NFC_DEP_STATE_RE;
                                continue;
                            }
                           // break;
                    }
                }
                break;
            case NFC_DEP_STATE_ATS:
                {
					if (att_count < NFC_MAX_RETRY)
                    {
						/* Increment the counter */
						att_count++;
                        /* Build header for Attention Request. */
                        phcsBflNfc_InitiatorWriteDepHeader(td,
                                                            NFC_DEP_SUPERVISORY,
                                                            dep_req_param->nad,
                                                            &index);

                        /* Write length-byte: */
                        td->mp_TRxBuffer[NFC_LEN_POS] = index;

                        /* Next state of error recovery is to analyse target response. */
                        err_state_ctrl <<= 4;
                        err_state_ctrl |= NFC_DEP_STATE_ATE;
                    } else
                    {
						/* Increment the counter */
						att_count++;
                        /* att_count exceeded the maximum number of recovery attempts: */
                        td->m_CtrlWord = NFC_DEP_STATE_RE;
                        status = PH_ERR_BFL_INVALID_FORMAT;
                        continue;
                    }
                }
                break;
            case NFC_DEP_STATE_ATE:
                {
                    /* Check kind of target response: */
                    switch (phcsBflNfc_InitiatorCheckPfb(td->mp_TRxBuffer[NFC_DEP_PFB_POS]))
                    {
                        case NFC_ATT:
                            {
                                /* Valid Attention response was received.               */
                                /* Continue with retransmission of previous data block. */
                                err_state_ctrl <<= 4;
                                err_state_ctrl |= NFC_DEP_STATE_ROK;
                                /* Clear State: */
                                td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;

                                /* Restore previously sent block: */
                                td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                td->m_CtrlWord |= ((td->m_CtrlWord >> 8) & NFC_CTRL_STATE_MASK);

                                /* Enable transmission of NAD, if necessary: */
                                if ((td->m_CtrlWord & NFC_CTRL_NAD_PREV_USED) ||
                                    (td->m_CtrlWord & NFC_CTRL_CHAINING_CONTD))
                                {
                                    td->m_CtrlWord &= NFC_CTRL_NAD_PRESENT_I;
                                }
                                continue;
                            }
                           // break;
                        default:
                            {
                                /* No expected response was received - exit request! */
                                status = PH_ERR_BFL_INVALID_FORMAT;
                                td->m_CtrlWord = NFC_DEP_STATE_RE;
                                continue;
                            }
                           // break;
                    }
                }
               // break;
            default: break;
        }

        if ((err_state_ctrl & NFC_CURRENT_ERROR_MASK) == NFC_DEP_STATE_ROK)
        {
            /* Regular protocol-request states: */
            switch (td->m_CtrlWord & NFC_CTRL_STATE_MASK)
            {
                    /* STATE_EA analyses the ACK response of the target
                     * first. Then this state continues with STATE_RS for
                     * sending the next information block to the target.  */
                case NFC_DEP_STATE_EA:
                    {
                        /* Check if TOX-bit is set: */
                        if (td->m_CtrlWord & NFC_CTRL_TOX_PRESENT)
                        {
                            /* Exit to reset TO-value. */
                            status = PH_ERR_BFL_TARGET_RESET_TOX;
                            dep_req_param->output_buffer_size = 0;
                            continue;
                        }

                        /* Check if target response is valid: */
                        if (phcsBflNfc_InitiatorCheckPfb(td->mp_TRxBuffer[NFC_DEP_PFB_POS]) == NFC_ACK)
                        {
                            if (phcsBflNfc_InitiatorCheckPni(td->m_BlNr,
                                                                     td->mp_TRxBuffer[NFC_DEP_PFB_POS]) == PH_ERR_BFL_SUCCESS)
                            {
                                /* Increment number of successfully sent bytes (without header length): */
                                if((td->m_CtrlWord & NFC_CTRL_STATE_MASK) == NFC_DEP_STATE_TE ||
                                    (td->m_CtrlWord & NFC_CTRL_STATE_MASK) == NFC_DEP_STATE_TS)
                                {
                                    /*
                                     * The last packet sent is smaller than the header to be sent:
                                     * To keep the protocol running it is necessary not to modify the
                                     * next start index (necessary during timeout extension handling).
                                     */
                                } else
                                {
                                    /*
                                     * In this case we get no "underflow" and the true amount of data
                                     * previously sent.
                                     * REMARK: Set only if NOT (!) recovering from TOX handling (observe)
                                     * the PREVIOUS state (!= TS). NextStartIndex is modified by the TE
                                     * state in this case already.
                                     */
                                    if (((td->m_CtrlWord & NFC_CTRL_PREV_STATE_MASK) >> 8) != NFC_DEP_STATE_TS)
                                    {
                                        td->m_NextStartIndex += td->m_TxBufferLength - td->m_HeaderSize;
                                    }
                                }
                                /* Increment PNI (Block Number). */
                                td->m_BlNr = (uint8_t)((td->m_BlNr + 1) % 4);
                            } else
                            {
                                /* Wrong PNI was received!
                                 * Continue with sending a NACK.          */
                                HANDLE_INVALID_RESPONSE(err_state_ctrl);
                                continue;
                            }
                        } else
                        {
                            /* A(CK) was expected but not received!
                             * Continue with sending a NACK.              */
                            HANDLE_INVALID_RESPONSE(err_state_ctrl);
                            continue;
                        }
                    }
                    if (exit_after_next_cycle)
                    {
                        td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                        td->m_CtrlWord |= NFC_DEP_STATE_OE;
                        td->m_CtrlWord |= NFC_CTRL_CHAINING_CONTD;
                        dep_req_param->output_buffer_size = 0;
                        continue;
                    } else
                    {
                        /* Continue with NFC_DEP_STATE_RS! No break required.
                         * This state builds a new information package with
                         * bytes stored in input_buffer. STATE_EA uses this
                         * state merely for this purpose.                     */
                    }
                case NFC_DEP_STATE_RS:
                    {
                        /* Calculate the position for the first byte of data.
                         * Header will be written later!                      */
                        index = NFC_MIN_DEP_REQ_LEN;
                        if (td->m_DID)
                        {
                            index++;
                        }
                        /* Send NAD only at the first block of data exchange: */
                        if (td->m_NadUsed &&
                            ((td->m_CtrlWord & NFC_CTRL_NAD_PRESENT) == 0) &&
                            (!(td->m_CtrlWord & NFC_CTRL_CHAINING_CONTD)))
                        {
                            index++;
                        }

                        /* Size of DEP_REQ header + rest of input buffer exceeds max. TRx buffer size, OR
                           "meta" chaining is done to signal that we send a continued request. */
                        if (((dep_req_param->input_buffer_size - td->m_NextStartIndex) + index) >
                                td->m_MaxPduBufferSize)
                        {
                            td->m_CtrlWord |= NFC_CTRL_MI_PRESENT;
                            /* Set next state to "Expect Acknowledge": */
                            /* Clear State: */
                            td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                            /* State = Expect Acknowledge: */
                            td->m_CtrlWord |= NFC_DEP_STATE_EA;
                            /* Continue after ACK. */
                            exit_after_next_cycle = 0;
                        } else
                        {
                            /* End of user buffer reached - no MI required, except the case that: ...*/
                            if (dep_req_param->flags & PHCS_BFLNFC_INITIATOR_CONT_CHAIN)
                            {
                                /* We exit temporarily for new information to be sent with the next call.
                                   MI has to be kept to 1, therefore the next call continues what the
                                   previous began. We're done with the call. */
                                td->m_CtrlWord |= NFC_CTRL_MI_PRESENT;
                                /* Set next state to "Expect Acknowledge": */
                                /* Clear State: */
                                td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                /* State = Expect Acknowledge: */
                                td->m_CtrlWord |= NFC_DEP_STATE_EA;
                                /* Tell the function to exit temporarily after sending the last block
                                   of the current buffer: No data return, but check of last ACK done. */
                                exit_after_next_cycle = 1;
                            } else
                            {
                                /* "Normal" operation: We're done with the transfer,
                                 * expect info from the Target: Set next state to "Expect Information",
                                 * due to no more data is there to be sent to the Target:
                                 */
                                /* Clear MI-bit: */
                                td->m_CtrlWord &= ~NFC_CTRL_MI_PRESENT;
                                /* Clear State: */
                                td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                /* State = Expect Information: */
                                td->m_CtrlWord |= NFC_DEP_STATE_EI;
                                /* Continue after INF, if req'd by the Target (reciving I(1)): */
                                exit_after_next_cycle = 0;

								/* Set the notification that the user buffer has to be reset.
								   Don't reset the buffer right now because it's still needed. */
								td->m_CtrlWord |= NFC_RESET_USER_BUFFER;
                            }
                        }

                        /* Write DEP-REQ header to the TRxBuffer. */
                        phcsBflNfc_InitiatorWriteDepHeader(td, NFC_DEP_INFORMATION, dep_req_param->nad, &index);
                        td->m_HeaderSize = index;

                        /* Reset the meta-chaining bit if this request handles all data: */
                        if ((td->m_CtrlWord & NFC_CTRL_CHAINING_CONTD) &&
                            (!(dep_req_param->flags & PHCS_BFLNFC_INITIATOR_CONT_CHAIN)))
                        {
                            td->m_CtrlWord &= ~NFC_CTRL_CHAINING_CONTD;
                        }

                        /* Append data bytes to the DEP_REQ header until max. size of transmission
                         * buffer is reached or no more bytes are left to send.                    */
                        td->m_TxBufferLength = phcsBflNfc_InitiatorWriteInputBuffer(td,
                                                                                    dep_req_param->input_buffer,
                                                                                    dep_req_param->input_buffer_size,
                                                                                    index);
                        /* Store TxBufferlength internally if error occures to restore latest transfer size
                         * needed especially using chaining from I->T with larger user buffer size.         */
                        TxBufferLength = td->m_TxBufferLength;

                        /* Finally the length-byte is written to the buffer.                        */
                        td->mp_TRxBuffer[NFC_LEN_POS] = td->m_TxBufferLength;

                        /* Check if Nad is used, but not sent at last block: */
                        if (td->m_NadUsed)
                        {
                            if (((td->m_CtrlWord & NFC_CTRL_MI_PRESENT) == 0) &&
                                ((td->mp_TRxBuffer[NFC_DEP_PFB_POS] & NFC_DEP_NAD_PRESENT) == 0))
                            {
                                /* Recalculate expected header size: */
                                td->m_HeaderSize++;
                                index++;
                            }
                        }

                        /* Save current state to repeat the request if necessary. */
                        td->m_CtrlWord &= NFC_CTRL_PREV_STATE_MASK_I;
                        td->m_CtrlWord |= (NFC_DEP_STATE_RS << 8);
                    }
                    break;

                    /* STATE_EI analyses the block received from the target
                     * first. Then it continues with STATE_SA and sends
                     * a further ACK, if the target has still some data
                     * available to be received.                             */
                case NFC_DEP_STATE_EI:
                    {
                        /* Check if TOX-bit is set: */
                        if (td->m_CtrlWord & NFC_CTRL_TOX_PRESENT)
                        {
                            /* Exit temporarily to reset TO-value. */
                            status = PH_ERR_BFL_TARGET_RESET_TOX;
                            dep_req_param->output_buffer_size = 0;
                            continue;
                        }

                        /* Check if target response is valid: */
                        if (phcsBflNfc_InitiatorCheckPfb(td->mp_TRxBuffer[NFC_DEP_PFB_POS]) == NFC_INF)
                        {
                            if (phcsBflNfc_InitiatorCheckPni(td->m_BlNr,
                                                                     td->mp_TRxBuffer[NFC_DEP_PFB_POS]) == PH_ERR_BFL_SUCCESS)
                            {
                                /* Check if expect information is called for the first time. */
                                /* Size of last block can be used as indicator for the type of block! */
                                if (td->m_TxBufferLength > NFC_MAX_DEP_REQ_LEN)
                                {
                                    /* Information pdu was sent -> Initialise NextStartIndex! */
                                    td->m_NextStartIndex = 0;
                                    /* All bytes were successfully sent! */
                                    dep_req_param->input_buffer_size = 0;
                                }

								if (td->m_CtrlWord & NFC_RESET_USER_BUFFER)
								{
									/* If user buffer and chaining is used and the response was successful,
									   set the user buffer pointer to 0 in for the incoming data. */
									td->m_CtrlWord &= ~NFC_RESET_USER_BUFFER;
									td->m_NextStartIndex = 0;
								}

                                /* Check if output buffer is big enough for received data: */
                                if ((td->m_NextStartIndex +
                                        td->mp_TRxBuffer[NFC_LEN_POS] -
                                            td->m_HeaderSize) >
                                                dep_req_param->output_buffer_size)
                                {
                                    /*
                                     * Nope, now what? The protocol is told at this point to exit
                                     * temporarily in order to give the embedding application the chance
                                     * to save data already received. When the protocol is called again
                                     * to resume receiving, the state it continues with is EI again.
                                     * It is told to continue by having the NFCI_CONTINUE_CHAINING bit
                                     * set by the embedding SW.
                                     */
                                    dep_req_param->output_buffer_size = (uint16_t)td->m_NextStartIndex;
                                    dep_req_param->flags |= PHCS_BFLNFC_INITIATOR_CONT_CHAIN;
                                    status = PH_ERR_BFL_USERBUFFER_FULL;
                                    continue;
                                }

                                /* Copy data from the receive buffer to the output buffer: */
                                for (index = td->m_HeaderSize; index < td->mp_TRxBuffer[NFC_LEN_POS]; index++)
                                {
                                    (dep_req_param->output_buffer)[index - td->m_HeaderSize + td->m_NextStartIndex] =
                                        td->mp_TRxBuffer[index];
                                }

                                /* Increment number of succesfully received bytes (without header length): */
                                td->m_NextStartIndex += td->mp_TRxBuffer[NFC_LEN_POS] - td->m_HeaderSize;

                                /* Increment PNI (Block Number). */
                                td->m_BlNr = (uint8_t)((td->m_BlNr + 1) % 4);
                            } else
                            {
                                /* Wrong PNI was received!
                                 * Continue with sending a NACK.          */
                                err_state_ctrl <<= 4;
                                err_state_ctrl |= NFC_DEP_STATE_NAS;
                                continue;
                            }
                        } else
                        {
                            /* A(CK) was expected but not received!
                             * Continue with sending a NACK.              */
                            HANDLE_INVALID_RESPONSE(err_state_ctrl);
                            continue;
                        }
                    }
                    /* NFC_DEP_STATE_SA is a part of EI - no break required!
                     * This state is required to send only an ACK-block in case
                     * of a NACK was sent to the target previously.                */
                case NFC_DEP_STATE_SA:
                    {
                        /* Check if MI-bit is still set by the target or the last
                         * ACK has to be retransmitted (ATT was previously sent).  */
                        if ((td->mp_TRxBuffer[NFC_DEP_PFB_POS] & NFC_DEP_MI_PRESENT) ||
                            ((err_state_ctrl & NFC_PREVIOUS_ERROR_MASK) == (NFC_DEP_STATE_ATE << 4)))
                        {
                            /* MI is still set!                          */
                            /* Continue with state "Expect Information": */
                            td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                            td->m_CtrlWord |= NFC_DEP_STATE_EI;
                        } else
                        {
                            /* Set next state to "End", due to no more data
                             * has to be received from the target. */
                            /* Clear State: */
                            td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                            /* State = End of DEP-Request: */
                            td->m_CtrlWord |= NFC_DEP_STATE_RE;
                            continue;
                        }

                        /* This point is only reached if an Acknowledge must be sent. */
                        phcsBflNfc_InitiatorWriteDepHeader(td,
                                                            NFC_DEP_ACKNOWLEDGE,
                                                            dep_req_param->nad,
                                                            &index);
                        td->m_HeaderSize = index;

                        /* Write length byte to the buffer. */
                        td->mp_TRxBuffer[NFC_LEN_POS]  = index;
                        td->m_TxBufferLength = index;

                        /* Save current state to repeat the request if necessary. */
                        td->m_CtrlWord &= NFC_CTRL_PREV_STATE_MASK_I;
                        td->m_CtrlWord |= (NFC_DEP_STATE_SA << 8);
                    }
                    break;
                case NFC_DEP_STATE_OE:
                case NFC_DEP_STATE_RE:
                    {
                        /* End of DEP-Request. */
                        continue;
                    }
                    //break;
                case NFC_DEP_STATE_TE:
                    {
                        /* TOX was received and now it must be handled. */
                        /* Check size of PDU: */
                        if ((td->mp_TRxBuffer[NFC_LEN_POS] > NFC_DEP_PFB_POS) &&
                            (td->mp_TRxBuffer[NFC_LEN_POS] > NFC_DEP_PFB_POS + 1 +
                                (td->mp_TRxBuffer[NFC_DEP_PFB_POS] & NFC_DEP_DID_PRESENT ? 1 : 0)))
                        {
                            /* Clear State: */
                            td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                            /* Start this function next time with State = TOX Start:    */
                            td->m_CtrlWord |= NFC_DEP_STATE_TS;
                            status = PH_ERR_BFL_TARGET_SET_TOX;

                            /* Return TOX value:
                             * RTOX value is coded in the last byte of target response. */
                            td->mp_TRxBuffer[td->mp_TRxBuffer[NFC_LEN_POS] - 1] &= NFC_DEP_RTOX_MASK;
                            td->m_ToxValue = td->mp_TRxBuffer[td->mp_TRxBuffer[NFC_LEN_POS] - 1];
                            dep_req_param->output_buffer_size = 0;
                            /* MI...?*/
                            if (td->m_CtrlWord & NFC_CTRL_MI_PRESENT)
                            {
                                /* Remember the "already-sent" byte count if more info is to be transmitted: */
                                td->m_NextStartIndex += td->m_TxBufferLength - td->m_HeaderSize;
                            } else
                            {
                                /* Do nothing, all set to 0 at the beginning of the new chaining frame. */
                            }
                            continue;
                        } else
                        {
                            /*
                             * An invalid frame must be handled: At this point we could say that,
                             * because the frame is invalid, the protocol did not know about its type.
                             * It is up to the Target to respond with the correct frame after the following
                             * NACK.
                             */
                            HANDLE_INVALID_RESPONSE(err_state_ctrl);
                            continue;
                        }
                    }
                   // break;
                case NFC_DEP_STATE_TS:
                    {
                        /* Reply with received TOX to the target to start RTW. */
                        /* Fill in data: */
                        td->mp_TRxBuffer[NFC_CMD0_POS]   = PHCS_BFLNFCCOMMON_REQ;
                        td->mp_TRxBuffer[NFC_CMD1_POS]   = PHCS_BFLNFCCOMMON_DEP_REQ;
                        td->mp_TRxBuffer[NFC_DEP_PFB_POS] &= NFC_PFB_7_5_MASK_I;
                        td->mp_TRxBuffer[NFC_DEP_PFB_POS] |= NFC_DEP_SUPERVISORY;
                        td->mp_TRxBuffer[NFC_DEP_PFB_POS] |= NFC_DEP_TO_EXT;
                        /* Set Length: we add 2 because index starts at 0 and TOX value is added.
                           Set TOX value (echo to Target): */
                        if (td->m_DID != 0)
                        {
                            td->mp_TRxBuffer[NFC_LEN_POS] = NFC_DEP_DID_POS + 2;
                            td->mp_TRxBuffer[NFC_DEP_DID_POS + 1] = td->m_ToxValue;
                        } else
                        {
                            td->mp_TRxBuffer[NFC_LEN_POS] = NFC_DEP_PFB_POS + 2;
                            td->mp_TRxBuffer[NFC_DEP_PFB_POS + 1] = td->m_ToxValue;
                        }
                        /* Set size of the buffer: */
                        td->m_TxBufferLength = td->mp_TRxBuffer[NFC_LEN_POS];
                        /* Chaining in progress ? */
                        if (td->m_CtrlWord & NFC_CTRL_MI_PRESENT)
                        {
                            /* MI is present! */
                            /* Clear State: */
                            td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                            /* State = Expect Acknowledge: */
                            td->m_CtrlWord |= NFC_DEP_STATE_EA;
                        } else
                        {
                            /* Clear State: */
                            td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                            /* State = Expect Information: */
                            td->m_CtrlWord |= NFC_DEP_STATE_EI;
                        }
                        /* Set TOX_PRESENT bit to indicate that the TO-value must be
                         * reset after the next target response.                         */
                        td->m_CtrlWord |= NFC_CTRL_TOX_PRESENT;

                        /* Save current state to repeat the request if necessary. */
                        td->m_CtrlWord &= NFC_CTRL_PREV_STATE_MASK_I;
                        td->m_CtrlWord |= (NFC_DEP_STATE_TS << 8);
                    }
                    break;
                default:
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                        td->m_CtrlWord = NFC_DEP_STATE_RE;
                        continue;
                    }
                   // break;
            }
        }

        /* las83 */
       // pfb = td->mp_TRxBuffer[NFC_DEP_PFB_POS];

        /* Make current err_state to "previous err_state": */
        err_state_ctrl &= ~NFC_PREVIOUS_ERROR_MASK;
        err_state_ctrl |= (err_state_ctrl << 4);

        /* Kick off (next) data packet to Target: */
        td->m_TxBufferLength = td->mp_TRxBuffer[NFC_LEN_POS];
        status = phcsBflNfc_InitiatorTRxAux((phcsBflNfc_Initiator_t*) dep_req_param->self,
                                                td->m_TxBufferLength,
                                                &rx_buffer_length);

        switch (status)
        {
            case PH_ERR_BFL_SUCCESS:
                {
                    /* Check if response is valid: */
                    if (RESPONSE_HEADER_INVALID(PHCS_BFLNFCCOMMON_RES, PHCS_BFLNFCCOMMON_DEP_RES, td))
                    {
                        /* Header invalid - send NACK! */
                        HANDLE_INVALID_RESPONSE(err_state_ctrl);
                    } else
                    {
                        if ((rx_buffer_length != td->mp_TRxBuffer[NFC_LEN_POS]))
                        {
                            /* Wrong number of bytes received - send NACK! */
                            HANDLE_INVALID_RESPONSE(err_state_ctrl);
                        } else
                        {
                            if ((phcsBflNfc_InitiatorCheckDepDid(td->mp_TRxBuffer, td->m_DID) == PH_ERR_BFL_SUCCESS) &&
                                (phcsBflNfc_InitiatorCheckDepNad(td,
                                                                         td->mp_TRxBuffer,
                                                                         dep_req_param->nad)   == PH_ERR_BFL_SUCCESS))
                            {
                                /* Responses OK: Information received: */
                                /* Reset error / retry counters: */
                                if ((err_state_ctrl & NFC_CURRENT_ERROR_MASK) == NFC_DEP_STATE_ROK)
                                {
                                    nak_count = 0;
                                    att_count = 0;
                                }

                                /* Check if Timeout-Extension was received: */
                                if (phcsBflNfc_InitiatorCheckPfb(td->mp_TRxBuffer[NFC_DEP_PFB_POS]) == NFC_TO_EXT)
                                {
                                    /* Clear State: */
                                    td->m_CtrlWord &= NFC_CTRL_STATE_MASK_I;
                                    /* State = TOX, temporary end of procedure: */
                                    td->m_CtrlWord |= NFC_DEP_STATE_TE;
                                    /* Set TOX-bit as indicator for TOX. */
                                    td->m_CtrlWord |= NFC_CTRL_TOX_PRESENT;
                                }
                            } else
                            {
                                /* Header invalid - send NACK */
                                HANDLE_INVALID_RESPONSE(err_state_ctrl)
                            }
                        }
                    }
                }
                break;
            case PH_ERR_BFL_IO_TIMEOUT:
                {
                    /* Timeout: Send Attention, except a NAK has been sent before: */
                    HANDLE_RESPONSE_TIMEOUT(err_state_ctrl);
                }
                break;
            default:
                {
                    /* CRC, ... error: Send NACK: */
                    HANDLE_INVALID_RESPONSE(err_state_ctrl);
                }
                break;
        }
    } while (((status & PH_ERR_BFL_ERR_MASK) != PH_ERR_BFL_TARGET_SET_TOX) &&
             ((td->m_CtrlWord & NFC_CTRL_STATE_MASK) != NFC_DEP_STATE_RE) &&
             ((td->m_CtrlWord & NFC_CTRL_STATE_MASK) != NFC_DEP_STATE_OE) &&
              ((status & PH_ERR_BFL_ERR_MASK) != PH_ERR_BFL_TARGET_RESET_TOX) &&
              ((status & PH_ERR_BFL_ERR_MASK) != PH_ERR_BFL_USERBUFFER_FULL));

    if ((td->m_CtrlWord & NFC_CTRL_STATE_MASK) == NFC_DEP_STATE_RE)
    {
        /* Set ouput_buffer_size in case of regular exit: */
        dep_req_param->output_buffer_size = (uint16_t)td->m_NextStartIndex;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorAttRequest(phcsBflNfc_InitiatorAttReqParam_t *att_req_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td  =
        (phcsBflNfc_InitiatorCommParams_t*)(((phcsBflNfc_Initiator_t*)(att_req_param->self))->mp_Members);
    uint16_t          PHCS_BFL_MEMLOC_REM receive_size;
    phcsBfl_Status_t               PHCS_BFL_MEMLOC_REM status              = PH_ERR_BFL_SUCCESS;
    uint8_t           PHCS_BFL_MEMLOC_COUNT  index;
    uint8_t           PHCS_BFL_MEMLOC_REM dummy_nad           = 0;

    /* Write DEP-Request header to the buffer of td. */
    phcsBflNfc_InitiatorWriteDepHeader(td, NFC_DEP_SUPERVISORY, &dummy_nad, &index);

    td->mp_TRxBuffer[NFC_LEN_POS] = index;
    status = phcsBflNfc_InitiatorTRxAux((phcsBflNfc_Initiator_t*) att_req_param->self,
                                        index,
                                        &receive_size);

    if (status == PH_ERR_BFL_SUCCESS)
    {
        if (receive_size == index)
        {
            /* OK, receive size is the same as the size of send buffer! */
            /* Check content: */
            if (RESPONSE_HEADER_INVALID(PHCS_BFLNFCCOMMON_RES, PHCS_BFLNFCCOMMON_DEP_RES, td))
            {
                /* Error: */
                status = PH_ERR_BFL_INVALID_FORMAT;
            } else
            {
                /* Check body: */
            }
        } else
        {
            /* The frame returned is invalid! */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* Merely return the status. */
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorDslRlsCombiRequest(phcsBflNfc_Initiator_t *current_object,
                                                    uint8_t              req_type,
                                                    uint8_t              exp_res_type,
                                                    uint8_t              min_rec_len)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td  =
        (phcsBflNfc_InitiatorCommParams_t*)(current_object->mp_Members);
    uint16_t          PHCS_BFL_MEMLOC_REM       receive_size;
    phcsBfl_Status_t  PHCS_BFL_MEMLOC_REM       status              = PH_ERR_BFL_SUCCESS;
    uint8_t           PHCS_BFL_MEMLOC_COUNT     index;
    uint8_t           PHCS_BFL_MEMLOC_REM      *trx_buffer          = td->mp_TRxBuffer;

    /* First byte of buffer is reserved for length-byte! */
    index = 1;
    trx_buffer[index] = PHCS_BFLNFCCOMMON_REQ;
    index++;
    trx_buffer[index] = req_type;
    index++;

    if (td->m_DID)
    {
        trx_buffer[index] = td->m_DID;
        index++;
    }

    trx_buffer[NFC_LEN_POS] = index;
    status = phcsBflNfc_InitiatorTRxAux(current_object,
                                        index,
                                        &receive_size);

    if (status == PH_ERR_BFL_SUCCESS)
    {
        if (RESPONSE_HEADER_INVALID(PHCS_BFLNFCCOMMON_RES, exp_res_type, td))
        {
            /* Response header not valid: */
            status = PH_ERR_BFL_INVALID_FORMAT;
        } else
        {
            status = phcsBflNfc_InitiatorCheckOptDidNad(trx_buffer,
                                                 index,
                                                 min_rec_len,
                                                 td->m_DID,
                                                 0);
            if(status == PH_ERR_BFL_SUCCESS)
            {
                /* replaces call of ResetProt because of incompatibility of parameters */
                td->m_BlNr     = 0;
                td->m_NadUsed  = 0;
                td->m_CtrlWord = NFC_DEP_STATE_RS;
            }
        }
    } else
    {
        /* Merely return the status. */
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorDslRequest(phcsBflNfc_InitiatorDslReqParam_t *dsl_req_param)
{
    return phcsBflNfc_InitiatorDslRlsCombiRequest((phcsBflNfc_Initiator_t*) dsl_req_param->self,
                                                 PHCS_BFLNFCCOMMON_DSL_REQ,
                                                 PHCS_BFLNFCCOMMON_DSL_RES,
                                                 NFC_MIN_DSL_RES_LEN);
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorRlsRequest(phcsBflNfc_InitiatorRlsReqParam_t *rls_req_param)
{
    return phcsBflNfc_InitiatorDslRlsCombiRequest((phcsBflNfc_Initiator_t*) rls_req_param->self,
                                                 PHCS_BFLNFCCOMMON_RLS_REQ,
                                                 PHCS_BFLNFCCOMMON_RLS_RES,
                                                 NFC_MIN_RLS_RES_LEN);
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorWupRequest(phcsBflNfc_InitiatorWupReqParam_t *wup_req_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td  =
        (phcsBflNfc_InitiatorCommParams_t*)(((phcsBflNfc_Initiator_t*)(wup_req_param->self))->mp_Members);
    uint16_t          PHCS_BFL_MEMLOC_REM  receive_size;
    phcsBfl_Status_t  PHCS_BFL_MEMLOC_REM  status              = PH_ERR_BFL_SUCCESS;
    uint8_t           PHCS_BFL_MEMLOC_COUNT   index;
    uint8_t           PHCS_BFL_MEMLOC_COUNT   counter;
    uint8_t           PHCS_BFL_MEMLOC_REM *trx_buffer          = td->mp_TRxBuffer;

    /* First byte of buffer is reserved for length-byte! */
    index = 1;
    trx_buffer[index] = PHCS_BFLNFCCOMMON_REQ;
    index++;
    trx_buffer[index] = PHCS_BFLNFCCOMMON_WUP_REQ;
    index++;

    for (counter = 0; counter < PHCS_BFLNFCCOMMON_ID_LEN; counter++)
    {
        trx_buffer[index] = td->mp_NfcIDt[counter];
        index++;
    }

    td->mp_TRxBuffer[index] = wup_req_param->did;
    index++;


    trx_buffer[NFC_LEN_POS] = index;
    status = phcsBflNfc_InitiatorTRxAux((phcsBflNfc_Initiator_t*) wup_req_param->self,
                                        index,
                                        &receive_size);

    if (status == PH_ERR_BFL_SUCCESS)
    {
        if (RESPONSE_HEADER_INVALID(PHCS_BFLNFCCOMMON_RES, PHCS_BFLNFCCOMMON_WUP_RES, td))
        {
            /* Response header not valid: */
            status = PH_ERR_BFL_INVALID_FORMAT;
        } else
        {
            /* Check received DID and length of response */
            status = phcsBflNfc_InitiatorCheckDid((uint8_t)receive_size,
                                                            NFC_WUP_RES_LEN,
                                                            wup_req_param->did,
                                                            td->mp_TRxBuffer[NFC_WUP_RES_DID_POS]);

            if (status == PH_ERR_BFL_SUCCESS)
            {
                td->m_DID = wup_req_param->did;
            }
        }
    } else
    {
        /* Merely return the status. */
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


void ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorInit(phcsBflNfc_Initiator_t          *cif,
                            phcsBflNfc_InitiatorCommParams_t  *td,
                            uint8_t                            tx_preamble_type,
                            uint8_t                            rx_preamble_type,
                            uint8_t                           *p_trxbuffer,
                            uint16_t                           trxbuffersize,
                            uint8_t                           *p_nfcidt_buffer,
                            phcsBflIo_t                       *p_lower)
{
    phcsBflNfc_InitiatorSetTRxPropParam_t PHCS_BFL_MEMLOC_REM set_trx_properties_param;
    phcsBflNfc_InitiatorResetParam_t      PHCS_BFL_MEMLOC_REM rst_param;

    td->m_MaxPduBufferSize  = NFC_DEFAULT_TRX_BUF_SIZE;      /* "Dummy Initialisation". */
    td->mp_NfcIDt           = p_nfcidt_buffer;
    cif->mp_Lower           = p_lower;
    cif->mp_Members         = td;

    cif->AtrRequest          = phcsBflNfc_InitiatorAtrRequest;
    cif->PslRequest          = phcsBflNfc_InitiatorPslRequest;
    cif->DepRequest          = phcsBflNfc_InitiatorDepRequest;
    cif->AttRequest          = phcsBflNfc_InitiatorAttRequest;
    cif->DslRequest          = phcsBflNfc_InitiatorDslRequest;
    cif->WupRequest          = phcsBflNfc_InitiatorWupRequest;
    cif->RlsRequest          = phcsBflNfc_InitiatorRlsRequest;
    cif->RequestedToxValue   = phcsBflNfc_InitiatorToxRequestValue;
    cif->SetTRxProp          = phcsBflNfc_InitiatorSetTRxProp;
    cif->ResetProt           = phcsBflNfc_InitiatorResetProt;

    rst_param.self = cif;
    phcsBflNfc_InitiatorResetProt(&rst_param);

    set_trx_properties_param.self               = cif;
    set_trx_properties_param.tx_preamble_type   = tx_preamble_type;
    set_trx_properties_param.rx_preamble_type   = rx_preamble_type;
    set_trx_properties_param.p_trxbuffer        = p_trxbuffer;
    set_trx_properties_param.trxbuffersize      = trxbuffersize;
    phcsBflNfc_InitiatorSetTRxProp(&set_trx_properties_param);
}


void ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorSetTRxProp(phcsBflNfc_InitiatorSetTRxPropParam_t *set_trx_properties_param)
{
    phcsBflNfc_Initiator_t PHCS_BFL_MEMLOC_REM *cif   = (phcsBflNfc_Initiator_t*) set_trx_properties_param->self;
    phcsBflNfc_InitiatorCommParams_t
                       PHCS_BFL_MEMLOC_REM *td = (phcsBflNfc_InitiatorCommParams_t*)
                                                                (cif->mp_Members);
    uint8_t
        PHCS_BFL_MEMLOC_REM tx_preamble_length = (uint8_t)
                                                    (set_trx_properties_param->tx_preamble_type >> 4);
    uint8_t
        PHCS_BFL_MEMLOC_REM rx_preamble_length = (uint8_t)
                                                    (set_trx_properties_param->rx_preamble_type >> 4);

    //uint8_t PHCS_BFL_MEMLOC_REM max_preamble_size;

    /* Adjust overall TRx buffer: */
    td->mp_TRxBuffer       = set_trx_properties_param->p_trxbuffer;
    td->m_MaxXchBufferSize = set_trx_properties_param->trxbuffersize;
    /* las083 */
    //max_preamble_size = (uint8_t)
    //    (tx_preamble_length > rx_preamble_length ? tx_preamble_length : rx_preamble_length);

    td->m_MaxPduBufferSize = (uint16_t)(td->m_MaxXchBufferSize -
                                (tx_preamble_length > rx_preamble_length ?
                                    tx_preamble_length : rx_preamble_length));

    /*
     * No checks are done with the preamble (done later, when inserting/removing it):
     * Types are initialised in any case!
     */
    td->m_RXPreambleType = set_trx_properties_param->rx_preamble_type;
    td->m_TXPreambleType = set_trx_properties_param->tx_preamble_type;

}


void ICACHE_FLASH_ATTR
phcsBflNfc_InitiatorResetProt(phcsBflNfc_InitiatorResetParam_t *rst_param)
{
    phcsBflNfc_InitiatorCommParams_t PHCS_BFL_MEMLOC_REM *td =
        (phcsBflNfc_InitiatorCommParams_t*)(((phcsBflNfc_Initiator_t*)(rst_param->self))->mp_Members);

    /* Same functionality is used in phcsBflNfc_InitiatorDslRlsCombiRequest. This function could not
       be used because of incompatibility of the input parameters.
       !!* Check that function if there are some modifications done herein *!!  */
    td->m_BlNr     = 0;
    td->m_NadUsed  = 0;
    td->m_CtrlWord = NFC_DEP_STATE_RS;
}

/* E.O.F. */
