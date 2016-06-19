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
 * \file phcsBflMfRd_Int.h
 *
 * Project: Object Oriented Library Framework Mifare Component.
 *
 *  Source: phcsBflMfRd_Int.h
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Wed Jun  7 09:47:46 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated 29. April 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLMFRD_INT_H
#define PHCSBFLMFRD_INT_H



/*! 
 *  \name Control Behaviour
 *  \brief  Control behaviour of the protocol upon reply: \n
 *   OR-ing of values allows the protocol to be prepared for multiple reply types to create a
 *   positive result (e.g. some cards return an ACK, some nothing upon certain commands).
 */
/*@{*/
/*!  \ingroup mfrd */
#define PHCS_BFLMFRD_EXPECT_TIMEOUT       ((uint8_t)0x01)   /*!< \brief Tells the library to expect a timeout. */
#define PHCS_BFLMFRD_EXPECT_ACK           ((uint8_t)0x02)   /*!< \brief Let the library expect an Acknowledge response. */
#define PHCS_BFLMFRD_EXPECT_DATA          ((uint8_t)0x04)   /*!< \brief The library shall expect data. */
/*!
 * (Not-) Acknowledge:
 * When a MF PICC returns ACK, a logical AND with the mask yields a value other than 0. Otherwise,
 * in case of NACK, the mask zeroes the byte (or nibble respectively).
 */
#define PHCS_BFLMFRD_ACK_MASK             ((uint8_t)0x0A)           /* */
/*! @} */


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Transcations according to MF spec:
*/
/*!
* \ingroup mfrd
* \param[in,out] *cmd_param     Struct with communication parameters 
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_COLLISION_ERROR        Collision detected during receive procedure.
* \retval  PH_ERR_BFL_PROTOCOL_ERROR         Protocol error as defined in the hardware register settings.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
*  The main MIFARE reader protocol entry point. All MIFARE functionality is concentrated in this
*  place. According to the cmd parameter all possible Mifare (R) commands are handled. \n
*  For a detailed description of all commands have a look at any Mifare documantation.
*/
phcsBfl_Status_t phcsBflMfRd_Transaction(phcsBflMfRd_CommandParam_t *cmd_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Generic single step transaction for:
*/
/*!
* \ingroup mfrd
* \param[in] *cmd_param     Pointer to the struct with communication parameters.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_COLLISION_ERROR        Collision detected during receive procedure.
* \retval  PH_ERR_BFL_PROTOCOL_ERROR         Protocol error as defined in the hardware register settings.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
*  This internal function performs a generic MIFARE communication TX-RX cycle. This function is called by the
*  phcsBflMfRd_Transaction method for I/O assistance.
*/
phcsBfl_Status_t phcsBflMfRd_GenericSingle(phcsBflMfRd_CommandParam_t *cmd_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Generic double step transaction for:
*/
/*!
* \ingroup mfrd
* \param[in,out] *cmd_param     Struct with communication parameters 
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_COLLISION_ERROR        Collision detected during receive procedure.
* \retval  PH_ERR_BFL_PROTOCOL_ERROR         Protocol error as defined in the hardware register settings.
* \retval  PH_ERR_BFL_INVALID_DEVICE_STATE   Internal state machine in different state.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
*  This internal function performs a generic MIFARE communication TX-RX-TX-RX cycle. This function is
*  called by the phcsBflMfRd_Transaction method for I/O assistance.
* 
*/
phcsBfl_Status_t phcsBflMfRd_GenericDouble(phcsBflMfRd_CommandParam_t *cmd_param);



#endif  /* PHCSBFLMFRD_INT_H */

/* E.O.F. */
