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
 * \file phcsBflI3P4_PcdInt.h
 *
 * Project: ISO14443-4 basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflI3P4_PcdInt.h
 * $Author: bs $
 * $Revision: 1.2 $
 * $Date: Fri Apr 28 09:19:33 2006 $
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


#ifndef PHCSBFLI3P4_PCDINT_H
#define PHCSBFLI3P4_PCDINT_H

#include <phcsBflI3P4.h>

#define I3P4_BLOCK_TYPE_MASK        (0xC0)                              /* */
#define I3P4_CRC_LEN                (0x02)                              /* */
#define I3P4_CLEAR_INDEX_MASK       (0x00)                              /* */
#define I3P4_CLEAR_SIZE_MASK        (0x00)                              /* */

#define I3P4_PCB_POS                (0x00)                              /* */
#define I3P4_CID_POS                (0x01)                                    /* */
#define I3P4_CID_MASK               (0x0F)                                  /* */
#define I3P4_CID_TEMPLATE           (0x00)                                  /* */
#define I3P4_CID_CHECK_MASK         (0x30)                                  /* */
#define I3P4_NAD_MASK               (0x88)                                  /* */
#define I3P4_NAD_TEMPLATE           (0x00)                                  /* */
#define I3P4_IRS_CID_PRESENT_MASK   (0x08)                                  /* */
#define I3P4_CHAINING_BIT_MASK      (0x10)                                  /* */

#define I3P4_BLOCK_TYPE_R           (0x80)                                    /* */
#define I3P4_R_BLOCK_MAX_LEN        (0x02)                                    /* */
#define I3P4_R_BLOCK_MIN_LEN        (0x01)                                    /* */
#define I3P4_R_BLOCK_TEMPLATE       (0xA2)                                    /* */
#define I3P4_R_BLOCKNR_MASK         (0x01)                                    /* */
#define I3P4_R_BLOCK_TEM_PICC       (0xA0)                                    /* */
#define I3P4_RACK_PCB_CHKMASK       (0xF4)                                    /* */
#define I3P4_RACK_PCB_BLOCKNR       (0x01)                                    /* */
#define I3P4_R_BLOCK_ACK            (0x00)                                    /* */
#define I3P4_R_BLOCK_NAK            (0x10)                                    /* */

#define I3P4_BLOCK_TYPE_S           (0xC0)                                    /* */
#define I3P4_S_BLOCK_MAX_LEN        (0x03)                                    /* */
#define I3P4_S_BLOCK_MIN_LEN        (I3P4_R_BLOCK_MIN_LEN)              /* */
#define I3P4_S_BLOCK_TEMPLATE       (0xC2)                                    /* */
#define I3P4_S_BLOCK_TEMP_PICC      (0xC0)                                    /* */
#define I3P4_S_BLOCK_DESELECT       (0x00)                                    /* */
#define I3P4_S_BLOCK_WTX            (0x30)                                    /* */
#define I3P4_SWTX_PCB_CHKMASK       (0xF4)                                    /* */
#define I3P4_WTX_MASK               (0x3F)   

#define I3P4_BLOCK_TYPE_I           (0x00)                                    /* */
#define I3P4_I_BLOCK_MIN_LEN        (I3P4_R_BLOCK_MIN_LEN)              /* */
#define I3P4_I_BLOCK_TEMPLATE       (0x02)                                    /* */
#define I3P4_I_BLOCKNR_MASK         (0x01)                                    /* */
#define I3P4_I_NAD_PRESENT_MASK     (0x04)                                  /* */
#define I3P4_I_PCB_CHKMASK          (0xE2)                                    /* */
#define I3P4_I_PCB_BLOCKNR          (0x01)                                    /* */


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Set Callback Addresses:
*/
/*!
* \ingroup iso14443_4
* \param[in] *set_cb_param  Pointer to the I/O parameter structure
*
* The function initialises the callback pointers to embedding software-defined functionality responsible
* for the application of reader settings. The function pointers in the set_cb_param structure must 
* point to valid locations for the adequate operation or to NULL. 
*
*/
void phcsBflI3P4_SetCallbacks(phcsBflI3P4_SetCbParam_t *set_cb_param);

/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Set Protocol Parameters:
*/
/*!
* \ingroup iso14443_4
* \param[in,out] *set_protocol_param    Pointer to the I/O parameter structure
*
* This function applies the session parameters (determined during activation) to the communication
* protocol. The same struct as used during activation can be used also here.
*/
void phcsBflI3P4_SetProt(phcsBflI3P4_SetProtParam_t *set_protocol_param);
                               
/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Reset Protocol:
*/
/*!
* \ingroup iso14443_4
* \param[in,out] *reset_protocol_param  Pointer to a phcsBflI3P4_ResetProtParam_t structure
* 
* This function sets the protocol to initial state (reset). 
* It shall be used for protocol-reinitialisation only. This might be the case when ended up in an 
* unrecoverable communication error condition. Additionally this might be called at the end of a 
* communication to be prepared to start a new one. \n
* It is called implicitely during initialisation procedure (phcsBflI3P4_Init).
*
*/
void phcsBflI3P4_ResetProt(phcsBflI3P4_ResetProtParam_t *reset_protocol_param);

/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL SetPUser:
*/
/*!
* \ingroup iso14443_4
* \param[in,out] *set_p_user_param      Pointer to the I/O parameter structure
* 
* This function set the user pointer.
*
*/
void phcsBflI3P4_SetPUser(phcsBflI3P4_SetPUserParam_t *set_p_user_param);

/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Exchange:
*/
/*!
* \ingroup iso14443_4
* \param[in,out] *exchange_param    Pointer to the I/O parameter structure
* 
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
* \retval  PH_ERR_BFL_BLOCKNR_NOT_EQUAL      Blocknumber missmatch, but frame is in principle ok.
* \retval  PH_ERR_BFL_USERBUFFER_FULL        The user buffer is full and should be emptied by the caller.
* \retval  Other_CB                         phcsBfl_Status_t values depending on the callback implementation if used.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* Data exchange protocol according to ISO 14443-4. 
*
*/
phcsBfl_Status_t phcsBflI3P4_Exchange(phcsBflI3P4_ExchangeParam_t *exchange_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Presence Check Request:
*/
/*!
* \ingroup iso14443_4
*
* \param[in,out] *presence_check_param      Pointer to the I/O parameter structure
* 
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other_CB                         phcsBfl_Status_t values depending on the callback implementation if used.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* This function checks the presence of a PICC in the field without altering its state. This
* is done by exploiting the scenarios in response to a R(NACK) block. \n
*
*/
phcsBfl_Status_t phcsBflI3P4_PresCheck(phcsBflI3P4_PresCheckParam_t *presence_check_param);


/*//////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Deselect Request:
*/
/*!
* \ingroup iso14443_4
* \param[in,out] *deselect_param        Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  Other_CB                         phcsBfl_Status_t values depending on the callback implementation if used.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* Deselect functionality according to ISO 14443-4 specification.
*/
phcsBfl_Status_t phcsBflI3P4_Deselect(phcsBflI3P4_DeselectParam_t *deselect_param);




#endif /* PHCSBFLI3P4_PCDINT_H */

/* E.O.F. */
