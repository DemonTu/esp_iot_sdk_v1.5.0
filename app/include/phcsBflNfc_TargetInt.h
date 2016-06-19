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
 * \file phcsBflNfc_TargetInternal.h
 *
 * Project: NFC Target basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflNfc_TargetInternal.h
 * $Author: mha $
 * $Revision: 1.2 $
 * $Date: Wed Jun  7 09:47:38 2006 $
 * $KeysEnd$
 *
 * Comment:
 * None
 *
 * History:
 *  MHa: Generated September 2005
 *
 */


#ifndef PHCSBFLNFC_TARGETINT_H
#define PHCSBFLNFC_TARGETINT_H

#include <phcsBflNfc.h>

/*
 *  REMARK:
 *      The buffers used for PDU composition and data exchange widely overlap. This is because of 
 *      preamble composition which shall be entirely unrecognised by the protocol flow.
 */

/* 
 * \name Defines for Target Protocol State-Machine
 * \ingroup nfct
 *
 *  @{
 */
#define NFC_TARGET_INITIAL              (0x01)  /*!< Only ATR_REQ is allowed at the INITIAL state.  */
#define NFC_TARGET_IN_PROTOCOL          (0x3D)  /*!< ATR_REQ, PSL_REQ, DEP_REQ, DSL_REQ and RLS_REQ 
                                                     are allowed at state IN_PROTOCOL.              */
#define NFC_TARGET_DESELECTED_A         (0x02)  /*!< Only WUP_REQ is allowed at state DESELECTED A. */
/*! @} */

/*! 
 *  \name Defines for Control-Word
 *  \ingroup nfct
 *
 *  @{
 */
#define NFC_TARGET_CTRL_0_1_MASK               (0x0003)  /*!< Mask for bit0 - bit1                                */
#define NFC_TARGET_CTRL_DEP_AFTER_TOX          (0x0004)  /*!< Bit as indicator for the entry into DEP after TOX.  */
#define NFC_TARGET_CTRL_1ST_REQ_AFT_ATR        (0x0008)  /*!< Bit as indicator for the first req. after ATR_REQ   */                   
#define NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I      (0xFFF7)  /*!< Bit inversion of NFC_TARGET_CTRL_1ST_REQ_AFT_ATR_I. */                   
#define NFC_TARGET_CTRL_PASSIVE_MODE           (0x0010)  /*!< If bit is set, passive mode is used.                */ 
#define NFC_TARGET_CTRL_PSLCB_REQ_TWICE        (0x0020)  /*!< Set during PSL handling, invoke CB a second time.   */
#define NFC_TARGET_CTRL_RCV_CHAIN_FIN          (0x0040)  /*!< Set if receive-chaining has finished.               */
#define NFC_TARGET_CTRL_NAD_PRESENT            (0x0080)  /*!< This bit is set if NAD was already sent
                                                              with the first block of data exchange.                */
#define NFC_TARGET_CTRL_NAD_PRESENT_I          (0xFF7F)  /*!< Bit inversion of NFC_TARGET_CTRL_NAD_PRESENT. */
#define NFC_TARGET_CTRL_MI_PRESENT             (0x0100)  /*!< Set if MI is present. Required if chaining
                                                              interrupted by TOX.                                 */
#define NFC_TARGET_CTRL_MI_PRESENT_I           (0xFEFF)  /*!< Bit inverted version of NFC_TARGET_CTRL_MI_PRESENT_I. */
#define NFC_TARGET_CTLR_NAD_PREV_USED          (0x0200)  /*!< NAD was used at previously sent block.              */
#define NFC_TARGET_CTLR_NAD_PREV_USED_I        (0xFDFF)  /*!< Bit inversion of NFC_TARGET_CTLR_NAD_PREV_USED.     */
#define NFC_TARGET_CTRL_TOX_ACTIVE             (0x0400)  /*!< This bit is set between TOX-Request (Target) and 
                                                              TOX-Response of the Initiator.                      */
#define NFC_TARGET_CTRL_BUF_RCV_OVF            (0x0800)  /*!< In automatic chaining mode, a Target Buffer is
                                                              specified and serves as the destination of received
                                                              data. In case of overflow, this flag is set. After
                                                              the endpoint call the bit is cleared again.         */
#define NFC_TARGET_CTRL_IN_OVF                 (0x1000)  /*!< This bit is set when the first overflow is detected.
                                                              It serves as an indicator for the protocol to 
                                                              idetify the first and subsequent overflows. This bit
                                                              remains set until the end of the transaction.       */
#define NFC_TARGET_CTRL_ALLOW_ATR              (0x2000)  /*<  Bit to indicate whether ATR_REQ is allowed or not,
                                                              according to the NFC specification. */
#define NFC_TARGET_CTRL_ALLOW_ATR_I            (0xDFFF)  /*<  Bit inversion of NFC_TARGET_CTRL_ALLOW_ATR.       */
#define NFC_TARGET_CTRL_DSLCB_REQ2             (0x4000)  /*!< Set during DSL handling, invoke CB a second time.   */
#define NFC_TARGET_CTRL_RLSCB_REQ2             (0x8000)  /*!< Set during RLS handling, invoke CB a second time.   */
/*! @} */


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Dispatch: Main entry point, contains state machine to handle request/response sequences.
*/
/*!
* \ingroup nfct
* \param[in] *dispatch_param        C object interface structure.
*                                   The (valid) pointer to  the pre-allocated structure must be
*                                   provided by the caller.
*                                   Please notice that any request must be propagated to the
*                                   Dispatch function. This is important because in the state after
*                                   ATR_REQ/ATR_RES any other incoming packet locks, according to the
*                                   specification, the PSL_REQ/PSL_RES functionality. In case of
*                                   corrupted packets (e.g. CRC error) the packet length in the
*                                   parameter must be set to zero.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_UNSUPPORTED_COMMAND    The device does not support the command chosen.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  PH_ERR_BFL_BUF_2_SMALL       Communication buffer size is insufficient.
* \retval  Other_EP                         phcsBfl_Status_t values depending on the Target endpoint implementation.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* Main entry point, contains state machine to handle request/response sequences.
* \note phcsBfl_Status_t code. If the return value indicates unsuccessful completion the embedding software (which is 
*       essentially the receive loop) must get back into receiving mode. No transmission has been triggered by 
*       the protocol in this case and no transmission must be performed by the embedding software.
*/
phcsBfl_Status_t phcsBflNfc_TargetDispatch(phcsBflNfc_TargetDispatchParam_t* dispatch_param);




/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Reset Target Protocol: Resets the protocol to initial state before ATR_REQ.
*/
/*!
* \ingroup nfct
* \param[in] *reset_param       Pointer to the I/O parameter structure
*
* This function resets the protocol to initial state before ATR_REQ. This function can also be called 
* after a RLS_RE* procedure to clear all internal states for the next activation.
*/
void phcsBflNfc_TargetResetProt(phcsBflNfc_TargetResetParam_t *reset_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Set or modify TargetBuffer for internal chaining:
*/
/*!
* \ingroup nfct
* \param[in] *user_buffer_param     Pointer to the I/O parameter structure
*
* This function sets or modifys the TargetBuffer for internal chaining. 
* \note The buffer specified with this function is a large user buffer for data to send
        or receive respectively. Chaining is done automatically, internally, by the 
        protocol, if such a buffer is specified.
*/
void phcsBflNfc_TargetSetUserBuf(phcsBflNfc_TargetSetUserBufParam_t *user_buffer_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Sets a user pointer which might used for several interactions between main function and Target
// callback implementation.:
*/
/*!
* \ingroup nfct
* \param[in] *p_user_param      Pointer to the I/O parameter structure
*
* Sets a user pointer which might used for several interactions between main function and Target
* callback implementation. This is an general purpose pointer which may carry different data.
* Both, the main function and the callback have to know about the contents!
*/
void phcsBflNfc_TargetSetPUser(phcsBflNfc_TargetSetPUserParam_t *p_user_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// NFC Set TX/RX buffer properties: Apply new TX/RX buffer settings.
*/
/*! 
* \ingroup nfct
* \param[in] *trx_buffer_param      Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
*
* This function applys new TX/RX buffer settings.
*/
phcsBfl_Status_t phcsBflNfc_TargetSetTRxBufProp(phcsBflNfc_TargetSetTRxBufPropParam_t *trx_buffer_param);
                

#endif /* PHCSBFLNFC_TARGETINT_H */

/* E.O.F. */
