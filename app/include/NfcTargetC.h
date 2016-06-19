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


/*! \file NfcTargetC.h
 *
 * Projekt: Object Oriented Reader Library Framework BAL component.
 *
 *  Source: NfcTargetC.h
 * $Author: mha $
 * $Revision: 1.11 $
 * $Date: Wed Jun  7 09:49:19 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

#ifndef __NFC_TARGET_C_H__
#define __NFC_TARGET_C_H__

#include <phcsBflNfc.h>

/*
 * This example shows how to act as NFC Target
 */
uint16_t ActivateNfcTarget(void *comHandle, uint32_t aSettings);


/* NFC Target endpoint class, according to abstract class */
phcsBfl_Status_t TargetEndpointCallback(phcsBflNfc_TargetEndpointParam_t *endpoint_param);

#endif /* __NFC_TARGET_C_H__ */
