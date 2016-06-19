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


/*! \file MifareReader.h
 *
 * Projekt: Object Oriented Reader Library Framework BAL component.
 *
 *  Source: MifareReader.h
 * $Author: mha $
 * $Revision: 1.13 $
 * $Date: Wed Jun  7 09:49:20 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

#ifndef __READER_MIFARE_H__
#define __READER_MIFARE_H__

/*
 * This example shows how to act as Mifare reader
 */
#define   COIN_OPT_INIT_PRE_FLOW        (0x8000)      /* 要初始化和前期的流程 */
#define   COIN_OPT_OUTPUT_ARGV_PARA     (0x4000)      /* argv作为输出指针 */
/*****************************************************************************/
/* init -> active -> req -> select [->RD/WR] -> halt*/
extern void  MifareReader_init(void);
extern int   ISO14443_active(int argc, char *argv[]);
extern int   MifareReader_requestA(int argc, char *argv[]);
extern int   MifareReader_halt(int argc, char *argv[]);
extern int   MifareReader_select(int argc, char *argv[],uint8_t *uid);
extern int   MifareReader_block_read(uint_fast8_t block, uint8_t *rd_buf);
extern int   MifareReader_block_write(uint_fast8_t block, uint8_t *wr_buf);
extern int   MifareReader_block_auth(uint_fast8_t block, uint8_t *uid, uint8_t *keyA, uint8_t *keyB);
/*****************************************************************************/
extern void  PN512_reset(void);
extern void  PN512_idle(void);
extern void  rfid_reset_timer(void);
extern void  rfid_reset_enable(uint_fast16_t _ms);
/*****************************************************************************/
extern uint_fast8_t PN512RDSerialnum(uint8_t * o_snr);
extern void PN512Close(void);

#endif /* __MIFARE_READER_H__ */

