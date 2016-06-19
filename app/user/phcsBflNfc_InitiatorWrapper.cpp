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

/*! \file phcsBflNfc_InitiatorWrapper.cpp
 *
 * Project: NFC Initiator.
 *
 * Workfile: phcsBflNfc_InitiatorWrapper.cpp
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Wed Jun  7 09:47:38 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  NFC class framework wrapper to enable multi-instance operation.
 *
 * History:
 *  GK: Generated 30. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 *
 */


#include <phcsBflNfcInitiatorWrapper.hpp>

using namespace phcs_BFL;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Glue:

phcsBfl_NfcInitatorGlue::phcsBfl_NfcInitatorGlue(void)
{
    // Initialise struct members with static class members:
    m_GlueStruct.Transceive = &phcsBfl_NfcInitatorGlue::Transceive;

    // We don't need other members in the glue!
    m_GlueStruct.MfAuthent      = NULL;
    m_GlueStruct.Transmit       = NULL;
    m_GlueStruct.Receive        = NULL;
    m_GlueStruct.mp_Lower       = NULL;
    m_GlueStruct.mp_Members     = NULL;
}


phcsBfl_NfcInitatorGlue::~phcsBfl_NfcInitatorGlue(void)
{
}


// Static class members, able to call into C++ again:

phcsBfl_Status_t phcsBfl_NfcInitatorGlue::Transceive(phcsBflIo_TransceiveParam_t *transceive_param)
{
    // Resolve the NFC-I wrapper object location:
    class phcsBfl_NfcInitator *nfci_wrapping_object = (class phcsBfl_NfcInitator*)
        (((phcsBflIo_t*)(transceive_param->self))->mp_CallingObject);

    // Call the lower instance:
    return nfci_wrapping_object->mp_Lower->Transceive(transceive_param);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Channel Implemenation:

phcsBflNfcInitator_Wrapper::phcsBflNfcInitator_Wrapper(void)
{
    m_ConstructorStatus = PH_ERR_BFL_SUCCESS;
    m_Interface.mp_Lower = NULL;
    m_CommunicationParams.m_NadUsed = 0;
}


phcsBflNfcInitator_Wrapper::phcsBflNfcInitator_Wrapper(uint8_t         tx_preamble_type,
                                                   uint8_t         rx_preamble_type,
                                                   uint8_t        *p_trxbuffer,
                                                   uint16_t        trxbuffersize,
                                                   uint8_t        *p_nfcidt_buffer,
                                                   class phcsBfl_Io *p_lower)
{
    m_ConstructorStatus = PH_ERR_BFL_SUCCESS;
    Initialise(tx_preamble_type,
                rx_preamble_type,
                p_trxbuffer,
                trxbuffersize,
                p_nfcidt_buffer,
                p_lower);
}


phcsBflNfcInitator_Wrapper::~phcsBflNfcInitator_Wrapper(void)
{
    // No action.
}


void phcsBflNfcInitator_Wrapper::Initialise(uint8_t         tx_preamble_type,
                                            uint8_t         rx_preamble_type,
                                            uint8_t        *p_trxbuffer,
                                            uint16_t        trxbuffersize,
                                            uint8_t        *p_nfcidt_buffer,
                                            class phcsBfl_Io *p_lower)
{
    phcsBflNfc_InitiatorInit(&m_Interface,
                           &m_CommunicationParams,
                            tx_preamble_type,
                            rx_preamble_type,
                            p_trxbuffer,
                            trxbuffersize,
                            p_nfcidt_buffer,
                           &(m_NfcGlue.m_GlueStruct)); // C++: C struct reference within glue class.
              
    // Additional initialisation beyond the scope of the C part:
    mp_Lower = p_lower; // Initialise C++ underlying object reference.
}


phcsBfl_Status_t phcsBflNfcInitator_Wrapper::AtrRequest(phcsBflNfc_InitiatorAtrReqParam_t *atr_req_param)
{
    phcsBfl_Status_t   status;  
    void       *p_self; 
    
    p_self = atr_req_param->self;
    atr_req_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(atr_req_param->self))->mp_Lower))->mp_CallingObject = this;
    status = m_Interface.AtrRequest(atr_req_param);
    atr_req_param->self = p_self;

    return status;
}


phcsBfl_Status_t phcsBflNfcInitator_Wrapper::PslRequest(phcsBflNfc_InitiatorPslReqParam_t *psl_req_param)
{
    phcsBfl_Status_t   status;  
    void       *p_self; 
    
    p_self = psl_req_param->self;
    psl_req_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(psl_req_param->self))->mp_Lower))->mp_CallingObject = this;
    status = m_Interface.PslRequest(psl_req_param);
    psl_req_param->self = p_self;

    return status;
}


phcsBfl_Status_t phcsBflNfcInitator_Wrapper::DepRequest(phcsBflNfc_InitiatorDepReqParam_t *dep_req_param)
{
    phcsBfl_Status_t   status;  
    void       *p_self; 
    
    p_self = dep_req_param->self;
    dep_req_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(dep_req_param->self))->mp_Lower))->mp_CallingObject = this;
    status = m_Interface.DepRequest(dep_req_param);
    dep_req_param->self = p_self;
    
    return status;
}


phcsBfl_Status_t phcsBflNfcInitator_Wrapper::AttRequest(phcsBflNfc_InitiatorAttReqParam_t *att_req_param)
{
    phcsBfl_Status_t    status;
    void        *p_self;
    
    p_self = att_req_param->self;
    att_req_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(att_req_param->self))->mp_Lower))->mp_CallingObject = this;
    status = m_Interface.AttRequest(att_req_param);
    att_req_param->self = p_self;

    return status;
}


phcsBfl_Status_t phcsBflNfcInitator_Wrapper::DslRequest(phcsBflNfc_InitiatorDslReqParam_t *dsl_req_param)
{
    phcsBfl_Status_t    status;
    void        *p_self;
    
    p_self = dsl_req_param->self;
    dsl_req_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(dsl_req_param->self))->mp_Lower))->mp_CallingObject = this;
    status = m_Interface.DslRequest(dsl_req_param);
    dsl_req_param->self = p_self;

    return status;
}


phcsBfl_Status_t phcsBflNfcInitator_Wrapper::WupRequest(phcsBflNfc_InitiatorWupReqParam_t *wup_req_param)
{
    phcsBfl_Status_t   status;  
    void       *p_self; 
    
    p_self = wup_req_param->self;
    wup_req_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(wup_req_param->self))->mp_Lower))->mp_CallingObject = this;
    status = m_Interface.WupRequest(wup_req_param);
    wup_req_param->self = p_self;

    return status;
}


phcsBfl_Status_t phcsBflNfcInitator_Wrapper::RlsRequest(phcsBflNfc_InitiatorRlsReqParam_t *rls_req_param)
{
    phcsBfl_Status_t   status;  
    void       *p_self; 

    p_self = rls_req_param->self;
    rls_req_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(rls_req_param->self))->mp_Lower))->mp_CallingObject = this;
    status = m_Interface.RlsRequest(rls_req_param);
    rls_req_param->self = p_self;

    return status;
}


void phcsBflNfcInitator_Wrapper::ResetProt(phcsBflNfc_InitiatorResetParam_t *rst_param)
{
    void       *p_self; 

    p_self = rst_param->self;
    rst_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(rst_param->self))->mp_Lower))->mp_CallingObject = this;
    m_Interface.ResetProt(rst_param);
    rst_param->self = p_self;
}


void phcsBflNfcInitator_Wrapper::SetTRxProp(phcsBflNfc_InitiatorSetTRxPropParam_t *set_trx_properties_param)
{
    void      *p_self;

    p_self = set_trx_properties_param->self;
    set_trx_properties_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(set_trx_properties_param->self))->mp_Lower))->mp_CallingObject = this;
    m_Interface.SetTRxProp(set_trx_properties_param);
    set_trx_properties_param->self = p_self;
}


uint8_t phcsBflNfcInitator_Wrapper::RequestedToxValue(phcsBflNfc_InitiatorToxReqParam_t *req_tox_param)
{
    uint8_t  tox_val;
    void          *p_self;

    p_self = req_tox_param->self;
    req_tox_param->self = &m_Interface;
    ((phcsBflIo_t*)(((phcsBflNfc_Initiator_t*)(req_tox_param->self))->mp_Lower))->mp_CallingObject = this;
    tox_val = m_Interface.RequestedToxValue(req_tox_param);
    req_tox_param->self = p_self;

    return tox_val;
}

// EOF
