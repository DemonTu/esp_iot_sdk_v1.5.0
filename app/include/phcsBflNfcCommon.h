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
 * \file phcsBflNfcCommon.h
 *
 * Project: NFC common definitions for Initiator and Target.
 *
 * Workfile: phcsBflNfcCommon.h
 * $Author: mha $
 * $Revision: 1.2 $
 * $Date: Thu Oct 27 13:27:15 2005 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated 29. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */

#ifndef PHCSBFLNFCCOMMON_H
#define PHCSBFLNFCCOMMON_H


/*! \name Preamble definitions
 *  \brief The type value is coded such that the lower nibble represents the type identifier and the upper
 *  half byte equals the length of the preamble. Type one with a length of one must therefore
 *  be 0x11, type <em>t</em> with number <em>n</em> of bytes n would be <em>0xnt</em>. \n
 *  <B>Caution:</B> Internal processing relies on this implicit length definition! */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_PREAM_OMIT       (0x00)   /*!< \brief Use no preamble.                             */

#define PHCS_BFLNFCCOMMON_PREAM_TYPE1      (0x11)   /*!< \brief Preamble 1 is type to use. Adds manually preamble for 106 kbps which
                                                            is defined in PHCS_BFLNFCCOMMON_PREAM_1BYTE. */
#define PHCS_BFLNFCCOMMON_PREAM_1BYTE      (0xF0)   /* Preamble used for 106 kbps communication.    */

#define PHCS_BFLNFCCOMMON_MAX_PREAM_LEN    (1)      /*!< \brief Extra space to be reserved for the preamble. */
/*@}*/


/*!\name NFCID Length
 */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_ID_LEN           (10)       /*!< \brief Length of NFCID-3.                                       */
/*@}*/


/*!
 *  \name Bit Defines for ATR_REQ/RES
 *  \brief These are general defines for the ATR_RE* sequence.
 */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_ATR_PP_NAD_PRES  (0x01)        /*!< \brief PP* bit 0. Indicates NAD is present. */
#define PHCS_BFLNFCCOMMON_ATR_PP_GI_PRES   (0x02)        /*!< \brief PP* bit 1. Indicates General bytes are used. */
/*@}*/


/*! \name Length reduction value for Initiator/Target
 *  \brief These values stand for the common buffer size, both Initiator and Target is using.
 */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_ATR_PP_LR_64      (0x00)        /*!< \brief LR*= b:0000 0000 LRi,LRt = 64. \n
                                                               This should be the length used for PC opoeration
                                                               because of limited speed and reaction time. */
#define PHCS_BFLNFCCOMMON_ATR_PP_LR_128     (0x10)        /*   LR*= b:0001 0000 LRi,LRt = 128.  */
#define PHCS_BFLNFCCOMMON_ATR_PP_LR_192     (0x20)        /*   LR*= b:0010 0000 LRi,LRt = 192.  */
#define PHCS_BFLNFCCOMMON_ATR_PP_LR_256     (0x30)        /*   LR*= b:0011 0000 LRi,LRt = 256.  */
/*@}*/


/*! \name Receive Bit Rate Defines for PSL_REQ/RES
 *  \brief Data rate Receive of Initiator/Target: */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_1      (0x00)        /*!< \brief DR* = b:0000 0000 DRi,DRt = 1.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_2      (0x01)        /*!< \brief DR* = b:0000 0001 DRi,DRt = 2.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_4      (0x02)        /*    DR* = b:0000 0010 DRi,DRt = 4.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_8      (0x03)        /*    DR* = b:0000 0011 DRi,DRt = 8.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_16     (0x04)        /*    DR* = b:0000 0100 DRi,DRt = 16.  */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_32     (0x05)        /*    DR* = b:0000 0101 DRi,DRt = 32.  */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_64     (0x06)        /*    DR* = b:0000 0110 DRi,DRt = 64.  */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_RFU    (0x07)        /*    DR* = b:0000 0111 DRi,DRt = RFU. */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DR_MASK   (0x07)        /*!< \brief Mask of DR bits in PSL command frame as specified in
                                                               ISO 18092. b:0000 0111 */
/*@}*/


/*! \name Send Bit Rate Defines for PSL_REQ/RES
 *  \brief Data rate Send of Initiator/Target: */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_1       (0x00)        /*!< \brief DS* = b:0000 0000 DSi,DSt = 1.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_2       (0x08)        /*!< \brief DS* = b:0000 1000 DSi,DSt = 2.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_4       (0x10)        /* DS* = b:0001 0000 DSi,DSt = 4.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_8       (0x18)        /* DS* = b:0001 1000 DSi,DSt = 8.   */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_16      (0x20)        /* DS* = b:0010 0000 DSi,DSt = 16.  */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_32      (0x28)        /* DS* = b:0010 1000 DSi,DSt = 32.  */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_64      (0x30)        /* DS* = b:0011 0000 DSi,DSt = 64.  */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_RFU     (0x38)        /* DS* = b:0011 1000 DSi,DSt = RFU. */
#define PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK    (0x38)        /*!< \brief Mask of DS bits in PSL command frame as specified in
                                                                ISO 18092. b:0011 1000 */
/*@}*/

/*! \name Defines for PSL_REQ/RES
 */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_PSL_FSL_LR_64       (0x00)        /*!< \brief Maximum data packet length to be choosen for communication. \n
                                                                 For PC environment only this value should be choosen because of
                                                                 limited speed and reaction time. */
#define PHCS_BFLNFCCOMMON_PSL_FSL_LR_128      (0x01)        /* */
#define PHCS_BFLNFCCOMMON_PSL_FSL_LR_192      (0x02)        /* */
#define PHCS_BFLNFCCOMMON_PSL_FSL_LR_256      (0x03)        /* */
#define PHCS_BFLNFCCOMMON_PSL_FSL_LR_MASK     (0x03)        /*!< \brief Mask of LRI bits in PSL command frame as specified in
                                                                ISO 18092. b:0000 0011.  */
/*@}*/

/*! \name NFC Command Codes
 *  \brief These codes can be used to determine the incoming request type within a Target/Card implementation
 *  before branching into a specific protocol handler. It defines the type of the sender of the data stream.
 *  According to ISO 18092 PHCS_BFLNFCCOMMON_REQ is always send by an Initiator, PHCS_BFLNFCCOMMON_RES always by a Target.
 */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_REQ               (0xD4)       /*!< \brief NFC Request Code (CMD0).
                                                              Always used by an Initiator.  */
#define PHCS_BFLNFCCOMMON_RES               (0xD5)       /*!< \brief NFC Response Code (CMD0).
                                                              Always used by a Target.      */
/*@}*/


/* NFC Commands:                                                         */
/*!
 *  \name Initiator Request Codes
 */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_ATR_REQ           (0x00)       /*!< \brief Attribite Request Code.  */
#define PHCS_BFLNFCCOMMON_WUP_REQ           (0x02)       /*!< \brief Wakeup Request Code.     */
#define PHCS_BFLNFCCOMMON_PSL_REQ           (0x04)       /*!< \brief Parameter Select Request Code.  */
#define PHCS_BFLNFCCOMMON_DEP_REQ           (0x06)       /*!< \brief Data Exchange Request Code.     */
#define PHCS_BFLNFCCOMMON_DSL_REQ           (0x08)       /*!< \brief Deselect Request Code.   */
#define PHCS_BFLNFCCOMMON_RLS_REQ           (0x0A)       /*!< \brief Release Request Code.    */
/*@}*/

/*!
 *  \name NFC Target Response Codes
 */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_ATR_RES           (0x01)       /*!< \brief Attribute Response Code. */
#define PHCS_BFLNFCCOMMON_WUP_RES           (0x03)       /*!< \brief Wakeup Response Code.    */
#define PHCS_BFLNFCCOMMON_PSL_RES           (0x05)       /*!< \brief Parameter Select Response Code. */
#define PHCS_BFLNFCCOMMON_DEP_RES           (0x07)       /*!< \brief Data Exchange Response Code.    */
#define PHCS_BFLNFCCOMMON_DSL_RES           (0x09)       /*!< \brief Deselect Response Code.  */
#define PHCS_BFLNFCCOMMON_RLS_RES           (0x0B)       /*!< \brief Release Response Code.   */
/*@}*/

/*! \name Send Bit Rate Defines for ATR_REQ/RES
 *  \brief Sending bit duration supported by the Initiator/Target: \n
 *  These values are coded into BSi / BSt bytes. If more than one bit rate
 *  setting is supported, multiple values are OR'ed together. */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_ATR_BS_DS_8       (0x01)        /*!< \brief BS* = b:0000 0001 DSi,DSt = 8.    */
#define PHCS_BFLNFCCOMMON_ATR_BS_DS_16      (0x02)        /* BS* = b:0000 0010 DSi,DSt = 16.   */
#define PHCS_BFLNFCCOMMON_ATR_BS_DS_32      (0x04)        /* BS* = b:0000 0100 DSi,DSt = 32.   */
#define PHCS_BFLNFCCOMMON_ATR_BS_DS_64      (0x08)        /* BS* = b:0000 1000 DSi,DSt = 64.   */
/*@}*/

/*! \name Receive Bit Rate Defines for ATR_REQ/RES
 *  \brief Receiving bit duration supported by the Initiator/Target: \n
 *  These values are coded into BRi / BRt bytes. If more than one bit rate
 *  setting is supported, multiple values are OR'ed together. */
/*@{*/
/*! \ingroup nfc */
#define PHCS_BFLNFCCOMMON_ATR_BR_DR_8       (0x01)        /*!< \brief BR* = b:0000 0001 DRi,DRt = 8.    */
#define PHCS_BFLNFCCOMMON_ATR_BR_DR_16      (0x02)        /* BR* = b:0000 0010 DRi,DRt = 16.   */
#define PHCS_BFLNFCCOMMON_ATR_BR_DR_32      (0x04)        /* BR* = b:0000 0100 DRi,DRt = 32.   */
#define PHCS_BFLNFCCOMMON_ATR_BR_DR_64      (0x08)        /* BR* = b:0000 1000 DRi,DRt = 64.   */
/*@}*/



#endif /* PHCSBFLNFCCOMMON_H */

/* E.O.F. */
