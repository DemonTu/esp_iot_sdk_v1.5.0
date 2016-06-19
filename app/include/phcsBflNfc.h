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
 * \file phcsBflNfc.h
 *
 * Project: NFC Target basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflNfc.h
 * $Author: bs $
 * $Revision: 1.5 $
 * $Date: Sat Jun 10 10:49:19 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  The functios declared here are used within the C and C++ of the implementation. When used by the
 *  C variant these functions, providing already full protocol facilities, are called directly. When
 *  used within a C++ environment the functions are encapsulated by a class which provides
 *  additional features such as multi-device capabilities per application. The problem is not to
 *  link underlying C to a C++ caller, it rather appears when attempting to do it vice versa.
 *  Therefore, a "glue" class exists which is a class with a static member to be called from
 *  whithin C because of its missing implicit "this" pointer.
 *
 * History:
 *  GK:  Generated 25. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLNFC_H
#define PHCSBFLNFC_H

#include <phcsBflIo.h>



/* //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// NFC Initiator functionality
*/
/*! \ingroup nfci
 *  \brief This bit instructs the DEP_REQ function to accept a further filled user buffer 
 *  for data transfer to the Target. If set, the function continues chaining to the Target
 *  with the following call. This makes two or more subsequent calls appear as one from
 *  the Target's point of view. OR this value into the flags member of DEP_REQ_PARAM.
 */
#define PHCS_BFLNFC_INITIATOR_CONT_CHAIN   (0x01)                                               /* */


/*! \ingroup nfci
 *  \brief Parameter structure for Attribute Request functionality.
 *
 *  The ATR_REQ command which takes a pointer to this structure expects the members initialised
 *  according to the NFC specification. Consequently the command returns contents according to
 *  the NFC specification.
 */
typedef struct 
{
    uint8_t *nfcid_it;    /*!< \brief [in] Pointer to a pre-initialised 10-byte (NFCID3-Initiator)
                                          buffer. \n
                                     [out] 10-byte NFCID3-Target within the pre-allocated buffer. */
                                    
    uint8_t  did;         /*!< \brief [in] DID of the Initiator. \n
                                     [out] DID of the Target(DIDs of Initiator and Target must match). */
    uint8_t  bs_it;       /*!< \brief [in] BSi byte, bit rates for sending data, supported by the Initiator. \n 
                                     [out] BSt byte, bit rates for sending data, supported by the Target. */
    uint8_t  br_it;       /*!< \brief [in] BRi byte, bit rates for receiving data, supported by the Initiator.\n 
                                     [out] BRt byte, bit rates for receiving data, supported by the Target.*/
    uint8_t  pp_it;       /*!< \brief [in] PPi byte, indicating the Initiator frame size as well as DID and NAD support
                                          by the Initiator. \n 
                                     [out] PPt byte, indicating the Target frame size as well as DID and NAD support
                                           by the Initiator.*/
    uint8_t  to_t;        /*!< \brief [out] Default timeout value of the NFC Target device. */
    uint8_t *g_it;        /*!< \brief [in] Generic bytes sent by the Initiator. \n
                                     [out] Generic bytes sent by the Target. */
    uint8_t  glen_i;      /*!< \brief [in] Length of the generic byte information field to send. */
    uint8_t  glen_t;      /*!< \brief [out] Length of the generic information sent by the Target. */
    void    *self;        /*!< \brief [in] Pointer to the C-interface of this component in   
                                          order to let this member function find its "object". 
                                          Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorAtrReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for Parameter Select Request functionality.
 */
typedef struct 
{
    uint8_t  BRS;         /*!< \brief [in] Bit rate integers for sending and receiving, according
                                          to the NFC spec, to be used by Initiator and Target. */
    uint8_t  FSL;         /*!< \brief [in] Frame size integer to be used by Initiator and Target. */
    void    *self;        /*!< \brief [in] Pointer to the C-interface of this component in   
                                          order to let this member function find its "object". 
                                          Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorPslReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for Data Exchange Request functionality.
 *
 *  DEP_REQ uses a pointer to this structure as its parameter. The behaviour of the request is according
 *  to the NFC specification. Please note that the two buffers in the parameter structure may overlap or
 *  even be the same.
 */
typedef struct 
{
    uint8_t  *input_buffer;       /*!< \brief [in] Pointer to a pre-allocated and initialised large input buffer (64K). */
    uint16_t  input_buffer_size;  /*!< \brief [in] Number of bytes to process and transmit. If required, the
                                                  function splits the input into small frames (chaining). */
    uint8_t  *output_buffer;      /*!< \brief [in] Pointer to a pre-allocated large output buffer. \n
                                             [out] Data, received from the Target and filled into the buffer. */
    uint16_t  output_buffer_size; /*!< \brief [in] Maximum size of the output buffer. The function uses this
                                                  value for buffer bounds check and returns an error in case
                                                  of overflow / insufficient space available. */
    uint8_t  *nad;                /*!< \brief [in] Pointer to the NAD used by the Initiator. A value of NULL
                                                  indicates that no NAD shall be used by the Initiator protocol. \n
                                             [out] If NULL, no NAD has been used by the Target. Otherwise,
                                                   the pointer directs to the first byte of the output buffer which
                                                   holds the NAD. In case of NAD usage the first valid output buffer
                                                   data byte is at position 1 as opposed to 0 when no NAD is used. */
    uint8_t   flags;              /*!< \brief [in,out] Contains flags to control the behaviour of DEP_REQ. The
                                                      flag masks are defined and documented in nfciflow.h. */
    void     *self;               /*!< \brief [in] Pointer to the C-interface of this component in   
                                                  order to let this member function find its "object". 
                                                  Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorDepReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for Attention Request functionality.
 */
typedef struct 
{
    void       *self;   /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorAttReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for Deselect Request functionality.
 */
typedef struct 
{
    void       *self;    /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorDslReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for Wakeup Request functionality.
 */
typedef struct 
{
    uint8_t  did;     /*!< \brief [in] New DID for the Initiator/Target pair to continue with
                                      after the WUP. */
    void    *self;    /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorWupReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for release Request functionality.
 */
typedef struct 
{
    void    *self;    /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorRlsReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for Request TOX functionality.
 *  The function uses a special formated frame of the DEP_REQ to answer.
 */
typedef struct 
{
    void   *self;    /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorToxReqParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for resetting the protocol.
 */
typedef struct 
{
    void   *self;    /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorResetParam_t;

/*! \ingroup nfci
 *  \brief Parameter structure for TX/RX properties modification functionality.
 */
typedef struct 
{
    uint8_t *p_trxbuffer;       /*!< \brief [in] Pointer to the new TRxBuffer to be used by now.*/
    uint16_t trxbuffersize;     /*!< \brief [in] Maximum size of the new TX/RX buffer.*/
    uint8_t  tx_preamble_type;  /*!< \brief [in] Specifies the type of prefix the protocol shall add to the frame. Defined in
                                         *        nfccommon.h .*/
    uint8_t  rx_preamble_type;  /*!< \brief [in] Specifies the type of prefix the protocol shall expect at the beginning of
                                         *        each frame. Defined in nfccommon.h .*/
    void    *self;    /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_InitiatorSetTRxPropParam_t;


/*  C-interface member function pointer types: */

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfcInitator_Wrapper::AtrRequest(phcsBflNfc_InitiatorAtrReqParam_t *atr_req_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Initiator_t::AtrRequest
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorAtrReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 *  This function issues an ATR_REQ and processed the Target's ATR_RES. The function is the entry
 *  point of the NFC protocol.
 *
 */
typedef phcsBfl_Status_t      (*pphcsBflNfc_InitiatorAtrReq_t)        (phcsBflNfc_InitiatorAtrReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfcInitator_Wrapper::PslRequest(phcsBflNfc_InitiatorPslReqParam_t *psl_req_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Initiator_t::PslRequest
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorPslReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 *  Immediately after ATR_RE*, the protocol allows to change the operating parameters (bit rates and
 *  frame sizes). This operation is done using this command.
 *
 */
typedef phcsBfl_Status_t      (*pphcsBflNfc_InitiatorPslReq_t)        (phcsBflNfc_InitiatorPslReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfcInitator_Wrapper::DepRequest(phcsBflNfc_InitiatorDepReqParam_t *dep_req_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Initiator_t::DepRequest
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorDepReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
*  The DEP_REQ function exchanges user data (payload) with the Target. It includes all functionality
*  according to the NFC specification (such as DID, NAD support, datalink functionality and error
*  handling). \n
*  When this function receives a request for timeout extension (RCLSTATUS_TARGET_SET_TOX) it exits.
*  In this case the application must get the requested value (NfcIRequestedToxValue) apply it and
*  restart the function. Then the function's behaviour is to exit, if everything goes well, a second
*  time however with status RCLSTATUS_TARGET_RESET_TOX. This shall trigger the application to apply
*  the old timeout values again with a subsequent second restart of the function. No modification of
*  the parameter must be done in the meantime.\n
*  The function also exits temporarily to indicate that the user buffer (the buffer the application
*  specifies and hands over to this function as one parameter member) is not sufficiently large for
*  all data to transfer. \n 
*  There are two cases:
*  <ul>
*  <li>
*  First, transfer form Initiator to Target: \n When the application sets the <em>NFCI_CONTINUE_CHAINING</em>
*  bit in the <em>flags</em> member of <em>dep_req_param</em> the protocol is instructed to indicate
*  "More Information" to the Target even if it is the last frame to be sent. Then the protocol stops
*  with no data return, waiting to be called again to resume transfer with the next buffer. This mechanism
*  can be called "Meta-Chaining" and enables the Initiator to send data of arbitrary length, exceeding 64K
*  or the limit of available RAM which could be much less.\n
*  </li>
*  <li>
*  Second, transfer from the Target to the Initiator: \n The application's responsibility is, among others, 
*  to provide RAM to the Initiator protocol for data return. The Target does not know how much the Initiator
*  has of that resource. If the Target returns more data than the Initiator buffer (again the user buffer) can
*  hold this function exits with <em>RCLSTATUS_USERBUFFER_OVERFLOW</em>, returning the data in the buffer.
*  Additionally the <em>NFCI_CONTINUE_CHAINING</em> bit in the <em>flags</em> member of <em>dep_req_param</em>
*  is set. The application can now save all data and resume receiving by simply calling the protocol again.
*  Please note that the <em>NFCI_CONTINUE_CHAINING</em> bit must remain set for the second (or further)
*  call(s), as long as the protocol signals this need. The transfer is finished when the bit is cleared
*  and/or the status is different than <em>RCLSTATUS_USERBUFFER_OVERFLOW</em>.\n
*  </li>
 *  </ul>
 *
 *  The "Meta-Chaining" feature becomes especially useful in a peripheral with limited RAM resources.
 *
 */
typedef phcsBfl_Status_t      (*pphcsBflNfc_InitiatorDepReq_t)        (phcsBflNfc_InitiatorDepReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfcInitator_Wrapper::AttRequest(phcsBflNfc_InitiatorAttReqParam_t *att_req_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Initiator_t::AttRequest
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorAttReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 * Under the hood, this function is a DEP_REQ. The Target confirms its presence with a
 * DEP(Attention)_RES. Moreover, the function is used for error handling.
 *
 */
typedef phcsBfl_Status_t      (*pphcsBflNfc_InitiatorAttReq_t)        (phcsBflNfc_InitiatorAttReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfcInitator_Wrapper::DslRequest(phcsBflNfc_InitiatorDslReqParam_t *dsl_req_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Initiator_t::DslRequest
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorDslReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * The Deselect function moves the Target out of the protocol, before the SDD loop in case of
 * passive communication mode. Targets in Active communication mode enter a special Deselect state. 
 * In order to exit this state a WUP_REQ is issued which makes it possible to apply a new DID.\n
 * All these steps allow a subsequent PSL_REQ, after SDD loop and ATR_REQ for passive or directly
 * after WUP for active Target operation.
 *
 */
typedef phcsBfl_Status_t      (*pphcsBflNfc_InitiatorDslReq_t)        (phcsBflNfc_InitiatorDslReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfcInitator_Wrapper::WupRequest(phcsBflNfc_InitiatorWupReqParam_t *wup_req_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Initiator_t::WupRequest
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorWupReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * \brief
 * This function is only available in Active communication mode and after DESELECT.
 * It allows to apply a new DID and to directly issue a further PSL_REQ.
 *
 */
typedef phcsBfl_Status_t      (*pphcsBflNfc_InitiatorWupReq_t)        (phcsBflNfc_InitiatorWupReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfcInitator_Wrapper::RlsRequest(phcsBflNfc_InitiatorRlsReqParam_t *rls_req_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Initiator_t::RlsRequest
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorRlsReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * \brief
 * The RLS_REQ function exits the protocol completely (state before selecting the communication mode,
 * RFCA, SDD).
 *
 */
typedef phcsBfl_Status_t      (*pphcsBflNfc_InitiatorRlsReq_t)        (phcsBflNfc_InitiatorRlsReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn uint8_t phcs_BFL::phcsBflNfcInitator_Wrapper::RequestedToxValue(phcsBflNfc_InitiatorToxReqParam_t *req_tox_param)
 * \else
 * \fn uint8_t phcsBflNfc_Initiator_t::RequestedToxValue
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorToxReqParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  When the NfcDataExchangeProtocolRequest exits with RCLSTATUS_TARGET_SET_TOX or
 *  RCLSTATUS_TARGET_RESET_TOX (defined in rclstatus.h) it is a temporary exit, indicating that the
 *  Target has requested extended processing time. This function enables the application to query
 *  the requested value.
 *
 */
typedef uint8_t               (*pphcsBflNfc_InitiatorToxReq_t)        (phcsBflNfc_InitiatorToxReqParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflNfcInitator_Wrapper::ResetProt(phcsBflNfc_InitiatorResetParam_t *rst_param)
 * \else
 * \fn void phcsBflNfc_Initiator_t::ResetProt
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorResetParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 * \brief
 * This function sets the protocol to initial state. The Initialise() function internally
 * calls this method. Used for protocol-reinitialisation.
 *
 */
typedef void                  (*pphcsBflNfc_InitiatorReset_t)         (phcsBflNfc_InitiatorResetParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflNfcInitator_Wrapper::SetTRxProp(phcsBflNfc_InitiatorSetTRxPropParam_t *set_trx_properties_param)
 * \else
 * \fn void phcsBflNfc_Initiator_t::SetTRxProp
 * \endif
 *
 * \ingroup nfci
 *  \par Parameters:
 *  \li \ref phcsBflNfc_InitiatorSetTRxPropParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 * The TRx buffer is internally used by the protocol to send and receive frames. This function
 * is intended for use after ATR/PSL when new settings shall be applied and the caller wants
 * to have full control over the memory available.
 * The NFC protocol can insert a preamble before the composed PDU. Select the type of preamble
 * with this function.
 *
 */
typedef void                  (*pphcsBflNfc_InitiatorSetTRxProp_t)    (phcsBflNfc_InitiatorSetTRxPropParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup nfci
 *  \brief C - Component interface: Structure to call NFC Initiator functionality. The functionality covers
 *  the NFC-Initiator part defined in the NFC specification ISO/IEC 18092.
 * 
 * \endif
 */
typedef struct 
{
    /* Methods: */
    pphcsBflNfc_InitiatorAtrReq_t AtrRequest;         /* ATR_REQ. */
    pphcsBflNfc_InitiatorPslReq_t PslRequest;         /* PSL_REQ. */
    pphcsBflNfc_InitiatorDepReq_t DepRequest;         /* DEP_REQ. */
    pphcsBflNfc_InitiatorAttReq_t AttRequest;         /* Attention command, a variant of DEP_REQ. */
    pphcsBflNfc_InitiatorDslReq_t DslRequest;         /* DSL_REQ. */
    pphcsBflNfc_InitiatorWupReq_t WupRequest;         /* WUP_REQ. */
    pphcsBflNfc_InitiatorRlsReq_t RlsRequest;         /* RLS_REQ. */
    pphcsBflNfc_InitiatorToxReq_t RequestedToxValue;  /* Get the Timeout extension value requested
                                                       by the Target. */
    pphcsBflNfc_InitiatorReset_t  ResetProt;          /* Reset the protocol into initial state. */
    pphcsBflNfc_InitiatorSetTRxProp_t
                 SetTRxProp;                        /* Change the TX/RX behaviour/resource settings. */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
    #endif

    /* Point to the lower I/O device: */
    phcsBflIo_t *mp_Lower;
} phcsBflNfc_Initiator_t;

/*! \ingroup nfci
 *  \brief Structure with internal operation parameters: 
 */
typedef struct 
{
    uint8_t   m_DID;              /*!< \brief Device ID.                                       */
    uint8_t   m_BlNr;             /*!< \brief Block Number for DEP REQ.                        */ 
    uint8_t  *mp_NfcIDt;          /*!< \brief Target NFCID.                                    */
    uint8_t   m_NadUsed;          /*!< \brief Indicate usage of NAD.                           */
    uint8_t  *mp_TRxBuffer;       /*!< \brief System-mamaged TRx buffer reference, used for
                                             internal PDU composition / analysis.             */
    uint16_t  m_MaxPduBufferSize; /*!< \brief Size of PDU buffer, excluding preamble.          */
    uint16_t  m_MaxXchBufferSize; /*!< \brief Size of PDU buffer, including preamble, for TRx. */
    uint8_t   m_TXPreambleType;   /*!< \brief Type of the preamble for the TX-frame.           */
    uint8_t   m_RXPreambleType;   /*!< \brief Type of the preamble for the RX-frame.           */
    uint16_t  m_CtrlWord;         /*!< \brief DEP REQ state control.                           */
    uint32_t  m_NextStartIndex;   /*!< \brief DEP REQ packet chaining helper var.              */
    uint8_t   m_HeaderSize;       /*!< \brief DEP_REQ header size of last block sent to target.*/
    uint8_t   m_TxBufferLength;   /*!< \brief DEP_REQ number of bytes sent to target (I-Pdu).  */
    uint8_t   m_ToxValue;         /*!< \brief TOX-value requested by the Target.
                                             Accessable via method RequestedToxValue.         */
} phcsBflNfc_InitiatorCommParams_t;




/* //////////////////////////////////////////////////////////////////////////////////////////////
// NFC Initialise:
*/
/*!
* \ingroup nfci
* \param[in] *cif   C object interface structure. The (valid) pointer to
*                   the pre-allocated structure must be provided by the caller.
* \param[in] *td    Pointer to the Communication Parameters structure. Again, this structure
*                   has to be pre-allocated.
* \param[in]  tx_preamble_type
*                   Specifies the type of prefix the protocol shall add to the frame. Defined in
*                   nfccommon.h .
* \param[in]  rx_preamble_type
*                   Specifies the type of prefix the protocol shall expect at the beginning of
*                   each frame. Defined in nfccommon.h .
* \param[in] *p_trxbuffer 
*                   Pointers to pre-allocated TX/RX buffer. This is the internal frame-composition
*                   buffer with a size of 64..255 bytes.
* \param[in]  trxbuffersize
*                   Size of the TX/RX frame composition buffer.
* \param[in] *p_nfcidt_buffer
*                   Pointer to a 10-byte pre-allocated buffer to receive the Target NFCID-3
*                   upon ATR_RES.
* \param[in] *p_lower
*                   Pointer to the lower layer, the instance of a phcsBflIo_t structure.
* 
* This function shall be called first to glue together the C-interface, its internal members parameter 
* and to establish a link to the next lower layer. Moreover, the initialisation of operation parameters 
* is done.
* 
*/
void  phcsBflNfc_InitiatorInit(phcsBflNfc_Initiator_t             *cif,
                             phcsBflNfc_InitiatorCommParams_t  *td,
                             uint8_t                          tx_preamble_type,
                             uint8_t                          rx_preamble_type,
                             uint8_t                         *p_trxbuffer,
                             uint16_t                         trxbuffersize,
                             uint8_t                         *p_nfcidt_buffer,
                             phcsBflIo_t                       *p_lower);





/* //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// NFC Target functionality
*/
/*! \name Action Type Defines for DEP_REQ/RES in the Target Endpoint
 *  \ingroup nfct
 */
/*@{*/
#define PHCS_BFLNFC_TARGET_RES_NONE     (0x10)     /*!< \brief  Send no response to previous request.\n
                                         The endpoint never receives this value from the protocol, however, if the 
                                         endpoint implementation (embedding system) decides to remain silent 
                                         (produce no response) this is the value returned to the protocol in the 
                                         endpoint parameter structure. This mechanism is enabled for DEP_RE* and 
                                         should be used testing purposes only. In order to avoid a response being 
                                         sent back to the Initiator the return status of the TargetEndpointCallback 
                                         method shall be set to a value other than PH_ERR_BFL_SUCCESS.                    */
#define PHCS_BFLNFC_TARGET_RES_TOX      (0x11)     /*!< \brief  Response with timeout extension frame.                     */
#define PHCS_BFLNFC_TARGET_RES_ACK      (0x12)     /*!< \brief  Response with acknowledge frame.                           */
#define PHCS_BFLNFC_TARGET_RES_INF      (0x13)     /*!< \brief  Response with information pdu.                             */

#define PHCS_BFLNFC_TARGET_REQ_ACK      (0x14)     /*!< \brief  DEP-REQ ACK was received. \n
                                         When chaining is used by the Target and the Initiator has confirmad the 
                                         reception of an Information Block by sending an ACK frame, the Target 
                                         endpoint receives this value. In this case, the application must generate 
                                         the next INF frame and set the action_type parameter to PHCS_BFLNFC_TARGET_RES_INF.                                  */
#define PHCS_BFLNFC_TARGET_REQ_NACK     (0x15)     /*!< \brief  DEP-REQ NACK was received. \n
                                         The application ust react according to the NFC specification, e.g by 
                                         returning the last INF block.*/
#define PHCS_BFLNFC_TARGET_REQ_INF      (0x16)     /*!< \brief  DEP-REQ information pdu was received. \n
                                         When there is no large buffer specified for the Target, this valus indicates 
                                         that an Information Block has been received. The *mi_present member indicates 
                                         wheter more information is about to come. In this case the application must 
                                         fill in ACK in order to confirm the reception of the block. This is done by 
                                         setting the action_type member to PHCS_BFLNFC_TARGET_RES_ACK and returning no data 
                                         (trx_buffer_size = 0).
                                         \n Otherwise, when the Initiator expects an Information Block 
                                         (*mi_present is FALSE) the application has to compose information, starting 
                                         with NAD, if NAD has been used by the Initiator and it is the first INF 
                                         block to return. */
#define PHCS_BFLNFC_TARGET_REQ_TOX      (0x17)     /*!< \brief  DEP-REQ TOX-reply of Initiator was received.
                                         \n Attention: Do not evaluate / change any data exchange related flags in here!
                                         These are correct again when calling next time ..REQ_INF or ..DEP_REQ! */
/*@}*/

/*! \name Action Type for 2nd PSL, DSL, RLS
 *  \ingroup nfct
 * The plain PSL_REQ, DSL_REQ, RLS_REQ code is sent to the endpoint as action ID upon PSL_REQ, DSL_REQ, 
 * RLS_REQ reception. This tells the Target application to generate, if successful, a PSL_RES, DSL_RES, 
 * RLS_RES. However, the response mustbe submitted with the old communication settings or adjustments. 
 * The Target application must change part of the parameters for PSL AFTER it has sent the response. 
 * Further more there might be some configuration actions after sending DSL_RES or RLS_RES.
 * Therefore the protocol invokes the callback a second time just to anable establishment of the new settings.
 */
/*@{*/
#define PHCS_BFLNFC_TARGET_SECOND_PSLCB    PHCS_BFLNFCCOMMON_PSL_RES       /* ... */
#define PHCS_BFLNFC_TARGET_SECOND_DSLCB    PHCS_BFLNFCCOMMON_DSL_RES       /* ... */
#define PHCS_BFLNFC_TARGET_SECOND_RLSCB    PHCS_BFLNFCCOMMON_RLS_RES       /* ... */
/*@}*/

/*! \name Target Endpoint parameter's FLAGS variable(s)
 *  \ingroup nfct
 */
/*@{*/
#define PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE         (0x01)  /*!< \brief  Is set when the Target is in active mode. */
#define PHCS_BFLNFC_TARGET_EP_FLAGS_MI_PRES        (0x02)  /*!< \brief  Is used when the Target is in non-automatic
                                                 *   chaining  mode. When the Target Endpoint
                                                 *   finds that this bit is set, more information from
                                                 *   the Initiator is about to come (chaining). If the
                                                 *   Target wants to perform chaining itself (tell the
                                                 *   Initiator that the current frame will be followed
                                                 *   by at least another one), it is the endpoint
                                                 *   implementation's responsibility to set this bit.
                                                 */
#define PHCS_BFLNFC_TARGET_EP_FLAGS_RX_OVL         (0x04)  /*!< \brief  Is set when the Target is about to get
                                                 *   a buffer overflow. The endpoint must check this
                                                 *   and move data away from the buffer behind the 
                                                 *   receive data location.
                                                 */
/*@}*/




/*! \ingroup nfct
 *  \brief Parameter structure for Target Endpoint
 *
 *  This structure is the main element for data exchange between the protocol and the user. \n
 *  \note The two pointers, representing the start of incoming/outgoing data reference the
 *  same buffer, but may point to different location within that memory range. The protocol usually
 *  changes the pointer values several times during a communication session. 
 */
typedef struct 
{
    /*! [in,out] Type of response to be generated by the application -
              defined endpoint. There are two groups of incoming values:
    \n\n      Administration Group:
    <ul>
    <li>      ATR_REQ:\n This value is handed over when the Target protocol receives
                    a valid ATR Request. The endpoint is responsible for retrieving
                    the values the Initiator has sent, starting with BSi. The protocol
                    removes information (header, DID) located before BSi. The byte order
                    and format is defined by the NFC specification (ATR_REQ sequence).
                    After retriving information from the Initiator the Target must fill
                    in its own values, again starting after the header and DID, with BSt.
                    The order of values is defined in the ATR Response, described by the
                    NFC specification.
                    \n The location to retrieve and fill in data is the trx_buffer member
                    of this structure. </li>
    <li>      PSL_REQ:\n The Target endpoint implementation is required to retrieve the BRS and
                    FSL values which have been sent by the Initiator. Again, the header is
                    omitted, therefore the data buffer (trx_buffer) starts with BRS.
                    \n  No information can be returned with this function.
                    \n  Internally, the protocol generates a response, still with the old parameters. </li>
    <li>      PSL_RES:\n After the protocol has generated its response to the PSL_REQ (see item above)
                    the endpoint is called again with this type of action. Since a PSL changes
                    communication parameters and the response is sent with the old values, the
                    Target must have the chance to apply the new settings, which is done in this 
                    function: The NFC Target endpoint implementation would now be required to change
                    the RF chip and communication settings to the new baudrate and frame size. This
                    function does not generate a response, no data can be returned. </li>
    <li>      DSL_REQ:\n The Target protocol has received a DSL request from the Initiator and
                    already entered deselected state. The purpose of the callback is to
                    inform the underlying application of this event. It neither provides nor
                    returns information. </li>
    <li>      WUP_REQ:\n This is the reverse operation to DSL. Also, this operation neither provides nor
                    returns information. The sole purpose is the information of the underlying
                    application.
                    \n This action is only enabled when in active mode, according to the NFC specification. </li>
    <li>      RLS_RES:\n When the protocol has already entered the released state the callback, again
                    providing and taking no information, merely informs the application of the event. </li>
    <li>      NFC_RES_NONE:\n The endpoint never receives this value from the protocol, however, if the endpoint
                    implementation (embedding system) decides to remain silent (produce no response) this
                    is the value returned to the protocol in the endpoint parameter structure. This mechanism
                    is enabled for DEP_RE* and should be used testing purposes only. In order to avoid a response
                    being sent back to the Initiator the return status of the TargetEndpointCallback method
                    shall be set to a value other than RCLSTATUS_SUCCESS.
    </ul>
            The action_type parameter needs to be left unchanged, only the return status of the
            callback can be used to indicate application-depending success or failure.

    \n\n    Communication Group
    <ul>
    <li>      NFC_DEP_REQ:\n In case that the NFC Target has been initialised to use a large buffer as an
                        application buffer the protocol performs chaining automatically. After receiving
                        a large amount of data and placing it in the buffer, the Target endpoint is called
                        in order to indicate the presence of data, sent by the Initiator. Data starts at
                        the beginning or at offset 1, depending on the content of the nad_present member.
                        \n When returning data the application must copy data into the trx_buffer. The protocol
                        performs chaining automatically. However, if the nad_preset member is TRUE, the
                        Target must also use NAD therefore the first data byte has to contain the NAD. </li>
    <li>      NFC_REQ_INF:\n When there is no large buffer specified for the Target, this valus indicates that an
                        Information Block has been received. The *mi_present member indicates wheter more
                        information is about to come. In this case the application must fill in ACK in order
                        to confirm the reception of the block. This is done by setting the action_type member
                        to NFC_RES_ACK and returning no data (trx_buffer_size = 0).
                        \n Otherwise, when the Initiator expects an Information Block (*mi_present is FALSE)
                        the application has to compose information, starting with NAD, if NAD has been used
                        by the Initiator and it is the first INF block to return. </li>
    <li>      NFC_REQ_ACK:\n When chaining is used by the Target and the Initiator has confirmad the reception of an
                        Information Block by sending an ACK frame, the Target endpoint receives this value. In this
                        case, the application must generate the next INF frame and set the action_type parameter to
                        NFC_RES_INF. </li>
    <li>      NFC_REQ_NACK:\n The application ust react according to the NFC specification, e.g by returning the last
                        INF block. </li>
    <li>      NFC_REQ_TOX:\n The Target has requested a TOX (Timeout Extension Request). A TOX can be sent instead of
                        an Information block. When this value is received the Target must continue with normal
                        operation.
                        \n When the Target sends a TOX the only byte in the trx_buffer must be the TOX value requested.
                        Moreover the Target must, in order to issue a TOX request, set the action_type member
                        to NFC_RES_TOX. </li>
    </ul>
    */
    uint8_t   action_type;                                    
    /*! \brief [in,out] Upon reception of information (INF block) this member indicates, when not NULL, that NAD has
                     been inserted into the (first) INF frame sent by the Initiator. Consequently, the Target has to insert
                     NAD into the first INF block to return. If the pointer is valid (nor NULL) it represents the
                     location where to read/write the NAD byte from/to. In non-automatic chaining mode the Target
                     endpoint receives the NAD from the Initiator with the first INF block arriving (nad != NULL for the
                     first time) and submits its own NAD byte with the first INF block returned (second time, nad != NULL). */
    uint8_t  *nad;
    /*! \brief [in] This is the pointer indicating the start of the incoming data memory range within the protocol-operated
                    data exchange buffer. The endpoint application uses this pointer to receive the incoming
                    data of a XXX_REQ frame or the entire data in case of automatic chaining. The pointer is dynamically changed
                    by the protocol. The memory ranges for incoming and outgoing data widely overlap. */
    uint8_t  *p_req_data;
    /*! \brief [out] This is the pointer indicating the start of the outgoing data memory range within the protocol-operated
                    data exchange buffer. The endpoint application uses this pointer to submit the outgoing
                    data of a XXX_RES frame or the entire data in case of automatic chaining. The pointer is dynamically changed
                    by the protocol. The memory ranges for incoming and outgoing data widely overlap. */
    uint8_t  *p_res_data;
    /*! \brief [in] A protocol's call to the endpoint sets this member to the current number of payload data bytes received
                    and ready to be processed. */
    uint16_t  req_data_length;
    /*! \brief [in,out] The protocol sets this member to the maximum number of bytes the endpoint can return with the current call.
                    The endpoint must set this member to the number of bytes to actually return. */
    uint16_t  res_data_length;
    /*! \brief [in] This member points to the address of the Target protocol. With an implementation, identifying multiple Target
                    objects by their addresses, it would be possible to run a "Logical Multi-Target" device. */
    void           *target;
    /*! \brief [in] The <em>flags</em> member holds additional information for the Target protocol. File nfctflow.h specifies constants
                    or bit-masks respectively, each representing a certain information. */
    uint8_t   flags;
    /*! \brief [out] Indicates if the endpoint is called 1, in the middle or the last during chaining procedure.*/
    uint8_t   ovr_type;
    /*! \brief [in] Pointer to a location which is user (embedding-software) defined. See NFC-Target interface function SetPUser .*/
    void     *p_user;  
    /*! \brief [in] This member points to the address of the Target protocol endpoint (current instance). It is valid
                    for the C-variant. */
    void     *self;

} phcsBflNfc_TargetEndpointParam_t;



/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBfl_NfcTargetEndpoint::TargetEndpointCallback(phcsBflNfc_TargetEndpointParam_t* endpoint_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_TargetEndpoint_t::TargetEndpointCallback
 * \endif
 *
 * \ingroup nfct
 *  \par Parameters:
 *  \li \ref phcsBflNfc_TargetEndpointParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  The NFC Target endpoint is the interface between the "re-acting" NFC_Target protocol and the
 *  application behind. Action processing, triggered by an incoming request sent by the Initiator,
 *  is done via an application-defined callback, namely the Target Endpoint. The application must
 *  take care of the implementation according to this interface as well as of correct data composition.
 */
typedef phcsBfl_Status_t (*pphcsBflNfc_TargetEndpoint_t) (phcsBflNfc_TargetEndpointParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup nfct
 *  \brief C - Component interface: Structure to call NFC Target functionality, according to ISO/IEC 18092.
 *
 *  The Target Endpoint Callback takes a pointer to the phcsBflNfc_TargetEndpointParam_t as its parameter
 *  structure and returnes a phcsBfl_Status_t value. The return value indicates success or failure of the
 *  application-related processing to the protocol. If a value other than RCLSTATUS_SUCCESS is returned
 *  by the endpoint the protocol will not transmit back any data and the main protocol dispatcher
 *  function also returns a value other than RCLSTATUS_SUCCESS. This value may be taken up by the 
 *  RX loop surrounding the dispatcher in order to re-initialise the receiver.
 * 
 * \endif
 */
typedef struct 
{
    /* Endpoint callback: */
    pphcsBflNfc_TargetEndpoint_t TargetEndpointCallback;

    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
    #endif
} phcsBflNfc_TargetEndpoint_t;



/* *************************************************************************************************
 * N F C   T A R G E T
 *
 * Target protocol interface:
 ************************************************************************************************ */
/*! \ingroup nfct
 *  \brief Parameter structure for Target Dispatch functionality.
 */
typedef struct 
{
    /*! \brief [in] System-managed TX/RX buffer reference, passed to the protocol by the embedding software. */
    uint8_t  *buffer;
    /*! \brief [in] TRX buffer length, including preamble. When the embedding receive-loop (managed by the
             system software, not the library) detects an error in the incoming request (e.g. CRC),
             the length parameter has to set to zero. This is because the protocol must be aware of
             any incoming request in order to properly track internal states. */
    uint16_t length;
    void     *self;  /*!< \brief [in] Pointer to the C-interface of this component in   
                                     order to let this member function find its "object". 
                                     Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_TargetDispatchParam_t;

/*! \ingroup nfct
 *  \brief Parameter structure for Target Reset functionality.
 */
typedef struct 
{
    uint8_t   passive_mode;   /*!< \brief [in] Boolean value, determining mode of operation, if TRUE,
                                              the Target is passive, if false, active. */
    void     *self;           /*!< \brief [in] Pointer to the C-interface of this component in   
                                              order to let this member function find its "object". 
                                              Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_TargetResetParam_t;

/*! \ingroup nfct
 *  \brief Parameter structure for Target buffer (large user buffer) settings.
 *
 *  This buffer must be specified (p_target_buffer != NULL  and  target_buffer_size != 0) in order to let
 *  the protocol perform automatic chaining.
 */
typedef struct 
{
    uint8_t  *p_target_buffer;    /*!< \brief [in] Large buffer, used by the Target application for
                                                  data exchange. The protocol automatically puts data sent
                                                  by the Initiator into this buffer. Consequently, the
                                                  protocol takes data to send to the Initiator from this buffer. */
    uint16_t  target_buffer_size; /*!< \brief [in] Size of the large buffer (max. 64KByte). */
    void     *self;               /*!< \brief [in] Pointer to the C-interface of this component in   
                                                  order to let this member function find its "object". 
                                                  Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_TargetSetUserBufParam_t;

/*! \ingroup nfct
 *  \brief Parameter structure for Target small TX/RX buffer (frame) properties.
 */
typedef struct 
{
    uint8_t  max_trx_buffer_size; /*!< \brief [in] Size of the internally used TX/RX frame buffer. This
                                                  buffer is managed by the embedding software and handed over
                                                  to the NFC-T protocol's Dispatch() function in the
                                                  phcsBflNfc_TargetDispatchParam_t. The protocol must know the
                                                  reserved size of this buffer in order to avoid buffer bounds
                                                  overflow errors and to perform chaining. */
    uint8_t  req_preamble_type;   /*!< \brief [in] Type of preamble (specified in nfccommon.h) the protocol
                                                  shall expect when receiving data from the Initiator. */
    uint8_t  res_preamble_type;   /*!< \brief [in] Type of preamble (specified in nfccommon.h) the protocol
                                                  shall create when sending data back to the Initiator. */
    void    *self;                /*!< \brief [in] Pointer to the C-interface of this component in   
                                                  order to let this member function find its "object". 
                                                  Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_TargetSetTRxBufPropParam_t;

/*! \ingroup nfct
 *  \brief Parameter structure for Target enpoint to set user pointer to hand over parameters.
 */
typedef struct 
{
    void          *p_user;              /*!< \brief [in] Pointer to a structure or other arbitrary information
                                                  which the embedding software wants to hand over to the
                                                  Target endpoint. Whenever the protocol calls the endpoint
                                                  the user pointer is accessible by the endpoint callback. */
    void          *self;                /*!< \brief [in] Pointer to the C-interface of this component in   
                                                  order to let this member function find its "object". 
                                                  Should be left dangling when calling the C++ wrapper.*/
} phcsBflNfc_TargetSetPUserParam_t;



/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfc_TargetWrapper::Dispatch(phcsBflNfc_TargetDispatchParam_t* dispatch_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Target_t::Dispatch
 * \endif
 *
 * \ingroup nfct
 *  \par Parameters:
 *  \li \ref phcsBflNfc_TargetDispatchParam_t [in]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  Main entry point, contains state machine to handle request/response sequences.
 *  \note phcsBfl_Status_t code \n If the return value indicates unsuccessful completion the embedding software (which is 
 *        essentially the receive loop) must get back into receiving mode. No transmission has been triggered by 
 *        the protocol in this case and no transmission must be performed by the embedding software.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflNfc_TargetDispatch_t)       (phcsBflNfc_TargetDispatchParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflNfc_TargetWrapper::ResetProt(phcsBflNfc_TargetResetParam_t *reset_param)
 * \else
 * \fn void phcsBflNfc_Target_t::ResetProt
 * \endif
 *
 * \ingroup nfct
 *  \par Parameters:
 *  \li \ref phcsBflNfc_TargetResetParam_t [in]: Pointer to the I/O parameter structure.
 *
 * This function resets the protocol to initial state before ATR_REQ. This function can also be called 
 * after a RLS_RE* procedure to clear all internal states for the next activation.
 *
 */
typedef void (*pphcsBflNfc_TargetReset_t)          (phcsBflNfc_TargetResetParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflNfc_TargetWrapper::SetTRxBufProp(phcsBflNfc_TargetSetTRxBufPropParam_t *trx_buffer_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflNfc_Target_t::SetTRxBufProp
 * \endif
 *
 * \ingroup nfct
 *  \par Parameters:
 *  \li \ref phcsBflNfc_TargetSetTRxBufPropParam_t [in]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *
 * This function sets or modifys the TargetBuffer for internal chaining. 
 * \note The buffer specified with this function is a large user buffer for data to send
 *       or receive respectively. Chaining is done automatically, internally, by the 
 *       protocol, if such a buffer is specified.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflNfc_TargetSetTRxBufProp_t)   (phcsBflNfc_TargetSetTRxBufPropParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflNfc_TargetWrapper::SetUserBuf(phcsBflNfc_TargetSetUserBufParam_t *user_buffer_param)
 * \else
 * \fn void phcsBflNfc_Target_t::SetUserBuf
 * \endif
 *
 * \ingroup nfct
 *  \par Parameters:
 *  \li \ref phcsBflNfc_TargetSetUserBufParam_t [in]: Pointer to the I/O parameter structure.
 *
 * This function sets or modifys the TargetBuffer for internal chaining. 
 * \note The buffer specified with this function is a large user buffer for data to send
        or receive respectively. Chaining is done automatically, internally, by the 
        protocol, if such a buffer is specified.
 */
typedef void (*pphcsBflNfc_TargetSetUserBuffer_t)       (phcsBflNfc_TargetSetUserBufParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflNfc_TargetWrapper::SetPUser(phcsBflNfc_TargetSetPUserParam_t *p_user_param)
 * \else
 * \fn void phcsBflNfc_Target_t::SetPUser
 * \endif
 *
 * \ingroup nfct
 *  \par Parameters:
 *  \li \ref phcsBflNfc_TargetSetPUserParam_t [in]: Pointer to the I/O parameter structure.
 *
 * Sets a user pointer which might used for several interactions between main function and Target
 * callback implementation. This is an general purpose pointer which may carry different data.
 * Both, the main function and the callback have to know about the contents!
 *
 */
typedef void (*pphcsBflNfc_TargetSetPUser_t)            (phcsBflNfc_TargetSetPUserParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup nfct
 *  C - Component interface: Structure to call NFC Target functionality.
 *
 *  The embedding software runs within a receive loop. Each time data has been received, the
 *  embedding software must call the Dispatch member function, handing over the paramter.
 * 
 * \endif
 */
typedef struct 
{
    /* Methods: */
    pphcsBflNfc_TargetDispatch_t                Dispatch;               /* Dispatch - Target protocol entry point.  */
    pphcsBflNfc_TargetReset_t                   ResetProt;          /* Reset the Target protocol state machine. */
    pphcsBflNfc_TargetSetTRxBufProp_t           SetTRxBufProp; /* Set small TX/RX frame buffer properties. */
    pphcsBflNfc_TargetSetUserBuffer_t           SetUserBuf;          /* Set large Target user buffer properties. */
    pphcsBflNfc_TargetSetPUser_t                SetPUser;               /* Set the user pointer of the C-interface. */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
    #endif

    phcsBflNfc_TargetEndpoint_t *mp_Upper;    /* Point to the "upper" target endpoint function: */
    phcsBflIo_t               *mp_Lower;    /* Point to the lower I/O device: */
} phcsBflNfc_Target_t;


/*! \ingroup nfct
 *   Structure with operation parameters, internally used by the protocol. This structure is
 *   referenced by the mp_Members pointer in the interface of the component.
 */
typedef struct 
{
    uint8_t   m_DID;                /*!< \brief Device ID.                                               */
    uint8_t   m_BlNr;               /*!< \brief Block Number for DEP REQ.                                */ 
    uint8_t  *mp_NfcIDt;            /*!< \brief Target NFCID.                                            */
    uint8_t  *mp_NfcIDi;            /*!< \brief Initiator NFCID.                                         */
    uint8_t   m_NadUsed;            /*!< \brief Indicate usage of NAD.                                   */
    uint8_t   m_NAD;                /*!< \brief Stores NAD for internal use.                             */
    uint8_t   m_State;              /*!< \brief Current state of Target protocol state machine.          */
    uint8_t   m_LastAction;         /*!< \brief Last action of protocol (repeat action i.c.o. error).    */
    uint16_t  m_CtrlWord;           /*!< Target state control and flags: \n
                                                    b0-b2: RFU, \n
                                                    b3   : First Request, \n
                                                    b4   : Passive Mode, \n
                                                    b5   : PSL handling \n
                                                    b6   : Rcv. Chaining finished, \n
                                                    b7   : NAD only valid at 1st block, \n
                                                    b8   : MI present if bit is set, \n
                                                    b9   : NAD was sent with previous block, \n
                                                    b10  : Set during Timeout Extension (TOX) handling, \n
                                                    b11  : Set in case of Target buffer overflow, \n
                                                    b12  : Set with b11, cleared when NFC-T has
                                                           finished receiving data.\n
                                                    b13  : Controls whether to respond to ATR_REQ.      */
    uint8_t   m_ReqPreambleType;    /*!< \brief The type of preamble the protocol expects for requests.  */  
    uint8_t   m_ResPreambleType;    /*!< \brief The type of preamble the protocol uses for responses.    */  
    uint8_t   m_MaxPduBufferSize;   /*!< \brief Maximum size of the PDU (de-)composition buffer.         */
    uint8_t   m_MaxXchBufferSize;   /*!< \brief Maximum size of the RX/TX system buffer.                 */
    uint8_t  *mp_TargetBuffer;      /*!< \brief Pointer to large data buffer with file / data to xch.    */
    uint16_t  m_TargetBufferSize;   /*!< \brief Number of bytes currently written to large buffer.       */
    uint16_t  m_MaxTargetBufferSize;/*!< \brief Maximum size of large file / data buffer.                */
    uint32_t  m_NextStartIndex;     /*!< \brief DEP_REQ packet chaining helper var.                      */
    uint16_t  m_SizeOfLastBlock;    /*!< \brief Size of last data block sent to Initiator.               */
    void     *mp_User;              /*!< \brief User-defined pointer to be handed over to the endpoint.  */
} phcsBflNfc_TargetCommParam_t;



/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Target Initialise: Sets up the protocol environment and resets the protocol to initial state
// before ATR_REQ.
*/
/*!
* \ingroup nfct
* \param[in] *cif                   Pointer to the C-interface of this component. Structure must 
*                                   be pre-allocated.
* \param[in] *td                    Pointer to the internal operation variables structure which
*                                   must be pre-allocated.
* \param[in]  req_preamble_type     Type of preamble to expect together with incoming requests,
*                                   defined in nfccommon.h .
* \param[in]  res_preamble_type     Type of preamble to return to the Initiator with the response,
*                                   defined in nfccommon.h .
* \param[in] *p_target_buffer       Pointer to a large user buffer, provided by the embedding software.
*                                   This buffer is used by the protocol as an information destination
*                                   when receiving and as a source when returning user data. Set to
*                                   NULL in order to disable automatic frame handling (chaining).
* \param[in]  target_buffer_size    Size of the large data buffer for application (embedding software)
*                                   use. Set to zero in order to disable automatic frame handling
*                                   (chaining).
* \param[in] *p_nfcidi_buffer       Pointer to a pre-allocated 10-byte buffer receiving the NFCID-3
*                                   of the Initiator upon ATR_REQ.
* \param[in] *p_nfcidt_buffer       Pre-allocated and initialised 10-byte buffer for the Target protocol
*                                   to provide the Target's NFCID-3.
* \param[in]  passive_mode          Boolean specifier of the operation mode. If set (TRUE) the Target
*                                   is told to work according to the specification of passive mode
*                                   operation. If zero (FALSE) the protocol selects active mode.
* \param[in] *p_upper               Pointer to the protocol's endpoint. This is a component with a defined
*                                   interface, however the implementation must be defined by the embedding
*                                   software (since it is the point where requests arrive and responses are
*                                   put together).
* \param[in] *p_lower               Pointer to a component representing the lower instance, responsible
*                                   for raw data exchange (RC-I/O component).
*
* \retval  PH_ERR_BFL_SUCCESS        Operation successful.
*
* \note The small TX/RX buffer (TRxBuffer) used for building individual frames needs not to be specified here.
*       It comes together with the Dispatch member function and must be managed by the embedding
*       software. The size of the buffer (to indicate how many bytes the Target can return) is set to the default
*       value (64). After ATR_REQ / PSL_REQ the final buffer size for the current session is fixed. In order to
*       apply the corresponding settings, call the SetTRxBufProp function.\n
*       The large buffer is for easy protocol access when there are sufficient resources. However, in a system
*       without abundant resources it may be desirable to control the Target protocol behaviour when sending and
*       receiving data. In order to disable automatic chaining (handle individual frames with the embedding software)
*       the user buffer pointer must be set to NULL and íts size to zero.
*/
void phcsBflNfc_TargetInit( phcsBflNfc_Target_t               *cif,
                            phcsBflNfc_TargetCommParam_t     *td,
                            uint8_t                    req_preamble_type,
                            uint8_t                    res_preamble_type,
                            uint8_t                   *p_target_buffer,
                            uint16_t                   target_buffer_size,
                            uint8_t                   *p_nfcidi_buffer,
                            uint8_t                   *p_nfcidt_buffer,
                            uint8_t                    passive_mode,
                            phcsBflNfc_TargetEndpoint_t      *p_upper,
                            phcsBflIo_t                 *p_lower);



#endif /* PHCSBFLNFC_H */

/* E.O.F. */
