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
 * \file phcsBflI3P3A_Hw1Int.h
 *
 * Project: Object Oriented Library Framework phcsBfl_I3P3A component for PN51x.
 *
 *  Source: phcsBflI3P3A_Hw1Int.h
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 30 11:01:48 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  MHa: Generated September 2005
 *
 */


#ifndef PHCSBFLI3P3A_HW1INT_H
#define PHCSBFLI3P3A_HW1INT_H

#include <phcsBflI3P3A.h>

/* Anticollision/Select codes */
#define SELECT_CASCADE_LEVEL_1  0x93 /*< \ingroup iso14443_3 
                                         \brief Anticollision/Select code for cascade level 1. */


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// phcsBfl_I3P3A Request for PN51x:
*/
/*!
* \ingroup iso14443_3
* \param[in,out] *request_a_param   Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_COLLISION_ERROR        Collision detected during receive procedure.
* \retval  PH_ERR_BFL_PROTOCOL_ERROR         Protocol error as defined in the hardware register settings.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This command handles the Request procedure of ISO14443-3. Depending on the Request Code (ISO14443_3_REQALL or 
* ISO14443_3_REQIDL) and the state of the cards in the field all cards reply with their Tag-Type synchronously. 
* The time between end of the Request command and start of reply of the card is exactly 8 * 9.44 us long. The 
* Tag-Type field is 16 bits long and only one bit out of 16 is set. \n
* Double and Triple serial numbers are possible. \n
*
* Relevant bit positions LSByte:  \n
*	 [8..7] UID size   \n
*	 \n 00 standard 32 bit long UID \n
*	 \n 01 UID size double (56 bit long) \n
*	 \n 10 UID size triple (80 bit long) \n 
*	 [5..1] if any bit is set, frame anticollision is supported;  tag type recognition \n
* The complete MSByte is RFU. \n
*
* \note In case of an error, the appropriate error code is set. Nevertheless all received data 
*       during the RF-Communication is returned for debugging reasons.
* \note Future cards will work also with other request codes.
*
*/
phcsBfl_Status_t phcsBflI3P3A_Hw1RequestA(phcsBflI3P3A_ReqAParam_t *request_a_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// phcsBfl_I3P3A AnticollSelect for PN51x:
*/
/*!
* \ingroup iso14443_3
* \param[in,out] *AnticollSelect_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
* \retval  PH_ERR_BFL_BITCOUNT_ERROR         Bits received do not match the expected one.
* \retval  PH_ERR_BFL_WRONG_UID_CHECKBYTE    Checkbyte of UID is wrong.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                             phcsBfl_Status_t values depending on the underlying components.
* 
* This command combines the ISO14443-3 functions of anticollision and select. The functionality is split up into two 
* independent internal procedures. One to perform the anticollision, the other one to perform the select.
* The cascade level is automatically increased if the Cascade Tag for a further level is received.
* The checkbyte is verified, but not stored in the buffer. \n
* The sel_lv1_code contains the select code of cascade level 1. This is 0x93 for ISO14443-3 compatible devices. The 
* select codes for cascade level 2 and 3 are chosen automatically according to the information in the SAK byte received. \n
* The uid references a buffer which may contain the known part of the uid when the function is called.
* When the function returns to the calling procedure, this buffer contains the received serial number of 
* the target device. The length is according to the cascade level of the ID. The indicator of another 
* cascade level (0x88) is not removed and also stored in the buffer. \n
* The uid_length parameter is the bit count (!) of the ID. As input it is the number of known bits,
* as output it shows the ID length, in bits, which is always a multiple of 32. \n
* At the end this function selects a card by the specified serial number. All other cards in the field fall back 
* into the idle mode and they are not longer involved during the further communication. 
*
* \note In case of an error, the appropriate error code is set. Nevertheless all received data 
*       during the RF-Communication is returned for debugging reasons.
*
*/
phcsBfl_Status_t phcsBflI3P3A_Hw1AnticollSelect(phcsBflI3P3A_AnticollSelectParam_t *AnticollSelect_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// phcsBfl_I3P3A SelectA for PN51x:
*/
/*!
* \ingroup iso14443_3
* \param[in,out] *select_param      Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
* \retval  PH_ERR_BFL_BITCOUNT_ERROR         Bits received do not match the expected one.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This command only performs the select of a known device. For detailed information on the 
* parameters see the decription of \ref phcsBflI3P3A_Hw1AnticollSelect command. \n
* The sel_lv1_code is the first command to be sent to the passive device.  \n
* The uid contains the ID of the device. All 3 cascade levels can be transferred at once to the 
* function. The uid_length parameter defines whether the ID is single, double or triple. \n
* At the end this function selects a card by the specified serial number. All other cards in the field fall back 
* into the idle mode and they are not longer involved during the further communication. 
*
* \note In case of an error, the appropriate error code is set. Nevertheless all received data 
*       during the RF-Communication is returned. This is done for debugging reasons.
*
*/
phcsBfl_Status_t phcsBflI3P3A_Hw1Select(phcsBflI3P3A_SelectParam_t *select_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// phcsBfl_I3P3A HaltA for PN51x:
*/
/*!
* \ingroup iso14443_3
* \param[in,out] *halt_a_param      Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This function handles the halt command for ISO14443-3 in both the reader and the card mode. \n
* In reader operating mode, this function sets a MIFARE (R) Classic compatible card into the halt state. 
* Having send the command to the card, the function does not expect a cards response. Only in case of any 
* error the card sends back a NACK. If the command was successful, the card does not return with an ACK. 
* Thus, the function is successful, if a timeout is indicated. This is handled internally.\n
* In card operating mode this functions evaluates the data received, sets the Halt flag of the PN51x and
* starts again the automatic anticolission procedure.
*/
phcsBfl_Status_t phcsBflI3P3A_Hw1HaltA(phcsBflI3P3A_HaltAParam_t *halt_a_param);


/*
 * \ingroup iso14443_3
 * \param[in,out] *set_wec_param   Pointer to the I/O parameter structure
 *
 * This function initializes the user defined callback (interrupt handler) which might be used
 * in the functions of this component.
 */
void phcsBflI3P3A_Hw1SetWaitEventCb(phcsBflI3P3A_SetWecParam_t *set_wec_param);



/*! \name Internal Functions
    \ingroup iso14443_3
    Functions to split anticollision and select internally for easier handling. */
/*@{*/
phcsBfl_Status_t phcsBflI3P3A_Hw1CascAnticoll(phcsBflI3P3A_t  *cif, 
                                          phcsBflRegCtl_t *p_lower,
                                          uint8_t        sel_code,
                                          uint8_t        bitcount,
                                          uint8_t       *snr,
					                      uint8_t       *flags);

phcsBfl_Status_t phcsBflI3P3A_Hw1IntSelect(phcsBflI3P3A_t     *cif,
                                       phcsBflRegCtl_t    *p_lower,
                                       uint8_t           sel_code,
                                       uint8_t          *snr,
                                       uint8_t          *sak);
/*@}*/


#endif /* PHCSBFLI3P3A_HW1INT_H */

/* E.O.F. */
