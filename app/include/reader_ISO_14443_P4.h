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


/*! \file Iso14443_4_Reader.h
 *
 * Projekt: Object Oriented Reader Library Framework BAL component.
 *
 *  Source: Iso14443_4_Reader.h
 * $Author: mha $
 * $Revision: 1.9 $
 * $Date: Wed Jun  7 09:49:19 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

#ifndef __READER_I3P4_H__
#define __READER_I3P4_H__

#include  "phcsBflRegCtl.h"
#include  "phcsBflOpCtl.h"

/******************************************************************************/
/*
 * This example shows how to act as ISO14443-4 reader
 */
int ActivateIso14443_4_Reader_init(int argc, char *argv[]);
int ActivateIso14443_4_Reader_request(int argc, char *argv[]);
int ActivateIso14443_4_Reader_select(int argc, char *argv[],uint8_t *uid,uint8_t *uid_len);
int ActivateIso14443_4_Reader_rats(int argc, char *argv[]);
int ActivateIso14443_4_Reader_pre_exchange(int argc, char *argv[]);
int ActivateIso14443_4_Reader_Apdu(uint8_t *apdu,int len);
int ActivateIso14443_4_Reader_deselect(int argc, char *argv[]);
int ActivateIso14443_4_Reader(uint8_t *send_buf, uint_fast8_t len);
/******************************************************************************/
extern int Cpu_Reader_Request(uint8_t *uid,uint8_t *uidlen);
extern int Cpu_Reader_Apdu(uint8_t *send_buf, int len);
extern int Cpu_Reader_Close(void);
extern int Cpu_Reader_Respone(uint8_t *rec);


/******************************************************************************/
phcsBfl_Status_t SetTimeOut(    phcsBflRegCtl_t *rc_reg_ctl,
                                phcsBflOpCtl_t *rc_op_ctl,
                                uint32_t aMicroSeconds,
                                uint8_t aFlags);


#define EXAMPLE_TRX_BUFFER_SIZE       64
#define PN51X_TMR_DEFAULT	0x00
#define PN51X_STEPS_10US	0x01
#define PN51X_STEPS_100US	0x02
#define PN51X_STEPS_500US	0x03
#define PN51X_TMR_MAX		0x10
#define PN51X_TMR_OFF		0x20
#define PN51X_START_NOW		0x80
/******************************************************************************/

/* nfc */
#define EXAMPLE_TARGET_BUFFER_SIZE    80
#define EXAMPLE_USER_BUFFER_SIZE      128

#endif /* __I3P4_READER_H__ */

