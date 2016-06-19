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
 * \file phcsBflI3P4_Internal.h
 *
 * Project: ISO14443-4 basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflI3P4_Internal.h
 * $Author: bs $
 * $Revision: 1.3 $
 * $Date: Tue Jul  4 16:43:42 2006 $
 * $KeysEnd$
 *
 * Comment:
 * None.
 *
 * History:
 *  MHa: Generated September 2005
 *
 */


#ifndef PHCSBFLI3P4_INT_H
#define PHCSBFLI3P4_INT_H

/* Bit inverted fields from external visible definitions */
#define PHCS_BFLI3P4_CONT_CHAINING_I                  (uint8_t)(0xFE)         /* */

/* Internal flags */
#define PHCS_BFLI3P4_FLAG_RESET_VALUE                 (uint8_t)(0x00)         /* */
#define PHCS_BFLI3P4_FLAG_CID_SUP_MASK                (uint8_t)(PHCS_BFLI3P4_COMMON_CID_SUP_MASK)  /* */
#define PHCS_BFLI3P4_FLAG_NAD_SUP_MASK                (uint8_t)(PHCS_BFLI3P4_COMMON_NAD_SUP_MASK)  /* */
#define PHCS_BFLI3P4_FLAG_INCLUDE_NAD                 (uint8_t)(0x04)         /* */
#define PHCS_BFLI3P4_FLAG_INCLUDE_NAD_I               (uint8_t)(0xFB)         /* */
#define PHCS_BFLI3P4_FLAG_PICC_CHAINING               (uint8_t)(0x08)         /* */
#define PHCS_BFLI3P4_FLAG_PICC_CHAINING_I             (uint8_t)(0xF7)         /* */
#define PHCS_BFLI3P4_FLAG_TIMEOUT                     (uint8_t)(0x10)         /* */
#define PHCS_BFLI3P4_FLAG_TIMEOUT_I                   (uint8_t)(0xEF)         /* */
#define PHCS_BFLI3P4_FLAG_PREVIOUS_MORE               (uint8_t)(0x20)         /* */
#define PHCS_BFLI3P4_FLAG_PREVIOUS_MORE_I             (uint8_t)(0xDF)         /* */
#define PHCS_BFLI3P4_FLAG_PICC_APDU_CHAIN             (uint8_t)(0x40)         /* */
#define PHCS_BFLI3P4_FLAG_PICC_APDU_CHAIN_I           (uint8_t)(0xBF)         /* */
#define PHCS_BFLI3P4_FLAG_INCLUDE_NAD_BEFORE          (uint8_t)(0x80)         /* */
#define PHCS_BFLI3P4_FLAG_INCLUDE_NAD_BEFORE_I        (uint8_t)(0x7F)         /* */
#define PHCS_BFLI3P4_FLAG_NO_HISTORY                  (uint8_t)(0x01)         /* */
#define PHCS_BFLI3P4_FLAG_NO_HISTORY_I                (uint8_t)(0xFE)         /* */

/* States */
#define PHCS_BFLI3P4_STATE_CURRENT                    (uint8_t)(0x01)         /* */
#define PHCS_BFLI3P4_STATE_PREVIOUS                   (uint8_t)(0x00)         /* */
#define PHCS_BFLI3P4_STATE_CUR_MASK                   (uint8_t)(0x0F)         /* */
#define PHCS_BFLI3P4_STATE_PREV_MASK                  (uint8_t)(0xF0)         /* */
#define PHCS_BFLI3P4_STATE_RESET_VALUE                (uint8_t)(0x00)         /* = RS ! */
#define PHCS_BFLI3P4_STATE_RS                         (uint8_t)(0x00)         /* */
#define PHCS_BFLI3P4_STATE_EA                         (uint8_t)(0x01)         /* */
#define PHCS_BFLI3P4_STATE_EI                         (uint8_t)(0x02)         /* */
#define PHCS_BFLI3P4_STATE_SA                         (uint8_t)(0x03)         /* */
#define PHCS_BFLI3P4_STATE_ST                         (uint8_t)(0x04)         /* */
#define PHCS_BFLI3P4_STATE_RE                         (uint8_t)(0x05)         /* */
#define PHCS_BFLI3P4_STATE_RSR                        (uint8_t)(0x0A)         /* */
#define PHCS_BFLI3P4_STATE_EACK                       (uint8_t)(0x0B)         /* */
#define PHCS_BFLI3P4_STATE_ENAK                       (uint8_t)(0x0C)         /* */


/* Others */
#define PHCS_BFLI3P4_INIT_RD_BLNR                     (uint8_t)(0x00)         /* */




#endif /* PHCSBFLI3P4_INT_H */

/* E.O.F. */

