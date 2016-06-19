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
 * \file phcsBflPolAct_Hw1Int.h
 *
 * Project: Object Oriented Library Passive Polling Activation Component for PN51x.
 *
 *  Source: phcsBflPolAct_Hw1Int.h
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Fri Jun 30 11:01:53 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  MHa: Generated September 2005
 *
 */


#ifndef PHCSBFLPOLACT_HW1INT_H
#define PHCSBFLPOLACT_HW1INT_H

#include <phcsBflPolAct.h>

/* Definitions for Felica Polling parameters: */
#define POLACT_MIN_DATA_LEN          0x04 /*!< \ingroup felpol Polling response min. length is 16 bytes */
#define POLACT_MAX_DATA_LEN          0x05 /*!< \ingroup felpol Polling response max. length is 20 bytes */



/* //////////////////////////////////////////////////////////////////////////////////////////////////
// phcsBflPolAct_Wrapper Polling for PN51x:
*/
/*!
* \ingroup felpol
* \param[in,out] *polling_param     Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_PROTOCOL_ERROR         Protocol error as defined in the hardware register settings.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
* 
* This function handles the felica polling procedure which is the same as the activation procedure for passive 
* communication mode at 212 and 424 kbps as specified in ISO18092. \n First RxMultiple functionality of PN51x is 
* activated. Then the data is prepared as described in the specification and sent using phcsBflAux_Hw1Command.\n
* The response data is analysed. If anythig was correct, the complete data steam is handed over to the 
* calling procedure (even length and command). So the response data stream looks like LEN, CMD, Data. Only the CRC
* information is skipped. \n
* If there was an error, the errorneous frame is not handed over to the caller but the error status is 
* returned to the calling procedure. If there is more than one valid response all correct data frames are handed
* over to the caller. 
* 
* \note Timeout handling has to be done by the caller!! The timer shall always be set in the way that all possible 
*       responses may be done during the specified time. In any other case only the first response might be read.
*
*/
phcsBfl_Status_t phcsBflPolAct_Hw1Polling(phcsBflPolAct_PollingParam_t *polling_param);


/*!
 * \ingroup felpol
 * \param[in]  set_wec_param    Pointer to the phcsBflIo_SetWecParam_t structure containing data
 *                              to initialise the underlying callback function for event detection.
 *
 * This function initializes the user defined callback (interrupt handler) which might be used
 * in the functions of this component.
 */
void phcsBflPolAct_Hw1SetWaitEventCb(phcsBflPolAct_SetWecParam_t *set_wec_param);


#endif /* PHCSBFLPOLACT_HW1INT_H */

/* E.O.F. */
