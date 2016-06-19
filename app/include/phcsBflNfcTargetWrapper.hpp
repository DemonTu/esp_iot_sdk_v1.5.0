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
 * \file phcsBflNfcTargetWrapper.hpp
 *
 * Project: NFC Target.   
 *
 * Workfile: phcsBflNfcTargetWrapper.hpp
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


#ifndef PHCSBFLNFCTARGETWRAPPER_HPP
#define PHCSBFLNFCTARGETWRAPPER_HPP


#include <phcsBflIoWrapper.hpp>


extern "C"
{
    #include <phcsBflNfc.h>
}


namespace phcs_BFL
{

/* *************************************************************************************************
 * N F C   T A R G E T   E N D P O I N T
 ************************************************************************************************ */
/*! \ingroup nfct
 *  \brief Target callback function, protocol endpoint
 *  (abstract class, used for the C++ interface).
 */
class phcsBfl_NfcTargetEndpoint
{
    public:
        /*!
         * \param[in] *endpoint_param   Pointer to the I/O parameter structure
         * \return                      phcsBfl_Status_t value depending on the C implementation
         *
         *  Interface definition:
         *  Command set used  by upper objects or application. 
         */
        virtual phcsBfl_Status_t TargetEndpointCallback(phcsBflNfc_TargetEndpointParam_t* endpoint_param) = 0;
};

/*! \cond cond_Cpp_complete */
/* *************************************************************************************************
 * N F C   T A R G E T
 ************************************************************************************************ */
/* \ingroup nfct
 * \brief Target protocol interface abstract class, used for the C++ interface.
 */
class phcsBfl_NfcTarget
{
    public:
        virtual void Initialise(uint8_t                req_preamble_type,
                                uint8_t                res_preamble_type,
                                uint8_t               *p_target_buffer,
                                uint16_t               target_buffer_size,
                                uint8_t               *p_nfcidi_buffer,
                                uint8_t               *p_nfcidt_buffer,                        
                                uint8_t                passive_mode,
                                class phcsBfl_NfcTargetEndpoint   *p_nfc_target_endpoint,
                                class phcsBfl_Io                  *p_lower)                       = 0;

        virtual phcsBfl_Status_t Dispatch(phcsBflNfc_TargetDispatchParam_t* dispatch_param)             = 0;
        virtual void ResetProt(phcsBflNfc_TargetResetParam_t *reset_param)                              = 0;
        virtual void SetUserBuf(phcsBflNfc_TargetSetUserBufParam_t *user_buffer_param)                  = 0;
        virtual phcsBfl_Status_t SetTRxBufProp(phcsBflNfc_TargetSetTRxBufPropParam_t *trx_buffer_param) = 0;
        virtual void SetPUser(phcsBflNfc_TargetSetPUserParam_t *p_user_param)                           = 0;

        /*! Lower edge interface: Not to be redefined in the derived class: */
        class phcsBfl_Io               *mp_Lower;
        /*! Upper edge (endpoint) interface: Not to be redefined in the derived class: */
        class phcsBfl_NfcTargetEndpoint *mp_Upper;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/* \ingroup nfct
 *  This class glues together the C-implementation of NFC-Target and the overlying functionality. 
 *  By declaring the member as _ static _ the method gets no implicit _ this _ pointer and can thus 
 *  be called by any C function. However, the function must be informed about the target object
 *  of type phcsBfl_NfcTarget (casted to void*) to be called further up the stack.
 */
class phcsBfl_NfcTargetGlue
{
    public:
        phcsBfl_NfcTargetGlue(void);
        ~phcsBfl_NfcTargetGlue(void);

        /*! \name Static function of lower layer, enables to call into C++ again: */
        //@{
        static phcsBfl_Status_t TargetEndpointCallback(phcsBflNfc_TargetEndpointParam_t* endpoint_param);
        static phcsBfl_Status_t Transmit(phcsBflIo_TransmitParam_t *transmit_param);
        //@}

        /*! \name Glue class structures
         *  Structures accessible from the upper object's pure C-kernel: \n
         *  These structures represent the Components the C-kernel expects, however thir sole
         *  purpose is to direct a call from within C back to C++.
         */
        //@{
        phcsBflIo_t              m_GlueRcioStruct;             /*!< Glue class-internal lower C-interface. */
        phcsBflNfc_TargetEndpoint_t   m_GlueProtocolEndpointStruct; /*!< Glue class-internal endpoint C-interface. */
        //@}
};
/*! \endcond */


////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \ingroup nfct 
 *  Class to present a communication channel to a specific target. */
class phcsBflNfc_TargetWrapper : public phcsBfl_NfcTarget
{
    public:
        phcsBflNfc_TargetWrapper(void);
        ~phcsBflNfc_TargetWrapper(void);
        
        /*
         * \param[in]  req_preamble_type        Software preamble to be added for receive.
         * \param[in]  res_preamble_type        Software preamble to be added for transmission.
         * \param[in] *p_target_buffer          Pointer to the transmit/receive buffer to be used for protocol operation.
         * \param[in]  target_buffer_size       Size of p_target_buffer.
         * \param[in] *p_nfcidi_buffer          Pointer to the NFCID3i buffer.
         * \param[in] *p_nfcidt_buffer          Pointer to the NFCID3t buffer.
         * \param[in]  passive_mode             Indicator of acting in passive or active mode.
         * \param[in] *p_nfc_target_endpoint    Pointer to the user endpoint callback function.
         * \param[in] *p_lower                  Pointer to the C++ object of the underlying layer.
         *
         * Construction with parameters to do basic and extended initialisation in one place. This
         * variant calls the Initialise function internally, therefore the parameters match.
         */
        phcsBflNfc_TargetWrapper(uint8_t                         req_preamble_type,
                          uint8_t                         res_preamble_type,
                          uint8_t                        *p_target_buffer,
                          uint16_t                        target_buffer_size,
                          uint8_t                        *p_nfcidi_buffer,
                          uint8_t                        *p_nfcidt_buffer,
                          uint8_t                         passive_mode,
                          class phcsBfl_NfcTargetEndpoint  *p_nfc_target_endpoint,
                          phcsBfl_Io                       *p_lower);

        
        /*!
         * \param[in]  req_preamble_type        Software preamble to be added for receive.
         * \param[in]  res_preamble_type        Software preamble to be added for transmission.
         * \param[in] *p_target_buffer          Pointer to the transmit/receive buffer to be used for protocol operation.
         * \param[in]  target_buffer_size       Size of p_target_buffer.
         * \param[in] *p_nfcidi_buffer          Pointer to the NFCID3i buffer.
         * \param[in] *p_nfcidt_buffer          Pointer to the NFCID3t buffer.
         * \param[in]  passive_mode             Indicator of acting in passive or active mode.
         * \param[in] *p_nfc_target_endpoint    Pointer to the user endpoint callback function.
         * \param[in] *p_lower                  Pointer to the C++ object of the underlying layer.
         *
         * Initialises object, in the same way as the constructor does:
         * The parameters widely match with those of NfcTInitialise, however some differences exist
         * regarding the type.
         */
        void Initialise(uint8_t                         req_preamble_type,
                        uint8_t                         res_preamble_type,
                        uint8_t                        *p_target_buffer,
                        uint16_t                        target_buffer_size,
                        uint8_t                        *p_nfcidi_buffer,
                        uint8_t                        *p_nfcidt_buffer,                        
                        uint8_t                         passive_mode,
                        class phcsBfl_NfcTargetEndpoint  *p_nfc_target_endpoint,
                        phcsBfl_Io                       *p_lower);

        phcsBfl_Status_t Dispatch(phcsBflNfc_TargetDispatchParam_t* dispatch_param);
        void ResetProt(phcsBflNfc_TargetResetParam_t *reset_param);
        phcsBfl_Status_t SetTRxBufProp(phcsBflNfc_TargetSetTRxBufPropParam_t *trx_buffer_param);
        void SetUserBuf(phcsBflNfc_TargetSetUserBufParam_t *user_buffer_param);
        void SetPUser(phcsBflNfc_TargetSetPUserParam_t *p_user_param);

        /*! \brief Status of Constructor, stored in this variable. Since the Constructor itself cannot
         *  return a value, it is recommended to check this member after object creation. */
        phcsBfl_Status_t          m_ConstructorStatus;

        /*! \brief Underlying C-kernel interface structure instance. */
        phcsBflNfc_Target_t             m_Interface;

        /*! \brief Parameter structure referenced by the C-kernel's mp_Members pointer. */
        phcsBflNfc_TargetCommParam_t   m_CommunicationParams;

    protected:
        /*! \brief Pointer to the Glue object, the bridge for the C-kernel back to C++. */
        class phcsBfl_NfcTargetGlue   m_NfcGlue;             
};

}   /* phcs_BFL */

#endif /* PHCSBFLNFCTARGETWRAPPER_HPP */

/* E.O.F. */
