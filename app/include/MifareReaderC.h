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


/*! \file MifareReaderC.h
 *
 * Projekt: Object Oriented Reader Library Framework BAL component.
 *
 *  Source: MifareReaderC.h
 * $Author: mha $
 * $Revision: 1.10 $
 * $Date: Wed Jun  7 09:49:20 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

#ifndef __MIFARE_READER_C_H__
#define __MIFARE_READER_C_H__

/*
 * This example shows how to act as Mifare reader 
 */
uint16_t ActivateMifareReader(void *comHandle, uint32_t aSettings);

#endif /* __MIFARE_READER_C_H__ */
