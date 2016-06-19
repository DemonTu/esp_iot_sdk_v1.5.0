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


/*! \file NfcInitiatorC.h
 *
 * Projekt: Object Oriented Reader Library Framework BAL component.
 *
 *  Source: NfcInitiatorC.h
 * $Author: mha $
 * $Revision: 1.13 $
 * $Date: Wed Jun  7 09:49:17 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

#ifndef __NFC_INITIATOR_C_H__
#define __NFC_INITIATOR_C_H__

/*
 * This example shows how to act as NFC Initiator
 * Arguments:
 *		comHandle			Valid handle to the COM port.
 *		isPassive			If equal zero, active mode is used, otherwise passive mode
 *		opSpeed				1 == 106kBit/s, 2 == 212kBit/s, 3 == 424kBit/s
 *		settings			Optional settings.
 */
phcsBfl_Status_t ActivateNfcInitiator(void *comHandle, 						
								    uint8_t isPassive,
								    uint8_t opSpeed );

#endif /* __NFC_INITIATOR_C_H__ */
