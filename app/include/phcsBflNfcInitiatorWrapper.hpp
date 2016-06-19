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

/*! 
 * \if cond_generate_filedocu 
 * \file phcsBflNfcInitiatorWrapper.hpp
 *
 * Project: NFC Initiator.
 *
 * Workfile: phcsBflNfcInitiatorWrapper.hpp
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Wed Jun  7 09:50:06 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  NFC class framework wrapper to enable multi-instance operation.
 *
 * History:
 *  GK:  Generated 30. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */

#ifndef PHCSBFLNFCINITIATORWRAPPER_HPP
#define PHCSBFLNFCINITIATORWRAPPER_HPP

#include <phcsBflIoWrapper.hpp>

extern "C"
{
    #include <phcsBflNfc.h>
}

namespace phcs_BFL
{

/*! \cond cond_Cpp_complete */
/* \ingroup nfci
 *  Functionality according to the NFC-I specification
 *  (abstract class, used for the C++ interface).
 */
class phcsBfl_NfcInitator
{
    public:
        virtual void Initialise(uint8_t              tx_preamble_type,
                                uint8_t              rx_preamble_type,
                                uint8_t             *p_trxbuffer,
                                uint16_t             trxbuffersize,
                                uint8_t             *p_nfcidt_buffer,
                                class phcsBfl_Io      *p_lower)                                   = 0;
        virtual phcsBfl_Status_t AtrRequest(phcsBflNfc_InitiatorAtrReqParam_t *atr_req_param)       = 0;
        virtual phcsBfl_Status_t PslRequest(phcsBflNfc_InitiatorPslReqParam_t *psl_req_param)       = 0;
        virtual phcsBfl_Status_t DepRequest(phcsBflNfc_InitiatorDepReqParam_t *dep_req_param)       = 0;
        virtual phcsBfl_Status_t AttRequest(phcsBflNfc_InitiatorAttReqParam_t *att_req_param)       = 0;
        virtual phcsBfl_Status_t DslRequest(phcsBflNfc_InitiatorDslReqParam_t *dsl_req_param)       = 0;
        virtual phcsBfl_Status_t WupRequest(phcsBflNfc_InitiatorWupReqParam_t *wup_req_param)       = 0;
        virtual phcsBfl_Status_t RlsRequest(phcsBflNfc_InitiatorRlsReqParam_t *rls_req_param)       = 0;
        virtual void ResetProt(phcsBflNfc_InitiatorResetParam_t *rst_param)                       = 0;
        virtual void SetTRxProp(phcsBflNfc_InitiatorSetTRxPropParam_t
                                               *set_trx_properties_param)                       = 0;
        virtual uint8_t RequestedToxValue(phcsBflNfc_InitiatorToxReqParam_t *req_tox_param)       = 0;

        /*! Lower edge interface: Not to be redefined in the derived class: */
        class phcsBfl_Io *mp_Lower;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/* \ingroup nfci
 *  This class glues together the C-implementation of NFC-Initiator and
 *  the underlying class framework. By declaring the member as _ static _ the
 *  method gets no implicit _ this _ pointer and can thus be called by any C
 *  function. However, the function must be informed about the target object
 *  of type phcsBfl_Io (casted to void*) to be called further down the stack.
 */
class phcsBfl_NfcInitatorGlue
{
    public:
        phcsBfl_NfcInitatorGlue(void);
        ~phcsBfl_NfcInitatorGlue(void);
        
        //! \name Static function, enables to call into C++ again.
        //@{
        static phcsBfl_Status_t Transceive(phcsBflIo_TransceiveParam_t *transceive_param);
        //@}

        //! Structure accessible from the upper object's pure C-kernel.
        phcsBflIo_t m_GlueStruct;
};
/*! \endcond */


////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \ingroup nfci
    \brief Class to present a communication channel to a specific target.
 */
class phcsBflNfcInitator_Wrapper : public phcsBfl_NfcInitator
{
    public:
        phcsBflNfcInitator_Wrapper(void);
        ~phcsBflNfcInitator_Wrapper(void);
        /*
         * \param[in]  tx_preamble_type Software preamble to be added for transmission.
         * \param[in]  rx_preamble_type Software preamble to be added for receive.
         * \param[in] *p_trxbuffer      Pointer to the transmit/receive buffer to be used for protocol operation.
         * \param[in]  trxbuffersize    Size of p_trxbuffer.
         * \param[in] *p_nfcidt_buffer  Pointer to the NFCID3t buffer.
         * \param[in] *p_lower          Pointer to the C++ object of the underlying layer.
         *
         *  The Constructor: This variant of the constructor does basic and extended (customised)
         *  initialisation in one place. It is not required to call the Initialise function of
         *  this class after allocating an object with this construction variant. Parameters of this
         *  function match those of the NfcIInitialise function regarding the purpose, however,
         *  please note that the data types partly differ.
         */
        phcsBflNfcInitator_Wrapper(uint8_t              tx_preamble_type,
                                 uint8_t              rx_preamble_type,
                                 uint8_t             *p_trxbuffer,
                                 uint16_t             trxbuffersize,
                                 uint8_t             *p_nfcidt_buffer,
                                 class phcsBfl_Io      *p_lower);

        /*!
         * \param[in]  tx_preamble_type Software preamble to be added for transmission.
         * \param[in]  rx_preamble_type Software preamble to be added for receive.
         * \param[in] *p_trxbuffer      Pointer to the transmit/receive buffer to be used for protocol operation.
         * \param[in]  trxbuffersize    Size of p_trxbuffer.
         * \param[in] *p_nfcidt_buffer  Pointer to the transmit/receive buffer to be used for protocol operation.
         * \param[in] *p_lower          Pointer to the C++ object of the underlying layer.
         *
         *  For more information see the constructor of this class (\sa phcsBflNfcInitator_Wrapper). */
        void Initialise(uint8_t             tx_preamble_type,
                        uint8_t             rx_preamble_type,
                        uint8_t            *p_trxbuffer,
                        uint16_t            trxbuffersize,
                        uint8_t            *p_nfcidt_buffer,
                        class phcsBfl_Io     *p_lower);

        phcsBfl_Status_t AtrRequest(phcsBflNfc_InitiatorAtrReqParam_t *atr_req_param);
        phcsBfl_Status_t PslRequest(phcsBflNfc_InitiatorPslReqParam_t *psl_req_param);
        phcsBfl_Status_t DepRequest(phcsBflNfc_InitiatorDepReqParam_t *dep_req_param);
        phcsBfl_Status_t AttRequest(phcsBflNfc_InitiatorAttReqParam_t *att_req_param);
        phcsBfl_Status_t DslRequest(phcsBflNfc_InitiatorDslReqParam_t *dsl_req_param);
        phcsBfl_Status_t WupRequest(phcsBflNfc_InitiatorWupReqParam_t *wup_req_param);
        phcsBfl_Status_t RlsRequest(phcsBflNfc_InitiatorRlsReqParam_t *rls_req_param);
        uint8_t RequestedToxValue(phcsBflNfc_InitiatorToxReqParam_t *req_tox_param);
        void ResetProt(phcsBflNfc_InitiatorResetParam_t *rst_param);
        void SetTRxProp(phcsBflNfc_InitiatorSetTRxPropParam_t *set_trx_properties_param);


        /*! \brief This value stores the status of object construction. Since the Constructor cannot
         *  return a value the status is stored here and should be checked upon allocation. */
        phcsBfl_Status_t m_ConstructorStatus;

        /*! \brief C-interface internal members, referenced by the mp_Members pointer in the C-structure. */
        phcsBflNfc_InitiatorCommParams_t m_CommunicationParams;
        
        /*! \brief The C-interface (kernel and function implementation) itself. */
        phcsBflNfc_Initiator_t                        m_Interface;

    protected:
        /*! \brief The Glue Class interface: This is to enable the the C-interface to call back into C++. */
        class phcsBfl_NfcInitatorGlue                 m_NfcGlue;
};

}   /* phcs_BFL */

#endif /* PHCSBFLNFCINITIATORWRAPPER_HPP */

/* E.O.F. */ 
