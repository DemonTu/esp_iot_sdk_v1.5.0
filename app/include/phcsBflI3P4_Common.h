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
 * \file phcsBflI3P4_Common.h
 *
 * Project: ISO14443-4 basic functionality and protocol flow implementation for activation.
 *
 * Workfile: phcsBflI3P4_Common.h
 * $Author: mha $
 * $Revision: 1.1 $
 * $Date: Mon Oct 24 11:40:48 2005 $
 * $KeysEnd$
 *
 * Comment:
 *  The functios declared here are used within the C and C++ of the implementation. When used by the
 *  C variant these functions, providing already full protocol facilities, are called directly. When
 *  used within a C++ environment the functions are encapsulated by a class which provides
 *  additional features such as multi-device capabilities per application.
 *
 * History:
 *  GK:  Generated 14. Okt. 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLI3P4_COMMON_H
#define PHCSBFLI3P4_COMMON_H

#include <phcsBflStatus.h>

#define PHCS_BFLI3P4_COMMON_MAX_CID              (14)     /*!< \brief Maximum CID number allowed in ISO/IEC 18092 */
#define PHCS_BFLI3P4_COMMON_CID_SUP_MASK   (0x02)   /*!< \brief Flag to indicate CID is supported. */
#define PHCS_BFLI3P4_COMMON_NAD_SUP_MASK   (0x01)   /*!< \brief Flag to indicate NAD is supported. */


#define PHCS_BFLI3P4_COMMON_BAUD_MASK         (0x03)      /*!< \brief Mask for data rate settings. */
#define PHCS_BFLI3P4_COMMON_BAUD_106_INT      (0x00)      /*!< \brief Configuration for 106 kbps. */
#define PHCS_BFLI3P4_COMMON_BAUD_212_INT      (0x01)      /*!< \brief Configuration for 212 kbps. */
#define PHCS_BFLI3P4_COMMON_BAUD_424_INT      (0x02)      /*!< \brief Configuration for 424 kbps. */
#define PHCS_BFLI3P4_COMMON_BAUD_848_INT      (0x03)      /*!< \brief Configuration for 848 kbps. */


/*! \ingroup iso14443_4
 *  Structure for ISO/IEC 14443.4 data transmission protocol initialisation. This structure has
 *  to be handed over to the protocol component instance prior to communication and after PICC
 *  protocol entry. The purpose is to inform the data exchange protocol about the PICC and Reader
 *  properties (parameters) to use during communication. \n
 *  This structure is filled automatically be the protocol entry interface C_ISO14443_4A_ACTIVATION
 *  for type A PICCs but has to be initialised member-by-member when operating type B PICCs.
 */
typedef struct
{
    uint8_t   *trx_buffer;        /*!< \brief [in] Pointer to a pre - allocated TX/RX buffer. This buffer
                                                  is used by the protocol for data exchange frame composition
                                                  and handed over to the underlying I/O features (RC-I/O). */
    uint16_t   trx_buffer_size;   /*!< \brief [in] Buffer size for frames composed by the protocol. Typical
                                                  values are 64, 128 or 256 [bytes]. */
    uint8_t    cid_supported;     /*!< \brief [in] A value other than zero (TRUE) indicates that CID is supported
                                                  and shall be used by the protocol. */
    uint8_t    nad_supported;     /*!< \brief [in] A value other than zero (TRUE) indicates that NAD is supported
                                                  and shall be used by the protocol. */
    uint8_t    cid;               /*!< \brief [in] If CID is supported this value will be inserted into the CID
                                                  field of exchanged frames. */
    uint8_t    tx_baud_int;       /*!< \brief [in] ISO/IEC 14443.4 definitions for the baud rate (DSI)
                                                  used for sending information to the PICC. */
    uint8_t    rx_baud_int;       /*!< \brief [in] ISO/IEC 14443.4 definitions for the baud rate (DRI)
                                                  used for receiving information from the PICC. */
    uint8_t    fwi;               /*!< \brief [in] ISO/IEC 14443.4 definitions for the timing information 
                                                  received from the PICC. */
    uint8_t    fsdi;              /*!< \brief [in] This value codes FSD. The FSD defines the maximum size of a 
                                                  frame the PCD is able to receive.*/
    uint8_t    fsci;              /*!< \brief [in] This value codes FSC. The FSC defines the maximum size of a 
                                                  frame accepted by the PICC */
                                                
} phcsBflI3P4_ProtParam_t;




#endif /* PHCSBFLI3P4_COMMON_H */

/* E.O.F. */
