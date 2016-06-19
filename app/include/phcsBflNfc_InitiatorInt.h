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
 * \file phcsBflNfc_InitiatorInt.h
 *
 * Project: NFC Initiator basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflNfc_InitiatorInt.h
 * $Author: mha $
 * $Revision: 1.2 $
 * $Date: Wed Jun  7 09:47:38 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  The functios declared here are used within the C and C++ of the implementation. When used by the
 *  C variant these functions, providing already full protocol facilities, are called directly. When
 *  used within a C++ environment the functions are encapsulated by a class which provides
 *  additional features such as multi-device capabilities per application.
 *
 * History:
 *  MHa: Generated September 2005
 *
 */


#ifndef PHCSBFLNFC_INITIATORINT_H
#define PHCSBFLNFC_INITIATORINT_H



/* DEP_REQ/RES frame types: */
#define NFC_NACK             (0)         /* ACK/NACK pdu is a NotAcknowledge frame.      */
#define NFC_ACK              (1)         /* ACK/NACK pdu is a Acknowledge frame.         */
#define NFC_TO_EXT           (2)         /* Timeout Extension frame.                     */
#define NFC_ATT              (3)         /* Attention frame.                             */
#define NFC_INF              (4)         /* Information pdu.                             */
#define NFC_ANY              (5)         /* No defined pdu!                              */

/* Defines for Control-Word:                                                                    */
#define NFC_CTRL_STATE_MASK      (uint16_t)(0x000F)    /* Mask to get state coded in the 
                                                        ctrl-word. bin: 00001111 b3-b0 used.            */
#define NFC_CTRL_STATE_MASK_I    (uint16_t)(0xFFF0)    /* Bit inversion of NFC_CTRL_STATE_MASK.        */
#define NFC_CTRL_TOX_PRESENT     (uint16_t)(0x0010)    /* Is set if TOX-request was handled by the
                                                        protocol and TO-value must be reset.            */
#define NFC_CTRL_MI_PRESENT      (uint16_t)(0x0020)    /* This bit is set if MI is has to be set.         */
#define NFC_CTRL_NAD_PRESENT     (uint16_t)(0x0040)    /* This bit is set if NAD was already sent     
                                                        in the first block of data exchange.            */
#define NFC_CTRL_NAD_PRESENT_I   (uint16_t)(0xFFBF)    /* Bit inversion of NFC_CTRL_NAD_PRESENT.        */
#define NFC_CTRL_NAD_PREV_USED   (uint16_t)(0x0080)    /* NAD was used at previously sent block.          */
#define NFC_CTRL_PREV_STATE_MASK (uint16_t)(0x0F00)    /* Mask to get the previous state coded in the
                                                        ctrl-word. bin: 1111 0000 00.. b11-b8 used.     */
#define NFC_CTRL_PREV_STATE_MASK_I (uint16_t)(0xF0FF)  /* Bit inversion of NFC_CTRL_PREV_STATE_MASK.   */
#define NFC_CTRL_CHAINING_CONTD  (uint16_t)(0x1000)    /* Set to indicate that the last function call
                                                        did not deliver all information to the Target.
                                                        More information is following with this call.   */
#define NFC_RESET_USER_BUFFER	 (uint16_t)(0x2000)    /* If user buffer and chaining for both sides is 
												        used the user buffer has to be set to zero when
												        the first data chunk is received from the target */

/* PFB defines:                                                                                 */
#define NFC_PFB_NAD_BIT_SET      (uint8_t)(0x08)      /* A mask to determine whether NAD bit is set 
                                                        in the PFB of a DEP_REQ/RES block.                */

/* Error masks:                                                                                 */
#define NFC_CURRENT_ERROR_MASK   (uint8_t)(0x0F)      /* Mask to get current error state  (0000 1111).   */
#define NFC_PREVIOUS_ERROR_MASK  (uint8_t)(0xF0)      /* Mask to get previous error state (1111 0000).   */

/* Defines for DEP-Request state-machine:                                                       */
#define NFC_DEP_STATE_RS        (0)      /* Start of DEP-Request.                               */   
#define NFC_DEP_STATE_EA        (1)      /* DEP Expect Acknowledge frame.                       */   
#define NFC_DEP_STATE_EI        (2)      /* DEP Expect Information frame.                       */   
#define NFC_DEP_STATE_SA        (3)      /* DEP Send Acknowledge frame. (part of EI)            */
#define NFC_DEP_STATE_RE        (4)      /* End of DEP-Request.                                 */   
#define NFC_DEP_STATE_TE        (5)      /* Temporary DEP end due to TOX request by Target.     */   
#define NFC_DEP_STATE_TS        (6)      /* Re-start of DEP after applying TOX.                 */
#define NFC_DEP_STATE_OE        (7)      /* Temp. end of DEP after end of user buffer reached.  */

/* Error state control: These values never make it to the Control Word (stored in err_state_ctrl instead). */
#define NFC_DEP_STATE_ROK       (0)      /* Request is OK, no error recovery required.   */    
#define NFC_DEP_STATE_NAS       (1)      /* Start Not Acknowledge Request.               */   
#define NFC_DEP_STATE_NAE       (2)      /* End NA: check target response and continue.  */   
#define NFC_DEP_STATE_ATS       (3)      /* Start Attention Request.                     */   
#define NFC_DEP_STATE_ATE       (4)      /* End Attention Request: check target response.*/


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Reset Protocol:
*/
/*!
* \ingroup nfci
* \param[in,out] *rst_param     Pointer to the I/O parameter structure
* 
* This function sets the protocol to initial state. The Initialise() function internally
* calls this method. Used for protocol-reinitialisation.
*/
void phcsBflNfc_InitiatorResetProt(phcsBflNfc_InitiatorResetParam_t *rst_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Set TRxBuffer:
*/
/*!
* \ingroup nfci
* \param[in,out] *set_trx_properties_param      Pointer to the I/O parameter structure
*
* The TRx buffer is internally used by the protocol to send and receive frames. This function
* is intended for use after ATR/PSL when new settings shall be applied and the caller wants
* to have full control over the memory available.
* The NFC protocol can insert a preamble before the composed PDU. Select the type of preamble
* with this function.
*/
void phcsBflNfc_InitiatorSetTRxProp(phcsBflNfc_InitiatorSetTRxPropParam_t *set_trx_properties_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Requested Tox Value:
*/
/*!
* \ingroup nfci
* \param[in,out] *req_tox_param     Pointer to the I/O parameter structure
*
* \return Requested TOX value of the Target.
*
*  When the NfcDataExchangeProtocolRequest exits with RCLSTATUS_TARGET_SET_TOX or
*  RCLSTATUS_TARGET_RESET_TOX (defined in rclstatus.h) it is a temporary exit, indicating that the
*  Target has requested extended processing time. This function enables the application to query
*  the requested value.
*/
uint8_t phcsBflNfc_InitiatorToxRequestValue(phcsBflNfc_InitiatorToxReqParam_t *req_tox_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Atribute Request:
*/
/*!
* \ingroup nfci
* \param[in,out] *atr_req_param     Struct with NFC communication parameters.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
*  This function issues an ATR_REQ and processed the Target's ATR_RES. The function is the entry
*  point of the NFC protocol.
*  
*/
phcsBfl_Status_t  phcsBflNfc_InitiatorAtrRequest(phcsBflNfc_InitiatorAtrReqParam_t *atr_req_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Parameter Select Request:
*/
/*!
* \ingroup nfci
* \param[in,out] *psl_req_param     Pointer to the I/O parameter structure.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
*  Immediately after ATR_RE*, the protocol allows to change the operating parameters (bit rates and
*  frame sizes). This operation is done using this command.
*/
phcsBfl_Status_t  phcsBflNfc_InitiatorPslRequest(phcsBflNfc_InitiatorPslReqParam_t *psl_req_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Data Exchange Protocol Request:
*/
/*!
* \ingroup nfci
* \param[in,out] *dep_req_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  RCLSTATUS_TARGET_SET_TOX         Target sent TOX, timeout shall be changed.
* \retval  RCLSTATUS_TARGET_RESET_TOX       Taarget has sent response, timeout shall be reset.
* \retval  PH_ERR_BFL_USERBUFFER_FULL        The user buffer is full and should be emptied by the caller.
* \retval  PH_ERR_BFL_BUF_2_SMALL       Communication buffer size is insufficient.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
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
*/
phcsBfl_Status_t  phcsBflNfc_InitiatorDepRequest(phcsBflNfc_InitiatorDepReqParam_t *dep_req_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Attention Request:
*/
/*!
* \ingroup nfci
* \param[in,out] *att_req_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* Under the hood, this function is a DEP_REQ. The Target confirms its presence with a
* DEP(Attention)_RES. Moreover, the function is used for error handling.
*/
phcsBfl_Status_t  phcsBflNfc_InitiatorAttRequest(phcsBflNfc_InitiatorAttReqParam_t *att_req_param);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Deselect Request:
*/
/*!
* \ingroup nfci
* \param[in,out] *dsl_req_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* The Deselect function moves the Target out of the protocol, before the SDD loop in case of
* passive communication mode. Targets in Active communication mode enter a special Deselect state. 
* In order to exit this state a WUP_REQ is issued which makes it possible to apply a new DID.\n
* All these steps allow a subsequent PSL_REQ, after SDD loop and ATR_REQ for passive or directly
* after WUP for active Target operation.
*/
phcsBfl_Status_t  phcsBflNfc_InitiatorDslRequest(phcsBflNfc_InitiatorDslReqParam_t *dsl_req_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Wakeup Request:
*/
/*!
* \ingroup nfci
* \param[in,out] *wup_req_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* This function is only available in Active communication mode and after DESELECT.
* It allows to apply a new DID and to directly issue a further PSL_REQ.
*/
phcsBfl_Status_t  phcsBflNfc_InitiatorWupRequest(phcsBflNfc_InitiatorWupReqParam_t *wup_req_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Release Request:
*/
/*!
* \ingroup nfci
* \param[in,out] *rls_req_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* The RLS_REQ function exits the protocol completely (state before selecting the communication mode,
* RFCA, SDD).
*/
phcsBfl_Status_t  phcsBflNfc_InitiatorRlsRequest(phcsBflNfc_InitiatorRlsReqParam_t *rls_req_param);



/* ////////////////////////////////////////////////////////////////////////////////////////////// */
/* "Private" functions and their prototypes, visible from inside this file only: */
/*//////////////////////////////////////////////////////////////////////////////////////////////////

// NFC phcsBflNfc_InitiatorCheckOptDidNad: 
// Check DID/NAD match in case that DID/NAD has been sent.
// IN:      * Pointer to the trx-buffer.
//          * Number of bytes sent.
//          * Min. length of valid response without optional params.
//          * Value of DID sent by initiator.
//          * Value of NAD sent by initiator.
//
// RETURN:  * phcsBfl_Status_t code.
*/
static phcsBfl_Status_t phcsBflNfc_InitiatorCheckOptDidNad(uint8_t *trx_buffer, 
                                                       uint8_t  tx_size, 
                                                       uint8_t  min_res_length,
                                                       uint8_t  did_i,
                                                       uint8_t *nad_i);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC phcsBflNfc_InitiatorCheckDid: 
// IN:      * Length of valid response.
//          * Number of bytes received.
//          * Value of DID sent by initiator.
//          * Value of DID received from target.
//
// RETURN:  * phcsBfl_Status_t code.
*/
static phcsBfl_Status_t phcsBflNfc_InitiatorCheckDid(uint8_t res_length,
                                                 uint8_t rx_size, 
                                                 uint8_t did_i,
                                                 uint8_t did_t);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC CheckResponsePdu: Check kind of target response pdu.
// IN:      * Value of PFB byte.
//
// RETURN:  * Kind of pdu (ACK, NACK, ATT, TOX, ...).
*/
static uint8_t phcsBflNfc_InitiatorCheckPfb(uint8_t pfb);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC phcsBflNfc_InitiatorCheckPni: 
// IN:      * PFB, or BlNr which contains value of PNI sent by initiator.
//          * PFB, which contains Value of PNI received from target.
//
// RETURN:  phcsBfl_Status_t.
*/
static phcsBfl_Status_t phcsBflNfc_InitiatorCheckPni(uint8_t pfb_i, uint8_t pfb_t);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC phcsBflNfc_InitiatorWriteDepHeader: 
// IN:      * Pointer to the Communication Parameters structure.
//          * Kind of DEP_REQ (Information, ACK/NACK or Supervisory pdu)
//          * Node address of Initiator and Target according to spec.
//          * Pointer to the used buffer index.
//
// COMMENT: MI-Bit in the Control Byte must be already set to the according value!
//          Length is not written to the buffer, because the final length of the
//          buffer is not determinable at this point!
*/
void phcsBflNfc_InitiatorWriteDepHeader(phcsBflNfc_InitiatorCommParams_t *td, 
                                      uint8_t                         req_type,
                                      uint8_t                        *nad,
                                      uint8_t                        *index);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC phcsBflNfc_InitiatorWriteInputBuffer: 
// IN:      * Pointer to the Communication Parameters structure.
//          * Pointer to input buffer.
//          * Length of the input buffer.
//          * Length of header.
//
// RETURN:  Total number of bytes written to the buffer (header + data bytes).
*/
uint8_t phcsBflNfc_InitiatorWriteInputBuffer(phcsBflNfc_InitiatorCommParams_t *td, 
                                           uint8_t                        *input_buffer,
                                           uint32_t                        input_buffer_size,
                                           uint8_t                         index);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC phcsBflNfc_InitiatorCheckDepDid: 
// IN:      * Pointer to received buffer.
//          * Used did value.
//
// RETURN:  Result of did check.
//
// COMMENT: Particular function for DEP-Request to check the validity of 
//          the received buffer concerning the used DID.
*/
phcsBfl_Status_t phcsBflNfc_InitiatorCheckDepDid(uint8_t *trx_buffer, uint8_t  did_i);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC phcsBflNfc_InitiatorCheckDepNad: 
// IN:      * Pointer to the Communication Parameters structure.
//          * Pointer to received buffer.
//          * Used nad value.
//
// RETURN:  Result of nad check.
//
// COMMENT: Particular function for DEP-Request to check the validity of 
//          the received buffer concerning the used NAD.
*/
phcsBfl_Status_t phcsBflNfc_InitiatorCheckDepNad(phcsBflNfc_InitiatorCommParams_t *td,
                                             uint8_t                        *trx_buffer,
                                             uint8_t                        *nad);

/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Initiator Transceive Auxiliary:
// IN:      * Pointer to the current instance.
//          * Number of bytes to transmit.
//          * Pointer to variable receiving the number of incoming bytes.
//
// RETURN:  Result of underlying Transceive operation or internal processing results.
//
// COMMENT: None.
//
*/
static phcsBfl_Status_t phcsBflNfc_InitiatorTRxAux(phcsBflNfc_Initiator_t *initiator_object,
                                               uint16_t             tx_buffer_size,
                                               uint16_t            *rx_buffer_size);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Deselect / Release Combi Request:
// IN:      * Pointer to the current instance.
//          * Type of request to perform (NFC_DSL_REQ or NFC_RLS_REQ).
//          * Expected return request code (NFC_DLS_RES or NFC_RLS_RES).
//          * Minimum length of the response to expect.
//
// RETURN:  Result of underlying operations or internal processing results.
//
// COMMENT: This function ties together DSL and RLS to save resources.
//
*/
phcsBfl_Status_t phcsBflNfc_InitiatorDslRlsCombiRequest(phcsBflNfc_Initiator_t *current_object,
                                                    uint8_t              req_type,
                                                    uint8_t              exp_res_type,
                                                    uint8_t              min_rec_len);




#endif /* PHCSBFLNFC_INITIATORINT_H */

/* E.O.F. */
