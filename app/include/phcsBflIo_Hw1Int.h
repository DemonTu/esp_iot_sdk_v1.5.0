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
 * \file phcsBflIo_Hw1Int.h
 *
 * Project: Object Oriented Library Framework I/O Component implementation for PN51x.
 *
 *  Source: phcsBflIo_Hw1Int.h
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Fri Jun 30 11:01:53 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated September 2005
 *
 */


#ifndef PHCSBFLIO_HW1INT_H
#define PHCSBFLIO_HW1INT_H

#include <phcsBflIo.h>


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// I/O Transceive for PN51x:
*/
/*!
* \ingroup rcio
* \param[in,out] *transceive_param      Pointer to the parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This function directly uses the auxiliary function phcsBflAux_Hw1Command for initiator mode with 
* the PN51x command Transceive (b1100) in the command register. \n
* For target mode this function is disabled and replaced by Transmit and Receive 
* which also use the transceive command for bit syncronicity. \n
* The rx_buffer_size parameter sets the maximum receive data length in bytes. Even if more data is 
* received, no buffer overflow may occure. \n
* In combination with a transparent register read and write command set, this function enables a 
* completely transparent communication between two devices. The evalution of occured transmission 
* errors is done internally.
*
* \note TX and RX buffers may overlap or even be the same.
* \note In case of an error, the appropriate error code is set. Nevertheless all received data 
*       during the RF-Communication is returned. This is done for debugging reasons.
* \note Neither framing and speed nor timeout and CRC are modified by this function. These 
*       parameters should be set in advance.
*
*/
phcsBfl_Status_t phcsBflIo_Hw1Transceive(phcsBflIo_TransceiveParam_t *transceive_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// I/O Transmit for PN51x:
*/
/*!
* \ingroup rcio
* \param[in] *transmit_param    Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This function is devided into two parts. One for the initiator mode and one for the target mode. \n
* In initiator mode the function phcsBflAux_Hw1Command is called and the transmit command of PN51x
* is executed. No response is expected. \n
* In target mode there is first a check if the transceive command is active, because sending without any 
* previous reception is forbidden in any protocol used. Then the function phcsBflAux_Hw1Command is 
* called using the transceive command of PN51x. The transmit command for target operation is never used!
*
* \note Neither framing and speed nor timeout and CRC are modified by this function. These 
*       parameters should be set in advance.
* 
*/
phcsBfl_Status_t phcsBflIo_Hw1Transmit(phcsBflIo_TransmitParam_t *transmit_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// I/O Receive for PN51x:
*/
/*!
* \ingroup rcio
* \param[in,out] *receive_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This function is devided into two parts. One for the initiator mode and one for the target mode. \n
* In initiator mode the function phcsBflAux_Hw1Command is called and the receive command of PN51x
* is executed. If no timeout is set and nothing is received this function never returns! \n
* In target mode the function phcsBflAux_Hw1Command is called and the transceive command of PN51x
* is executed. This functionis also re-setup so that reception is possible in any case. \n
* In both modes the rx_buffer_size parameter sets the maximum receive data length in bytes. Even if 
* more data is received, no buffer overflow may occure. \n
* 
* \note In case of an error, the appropriate error code is set. Nevertheless all received data 
*       during the RF-Communication is returned. This is done for debugging reasons.
* \note Neither framing and speed nor timeout and CRC are modified by this function. These 
*       parameters should be set in advance.
*
*/
phcsBfl_Status_t phcsBflIo_Hw1Receive(phcsBflIo_ReceiveParam_t *receive_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// I/O Mifare Authentication for PN51x:
*/
/*!
* \ingroup rcio
* \param[in] *mfauthent_param   Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This function is directly called from Mifare Authentication function where the data is prepared 
* and handled over using the tx_buffer. The function calls phcsBflAux_Hw1Command where the
* Authentication command of PN51x is performed. If used separated from the Mifare (R) protocol
* the data structure as defined in the data sheet of the PN51x has to be used!
* 
* \note Neither framing and speed nor timeout and CRC are modified by this function. These 
*       parameters should be set in advance.
* 
*/
phcsBfl_Status_t phcsBflIo_Hw1MfAuthent(phcsBflIo_MfAuthentParam_t *mfauthent_param);

/*!
 * \ingroup rcio
 * \param[in] *set_wec_param    Pointer to the phcsBflIo_SetWecParam_t structure containing data
 *                              to initialise the underlying callback function for event detection.
 *
 * This function initializes the user defined callback (interrupt handler) which might be used
 * in the functions of this component.
 */
void phcsBflIo_Hw1SetWaitEventCb(phcsBflIo_SetWecParam_t *set_wec_param);


#endif /* PHCSBFLIO_HW1INT_H */

/* E.O.F. */
