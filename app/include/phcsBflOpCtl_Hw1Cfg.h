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
 * \file phcsBflOpCtl_Hw1Cfg.h
 *
 * Project: Object Oriented Library Framework OperationControl Component matrix for differnet hardware versions.
 *
 *  Source: phcsBflOpCtl_Hw1Cfg.h
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 30 11:01:52 2006 $
 *
 * Comment:
 *  Configuration matrices for all used communication modes at different data rates.
 *
 * History:
 *  MHa: Generated 14. October 2004
 *  MHa: Migrated to MoReUse September 2005
 *
 */


#ifndef PHCSBFLOPCTL_HW1CFG_H
#define PHCSBFLOPCTL_HW1CFG_H

#include <phcsBflHw1Reg.h>
#include <phcsBflOpCtl.h>


/* 
 * Define parameter sets for hardware type 1 for all communication modes and data rates.
 * The order of the definition is the same as the order of execution.
 * This parameter set is valid for the hardware version 0x28 (value of Version register) and is used as default 
 * value if the current hardware is unknown.
 */
PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW1ACT_106)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x61)                /* DemodReg:        set to 0x61 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       0xFF,               PHCS_BFL_JBIT_FORCE100ASK)    /* TXAutoReg:       set bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xF4)                /* GsNReg:          set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xF4)                /* GsNLoadModReg:   set to 0xF4 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW1ACT_212424)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x61)                /* DemodReg:        set to 0x61 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x69)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xFF)                /* GsNReg:          set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xFF)                /* GsNLoadModReg:   set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x11)                /* ModGsPReg:       set to 0x11 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW1PASI_106)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x4D)                /* DemodReg:        set to 0x4D */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x59 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       0xFF,               PHCS_BFL_JBIT_FORCE100ASK)    /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xF4)                /* GsNReg:          set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xF4)                /* GsNLoadModReg:   set to 0xF4 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW1PASI_212424)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x41)                /* DemodReg:        set to 0x41 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x69)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xFF)                /* GsNReg:          set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xFF)                /* GsNLoadModReg:   set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x11)                /* ModGsPReg:       set to 0x11 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW1PAST)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MIFARE,       0x00,               0x4A)                /* MifareReg:       set to 0x4A */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x07)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x07 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x61)                /* DemodReg:        set to 0x61 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0x6F)                /* GsNReg:          set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0x6F)                /* GsNLoadModReg:   set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x59 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

/* End of parameter set for hardware type 1 */



/* 
 * Define parameter sets for hardware type 2 and 3 for all communication modes and data rates.
 * The order of the definition is the same as the order of execution.
 * This parameter set is valid for the hardware version 0x26 & 0x27 (value of Version register).
*/
PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW2ACT_106)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x2C)                /* DemodReg:        set to 0x2C */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       0xFF,               PHCS_BFL_JBIT_FORCE100ASK)    /* TXAutoReg:       set bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXSEL,        0x00,               0x84)                /* RxSelReg:        set to 0x84 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x44)                /* RxTresholdReg:   set to 0x44 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xF4)                /* GsNReg:          set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xF4)                /* GsNLoadModReg:   set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MIFARE,       0x00,               0x62)                /* MifareReg:       set to 0x62 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW2ACT_212424)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x2C)                /* DemodReg:        set to 0x2C */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXSEL,        0x00,               0x84)                /* RxSelReg:        set to 0x84 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x33)                /* RxTresholdReg:   set to 0x33 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x69)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xFF)                /* GsNReg:          set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xFF)                /* GsNLoadModReg:   set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x0F)                /* ModGsPReg:       set to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MIFARE,       0x00,               0x62)                /* MifareReg:       set to 0x62 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW2PASI_106)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       0xFF,               PHCS_BFL_JBIT_FORCE100ASK)    /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0xAD)                /* DemodReg:        set to 0xAD */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXSEL,        0x00,               0x84)                /* RxSelReg:        set to 0x84 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x44)                /* RxTresholdReg:   set to 0x44 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xF4)                /* GsNReg:          set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xF4)                /* GsNLoadModReg:   set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MIFARE,       0x00,               0x62)                /* MifareReg:       set to 0x62 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW2PASI_212424)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x4C)                /* DemodReg:        set to 0x4C */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXSEL,        0x00,               0x84)                /* RxSelReg:        set to 0x84 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x33)                /* RxTresholdReg:   set to 0x33 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x69)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xFF)                /* GsNReg:          set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0xFF)                /* GsNLoadModReg:   set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x0F)                /* ModGsPReg:       set to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MIFARE,       0x00,               0x62)                /* MifareReg:       set to 0x62 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW2PAST)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MIFARE,       0x00,               0x6A)                /* MifareReg:       set to 0x6A */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x09)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x09 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXSEL,        0x00,               0x84)                /* RxSelReg:        set to 0x84 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x44)                /* RxTresholdReg:   set to 0x44 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x2C)                /* DemodReg:        set to 0x2C */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x59 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0x6F)                /* GsNReg:          set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0x6F)                /* GsNLoadModReg:   set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x20)                /* CWGsPReg:        set to 0x20 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x20)                /* ModGsPReg:       set to 0x20 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

/* End of parameter set for hardware type 2 and 3 */


/* 
 * Define parameter sets for hardware type 4 and 5 for all communication modes and data rates.
 * The order of the definition is the same as the order of execution.
 * This parameter set is valid for the hardware version 0xA0 and 0x80 (value of Version register) and is used as default 
 * value if the current hardware is unknown.
 */
PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW4ACT_106)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       0xFF,               PHCS_BFL_JBIT_FORCE100ASK)    /* TXAutoReg:       set bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x61)                /* DemodReg:        set to 0x61 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0x6F)                /* GsNLoadModReg:   set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xF4)                /* GsNReg:          set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x11)                /* ModGsPReg:       set to 0x11 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW4ACT_212424)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x61)                /* DemodReg:        set to 0x61 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0x6F)                /* GsNLoadModReg:   set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x69)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xFF)                /* GsNReg:          set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x14)                /* ModGsPReg:       set to 0x14 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW4PASI_106)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       0xFF,               PHCS_BFL_JBIT_FORCE100ASK)    /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x4D)                /* DemodReg:        set to 0x4D */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0x6F)                /* GsNLoadModReg:   set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x59 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xF4)                /* GsNReg:          set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x11)                /* ModGsPReg:       set to 0x11 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW4PASI_212424)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:       clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x0F)                /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                    set other to 0x0F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x41)                /* DemodReg:        set to 0x41 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0x6F)                /* GsNLoadModReg:   set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x69)                /* RFCfgReg:        set to 0x69 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xFF)                /* GsNReg:          set to 0xFF */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x14)                /* ModGsPReg:       set to 0x14 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(PHCS_BFLOPCTL_VAL_HW4PAST)
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MIFARE,       0x00,               0x62)                /* MifareReg:       set to 0x62 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXAUTO,       (uint8_t)~PHCS_BFL_JBIT_FORCE100ASK,  0x00) /* TXAutoReg:    clear bit Force100ASK */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_TXBITPHASE,   PHCS_BFL_JBIT_RCVCLKCHANGE,  0x07)       /* TxBitphaseReg:   unchanged bit RcvClkChange, 
                                                                                                                           set other to 0x07 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RXTRESHOLD,   0x00,               0x55)                /* RxTresholdReg:   set to 0x55 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODWIDTH,     0x00,               0x26)                /* ModWidthReg:     set to 0x26 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_DEMOD,        0x00,               0x61)                /* DemodReg:        set to 0x61 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSNLOADMOD,   0x00,               0x6F)                /* GsNLoadModReg:   set to 0x6F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_RFCFG,        0x00,               0x59)                /* RFCfgReg:        set to 0x59 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_GSN,          0x00,               0xF4)                /* GsNReg:          set to 0xF4 */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_CWGSP,        0x00,               0x3F)                /* CWGsPReg:        set to 0x3F */
    PHCS_BFLOPCTL_VAL_PARAM_ENTRY(PHCS_BFL_JREG_MODGSP,       0x00,               0x11)                /* ModGsPReg:       set to 0x11 */
PHCS_BFLOPCTL_VAL_END_PARAM_SET

/* End of parameter set for hardware type 4 and 5 */



/* 
 * Define parameter sets for hardware type 6 for all communication modes and data rates in the same manner as
 * for hardware type 1 to 5 as seen above. The order of the definition is the same as the order of execution.
 *
 * Note: Check also if initialisation in phcsBflOpCtl_Hw1.c is according to new hardware sets. If not, add new setup
 *       configuration to initialisation and to SetAttrib command.
*/

/* End of parameter set for hardware type 5 */


#endif /* PHCSBFLOPCTL_HW1CFG_H */
