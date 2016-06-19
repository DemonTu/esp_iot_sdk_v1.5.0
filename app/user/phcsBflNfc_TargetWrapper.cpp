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

/*! \file phcsBflNfc_TargetWrapper.cpp
 *
 * Project: NFC Target.
 *
 * Workfile: phcsBflNfc_TargetWrapper.cpp
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Wed Jun  7 09:47:38 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  NFC class framework wrapper to enable multi-instance operation.
 *
 * History:
 *  GK:  Generated 30. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 *
 */



#include <phcsBflNfcTargetWrapper.hpp>

using namespace phcs_BFL;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Glue C to C++:

phcsBfl_NfcTargetGlue::phcsBfl_NfcTargetGlue(void)
{
    m_GlueRcioStruct.Transmit                           = &phcsBfl_NfcTargetGlue::Transmit;
    m_GlueProtocolEndpointStruct.TargetEndpointCallback = &phcsBfl_NfcTargetGlue::TargetEndpointCallback;
    
    // We don't need other functions:
    m_GlueRcioStruct.MfAuthent                          = NULL;
    m_GlueRcioStruct.Receive                            = NULL;
    m_GlueRcioStruct.Transceive                         = NULL;
    m_GlueRcioStruct.mp_Members                         = NULL;
    m_GlueRcioStruct.mp_Lower                           = NULL;
}


phcsBfl_NfcTargetGlue::~phcsBfl_NfcTargetGlue(void)
{
}


phcsBfl_Status_t phcsBfl_NfcTargetGlue::TargetEndpointCallback(phcsBflNfc_TargetEndpointParam_t* endpoint_param)
{
    class phcsBfl_NfcTarget *target_wrapping_object =
        (class phcsBfl_NfcTarget*)((phcsBflNfc_TargetEndpoint_t*)(endpoint_param->self))->mp_CallingObject;
    return target_wrapping_object->mp_Upper->TargetEndpointCallback(endpoint_param);   
}


phcsBfl_Status_t phcsBfl_NfcTargetGlue::Transmit(phcsBflIo_TransmitParam_t *transmit_param)
{
    class phcsBfl_NfcTarget *target_wrapping_object =
        (class phcsBfl_NfcTarget*)((phcsBflIo_t*)(transmit_param->self))->mp_CallingObject;
    return target_wrapping_object->mp_Lower->Transmit(transmit_param);
}




////////////////////////////////////////////////////////////////////////////////////////////////////
// Channel:

phcsBflNfc_TargetWrapper::phcsBflNfc_TargetWrapper(void)
{
    m_ConstructorStatus = PH_ERR_BFL_SUCCESS;
    m_Interface.mp_Upper = NULL;
    m_CommunicationParams.mp_TargetBuffer = NULL;
}


phcsBflNfc_TargetWrapper::phcsBflNfc_TargetWrapper(uint8_t                        req_preamble_type,
                                     uint8_t                        res_preamble_type,
                                     uint8_t                       *p_target_buffer,
                                     uint16_t                       target_buffer_size,
                                     uint8_t                       *p_nfcidi_buffer,
                                     uint8_t                       *p_nfcidt_buffer,
                                     uint8_t                        passive_mode,
                                     class phcsBfl_NfcTargetEndpoint *p_nfc_target_endpoint,
                                     class phcsBfl_Io                *p_lower)
{
    m_ConstructorStatus = PH_ERR_BFL_SUCCESS;
    this->Initialise(req_preamble_type,
                     res_preamble_type,
                     p_target_buffer,
                     target_buffer_size,
                     p_nfcidi_buffer,
                     p_nfcidt_buffer,
                     passive_mode,
                     p_nfc_target_endpoint,
                     p_lower);
}


phcsBflNfc_TargetWrapper::~phcsBflNfc_TargetWrapper(void)
{   
    // No action.
}


void phcsBflNfc_TargetWrapper::Initialise(uint8_t                        req_preamble_type,
                                   uint8_t                        res_preamble_type,
                                   uint8_t                       *p_target_buffer,
                                   uint16_t                       target_buffer_size,
                                   uint8_t                       *p_nfcidi_buffer,
                                   uint8_t                       *p_nfcidt_buffer,
                                   uint8_t                        passive_mode,
                                   class phcsBfl_NfcTargetEndpoint *p_upper,
                                   class phcsBfl_Io                *p_lower)
{
    phcsBflNfc_TargetInit(&m_Interface,
                  &m_CommunicationParams,
                   req_preamble_type,
                   res_preamble_type,
                   p_target_buffer,
                   target_buffer_size,
                   p_nfcidi_buffer,
                   p_nfcidt_buffer,
                   passive_mode,
                  &(m_NfcGlue.m_GlueProtocolEndpointStruct),
                  &(m_NfcGlue.m_GlueRcioStruct));

    // Additional initialisation beyond the scope of the C part:
    this->mp_Lower = p_lower;
    this->mp_Upper = p_upper;
}


void phcsBflNfc_TargetWrapper::ResetProt(phcsBflNfc_TargetResetParam_t *reset_param)
{
    void       *p_self;

    p_self = reset_param->self;    
    reset_param->self = &m_Interface;
    m_Interface.ResetProt(reset_param);
    reset_param->self = p_self;
}


void phcsBflNfc_TargetWrapper::SetUserBuf(phcsBflNfc_TargetSetUserBufParam_t *user_buffer_param)
{
    void       *p_self;

    p_self = user_buffer_param->self;    
    user_buffer_param->self = &m_Interface;
    m_Interface.SetUserBuf(user_buffer_param);
    user_buffer_param->self = p_self;
}


phcsBfl_Status_t phcsBflNfc_TargetWrapper::SetTRxBufProp(phcsBflNfc_TargetSetTRxBufPropParam_t *trx_buffer_param)
{
    phcsBfl_Status_t   status;
    void       *p_self;

    p_self = trx_buffer_param->self;    
    trx_buffer_param->self = &m_Interface;
    status = m_Interface.SetTRxBufProp(trx_buffer_param);
    trx_buffer_param->self = p_self;

    return status;    
}


void phcsBflNfc_TargetWrapper::SetPUser(phcsBflNfc_TargetSetPUserParam_t *p_user_param)
{
    void       *p_self;

    p_self = p_user_param->self;    
    p_user_param->self = &m_Interface;
    m_Interface.SetPUser(p_user_param);
    p_user_param->self = p_self;
}


phcsBfl_Status_t phcsBflNfc_TargetWrapper::Dispatch(phcsBflNfc_TargetDispatchParam_t* dispatch_param)
{
    phcsBfl_Status_t   status;
    void       *p_self;

    p_self = dispatch_param->self;    
    
    // Let the C-kernel remember its "object" (interface structure):
    dispatch_param->self = &m_Interface;
    // Tell the glue class, connecting to "upper" Target endpoint who is the calling instance.
    (((phcsBflNfc_Target_t*)(dispatch_param->self))->mp_Upper)->mp_CallingObject = this;
    // The same with the lower (RC-I/O) edge.
    (((phcsBflNfc_Target_t*)(dispatch_param->self))->mp_Lower)->mp_CallingObject = this;
    status = m_Interface.Dispatch(dispatch_param);
    dispatch_param->self = p_self;

    return status;
}
