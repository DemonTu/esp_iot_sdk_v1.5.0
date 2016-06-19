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

/*! \file phcsBflNfc_Target.c
 *
 * Project: NFC Target basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflNfc_Target.c
 * $Author: bs $
 * $Revision: 1.11 $
 * $Date: Wed Jul  5 14:36:19 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated 25. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 *
 */
 
#include <phcsBflNfcCommon.h>
#include "phcsBflNfc_CommonInt.h"
#include "phcsBflNfc_TargetInt.h"


/* Macro to make the source code around the endpoint parameter initialisation appear smaller: */
#define PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE ((uint8_t)(0 | ((td->m_CtrlWord & NFC_TARGET_CTRL_PASSIVE_MODE) ? 0 : PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE))); /* */


/**************************************************************************************************/
/* Prototypes of local ("private") functions: */
/**************************************************************************************************/

static phcsBfl_Status_t phcsBflNfc_TargetAtrResponse(phcsBflNfc_Target_t                     *cif,
                                            phcsBflNfc_TargetCommParam_t  *td,
                                            uint8_t                         *buffer,
                                            uint16_t                        *length);

static phcsBfl_Status_t phcsBflNfc_TargetPslResponse(phcsBflNfc_Target_t                        *cif,
                                            phcsBflNfc_TargetCommParam_t *td,
                                            uint8_t                       *buffer,
                                            uint16_t                      *length);

static phcsBfl_Status_t phcsBflNfc_TargetDepResponse(phcsBflNfc_Target_t           *cif,
                                            phcsBflNfc_TargetCommParam_t *td,
                                            uint8_t               *buffer,
                                            uint16_t              *length);

static phcsBfl_Status_t phcsBflNfc_TargetWupResponse(phcsBflNfc_Target_t           *cif,
                                            phcsBflNfc_TargetCommParam_t *td,
                                            uint8_t               *buffer,
                                            uint16_t              *length);

static phcsBfl_Status_t phcsBflNfc_TargetDslResponse(phcsBflNfc_Target_t           *cif,
                                            phcsBflNfc_TargetCommParam_t *td,
                                            uint8_t               *buffer,
                                            uint16_t              *length);

static phcsBfl_Status_t phcsBflNfc_TargetRlsResponse(phcsBflNfc_Target_t           *cif,
                                            phcsBflNfc_TargetCommParam_t *td,
                                            uint8_t               *buffer,
                                            uint16_t              *length);

/* /////////////////////////////////////////////////////////////////////////////////////////////////
// NFC phcsBflNfc_TargetCheckRequestDid: Check optional DID byte.
// IN:      * Pointer to receive buffer.
//          * Length of buffer.
//          * Value to compare length with.
//
// RETURN:  * phcsBfl_Status_t code.
*/
static phcsBfl_Status_t phcsBflNfc_TargetCheckRequestDid(phcsBflNfc_TargetCommParam_t *td,
                                                        uint8_t                       *buffer,
                                                        uint16_t                      *length,
                                                        uint16_t                       compare_length);

/* /////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Data Exchange Protocol Chaining
//
// RETURN:  * phcsBfl_Status_t code.
*/
static phcsBfl_Status_t phcsBflNfc_TargetDepChaining(phcsBflNfc_Target_t            *cif,
                                            phcsBflNfc_TargetCommParam_t  *td,
                                            uint8_t                *action_type,
                                            uint8_t                *mi_present,
                                            uint8_t                 nad_presence,
                                            uint8_t                *trx_buffer,
                                            uint16_t               *trx_buffer_size,
                                            uint8_t                 header_size); 


/* /////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Deselect / Release Combi Response:
// IN/OUT:  * C-Interface structure.
//          * Internal variable struct..
//          * Pointer to received bytes.
//          * Number of received bytes.
//          * Type of operation.
//
// RETURN:  * phcsBfl_Status_t code.
*/
static phcsBfl_Status_t phcsBflNfc_TargetDslRlsCombiResponse(phcsBflNfc_Target_t           *cif,
                                                    phcsBflNfc_TargetCommParam_t *td,
                                                    uint8_t               *buffer,
                                                    uint16_t              *length,
                                                    uint8_t                combi_req_action_type);


/* /////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Remove Incoming (REQ) preamble:
// IN/OUT:  * Target Dispatch Parameter.
//
*/
static void phcsBflNfc_TargetRemIncomingPreamble(phcsBflNfc_TargetDispatchParam_t *dispatch_param);


/* /////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Target Auxiliary Transmit Function:
// IN/OUT:  * Target Object Pointer.
//          * Response buffer.
//          * Number of valid bytes in the response buffer.
//
*/
static phcsBfl_Status_t phcsBflNfc_TargetTxAux(phcsBflNfc_Target_t    *target_object,
                                      uint8_t        *res_buffer,
                                      uint16_t       *res_buffer_size);


/* /////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Target Auxiliary Reset function (used by ResetProt and in reponse to DSL_REQ):
// IN:  * Internal member var's.
//      * State to be set to.
//      * Mode to be applied (if 1: passive).
//
*/
static void phcsBflNfc_TargetResetProtAux(phcsBflNfc_TargetCommParam_t   *td,
                                   uint8_t                  state,
                                   uint8_t                  passive,
                                   uint8_t                  allow_atr);


/**************************************************************************************************/
/* Target functionality (Initialisation, entry point): */
/**************************************************************************************************/

void phcsBflNfc_TargetInit(phcsBflNfc_Target_t               *cif,
                              phcsBflNfc_TargetCommParam_t     *td,
                              uint8_t                    req_preamble_type,
                              uint8_t                    res_preamble_type,
                              uint8_t                   *p_target_buffer,
                              uint16_t                   target_buffer_size,
                              uint8_t                   *p_nfcidi_buffer,
                              uint8_t                   *p_nfcidt_buffer,
                              uint8_t                    passive_mode,
                              phcsBflNfc_TargetEndpoint_t      *p_upper,
                              phcsBflIo_t                 *p_lower)

{
    phcsBflNfc_TargetResetParam_t          PHCS_BFL_MEMLOC_REM rpp;     
    phcsBflNfc_TargetSetTRxBufPropParam_t  PHCS_BFL_MEMLOC_REM stpp;
    phcsBflNfc_TargetSetPUserParam_t       PHCS_BFL_MEMLOC_REM spup;

    /* Link protocol variables to the interface: */
    cif->mp_Members           = td;

    /* Initialise member functions and vars: */
    cif->Dispatch       = phcsBflNfc_TargetDispatch;
    cif->ResetProt      = phcsBflNfc_TargetResetProt;
    cif->SetUserBuf     = phcsBflNfc_TargetSetUserBuf;
    cif->SetTRxBufProp  = phcsBflNfc_TargetSetTRxBufProp;
    cif->SetPUser       = phcsBflNfc_TargetSetPUser;
    cif->mp_Lower       = p_lower;
    cif->mp_Upper       = p_upper;
    
    /* Reset internals: */
    td->m_MaxXchBufferSize    = NFC_DEFAULT_TRX_BUF_SIZE;      /* Set default. */
    td->mp_TargetBuffer       = p_target_buffer;
    td->m_TargetBufferSize    = 0;
    td->m_SizeOfLastBlock     = 0;
    td->m_MaxTargetBufferSize = target_buffer_size;
    td->m_NextStartIndex      = 0;
    td->mp_NfcIDi             = p_nfcidi_buffer;
    td->mp_NfcIDt             = p_nfcidt_buffer; 

    /* Reset the protocol: */
    rpp.passive_mode          = passive_mode;
    rpp.self                  = cif;
    phcsBflNfc_TargetResetProt(&rpp);
    
    /* Buffer preamble: */
    stpp.max_trx_buffer_size  = NFC_DEFAULT_TRX_BUF_SIZE;
    stpp.req_preamble_type    = req_preamble_type;
    stpp.res_preamble_type    = res_preamble_type;
    stpp.self                 = cif;
    phcsBflNfc_TargetSetTRxBufProp(&stpp);

    /* User pointer: */
    spup.p_user               = NULL;
    spup.self                 = cif;
    phcsBflNfc_TargetSetPUser(&spup);
}


void phcsBflNfc_TargetResetProtAux(phcsBflNfc_TargetCommParam_t   *td,
                            uint8_t                  state,
                            uint8_t                  passive,
                            uint8_t                  allow_atr)
{
    td->m_BlNr                = 0;
    td->m_State               = state;
    td->m_NadUsed             = 0;
    td->m_LastAction          = 0;

    /* Mode: */
	td->m_CtrlWord = 0;
	
	if (passive)
    {
        td->m_CtrlWord |= NFC_TARGET_CTRL_PASSIVE_MODE;
    } else
    {
        td->m_CtrlWord &= ~NFC_TARGET_CTRL_PASSIVE_MODE;
    }

    if (allow_atr)
    {
        td->m_CtrlWord |= NFC_TARGET_CTRL_ALLOW_ATR;
    } else
    {
        td->m_CtrlWord &= NFC_TARGET_CTRL_ALLOW_ATR_I;
    }

	/* Reset target buffer length */
	td->m_TargetBufferSize = 0;
}



void phcsBflNfc_TargetResetProt(phcsBflNfc_TargetResetParam_t *reset_param)
{
    phcsBflNfc_Target_t                        PHCS_BFL_MEMLOC_REM *cif = (phcsBflNfc_Target_t*) reset_param->self;
	phcsBflNfc_TargetCommParam_t PHCS_BFL_MEMLOC_REM *td  = (phcsBflNfc_TargetCommParam_t*) cif->mp_Members;
    phcsBflNfc_TargetResetProtAux(td,
                                        NFC_TARGET_INITIAL,            /* Initial state. */
                                        reset_param->passive_mode,     /* Mode from reset_param. */
                                        1);                            /* Initially: ATR allowed. */
}


void phcsBflNfc_TargetSetUserBuf(phcsBflNfc_TargetSetUserBufParam_t *user_buffer_param)
{
    phcsBflNfc_Target_t                        PHCS_BFL_MEMLOC_REM *cif = (phcsBflNfc_Target_t*) user_buffer_param->self;
    phcsBflNfc_TargetCommParam_t PHCS_BFL_MEMLOC_REM *td  = (phcsBflNfc_TargetCommParam_t*) cif->mp_Members;

    td->mp_TargetBuffer       = user_buffer_param->p_target_buffer;
    td->m_MaxTargetBufferSize = user_buffer_param->target_buffer_size;
}


void phcsBflNfc_TargetSetPUser(phcsBflNfc_TargetSetPUserParam_t *p_user_param)
{
    phcsBflNfc_Target_t                        PHCS_BFL_MEMLOC_REM *cif = (phcsBflNfc_Target_t*) p_user_param->self;
    phcsBflNfc_TargetCommParam_t PHCS_BFL_MEMLOC_REM *td  = (phcsBflNfc_TargetCommParam_t*) cif->mp_Members;

    td->mp_User = p_user_param->p_user;
}


phcsBfl_Status_t phcsBflNfc_TargetSetTRxBufProp(phcsBflNfc_TargetSetTRxBufPropParam_t *trx_buffer_param)
{
    phcsBflNfc_Target_t PHCS_BFL_MEMLOC_REM *cif                = (phcsBflNfc_Target_t*) trx_buffer_param->self;
    phcsBflNfc_TargetCommParam_t
        PHCS_BFL_MEMLOC_REM *td                 = (phcsBflNfc_TargetCommParam_t*) cif->mp_Members;
    uint8_t
        PHCS_BFL_MEMLOC_REM req_preamble_length = (uint8_t)
                                                      (trx_buffer_param->req_preamble_type >> 4);
    uint8_t
        PHCS_BFL_MEMLOC_REM res_preamble_length = (uint8_t)
                                                      (trx_buffer_param->res_preamble_type >> 4);

    td->m_MaxXchBufferSize = trx_buffer_param->max_trx_buffer_size;
    td->m_MaxPduBufferSize = (uint8_t)(td->m_MaxXchBufferSize -
                                (req_preamble_length > res_preamble_length ?
                                    req_preamble_length : res_preamble_length));

    td->m_ReqPreambleType = trx_buffer_param->req_preamble_type;
    td->m_ResPreambleType = trx_buffer_param->res_preamble_type;

    return PH_ERR_BFL_SUCCESS;
}


phcsBfl_Status_t phcsBflNfc_TargetDispatch(phcsBflNfc_TargetDispatchParam_t *dispatch_param)
{
    phcsBfl_Status_t                    PHCS_BFL_MEMLOC_REM  status         = PH_ERR_BFL_SUCCESS;
    uint8_t                             PHCS_BFL_MEMLOC_REM  action_type;
    phcsBflNfc_Target_t                 PHCS_BFL_MEMLOC_REM *cif = (phcsBflNfc_Target_t*) dispatch_param->self;
    phcsBflNfc_TargetCommParam_t        PHCS_BFL_MEMLOC_REM *td  = (phcsBflNfc_TargetCommParam_t*) cif->mp_Members;
    phcsBflNfc_TargetEndpointParam_t    PHCS_BFL_MEMLOC_REM  endpoint_parameter;
    phcsBflNfc_TargetResetParam_t       PHCS_BFL_MEMLOC_REM  reset_param;

    if (dispatch_param->buffer != NULL)
    {
        /* First throw away the preamble and don't even look at it: From now on the protocol will
         * merely see the PDU buffer, not the entire exchange buffer as provided by the system.
         */
        phcsBflNfc_TargetRemIncomingPreamble(dispatch_param);
        
        if ((dispatch_param->length > NFC_CMD1_POS) && 
            (dispatch_param->length == (dispatch_param->buffer)[NFC_LEN_POS]) &&
            ((dispatch_param->buffer)[NFC_CMD0_POS] == PHCS_BFLNFCCOMMON_REQ))
        {
            /*
             * Check if received request is valid at the current protocol state:
             * Manipulate and check against state control variable:
             */
            if ((0x01 << ((dispatch_param->buffer)[NFC_CMD1_POS] >> 1)) & td->m_State)
            {
                switch ((dispatch_param->buffer)[NFC_CMD1_POS])
                {
                    case PHCS_BFLNFCCOMMON_ATR_REQ:
                        status = phcsBflNfc_TargetAtrResponse(cif,
                                                                    td,
                                                                    dispatch_param->buffer,
                                                                    &(dispatch_param->length));

                        if (status == PH_ERR_BFL_SUCCESS)
                        {
                            td->m_State = NFC_TARGET_IN_PROTOCOL;
                            /* Set bit to enable PSL_REQ as first cmd after ATR_REQ.   */
                            td->m_CtrlWord |= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR;
                        }
                        /* Allow further ATR_RE*: */
                        td->m_CtrlWord |= NFC_TARGET_CTRL_ALLOW_ATR;
                        break;

                    case PHCS_BFLNFCCOMMON_WUP_REQ:
                        status = phcsBflNfc_TargetWupResponse(cif,
                                                                 td,
                                                                 dispatch_param->buffer,
                                                                 &(dispatch_param->length));
                        if (status == PH_ERR_BFL_SUCCESS)
                        {
                            /* We have received a valid WUP_REQ with matching DID: */
                            td->m_State = NFC_TARGET_IN_PROTOCOL;
                            /* Set First-Request bit, thus PSL_REQ is enabled. */
                            td->m_CtrlWord |= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR;
                            /* Disable further ATR_RE*: */
                            td->m_CtrlWord &= NFC_TARGET_CTRL_ALLOW_ATR_I;
                        }
                        break;

                    case PHCS_BFLNFCCOMMON_PSL_REQ:
                        status = phcsBflNfc_TargetPslResponse(cif,
                                                                            td,
                                                                            dispatch_param->buffer,
                                                                            &(dispatch_param->length));
                        /* Clear First-Request bit, thus PSL_REQ is disabled. */
                        td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
                        /* Disable further ATR_RE*: */
                        td->m_CtrlWord &= NFC_TARGET_CTRL_ALLOW_ATR_I;
                        break;

                    case PHCS_BFLNFCCOMMON_DEP_REQ:
                        status = phcsBflNfc_TargetDepResponse(cif,
                                                                 td,
                                                                 dispatch_param->buffer,
                                                                 &(dispatch_param->length));
                        /* Clear First-Request bit, thus PSL_REQ is disabled. */
                        td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
                        /* Disable further ATR_RE*: */
                        td->m_CtrlWord &= NFC_TARGET_CTRL_ALLOW_ATR_I;
                        break;

                    case PHCS_BFLNFCCOMMON_DSL_REQ:
                        status = phcsBflNfc_TargetDslResponse(cif,
                                                                    td,
                                                                    dispatch_param->buffer,
                                                                    &(dispatch_param->length));
                        if (status != PH_ERR_BFL_SUCCESS)
                        {
                            /* Error: Clear First-Request bit, thus PSL_REQ is disabled. */
                            td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
                        }
                        break;

                    case PHCS_BFLNFCCOMMON_RLS_REQ:
                        status = phcsBflNfc_TargetRlsResponse(cif,
                                                                    td,
                                                                    dispatch_param->buffer,
                                                                    &(dispatch_param->length));
                        if (status != PH_ERR_BFL_SUCCESS)
                        {
                            /* Error: Clear First-Request bit, thus PSL_REQ is disabled. */
                            td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
                        }
                        break;

                    default:
                        status = PH_ERR_BFL_UNSUPPORTED_COMMAND;
                        break;
                }

                if (status == PH_ERR_BFL_SUCCESS)
                {
                    /*
                     * Request valid: Build response and send it back to the Initiator using
                     * a Transmit() function from Io
                     */
                    (dispatch_param->buffer)[NFC_LEN_POS] = (uint8_t)(dispatch_param->length);
                    (dispatch_param->buffer)[NFC_CMD0_POS] += 1;
                    (dispatch_param->buffer)[NFC_CMD1_POS] += 1;

                    /* Send Response, including preamble handling: */
                    status = phcsBflNfc_TargetTxAux(cif, dispatch_param->buffer, &(dispatch_param->length));
                   
                    /*
                     * Jump into PSL callback a second time in case of a PSL request. This enables
                     * the Target to set its new communication Parameters after successfully handled
                     * the Initiator request to do so.
                     */
                    if (td->m_CtrlWord & NFC_TARGET_CTRL_PSLCB_REQ_TWICE)
                    {
                        /*
                         * Build structure for NFC-Target Endpoint callback:
                         */
                        action_type = PHCS_BFLNFC_TARGET_SECOND_PSLCB;

                        endpoint_parameter.flags              = PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
                        endpoint_parameter.action_type        = action_type;
                        endpoint_parameter.nad                = NULL;
                        endpoint_parameter.p_req_data         = NULL;
                        endpoint_parameter.p_res_data         = NULL;
                        endpoint_parameter.req_data_length    = 0;
                        endpoint_parameter.res_data_length    = 0;
                        endpoint_parameter.self               = cif->mp_Upper;
                        endpoint_parameter.target             = cif;
                        endpoint_parameter.p_user             = td->mp_User;
                        
                        status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);

                        /* Clear bit: */
                        td->m_CtrlWord &= ~NFC_TARGET_CTRL_PSLCB_REQ_TWICE;
                    } else if (td->m_CtrlWord & NFC_TARGET_CTRL_DSLCB_REQ2)
                    {
                        /* reset protocol here */
                        if (td->m_CtrlWord & NFC_TARGET_CTRL_PASSIVE_MODE)
                        {
                            /* Passive mode is used. */
                            phcsBflNfc_TargetResetProtAux(td,
                                                 NFC_TARGET_INITIAL,
                                                 1,                     /* Passive. */
                                                 1);                    /* ATR allowed. */
                        } else
                        {
                            /* Active mode is used.  */
                            phcsBflNfc_TargetResetProtAux(td,
                                                 NFC_TARGET_DESELECTED_A,
                                                 0,                     /* Active. */
                                                 0);                    /* ATR OFF. */
                        }
                        /*
                         * Build structure for NFC-Target Endpoint callback:
                         */
                        action_type = PHCS_BFLNFC_TARGET_SECOND_DSLCB;

                        endpoint_parameter.flags              = PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
                        endpoint_parameter.action_type        = action_type;
                        endpoint_parameter.nad                = NULL;
                        endpoint_parameter.p_req_data         = NULL;
                        endpoint_parameter.p_res_data         = NULL;
                        endpoint_parameter.req_data_length    = 0;
                        endpoint_parameter.res_data_length    = 0;
                        endpoint_parameter.self               = cif->mp_Upper;
                        endpoint_parameter.target             = cif;
                        endpoint_parameter.p_user             = td->mp_User;
                        
                        status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);

                        /* Clear bit: */
                        td->m_CtrlWord &= ~NFC_TARGET_CTRL_DSLCB_REQ2;
                    } else if (td->m_CtrlWord & NFC_TARGET_CTRL_RLSCB_REQ2)
                    {
                        /*
                         * Build structure for NFC-Target Endpoint callback:
                         */
                        action_type = PHCS_BFLNFC_TARGET_SECOND_RLSCB;

                        endpoint_parameter.flags              = PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
                        endpoint_parameter.action_type        = action_type;
                        endpoint_parameter.nad                = NULL;
                        endpoint_parameter.p_req_data         = NULL;
                        endpoint_parameter.p_res_data         = NULL;
                        endpoint_parameter.req_data_length    = 0;
                        endpoint_parameter.res_data_length    = 0;
                        endpoint_parameter.self               = cif->mp_Upper;
                        endpoint_parameter.target             = cif;
                        endpoint_parameter.p_user             = td->mp_User;
                        
                        status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);

                        /* Clear bit: */
                        td->m_CtrlWord &= ~NFC_TARGET_CTRL_RLSCB_REQ2;

                        /* Reset the protocol: */
                        reset_param.self = cif;
                        reset_param.passive_mode = (uint8_t)
                            (td->m_CtrlWord & NFC_TARGET_CTRL_PASSIVE_MODE ? 1 : 0);
                        
                        phcsBflNfc_TargetResetProt(&reset_param);
                    }else
                    {
                        /* do nothing in that case */
                    }
                } else
                {
                    /*
                     * No response, if internal states, block numbers, return values of
                     * internal functions and the like indicate an error. 
                     * Clear First-Request bit, thus PSL_REQ is disabled.
                     * Merely return the status.
                     */
                    if (td->m_State == NFC_TARGET_IN_PROTOCOL)
                    {
                        td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
                    }
                    dispatch_param->length = 0;
                }
            } else
            {
                /*
                 * No response due to invalid request (not allowed in current state).
                 * Clear First-Request bit, thus PSL_REQ is disabled.
                 */
                if (td->m_State == NFC_TARGET_IN_PROTOCOL)
                {
                    td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
                }
                dispatch_param->length = 0;
                status = PH_ERR_BFL_INVALID_DEVICE_STATE;
            }
        } else
        {
            /*
             * No response due to invalid length (erroneously composed PDU).
             * Clear First-Request bit, thus PSL_REQ is disabled.
             */
            if (td->m_State == NFC_TARGET_IN_PROTOCOL)
            {
                td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
            }
            dispatch_param->length = 0;
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* Buffer is NULL, thus no response is possible. */
        if (td->m_State == NFC_TARGET_IN_PROTOCOL)
        {
            td->m_CtrlWord &= NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I;
        }
        dispatch_param->length = 0;
        status = PH_ERR_BFL_BUF_2_SMALL;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}



/**************************************************************************************************/
/* "Private" functions (support facility): */
/**************************************************************************************************/

static void phcsBflNfc_TargetRemIncomingPreamble(phcsBflNfc_TargetDispatchParam_t *dispatch_param)
{
    phcsBflNfc_TargetCommParam_t PHCS_BFL_MEMLOC_REM *td = (phcsBflNfc_TargetCommParam_t*)
        ((phcsBflNfc_Target_t*)(dispatch_param->self))->mp_Members;
    uint8_t       PHCS_BFL_MEMLOC_REM   req_preamble_length;
    uint16_t      PHCS_BFL_MEMLOC_COUNT    loopcount;
    uint16_t      PHCS_BFL_MEMLOC_REM   req_buffer_size;
    uint8_t       PHCS_BFL_MEMLOC_BUF    *req_buffer;

    /* Throw away RX preamble (don't look at it) by moving th buffer back to [0]. */
    req_preamble_length = (uint8_t)(td->m_ReqPreambleType >> 4);
    if (req_preamble_length > 0)
    {
        if (dispatch_param->length > req_preamble_length)
        {
            req_buffer_size = (uint16_t)(dispatch_param->length - req_preamble_length);
            req_buffer = dispatch_param->buffer;
            for (loopcount = 0; loopcount < req_buffer_size; loopcount++)
            {
                req_buffer[loopcount] = req_buffer[loopcount + req_preamble_length];
            }
        } else
        {
            /* We do nothing with the buffer since it is not larger than the preamble.
             * This potentially invalid buffer content shall be handled by the further
             * checks in the Dispatch function.
             */
            req_buffer_size = dispatch_param->length;
        }
    } else
    {
        req_buffer_size = dispatch_param->length;
    }
    dispatch_param->length = req_buffer_size;
}


static phcsBfl_Status_t phcsBflNfc_TargetTxAux(phcsBflNfc_Target_t    *target_object,
                                      uint8_t        *res_buffer,
                                      uint16_t       *res_buffer_size)
{
    phcsBflNfc_TargetCommParam_t PHCS_BFL_MEMLOC_REM *td = (phcsBflNfc_TargetCommParam_t*) target_object->mp_Members;
    phcsBflIo_TransmitParam_t   PHCS_BFL_MEMLOC_REM  transmit_param;
    uint8_t         PHCS_BFL_MEMLOC_REM  res_preamble_length;
    uint16_t        PHCS_BFL_MEMLOC_COUNT   loopcount;

    /* Move the buffer to [tx_preamble_length] in order to make the preamble fit: */
    res_preamble_length = (uint8_t)(td->m_ResPreambleType >> 4);
    if (res_preamble_length > 0)
    {
        loopcount = *res_buffer_size;
        do
        {
            loopcount--;
            res_buffer[loopcount + res_preamble_length] = res_buffer[loopcount];

        } while (loopcount);
        *res_buffer_size = (uint16_t)(*res_buffer_size + res_preamble_length);

        /* Fill in RES preamble: */
        switch (td->m_ResPreambleType)
        {
            case PHCS_BFLNFCCOMMON_PREAM_TYPE1:
                res_buffer[0] = PHCS_BFLNFCCOMMON_PREAM_1BYTE;
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

    
    /* Transmit RESPONSE: */
    transmit_param.self                 = target_object->mp_Lower;
    transmit_param.tx_buffer            = res_buffer;
    transmit_param.tx_buffer_size       = *res_buffer_size;
    return target_object->mp_Lower->Transmit(&transmit_param);
}




static phcsBfl_Status_t phcsBflNfc_TargetAtrResponse(phcsBflNfc_Target_t             *cif,
                                            phcsBflNfc_TargetCommParam_t   *td,
                                            uint8_t                 *buffer,
                                            uint16_t                *length)
{
    phcsBfl_Status_t              PHCS_BFL_MEMLOC_REM status              = PH_ERR_BFL_SUCCESS;
    phcsBflNfc_TargetEndpointParam_t   PHCS_BFL_MEMLOC_REM endpoint_parameter;
    uint8_t                     PHCS_BFL_MEMLOC_COUNT  index;
    uint8_t                     PHCS_BFL_MEMLOC_COUNT  loopcount;
    uint8_t                     PHCS_BFL_MEMLOC_REM action_type;

    if (td->m_CtrlWord & NFC_TARGET_CTRL_ALLOW_ATR)
    {
        /* Get data out of received buffer: */
        index = NFC_ATR_NFCID_POS;
        /* Read out NFCIDi: */
        for (loopcount = 0; loopcount < PHCS_BFLNFCCOMMON_ID_LEN; loopcount++)
        {
            td->mp_NfcIDi[loopcount] = buffer[index];
            index++;
        }

        /* Get all params out of the received data. */
        td->m_DID = buffer[index];
        index++;

        *length = (uint16_t)(*length - index);
        action_type = PHCS_BFLNFCCOMMON_ATR_REQ;

        /* Now we are at the position of the first byte after the DID: */
    
        /* Call the protocol endpoint: */
        endpoint_parameter.flags                =  PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
        endpoint_parameter.action_type          =  action_type;
        endpoint_parameter.nad                  =  NULL;               /* No NAD used*/
        endpoint_parameter.p_req_data           = &buffer[index];      /* Protocol itself handles NFCIDs, DIDs! */
        endpoint_parameter.p_res_data           = &buffer[index];
        endpoint_parameter.req_data_length      = *length;
        endpoint_parameter.res_data_length      =  
            (uint16_t)(td->m_MaxPduBufferSize - index);           /* Max # bytes to return.    */
        endpoint_parameter.self                 =  cif->mp_Upper;
        endpoint_parameter.target               =  cif;
        endpoint_parameter.p_user               =  td->mp_User;
        status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);
        action_type     = endpoint_parameter.action_type;
        /* We're done with the call! */

        if (status == PH_ERR_BFL_SUCCESS)
        {
            /* Restore length including header: */
            *length = (uint16_t)(endpoint_parameter.res_data_length + index);

            index = NFC_ATR_NFCID_POS;
            /* Fill in NFCIDt: */
            for (loopcount = 0; loopcount < PHCS_BFLNFCCOMMON_ID_LEN; loopcount++)
            {
                buffer[index] = td->mp_NfcIDt[loopcount];
                index++;
            }
        } else
        {
            *length = 0;
        }
    } else
    {
        /* The device is in a state which does not accept such requests: */
        status = PH_ERR_BFL_INVALID_DEVICE_STATE;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static phcsBfl_Status_t phcsBflNfc_TargetPslResponse(phcsBflNfc_Target_t    *cif,
                                            phcsBflNfc_TargetCommParam_t    *td,
                                            uint8_t                         *buffer,
                                            uint16_t                        *length)
{
    phcsBfl_Status_t                    PHCS_BFL_MEMLOC_REM status              = PH_ERR_BFL_SUCCESS;
    phcsBflNfc_TargetEndpointParam_t    PHCS_BFL_MEMLOC_REM endpoint_parameter;
    /* Index variable if DID used. Set index to the parameter position. */
    uint16_t                            PHCS_BFL_MEMLOC_COUNT  index = NFC_PSL_DID_POS + 1;
    uint8_t                             PHCS_BFL_MEMLOC_REM action_type;

    if (td->m_CtrlWord & NFC_TARGET_CTRL_1ST_REQ_AFT_ATR)
    {
        if (td->m_DID != 0)
        {
            if (*length != NFC_MAX_PSL_REQ_LEN || td->m_DID != buffer[NFC_DSL_DID_POS])
            {
                /* Wrong DID was received or wrong length, do not response! */
                status = PH_ERR_BFL_INVALID_FORMAT;
            } else
            {   /* Did OK! Correct value already set during initialisation! */
            }
        } else
        {
            if (*length != NFC_MIN_PSL_REQ_LEN)
            {
                /* DID was expected but not received! */
                status = PH_ERR_BFL_INVALID_FORMAT;
            } else
            {   /* Length OK! */
                /* Set index: */
                index = NFC_PSL_DID_POS;    /* DID not used. Set index to the parameter position. */
            }
        }

        if (status == PH_ERR_BFL_SUCCESS)
        {
            /* Prepare for endpoint: */
            action_type = PHCS_BFLNFCCOMMON_PSL_REQ;
            *length = (uint16_t)(*length - index);
            /* Call the protocol endpoint: */
            endpoint_parameter.flags                =  PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
            endpoint_parameter.action_type          =  action_type;
            endpoint_parameter.nad                  =  NULL;               /* No NAD used*/
            endpoint_parameter.p_req_data           = &(buffer[index]);
            endpoint_parameter.p_res_data           =  NULL;               /* PSL_RES does not return data. */
            endpoint_parameter.req_data_length      = *length;
            endpoint_parameter.res_data_length      =  0;
            endpoint_parameter.self                 =  cif->mp_Upper;
            endpoint_parameter.target               =  cif;
            endpoint_parameter.p_user               =  td->mp_User;
            status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);
            action_type     = endpoint_parameter.action_type;
            /* We're done with the call! */

            if (td->m_DID != 0)
            {
                *length = NFC_MAX_PSL_RES_LEN;
            } else
            {
                *length = NFC_MIN_PSL_RES_LEN;
            }
            td->m_CtrlWord |= NFC_TARGET_CTRL_PSLCB_REQ_TWICE;
        } else
        {
            /* Different DID was received, no response! */
            status = PH_ERR_BFL_INVALID_PARAMETER;
        }
    } else
    {
        /*
         * Is not the first request after ATR-REQ: The device has already left the state in which
         * it is ready to acept a PSL-REQ.
         */
        status = PH_ERR_BFL_INVALID_DEVICE_STATE;
    }

    if (status != PH_ERR_BFL_SUCCESS)
    {
        *length = 0;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static phcsBfl_Status_t phcsBflNfc_TargetDepResponse(phcsBflNfc_Target_t              *cif,
                                            phcsBflNfc_TargetCommParam_t    *td,
                                            uint8_t                  *buffer,
                                            uint16_t                 *length)
{
    #define                     NFCT_DEPR_INCOMING_NAD_PRESENT                      0x01 /* */
    #define                     NFCT_DEPR_OUTGOING_NAD_PRESENT                      0x02 /* */
    #define                     NFCT_DEPR_NO_NAD_PRESENT                            0x00 /* */

    phcsBfl_Status_t              PHCS_BFL_MEMLOC_REM status                          = PH_ERR_BFL_SUCCESS;
    phcsBflNfc_TargetEndpointParam_t   PHCS_BFL_MEMLOC_REM endpoint_parameter;
    uint8_t                     PHCS_BFL_MEMLOC_REM mi_present                      = 0;
    uint8_t                     PHCS_BFL_MEMLOC_COUNT  index;
    uint8_t                     PHCS_BFL_MEMLOC_REM action_type                     = PHCS_BFLNFC_TARGET_REQ_INF;
    uint8_t                     PHCS_BFL_MEMLOC_REM response_processing_required    = 0;
    uint8_t                     PHCS_BFL_MEMLOC_REM nad_presence                    = NFCT_DEPR_NO_NAD_PRESENT;

    /* Read out params of request:                                            */
    index = NFC_DEP_DID_POS;
    if (buffer[NFC_DEP_PFB_POS] & NFC_DEP_DID_PRESENT)
    {
        if (td->m_DID != buffer[NFC_DEP_DID_POS])
        {
            status = PH_ERR_BFL_INVALID_PARAMETER;
        }
        index++;
    }

    if (status == PH_ERR_BFL_SUCCESS)
    {
        /* DID was OK, check incoming frame NAD presence: */
        if (buffer[NFC_DEP_PFB_POS] & NFC_DEP_NAD_PRESENT)
        {
            /* NFC_TARGET_CTRL_MI_PRESENT: Set during incoming chaining.
             * m_NadUsed: Set upon detection of a NAD.
             */
            if ((td->m_NadUsed) || (td->m_CtrlWord & NFC_TARGET_CTRL_MI_PRESENT))
            {
                /* NAD usage error: NAD must only be sent with the first block, thus it's an error, if:
                 * - NAD is sent in subsequent blocks (again).
                 * - NAD is not sent in the first block, but in a later one.
                 */
                status = PH_ERR_BFL_INVALID_FORMAT;
            } else
            {
                /* OK: NadUsed is set during whole DEP-chaining. */
                td->m_NadUsed = 1;
                /* nad_present only set for the first DEP-chaining block. */
                nad_presence |= NFCT_DEPR_INCOMING_NAD_PRESENT;
                /* store NAD to internal buffer, index variable is set according to DID from above */
                td->m_NAD = buffer[index];
            }
        }

        if (status == PH_ERR_BFL_SUCCESS)
        {
            switch (buffer[NFC_DEP_PFB_POS] & NFC_PFB_7_5_MASK)
            {
                case NFC_DEP_ACKNOWLEDGE:
                    if (buffer[NFC_DEP_PFB_POS] & NFC_DEP_NACK)
                    {
                        /* NACK was received - decrement block number in case of no
                         * TOX is currently active.                                  */
                        if ((td->m_CtrlWord & NFC_TARGET_CTRL_TOX_ACTIVE) == 0)
                        {
                            if (td->m_BlNr > 0)
                            {
                                td->m_BlNr--;
                            } else
                            {
                                td->m_BlNr = 3;
                            }
                        }

                        /* If the previous response was a ACK, retransmit this ACK:  */
                        if (td->m_LastAction == PHCS_BFLNFC_TARGET_RES_ACK)
                        {
                            /* Just clear NACK bit and send it back with target PNI. */
                            buffer[NFC_DEP_PFB_POS] &= NFC_DEP_NACK_I;
                            /* Set PNI:                                              */
                            buffer[NFC_DEP_PFB_POS] &= NFC_DEP_PNI_MASK_I;
                            buffer[NFC_DEP_PFB_POS] |= td->m_BlNr;

                            /* Increment PNI:                                  */
                            td->m_BlNr = (uint8_t)((td->m_BlNr + 1) % 4); 
                            break;
                        }

                        if (td->m_CtrlWord & NFC_TARGET_CTLR_NAD_PREV_USED)
                        {
                            /* Clear flag which disables further nads, to enable appending a
                             * nad byte for the issued retransmission.                        */
                            td->m_CtrlWord &= NFC_TARGET_CTLR_NAD_PREV_USED_I;
                            /* We must tell the callback to handle buffer pointers accordingly:
                               Since we had a NAD in the previously lost outgoing block the
                               retransmission must take this into account. */
                            nad_presence |= NFCT_DEPR_OUTGOING_NAD_PRESENT;
                        }

                        action_type = PHCS_BFLNFC_TARGET_REQ_NACK;
                        response_processing_required = 1;
                    } else
                    {
                        /* Do nothing, just continue with response processing. */
                        response_processing_required = 1;
                        action_type = PHCS_BFLNFC_TARGET_REQ_ACK;
                    }
                    break;

                case NFC_DEP_INFORMATION:
                    /* Check if received block has correct PNI: */
                    if (td->m_BlNr != (buffer[NFC_DEP_PFB_POS] & NFC_DEP_PNI_MASK))
                    {
                        /* Wrong PNI number, don't send any response! */
                        response_processing_required = 0;
                        status = PH_ERR_BFL_INVALID_FORMAT;
                    } else
                    {
                        /* Check if DEP-chaining is used. */
                        if (buffer[NFC_DEP_PFB_POS] & NFC_DEP_MI_PRESENT)
                        {
                            mi_present = 1;
                        }

                        /* Auto chaining possible ??? */
                        if ((td->mp_TargetBuffer != NULL) &&
                            (td->m_MaxTargetBufferSize > 0))
                        {
                            action_type = PHCS_BFLNFCCOMMON_DEP_REQ;
                        } else
                        {
                            action_type = PHCS_BFLNFC_TARGET_REQ_INF;
                        }
                        response_processing_required = 1;
                    }

                    /* Clear Nad-present bit, used to send Nad only at first block.        */
                    td->m_CtrlWord &= NFC_TARGET_CTRL_NAD_PRESENT_I;
                    break;

                case NFC_DEP_SUPERVISORY:
                    if ((buffer[NFC_DEP_PFB_POS] & NFC_DEP_TO_EXT) == 0)
                    {
                        /* Response to Attention-Req is handled at the dispatcher above.   */
                        response_processing_required = 0;
                    } else
                    {
                        /* Timeout-Extension - Continue:                                   */
                        action_type = PHCS_BFLNFC_TARGET_REQ_TOX;
                        response_processing_required = 1;
                        /* Clear bit after TOX-response of the Initiator was received.     */
                        td->m_CtrlWord &= ~NFC_TARGET_CTRL_TOX_ACTIVE;
                        
                        /* Rebuild PNI here for correct handling of response data (received value shall be 0) */
                        buffer[NFC_DEP_PFB_POS] |= (td->m_BlNr & NFC_DEP_PNI_MASK);
                    }
                    break;

                default:
                    /* Do not reply to an unknown request:             */
                    status = PH_ERR_BFL_INVALID_FORMAT;
                    response_processing_required = 0;
                    break;
            }

            /* Check if any response processing has to be done.        */
            if (response_processing_required)
            {
                /* Variable length is without header but still including optional NAD-byte: */
                *length = (uint16_t)(*length - index);

                if ((td->m_NadUsed) &&
                    (!mi_present) &&
                    (!(td->m_CtrlWord & NFC_TARGET_CTLR_NAD_PREV_USED)))
                {
                    /* NAD presence must also be indicated when the last frame from the Initiator
                     * to the Target has been sent and the Target responds with its first INF block,
                     * containing a NAD, if the Initiator has sent one. In case of following INF
                     * responses the NAD must not be used.
                    */
                    nad_presence |= NFCT_DEPR_OUTGOING_NAD_PRESENT;
                }

                /* Used in case of special TOX handling separated from data exchange (rejump into DEP endpoint). */
                NFCT_TOX_RETURN_JMP:;
                
                /* Auto chaining possible ??? */
                if ((td->mp_TargetBuffer != NULL) &&
                    (td->m_MaxTargetBufferSize > 0))
                {
                    /* YES - Use this method only in case of a present internal DEP-chaining buffer. */
                    status = phcsBflNfc_TargetDepChaining(cif,
                                                   td,
                                                   &action_type, 
                                                   &mi_present, 
                                                   nad_presence,
                                                   buffer + index, 
                                                   length,
                                                   index);
                } else
                {
                    /* NOPE - Callback to Protocol-Endpoint: */
                    if (td->m_CtrlWord & NFC_TARGET_CTLR_NAD_PREV_USED)
                    {
                        /* 
                         * In non-automatic chaining, we do not need to use a NAD from now on since
                         * this information has been already sent back to the NFCI:
                         */
                        td->m_NadUsed = 0;
                    }
                    /* Call the protocol endpoint:
                     * If a NAD is present (originally the first data buffer byte) the begin is shifted
                     * by one byte in order to have the plain buffer available only.
                     * Since the modification of NAD behaviour and endpoint interface parameters
                     * is independent from the design of the protocol so far, this is the point
                     * where to link between the behaviour of the "old" part and the new endpoint.
                     */
                    endpoint_parameter.flags                =  PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
                    endpoint_parameter.flags               |=  mi_present ? PHCS_BFLNFC_TARGET_EP_FLAGS_MI_PRES : 0;
                    endpoint_parameter.action_type          =  action_type;
                    endpoint_parameter.p_req_data           =  
                        buffer + index + ((nad_presence & NFCT_DEPR_INCOMING_NAD_PRESENT) ? 1 : 0);
                    endpoint_parameter.p_res_data           =  
                        buffer + index + ((nad_presence & NFCT_DEPR_OUTGOING_NAD_PRESENT) ? 1 : 0);
                    endpoint_parameter.nad                  =  nad_presence ? (&td->m_NAD) : NULL;
                    endpoint_parameter.req_data_length      =  
                        (uint16_t)(*length - (nad_presence & NFCT_DEPR_INCOMING_NAD_PRESENT ? 1 : 0));
                    endpoint_parameter.res_data_length      =  
                        (uint16_t)(td->m_MaxPduBufferSize - index - (nad_presence & NFCT_DEPR_OUTGOING_NAD_PRESENT ? 1 : 0));
                    endpoint_parameter.self                 =  cif->mp_Upper;
                    endpoint_parameter.target               =  cif;
                    endpoint_parameter.p_user               =  td->mp_User;
                    status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);
                    action_type                             = endpoint_parameter.action_type;
                    mi_present                              = (uint8_t)(endpoint_parameter.flags & PHCS_BFLNFC_TARGET_EP_FLAGS_MI_PRES ? 1 : 0);
                    /* store NAD in the TX buffer in case it has been overwritten before only if NAD is used for send operation. */
                    if(action_type != PHCS_BFLNFC_TARGET_RES_TOX)
                    {
                        if(nad_presence & NFCT_DEPR_OUTGOING_NAD_PRESENT)
                            buffer[index] = td->m_NAD;
                        /* The "old" part considers NAD to be the first byte in the buffer. In the "new"
                         * endpoint however this is not the case. Length adjustment closes that gap.
                         */
                        *length = (uint16_t)(endpoint_parameter.res_data_length + (nad_presence & NFCT_DEPR_OUTGOING_NAD_PRESENT ? 1 : 0));
                    } else if (nad_presence)
                    {
                        /* copy TOX byte to position NAD only if NAD is used. */
                        buffer[index] = buffer[index+1];
                        *length = endpoint_parameter.res_data_length;
                    } else
                    {
                        *length = (uint16_t)(endpoint_parameter.res_data_length + (nad_presence & NFCT_DEPR_OUTGOING_NAD_PRESENT ? 1 : 0));
                    }
                    /* We're done with the call! */
                }
            
                /* Fall through by TOX endpoint operation with the same action type,
                 * Set up REQ_INF / DEP_REQ code for next operation.
                 * Do not send anything here! */
                if(action_type == PHCS_BFLNFC_TARGET_REQ_TOX)
                {
                    if ((td->mp_TargetBuffer != NULL) &&
                        (td->m_MaxTargetBufferSize > 0))
                    {
                        action_type = PHCS_BFLNFCCOMMON_DEP_REQ;
                    } else
                    {
                        action_type = PHCS_BFLNFC_TARGET_REQ_INF;
                    }
                    if(td->m_NadUsed)
                    {
                        *length = 1;
                    } else
                    {
                        *length = 0;
                    }
                    goto NFCT_TOX_RETURN_JMP;
                }

                /* Restore original header size: */
                *length = (uint16_t)(*length + index);
                if (*length > td->m_MaxPduBufferSize)
                {
                    /* Truncate buffer if too long: */
                    *length = td->m_MaxPduBufferSize;
                }

                /* Prepare response:             */
                switch (action_type)
                {
                    case PHCS_BFLNFC_TARGET_RES_TOX:
                        /* We return a length > 0 since the first info byte is RTOX. */
                        /* Clear bits 1-0:                             */
						buffer[NFC_DEP_PFB_POS] &= NFC_DEP_PNI_MASK_I;
                        /* Clear bits 5-7:                             */
                        buffer[NFC_DEP_PFB_POS] &= NFC_PFB_7_5_MASK_I;
                        /* Set to TOX frame:                           */
                        buffer[NFC_DEP_PFB_POS] |= NFC_DEP_SUPERVISORY;
                        buffer[NFC_DEP_PFB_POS] |= NFC_DEP_TO_EXT;
                        /* NAD is not used for TOX, so clear it.       */
                        buffer[NFC_DEP_PFB_POS] &= NFC_DEP_NAD_PRESENT_I;
                        /* If the buffer begin has been shifted by one byte because of NAD presence
                         * the value of timeout extension requested by the Target must be shifted
                         * back since it would be at the wrong position otherwise.
                         */

                        /* Set bit if TOX-request is active.           */
                        td->m_CtrlWord |= NFC_TARGET_CTRL_TOX_ACTIVE;
                        break;

                    case PHCS_BFLNFC_TARGET_RES_ACK:
                        /* Clear bits 5-7:                             */
                        buffer[NFC_DEP_PFB_POS] &= NFC_PFB_7_5_MASK_I;
                        /* Set to acknowledge frame:                   */
                        buffer[NFC_DEP_PFB_POS] |= NFC_DEP_ACKNOWLEDGE;
                        buffer[NFC_DEP_PFB_POS] &= NFC_DEP_NACK_I;

                        /* Calculate length of response:               */
                        *length = NFC_MIN_DEP_RES_LEN;
                        if (buffer[NFC_DEP_PFB_POS] & NFC_DEP_DID_PRESENT) (*length)++;

                        /* NAD is not used for ACK, so clear it.       */
                        buffer[NFC_DEP_PFB_POS] &= NFC_DEP_NAD_PRESENT_I;

                        /* Increment block number PNI:                 */
                        td->m_BlNr = (uint8_t)((td->m_BlNr + 1) % 4); 
                        
                       /* reset status flag for remembering DEP is needed after TOX */
                        td->m_CtrlWord &= ~NFC_TARGET_CTRL_DEP_AFTER_TOX;
                        break;

                    case PHCS_BFLNFCCOMMON_DEP_REQ:
                    case PHCS_BFLNFC_TARGET_RES_INF:
                        /* Clear bits 5-7:                             */
                        buffer[NFC_DEP_PFB_POS] &= NFC_PFB_7_5_MASK_I;
                        /* Set to information pdu:                     */
                        buffer[NFC_DEP_PFB_POS] |= NFC_DEP_INFORMATION;

                        /* Set NAD-bit in case of Nad is used and first 
                         * block is goinig to be sent:                 */                    
                        if (((td->m_CtrlWord & NFC_TARGET_CTRL_NAD_PRESENT) == 0) &&
                              td->m_NadUsed)
                        {
                            buffer[NFC_DEP_PFB_POS] |= NFC_DEP_NAD_PRESENT;
                            td->m_CtrlWord |= NFC_TARGET_CTLR_NAD_PREV_USED;
                            /* set NAD, index still unchaged from above */
                            buffer[index] = td->m_NAD; 
                        } else
                        {
                            buffer[NFC_DEP_PFB_POS] &= NFC_DEP_NAD_PRESENT_I;
                            td->m_CtrlWord &= NFC_TARGET_CTLR_NAD_PREV_USED_I;
                        }

                        /* Set/clear more-information bit if necessary:*/
                        if (mi_present)
                        {
                            buffer[NFC_DEP_PFB_POS] |= NFC_DEP_MI_PRESENT;
                            td->m_CtrlWord |= NFC_TARGET_CTRL_NAD_PRESENT;
                        } else
                        {
                            /* Last block to transfer: */
                            buffer[NFC_DEP_PFB_POS] &= NFC_DEP_MI_PRESENT_I;
                            td->m_CtrlWord &= NFC_TARGET_CTRL_NAD_PRESENT_I;
                            td->m_CtrlWord &= NFC_TARGET_CTLR_NAD_PREV_USED_I;
							td->m_NadUsed = 0;
                        }

                        /* Increment block number PNI:                 */
                        td->m_BlNr = (uint8_t)((td->m_BlNr + 1) % 4); 
                        break;
                    default:
                        break;
                }
                /* Set last action: */
                td->m_LastAction = action_type;
            }
        } else
        {
            /* Incoming NAD check NOK. */
        }

    } else
    {
        /* DID mismatch. */
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


/* Called in case of auto-chaining: */
phcsBfl_Status_t phcsBflNfc_TargetDepChaining(phcsBflNfc_Target_t            *cif,
                                     phcsBflNfc_TargetCommParam_t  *td,
                                     uint8_t                *action_type,
                                     uint8_t                *mi_present,
                                     uint8_t                 nad_presence,
                                     uint8_t                *trx_buffer,
                                     uint16_t               *trx_buffer_size,
                                     uint8_t                 header_size)
{
    #define NFCT_OVR_NONE            0 /* */
    #define NFCT_OVR_FIRST           1 /* */
    #define NFCT_OVR_REPEATING       2 /* */
    #define NFCT_OVR_END_RX_NONE     3 /* */
    
    phcsBfl_Status_t                   PHCS_BFL_MEMLOC_REM status              = PH_ERR_BFL_SUCCESS;
    phcsBflNfc_TargetEndpointParam_t   PHCS_BFL_MEMLOC_REM endpoint_parameter = {0};
    uint8_t               PHCS_BFL_MEMLOC_COUNT  index               = 0;
    uint32_t              PHCS_BFL_MEMLOC_COUNT  size;
    static uint8_t        PHCS_BFL_MEMLOC_REM ovr_type;
    uint8_t               PHCS_BFL_MEMLOC_REM tox_byte            = 0;


	/* In case of chaining (in endpoint CB the MI flag is set),
       we have to ensure that we step into the DEP_REQ case for 
       sending the next chaining block */
    if ((*action_type == PHCS_BFLNFC_TARGET_REQ_ACK) && (td->m_CtrlWord & 0x01)) 
    {
        *action_type = PHCS_BFLNFCCOMMON_DEP_REQ;
        /* Clearing */
        td->m_CtrlWord &= ~ 0x01;
    }


    /* What kind of block has Target received ??? */
    switch (*action_type)
    {
        case PHCS_BFLNFC_TARGET_REQ_TOX:
            /* Intended fall-through! */
        case PHCS_BFLNFC_TARGET_REQ_INF:
        case PHCS_BFLNFCCOMMON_DEP_REQ:
            /* Data is received from Initiator. */
            if ((*action_type == PHCS_BFLNFC_TARGET_REQ_INF) || (*action_type == PHCS_BFLNFCCOMMON_DEP_REQ))
            {
                /* Check if buffer size has to be reset: */
                if (td->m_CtrlWord & NFC_TARGET_CTRL_RCV_CHAIN_FIN)
                {
                    /* Clear bit and reset counter: */
                    td->m_CtrlWord &= ~NFC_TARGET_CTRL_RCV_CHAIN_FIN;
                    td->m_TargetBufferSize = 0;
                    td->m_NadUsed = (uint8_t)(nad_presence ? 1 : 0);
                }

                /* Check if buffer is big enough to append received block. */
                size = td->m_TargetBufferSize + *trx_buffer_size;
                /* Reset index for all branches: */
                index = 0;
                if (size > td->m_MaxTargetBufferSize)
                {
                    /* Copy until the end of the Target buffer is reached and indicate overflow: */
                    while (td->m_TargetBufferSize < td->m_MaxTargetBufferSize)
                    {
                         td->mp_TargetBuffer[td->m_TargetBufferSize] = trx_buffer[index];
                         td->m_TargetBufferSize++;
                         index++;
                    }
                    /* We got an overflow! */
                    td->m_CtrlWord |= (uint16_t)(NFC_TARGET_CTRL_BUF_RCV_OVF);

                    /* Keep index in mind for after the endpoint call. Fill the remaining bytes
                       into the Target buffer after the endpoint call. */
                } else
                {
                    /* in case of NAD is used and special operation because of entering DEP directly after
                       TOX decrement *trx_buffer_size to correct NAD byte */
                    if((td->m_CtrlWord & NFC_TARGET_CTRL_DEP_AFTER_TOX) && td->m_NadUsed)
                    {
                        (*trx_buffer_size)--;
                        size--;
                    }
                    /* Copy, if enough space: (index already reset!) */
                    for (; index < *trx_buffer_size; index++)
                    {
                        td->mp_TargetBuffer[td->m_TargetBufferSize + index] = trx_buffer[index];
                    }

                    /* Calculate new size of local buffer. */
                    td->m_TargetBufferSize = (uint16_t)size;
                    /* No overflow this time! */
                    td->m_CtrlWord &= (uint16_t)(~NFC_TARGET_CTRL_BUF_RCV_OVF);
                }
            } else
            {
                /* In case of TOX confirmation sent by NFCI restore MI of last block: */
                if (td->m_CtrlWord & NFC_TARGET_CTRL_MI_PRESENT)
                {
                    *mi_present = 1;
                } else
                {
                    *mi_present = 0;
                }
            }
            
            /* Do not change overflow handling when TOX is received. */
            if(*action_type != PHCS_BFLNFC_TARGET_REQ_TOX)
            {
                /* Overflow handling:
                * We indicate to the endpoint which type of overflow we have got
                * (none, first, repeating, end of RX - none).
                * In the endpoint parameter handling we take this into account.
                */
                if (td->m_CtrlWord & NFC_TARGET_CTRL_IN_OVF)
                {
                    if (td->m_CtrlWord & NFC_TARGET_CTRL_BUF_RCV_OVF)
                    {
                        /* We already had an overflow and we have one again, so we are
                        in the middle of a transaction with repeating buffer size problems. */
                        ovr_type = NFCT_OVR_REPEATING;
                    } else
                    {
                        /* We already had an overflow but this time the buffer fits, we are done
                        with the current incoming frames. */
                        ovr_type = NFCT_OVR_END_RX_NONE;
                    }
                } else
                {
                    if (td->m_CtrlWord & NFC_TARGET_CTRL_BUF_RCV_OVF)
                    {
                        /* This is the first overflow to occur. */
                        td->m_CtrlWord |= (uint16_t)(NFC_TARGET_CTRL_IN_OVF);
                        ovr_type = NFCT_OVR_FIRST;
                    } else
                    {
                        /* We have no overflow. */
                        ovr_type = NFCT_OVR_NONE;
                    }
                }

                /* If we need to inform the endpoint that we have an overflow we must
                   not send back an ACK frame. The endpoint must have the chance to
                   move away the transferred data first. */
                if (*mi_present || ((td->m_CtrlWord & NFC_TARGET_CTRL_MI_PRESENT) && (td->m_CtrlWord & NFC_TARGET_CTRL_DEP_AFTER_TOX)))
                {
                    /* Reply with an Acknowledge. */
                    *action_type = PHCS_BFLNFC_TARGET_RES_ACK;
                    /* Set MI bit: */
                    td->m_CtrlWord |= NFC_TARGET_CTRL_MI_PRESENT;

                    /* Leave this method if no overflow hits us! */
                    if ((ovr_type == NFCT_OVR_NONE) || (ovr_type == NFCT_OVR_END_RX_NONE))
                    {
                        break;
                    } else
                    {
                        /* Regardless of MI we continue with an endpoint call because we need to move
                        away data from the full Target buffer in order to enable the protocol to
                        fill in more data to come. No action req'd. */
                    }
                } else
                {
                    /* reset status flag for remembering DEP is needed after TOX */
                    td->m_CtrlWord &= ~NFC_TARGET_CTRL_DEP_AFTER_TOX;
                }
            }
            
            NFCT_AC_LAST_FRAME_PROPAGATION:;

			/* Clr MI (*mi_present == 0) bit of CtrlWord in case of no overflow: */
            if ((ovr_type == NFCT_OVR_NONE) || (ovr_type == NFCT_OVR_END_RX_NONE))
            {
                td->m_CtrlWord &= NFC_TARGET_CTRL_MI_PRESENT_I;
                /* Receive procedure has finished, continue with sending response. */
                td->m_CtrlWord |= NFC_TARGET_CTRL_RCV_CHAIN_FIN;
            }

            /* Call the protocol endpoint: */
            endpoint_parameter.action_type              = (uint8_t)(((ovr_type == NFCT_OVR_NONE) || (ovr_type == NFCT_OVR_END_RX_NONE) || (*action_type == PHCS_BFLNFC_TARGET_REQ_TOX)) ? *action_type : PHCS_BFLNFCCOMMON_DEP_REQ);
            endpoint_parameter.flags                    =  PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
            endpoint_parameter.self                     =  cif->mp_Upper;
            endpoint_parameter.target                   =  cif;
            endpoint_parameter.p_user                   =  td->mp_User;
            switch (ovr_type)
            {
                case NFCT_OVR_FIRST:
                    endpoint_parameter.flags           |=  (uint8_t)PHCS_BFLNFC_TARGET_EP_FLAGS_RX_OVL;
                    endpoint_parameter.p_req_data       =  td->mp_TargetBuffer + (td->m_NadUsed ? 1 : 0);
                    endpoint_parameter.p_res_data       =  td->mp_TargetBuffer;
                    endpoint_parameter.req_data_length  =  (uint16_t)(td->m_TargetBufferSize - (td->m_NadUsed ? 1 : 0));
                    endpoint_parameter.res_data_length  =  (uint16_t)0;
                    endpoint_parameter.nad              =  td->m_NadUsed ? (&td->m_NAD) : NULL;
                    break;

                case NFCT_OVR_REPEATING:
                    endpoint_parameter.flags           |=  (uint8_t)PHCS_BFLNFC_TARGET_EP_FLAGS_RX_OVL;
                    endpoint_parameter.p_req_data       =  td->mp_TargetBuffer;
                    endpoint_parameter.p_res_data       =  endpoint_parameter.p_req_data;
                    endpoint_parameter.req_data_length  =  td->m_TargetBufferSize;
                    endpoint_parameter.res_data_length  =  (uint16_t)0;
                    endpoint_parameter.nad              =  NULL;
                    break;

                case NFCT_OVR_END_RX_NONE:
                    endpoint_parameter.p_req_data       =  td->mp_TargetBuffer;
                    endpoint_parameter.p_res_data       =  td->mp_TargetBuffer + (td->m_NadUsed ? 1 : 0);
                    endpoint_parameter.req_data_length  =  td->m_TargetBufferSize;
                    endpoint_parameter.res_data_length  =  (uint16_t)(td->m_MaxTargetBufferSize - (td->m_NadUsed ? 1 : 0));
					endpoint_parameter.nad              =  td->m_NadUsed ? (&td->m_NAD) : NULL;
                    break;

                /* The "original" variant: */
                case NFCT_OVR_NONE:
                default:
                    endpoint_parameter.p_req_data       =  td->mp_TargetBuffer + (td->m_NadUsed ? 1 : 0);
                    endpoint_parameter.p_res_data       =  endpoint_parameter.p_req_data;
                    endpoint_parameter.req_data_length  =  (uint16_t)(td->m_TargetBufferSize - (td->m_NadUsed ? 1 : 0));
                    endpoint_parameter.res_data_length  =  (uint16_t)(td->m_MaxTargetBufferSize - (td->m_NadUsed ? 1 : 0));
                    endpoint_parameter.nad              =  td->m_NadUsed ? (&td->m_NAD) : NULL;
                    break;
            }

			/* Added by CT
			 */  
			endpoint_parameter.ovr_type = ovr_type;
			/* Until here */

			status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);
            
            /* In case of no overflow handling ("original variant") we apply the new values, otherwise we
               continue as if there had been no endpoint call. */
            if ((ovr_type == NFCT_OVR_NONE) || (ovr_type == NFCT_OVR_END_RX_NONE))
            {
                /* When we must return information (end of downstream to Target) we apply the values returned
                   by the endpoint callback routine. */
                /* Apply returned action type: */
                *action_type                                =  endpoint_parameter.action_type;
                /* Apply returned buffer size:*/
                td->m_TargetBufferSize                      =  (uint16_t)(endpoint_parameter.res_data_length + (td->m_NadUsed ? 1 : 0));
                /* Break in case of TOX response and re-entry in information state */
                if(*action_type == PHCS_BFLNFC_TARGET_REQ_TOX)
                {
                    td->m_CtrlWord |= NFC_TARGET_CTRL_DEP_AFTER_TOX;
                    break;
                }
            } else
            {
                /* Used for possibility to send TOX also in between meta-chaining.
                 * TOX value has to be stored internally because TRX buffer might have some left content. */
                if(endpoint_parameter.action_type == PHCS_BFLNFC_TARGET_RES_TOX && endpoint_parameter.res_data_length == 0x01)
                {
                    tox_byte = endpoint_parameter.p_res_data[0];
                    *action_type = endpoint_parameter.action_type;
                }
                
                /* Otherwise, when the only purpose of the endpoint call is to save data from buffer overflow,
                   we keep the original action type (e.g. NFC_RES_ACK) and copy the remaining bytes of
                   trx_buffer into the Target buffer (we remember "index"). */
                /* If action_type is different to NFC_REQ_TOX the endpoint has (should have) copied away all 
                 * data, we start over with size = 0. In any other case we keep the previous number. */
                if(*action_type != PHCS_BFLNFC_TARGET_REQ_TOX)
                {
                    td->m_TargetBufferSize = 0;
                    for (; index < *trx_buffer_size; index++) /* Optional "for-init" omitted, we remember index! */
                    {
                        td->mp_TargetBuffer[td->m_TargetBufferSize] = trx_buffer[index];
                        td->m_TargetBufferSize++;
                    }
                } else
                {
                    td->m_CtrlWord |= NFC_TARGET_CTRL_DEP_AFTER_TOX;
                }
                
                /* Data has been copied before, restore TOX byte for transmission. 
                 * Break at the end to not change internal flags which are used for next communication. */
                if(*action_type == PHCS_BFLNFC_TARGET_RES_TOX)
                {
                    trx_buffer[0] = tox_byte;
					*trx_buffer_size = 1; 
                    break; 
                }
                
                /* We have copied all remaining bytes to have subsequent frames appended again.
                 * If, however, no frame follows (*mi_present == 0) the endpoint is not called again
                 * by an outside action. At this point, we must take care of it ourselves:
                 */
                if (*mi_present == 0)
                {
                    /* Clear the overflow bit in the CtrlWord because we know that the transaction is over: */
                    td->m_CtrlWord &= (uint16_t)(~NFC_TARGET_CTRL_BUF_RCV_OVF);
                    td->m_CtrlWord &= (uint16_t)(~NFC_TARGET_CTRL_IN_OVF);
                    /* Adapt pre-conditions to meet data propagation requirements: */
                    ovr_type = NFCT_OVR_END_RX_NONE;
                    *action_type = PHCS_BFLNFCCOMMON_DEP_REQ;
                    /* We loop back to the endpoint execution with a - uh, nasty - goto: This is because
                       the alternative solution, an endless for(;;) with a break would make the other break
                       in the next branch ineffective, so this is the easiest solution. */
                    goto NFCT_AC_LAST_FRAME_PROPAGATION;
                } else
                {
                    if (*action_type == PHCS_BFLNFC_TARGET_RES_ACK)
                    {
                        /* The protocol must confirm the last block received and perform
                           no further action here, we leave this method. With the next call (INF from the
                           Initiator), the content arriving is simply appended to the Target Buffer. */
                        break;
                    }
                }
            }
            /* We're done with the call! */
            /* Intended fall-through if callback is processed "normally" without overflow handling. */
            /* Here we reset the overflow flags: */
            td->m_CtrlWord &= (uint16_t)(~NFC_TARGET_CTRL_BUF_RCV_OVF);
            td->m_CtrlWord &= (uint16_t)(~NFC_TARGET_CTRL_IN_OVF);

        case PHCS_BFLNFC_TARGET_REQ_NACK:
            if (*action_type == PHCS_BFLNFC_TARGET_REQ_NACK)
            {
                /* Calculate start index of last transaction: */
                td->m_NextStartIndex = td->m_NextStartIndex - td->m_SizeOfLastBlock;

                /* In case of a NACK and TOX re-transmit TOX: */
                if (td->m_LastAction == PHCS_BFLNFC_TARGET_RES_TOX)
                {
                    *action_type = PHCS_BFLNFC_TARGET_RES_TOX;
                }
            }
            /* Intended fall-through! */

        case PHCS_BFLNFC_TARGET_RES_INF:
            /* First frame to return: */
            td->m_NextStartIndex = 0;
        case PHCS_BFLNFC_TARGET_REQ_ACK:

			/* An ACK frame has been sent by the Initiator or it is the first
               INF to return. */
            /* Send first/next block to Initiator. */
            /* Prepare copying: */
            size = td->m_MaxPduBufferSize - header_size;
            if ((td->m_TargetBufferSize - td->m_NextStartIndex) > size)
            {
                *mi_present = 1;
                /* Fill in buffer: */
            } else
            {
                /* If the chaining block size < than the TargetBufferLength,
                   but chaining shall be done, in the endpoint CB the more 
                   information flag is set. (also *mi_present = 1;).
                   After sending the first chaining block we have to step 
                   into the
                   case DEP_REQ again for sending the next chaining block 
                   (a.s.o).
                   This we mark in the td->m_CtrlWord.
                   (Attention: This is different to the automatic chaining.
                               (chaining frame > than TargetBufferLength) 
                               In case of this chaining the other blocks were 
                               sent 
                               internally.) 
                */
                if (endpoint_parameter.flags & PHCS_BFLNFC_TARGET_EP_FLAGS_MI_PRES)
                {
                    *mi_present = 1;
                    size = td->m_TargetBufferSize;
                    td->m_CtrlWord |= 0x01;
                } else
                {
                    *mi_present = 0;
                    size = td->m_TargetBufferSize - td->m_NextStartIndex;
                }
            }

            /* Fill in buffer: */
            for (index = 0; index < size; index++)
            {
                trx_buffer[index] = td->mp_TargetBuffer[td->m_NextStartIndex + index];
            }
 
            /* Set return values: */
            *trx_buffer_size = index;
            if ( (*action_type != PHCS_BFLNFC_TARGET_RES_TOX ) && (*action_type != PHCS_BFLNFC_TARGET_REQ_TOX) )
            {
                /* Calculate start index for next transmission: */
                td->m_NextStartIndex = td->m_NextStartIndex + index;
                td->m_SizeOfLastBlock = index;
 
                *action_type = PHCS_BFLNFC_TARGET_RES_INF;
            }
            break;

        default:
            status = PH_ERR_BFL_UNSUPPORTED_COMMAND;
            break;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;

    #undef NFCT_DEPR_INCOMING_NAD_PRESENT   /* */
    #undef NFCT_DEPR_OUTGOING_NAD_PRESENT   /* */
    #undef NFCT_DEPR_NO_NAD_PRESENT         /* */
    #undef NFCT_OVR_NONE                    /* */
    #undef NFCT_OVR_FIRST                   /* */
    #undef NFCT_OVR_REPEATING               /* */
    #undef NFCT_OVR_END_RX_NONE             /* */
}


static phcsBfl_Status_t phcsBflNfc_TargetWupResponse(phcsBflNfc_Target_t            *cif,
                                            phcsBflNfc_TargetCommParam_t  *td,
                                            uint8_t                *buffer,
                                            uint16_t               *length)
{
    phcsBfl_Status_t              PHCS_BFL_MEMLOC_REM status              = PH_ERR_BFL_SUCCESS;
    phcsBflNfc_TargetEndpointParam_t   PHCS_BFL_MEMLOC_REM endpoint_parameter;
    uint8_t                     PHCS_BFL_MEMLOC_COUNT  loopcount;
    uint8_t                     PHCS_BFL_MEMLOC_COUNT  index;
    uint8_t                     PHCS_BFL_MEMLOC_REM action_type;

    /* Check length of WUP-REQ first: */
    if (*length == NFC_WUP_REQ_LEN)
    {
        index = NFC_WUP_NFCID_POS;
        for (loopcount = 0; loopcount < PHCS_BFLNFCCOMMON_ID_LEN; loopcount++)
        {
            if (td->mp_NfcIDt[loopcount] != buffer[index])
            {
                status = PH_ERR_BFL_INVALID_FORMAT;
                break;
            }
            index++;
        }

        /* Check if all digits of Nfcid were equal. */
        if ((loopcount == PHCS_BFLNFCCOMMON_ID_LEN) && (status == PH_ERR_BFL_SUCCESS))
        {
            /* Nfcid was equal - accept new DID. */
            td->m_DID = buffer[NFC_WUP_REQ_DID_POS];

            /* Notify Application: */
            action_type = PHCS_BFLNFCCOMMON_WUP_REQ;

            /* Call the protocol endpoint: */
            endpoint_parameter.flags                =  PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
            endpoint_parameter.action_type          =  action_type;
            endpoint_parameter.nad                  =  NULL;                       /* No NAD used. */
            endpoint_parameter.p_req_data           =  NULL;                       /* Does not apply. */
            endpoint_parameter.p_res_data           =  NULL;                       /* Does not apply. */
            endpoint_parameter.req_data_length      =  0;
            endpoint_parameter.res_data_length      =  0;
            endpoint_parameter.self                 =  cif->mp_Upper;
            endpoint_parameter.target               =  cif;
            endpoint_parameter.p_user               =  td->mp_User;
            status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);
            action_type = endpoint_parameter.action_type;
            /* We're done with the call! */

            /* Build response: */
            buffer[NFC_WUP_RES_DID_POS] = td->m_DID;
            *length = NFC_WUP_RES_LEN;
        }
    } else
    {
        status = PH_ERR_BFL_INVALID_FORMAT;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static phcsBfl_Status_t phcsBflNfc_TargetCheckRequestDid(phcsBflNfc_TargetCommParam_t *td,
                                                uint8_t               *buffer,
                                                uint16_t              *length,
                                                uint16_t               compare_length)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;

    if (td->m_DID == 0)
    {
        /* No DID is used: */
        if (*length == compare_length)
        {
            /* Did OK! */
        } else
        {
            /* DID was received, but not expected! */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* DID used:       */
        if (*length > compare_length)
        {
            if (td->m_DID == buffer[NFC_DSL_DID_POS])
            {
                /* Did OK! */
            } else
            {
                /* Wrong DID was received, do not response! */
                status = PH_ERR_BFL_INVALID_FORMAT;
            }
        } else
        {
            /* DID was expected but not received! */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static phcsBfl_Status_t phcsBflNfc_TargetDslRlsCombiResponse(phcsBflNfc_Target_t              *cif,
                                                    phcsBflNfc_TargetCommParam_t    *td,
                                                    uint8_t                  *buffer,
                                                    uint16_t                 *length,
                                                    uint8_t                   combi_req_action_type)
{
    phcsBfl_Status_t             PHCS_BFL_MEMLOC_REM status              = PH_ERR_BFL_SUCCESS;
    phcsBflNfc_TargetEndpointParam_t  PHCS_BFL_MEMLOC_REM endpoint_parameter;
    uint8_t                    PHCS_BFL_MEMLOC_REM action_type;

    status = phcsBflNfc_TargetCheckRequestDid(td, buffer, length, NFC_MIN_DSL_REQ_LEN); 

    if (status == PH_ERR_BFL_SUCCESS)
    {
        /* Notify Application: */
        action_type = combi_req_action_type;

        /* Call the protocol endpoint: */
        endpoint_parameter.flags                =  PHCS_BFLNFC_TARGET_FLAGS_ACTIVE__PASSIVE;
        endpoint_parameter.action_type          =  action_type;
        endpoint_parameter.nad                  =  NULL;                       /* No NAD used. */
        endpoint_parameter.p_req_data           =  NULL;                       /* Does not apply. */
        endpoint_parameter.p_res_data           =  NULL;                       /* Does not apply. */
        endpoint_parameter.req_data_length      =  0;
        endpoint_parameter.res_data_length      =  0;
        endpoint_parameter.self                 =  cif->mp_Upper;
        endpoint_parameter.target               =  cif;
        endpoint_parameter.p_user               = td->mp_User;
        status = cif->mp_Upper->TargetEndpointCallback(&endpoint_parameter);
        action_type = endpoint_parameter.action_type;
        /* We're done with the call! */
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


    
static phcsBfl_Status_t phcsBflNfc_TargetDslResponse(phcsBflNfc_Target_t               *cif,
                                             phcsBflNfc_TargetCommParam_t    *td,
                                             uint8_t                  *buffer,
                                             uint16_t                 *length)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;

    status = phcsBflNfc_TargetDslRlsCombiResponse(cif,
                                           td,
                                           buffer,
                                           length,
                                           PHCS_BFLNFCCOMMON_DSL_REQ);

    if (status == PH_ERR_BFL_SUCCESS || (status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_DESELECTED)
    {
        td->m_CtrlWord |= NFC_TARGET_CTRL_DSLCB_REQ2;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}


static phcsBfl_Status_t phcsBflNfc_TargetRlsResponse(phcsBflNfc_Target_t               *cif,
                                            phcsBflNfc_TargetCommParam_t     *td,
                                            uint8_t                   *buffer,
                                            uint16_t                  *length)
{
    phcsBfl_Status_t PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;

    status = phcsBflNfc_TargetDslRlsCombiResponse(cif,
                                           td,
                                           buffer,
                                           length,
                                           PHCS_BFLNFCCOMMON_RLS_REQ);

    if (status == PH_ERR_BFL_SUCCESS || (status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_RELEASED)   
    {
        td->m_CtrlWord |= NFC_TARGET_CTRL_RLSCB_REQ2;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_NFC)
    return status;
}
 

/* E.O.F. */
