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
 * \file phcsBflNfc_CommonInt.h
 *
 * Project: NFC common definitions for Initiator and Target.
 *
 * Workfile: phcsBflNfc_CommonInt.h
 * $Author: bs $
 * $Revision: 1.6 $
 * $Date: Fri Jun 30 09:51:11 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  MHa: Generated September 2005
 *
 */

#ifndef PHCSBFLNFC_COMMONINT_H
#define PHCSBFLNFC_COMMONINT_H

#include <phcsBflStatus.h>

/*! \name Generic NFC definitions
 *  \ingroup nfc */
/*@{*/

#define NFC_MAX_RETRY                   (uint8_t)(2)      /*!< Maximum number of communication attempts in case of 
                                                               failure. The Initiator trys additional once to send the 
															   data to the Target before returning an error to the caller. */
#define NFC_TIMEOUT                     (106)    /*!< Timeout in etu's for NFC.       */
#define NFC_DEFAULT_TRX_BUF_SIZE        (64)     /*!< Default size of one data block. */
#define NFC_MAX_TRX_BUF_SIZE            (uint8_t)(0xFF)   /*!< Maximum size of one data block. */
/*@}*/


/*!
 *  \name Length definitions for NFC-Initiator
 *  \ingroup nfci
 *
 *  @{
 */
#define NFC_WUP_REQ_LEN      (14)       /* Size of WUP_REQ.                                           */
#define NFC_MIN_PSL_REQ_LEN   (5)       /* Size of PSL_REQ. (without otional DID).                    */
#define NFC_MAX_PSL_REQ_LEN   (6)       /* Size of PSL_REQ. (with otional DID).                       */
#define NFC_MIN_DSL_REQ_LEN   (3)       /* Minimum size of DSL_REQ (without optional DID).            */
#define NFC_MIN_RLS_REQ_LEN   (3)       /* Minimum size of RLS_REQ (without optional DID).            */
#define NFC_MIN_DEP_REQ_LEN   (4)       /* Minimum size of DEP_REQ (without optional DID and NAD).    */
#define NFC_MAX_DEP_REQ_LEN   (6)       /* Maximum size of DEP_REQ Header (with optional DID and NAD).*/
/*! @} */

/*!
 *  \name Length definitions for NFC Target
 *  \ingroup nfct
 *
 *  @{
 */
#define NFC_WUP_RES_LEN       (4)       /* Size of WUP_RES.                                           */
#define NFC_MAX_PSL_RES_LEN   (4)       /* Maximum Size of PSL_RES.                                   */
#define NFC_MIN_PSL_RES_LEN   (3)       /* Minimum size of PSL_RES (without optional DID).            */
#define NFC_MIN_ATR_RES_LEN  (18)       /* Minimum size of ATR_RES wo. optional generics.             */
#define NFC_MIN_DSL_RES_LEN   (3)       /* Minimum size of DSL_RES (without optional DID).            */
#define NFC_MIN_RLS_RES_LEN   (3)       /* Minimum size of RLS_RES (without optional DID).            */
#define NFC_MIN_DEP_RES_LEN   (4)       /* Minimum size of DEP_RES (without optional DID and NAD).    */
/*! @} */

/*!
 *  \name Length and Positions
 *  \ingroup nfc
 *
 *  @{
 */
#define NFC_LEN_POS           (0)       /* Position of length byte.                                   */
#define NFC_COMMAND_LEN       (2)       /* First 2 bytes are used for NFC command.                    */
#define NFC_CMD0_POS          (1)       /* CMDO-byte position.                                        */
#define NFC_CMD1_POS          (2)       /* CMD1-byte position.                                        */
/*! @} */



/*!
 *  \name Defines for DEP_REQ/RES
 *  \ingroup nfc
 *
 *  @{
 */
#define NFC_PFB_7_5_MASK      (uint8_t)(0xE0)        /* b:1110 0000 mask for bits 7-5.    */
#define NFC_PFB_7_5_MASK_I    (uint8_t)(0x1F)        /* Bit inversion of NFC_PFB_7_5_MASK. */
#define NFC_DEP_SUPERVISORY   (uint8_t)(0x80)        /* PFB= b:1000 0000.                 */
#define NFC_DEP_ACKNOWLEDGE   (uint8_t)(0x40)        /* PFB= b:0100 0000.                 */
#define NFC_DEP_INFORMATION   (uint8_t)(0x00)        /* PFB= b:0000 0000.                 */
#define NFC_DEP_MI_PRESENT    (uint8_t)(0x10)        /*                                   */
#define NFC_DEP_MI_PRESENT_I  (uint8_t)(0xEF)        /*                                   */
#define NFC_DEP_NAD_PRESENT   (uint8_t)(0x08)        /*                                   */
#define NFC_DEP_NAD_PRESENT_I (uint8_t)(0xF7)        /*                                   */
#define NFC_DEP_DID_PRESENT   (uint8_t)(0x04)        /*                                   */
#define NFC_DEP_NACK          (uint8_t)(0x10)        /* Used to test if NACK bit is set.  */
#define NFC_DEP_NACK_I        (uint8_t)(0xEF)        /* Bit inversion of NFC_DEP_NACK.    */
#define NFC_DEP_TO_EXT        (uint8_t)(0x10)        /* Used to test if Timeout Extension
                                             * bit is set.                       */
#define NFC_DEP_TO_EXT_I      (uint8_t)(0xEF)        /* Bit inversion of NFC_DEP_TO_EXT.  */
#define NFC_DEP_RTOX_MASK     (uint8_t)(0x3F)        /* b: 0011 1111, coding for TOX-byte.*/
#define NFC_DEP_PNI_MASK      (uint8_t)(0x03)        /*                                   */
#define NFC_DEP_PNI_MASK_I    (uint8_t)(0xFC)        /*                                   */
#define NFC_DEP_PFB_POS       (uint8_t)(0x03)        /* PFB position on the DEP_RE* frame.*/
#define NFC_DEP_DID_POS       (uint8_t)(0x04)        /* DID position in DEP_RE* frame.    */
/*! @} */


/*!
 *  \name General Defines for ATR_REQ/RES
 *  \ingroup nfc
 *  These are general defines for the ATR_RE* sequence.
 *
 *  @{
 */
#define NFC_ATR_NAD_PRESENT   (uint8_t)(0x01)        /* PP* bit 0.                     */
#define NFC_ATR_GI_PRESENT    (uint8_t)(0x02)        /* PP* bit 1.                     */
#define NFC_ATR_NFCID_POS     (0x03)        /* Start-Position of nfcid.       */
#define NFC_ATR_DID_POS       (0x0D)        /* Position of DID byte.          */
#define NFC_ATR_BS_POS        (0x0E)        /* Position of BS I/T byte.       */
#define NFC_ATR_BR_POS        (0x0F)        /* Position of BR I/T byte.       */
#define NFC_ATR_PPI_POS       (0x10)        /* Position of PPI byte.          */
#define NFC_ATR_PPT_POS       (0x11)        /* Position of PPT byte.          */
#define NFC_ATR_TO_POS        (0x10)        /* Position of TO byte.           */
#define NFC_ATR_GI_POS        (0x11)        /* Pos. of Initiator Generic bytes.*/
#define NFC_ATR_GT_POS        (0x12)        /* Pos. of Target Generic bytes.   */

#define NFC_ATR_LRI_MASK      (0x30)        /* Mask of LRI bits 0011 0000     */
#define NFC_ATR_LRT_MASK      (0x30)        /* Mask of LRT bits 0011 0000     */
/*! @} */

/*! \name Default Timeout Definition for ATR_REQ/RES
 *  \ingroup nfc
 */
/*@{*/
#define NFC_ATR_TO_DEFAULT    (0x04)        /* Default timeout for ATR.        */
/*@}*/


/*!
 *  \name General Defines for PSL_REQ/RES
 *  \ingroup nfc
 *
 *  @{
 */
#define NFC_PSL_DID_POS       (0x03)        /* Position of DID Byte.           */
#define NFC_PSL_BRS_POS       (0x04)        /* Position of BRS Byte.           */
#define NFC_PSL_FSL_POS       (0x05)        /* Position of FSL Byte.           */
/*! @} */



/*!
 *  \name Defines for DSL_REQ/RES:
 *  \ingroup nfc
 *
 *  @{
 */
#define NFC_DSL_DID_POS       (0x03)        /* Position of DID Byte.           */
/*! @}
*/  /* DSL */


/*!
 *  \name Defines for WUP_REQ/RES:
 *  \ingroup nfc
 *
 *  @{
 */
#define NFC_WUP_REQ_DID_POS   (0x0D)        /* Position of DID Byte at WUP_REQ. */
#define NFC_WUP_RES_DID_POS   (0x03)        /* Position of DID Byte at WUP_RES. */
#define NFC_WUP_NFCID_POS     (0x03)        /* Start-Position of NFCID.         */
/*! @}
 */ /* WUP */

/*!
 *  \name Defines for RLS_REQ/RES:
 *  \ingroup nfc
 *
 *  @{
 */
#define NFC_RLS_DID_POS       (0x03)        /* Position of DID Byte.           */
/*! @}
 */ /* RLS */

#endif /* PHCSBFLNFC_COMMONINT_H */

/* E.O.F. */
