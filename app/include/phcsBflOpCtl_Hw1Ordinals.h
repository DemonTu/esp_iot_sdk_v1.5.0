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
 * \file phcsBflOpCtl_Hw1Cfg.h
 *
 * Project: Object Oriented Library Framework OperationControl Component parameter definitions.
 *
 *  Source: phcsBflOpCtl_Hw1Cfg.h
 * $Author: mha $
 * $Revision: 1.6 $
 * $Date: Fri Jun 23 07:20:54 2006 $
 *
 * Comment:
 *  This file defines ordinal numbers for configuration types and settings or values respectively.
 *
 * History:
 *  GK:  Generated 11. June 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLOPCTL_HW1ORDINALS_H
#define PHCSBFLOPCTL_HW1ORDINALS_H

/* Ordinals are all uint8_t ! */

/*! \name Metrics
    \brief Definitions for static settings, version and there more.   
*/
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_GROUP_RCMETRICS             (0x00)    /*!< \brief Metrics group for static settings, versions and there more. */
 #define PHCS_BFLOPCTL_ATTR_VERSION               (0x00)    /*!< \brief Hardware version retrieval. */
/*@}*/

/*! \name Operation Modes
    \brief Definitions for dedicated mode settings including Softreset, Reader's, Initiator and Target.
*/
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_GROUP_MODE                  (0x01)    /*!< \brief Operation mode group to configure the hardware according to a 
                                                       communication mode, e.g. NFC and Mifare. */
 #define PHCS_BFLOPCTL_ATTR_ACTIVE                (0x01)    /*!< \brief Mode used for communicating as NFC Initiator and as NFC Target
                                                                in active communication mode.
                                                                
                                                                param_a: TxSpeed, param_b: RxSpeed \n
                                                                This function changes in addition to the matrix defined:
                                                                    <ul>
                                                                    <li> Register TxMode: Set TxCRC, TxSpeed according to 
                                                                            input parameter and TxFraming to b01. </li>
                                                                    <li> Register RxMode: Set RxCRC, RxSpeed according to 
                                                                            input parameter and RxFraming to b01. </li>
                                                                    <li> Register TxAuto: Set AutoRfOff and CAOn 
                                                                    </ul>                                                       */
 #define PHCS_BFLOPCTL_ATTR_PASSIVE               (0x02)    /*!< \brief Mode used for communicating as NFC Target at all data rates
                                                                or other Passive device. 
                                                                    
                                                                param_a: TxSpeed, param_b: RxSpeed  \n
                                                                This function changes in addition to the matrix defined:
                                                                    <ul>
                                                                    <li> Register TxAuto: Clear AutoRfOff, Force100ASK, CAOn, 
                                                                            InitialRfOn, Tx2RFAutoEn and Tx1RFAutoEn </li>
                                                                    <li> Register TxControl: Clear Tx2RFEn and Tx1RFEn </li>
                                                                    <li> Register TxMode: Set TxCRC, TxSpeed according to 
                                                                            input parameter and TxFraming to b00. </li>
                                                                    <li> Register RxMode: Set RxCRC, RxSpeed according to 
                                                                            input parameter and RxFraming to b00. </li>
                                                                    </ul>                                                       */
 #define PHCS_BFLOPCTL_ATTR_MFRD_A                (0x03)    /*!< \brief  Mode used for communicating as Mifare Reader or as NFC Initiator 
                                                                in passive communication mode at 106 kbps. 
                                                                   
                                                                param_a: TxSpeed, param_b: RxSpeed \n
                                                                This function changes in addition to the matrix defined:
                                                                    <ul>
                                                                    <li> Register TxMode: Set TxCRC, TxSpeed according to 
                                                                            input parameter and TxFraming to b00. </li>
                                                                    <li> Register RxMode: Set RxCRC, RxSpeed according to 
                                                                            input parameter and RxFraming to b00. </li>
                                                                    <li> Register TxAuto: Clear AutoRfOff and CAOn </li>
                                                                    <li> Register Modwidth: Set value according to selected 
                                                                            TxSpeed \n
                                                                            Speed equal to 106 kbps: value is 0x26 \n
                                                                            Speed equal to 212 kbps: value is 0x15 \n
                                                                            Speed equal to 424 kbps: value is 0x0A \n
                                                                            Speed equal to 848 kbps: value is 0x05 \n
                                                                            all other values keep the same. \n
                                                                            Precondition: For activation data rates of 212, 424 and
                                                                            848 kbps the first initialization has to be done with 
                                                                            a data rate of 106 kbps. </li>
                                                                    </ul>                                                       */
 #define PHCS_BFLOPCTL_ATTR_PAS_POL               (0x04)    /*!< \brief Mode used for communicating as FeliCa Reader and as NFC Initator
                                                                in passive communication mode at 212 and 424 kbps. 
                                                                   
                                                                param_a: TxSpeed, param_b: RxSpeed \n
                                                                This function changes in addition to the matrix defined:
                                                                    <ul>
                                                                    <li> Register TxMode: Set TxCRC, TxSpeed according to 
                                                                            input parameter and TxFraming to b02. </li>
                                                                    <li> Register RxMode: Set RxCRC, RxSpeed according to 
                                                                            input parameter and RxFraming to b02. </li>
                                                                    <li> Register TxAuto: Clear AutoRfOff, Force100ASK and CAOn 
                                                                    </ul>                                                       */
 #define PHCS_BFLOPCTL_ATTR_MFCARD                (0x05)    /*!< \brief Mode used for communicating as Mifare Card. 
 
                                                                These configuration 
                                                                is based on the configuration as PHCS_BFLOPCTL_ATTR_PASSIVE and is used
                                                                for higher / asymmetric datarates for Mifare Card operating mode. \n
                                                                Precondition: PHCS_BFLOPCTL_ATTR_PASSIVE, for general passive settings. \n
                                                                param_a: TxSpeed, param_b: RxSpeed \n
                                                                This function changes:
                                                                    <ul>
                                                                    <li> Register TxMode: Set TxCRC, TxSpeed according to 
                                                                            input parameter and TxFraming to b00. </li>
                                                                    <li> Register RxMode: Set RxCRC, RxSpeed according to 
                                                                            input parameter and RxFraming to b00. </li>
                                                                    <li> Register TxBitPhase: Set TxBitPhase to 0x07 </li>
                                                                    <li> Register Mifare: Set SensMiller and TauMiller according to
                                                                            RxSpeed to \n
                                                                            SensMiller: b011, TauMiller: b01 for 106 kbps \n
                                                                            SensMiller: b100, TauMiller: b01 for 212 kbps \n
                                                                            SensMiller: b101, TauMiller: b10 for 424 kbps \n
                                                                            SensMiller: b110, TauMiller: b11 for 848 kbps
                                                                    </li>
                                                                    </ul>                                                       */
  #define PHCS_BFLOPCTL_VAL_RF106K                (0x00)    /*<! \brief RF UART speed setting for 106 kbps. Might be used for Tx and 
                                                                   Rx configuration. */
  #define PHCS_BFLOPCTL_VAL_RF212K                (0x01)    /*<! \brief RF UART speed setting for 212 kbps. Might be used for Tx and 
                                                                   Rx configuration. */
  #define PHCS_BFLOPCTL_VAL_RF424K                (0x02)    /*<! \brief RF UART speed setting for 424 kbps. Might be used for Tx and 
                                                                   Rx configuration. */
  #define PHCS_BFLOPCTL_VAL_RF848K                (0x03)    /* */
  #define PHCS_BFLOPCTL_VAL_RF1M6                 (0x04)    /* */
  #define PHCS_BFLOPCTL_VAL_RF3M2                 (0x05)    /* */
  #define PHCS_BFLOPCTL_VAL_RF6M4                 (0x06)    /* */
/*@}*/

/*! \name Communication
    \brief Definitions for serial interface and contactless interface.
*/
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_GROUP_COMM                  (0x02)    /*!< \brief Communications group for settings different speed options. */
 #define PHCS_BFLOPCTL_ATTR_RS232BAUD             (0x01)    /*!< \brief Serial interface data rate setting. */
  #define PHCS_BFLOPCTL_VAL_SER9600               (0x00)    /* Serial RS232 speed setting(s). */
  #define PHCS_BFLOPCTL_VAL_SER19200              (0x01)    /* */
  #define PHCS_BFLOPCTL_VAL_SER38400              (0x02)    /* */
  #define PHCS_BFLOPCTL_VAL_SER57600              (0x03)    /* */
  #define PHCS_BFLOPCTL_VAL_SER115200             (0x04)    /* */
  #define PHCS_BFLOPCTL_VAL_SER230400             (0x05)    /* */
  #define PHCS_BFLOPCTL_VAL_SER460800             (0x06)    /* */
  #define PHCS_BFLOPCTL_VAL_SER921600             (0x07)    /* */
  #define PHCS_BFLOPCTL_VAL_SER1288000            (0x08)    /* */
/*@}*/


/*! \name Smart Connect signal settings (SAM)
    \brief Definitions for S2C settings for different configuration possibilities, e.g. Host connected to security chip, 
    antenna connected to security chip (SmartMX, SMX), ... .
*/
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_GROUP_SIGNAL                (0x05)    /*!< \brief Signal interface group for changing signal paths. */
 #define PHCS_BFLOPCTL_ATTR_SIGINOUT              (0x11)    /*!< \brief Defines if SIGIN/SIGOUT (S2C) shall be configured. */
  #define PHCS_BFLOPCTL_VAL_READER                (0x00)    /*!< \brief PN51x shall act as a reader device if this value is set in param_a. */
  #define PHCS_BFLOPCTL_VAL_CARD                  (0x01)    /*!< \brief PN51x shall act as card device if this value is set in param_a. */
  #define PHCS_BFLOPCTL_VAL_NOSIGINOUT            (0x00)    /*!< \brief Don't use SIGIN/SIGOUT if this value is set as param_b. */
  #define PHCS_BFLOPCTL_VAL_SIGINOUT              (0x01)    /*!< \brief Use SIGIN/SIGOUT if this value is set as param_b. */
  #define PHCS_BFLOPCTL_VAL_ONLYSIGINOUT		  (0x02)    /*!< \brief Use SIGIN/SIGOUT without any communication done by the PN51x if this
                                                                value is set in param_b. Only applicable when PN51x is used as card. 
                                                                (param_a set to PHCS_BFLOPCTL_VAL_CARD) */
  #define PHCS_BFLOPCTL_VAL_COMM_SIGINOUT         (0x03)    /*!< \brief Dynamical switch of communication between SigInOut and Antenna. \n
                                                                Some preconditions must be set. These are: configuration of the Antenna 
                                                                accodring to the communication mode (e.g. calling 
                                                                SetAttrib(PHCS_BFLOPCTL_ATTR_MFRD_A, PHCS_BFLOPCTL_VAL_RF106K, ...) and 
                                                                configuration of the SigInOut using SetAttrib(PHCS_BFLOPCTL_ATTR_SIGINOUT, 
                                                                PHCS_BFLOPCTL_VAL_READER, PHCS_BFLOPCTL_VAL_SIGINOUT). \n
                                                                Thereafter the communiation could be switched using PHCS_BFLOPCTL_VAL_COMM_SIGINOUT 
                                                                and PHCS_BFLOPCTL_VAL_COMM_ANTENNA.  \n
                                                                Deactivation is done using SetAttrib(.. PHCS_BFLOPCTL_VAL_NOSIGINOUT ..) */
  #define PHCS_BFLOPCTL_VAL_COMM_ANTENNA          (0x04)    /*!< \brief Dynamical switch of communication between SigInOut and Antenna. \n
                                                                Some preconditions must be set. These are: configuration of the Antenna 
                                                                accodring to the communication mode (e.g. calling 
                                                                SetAttrib(PHCS_BFLOPCTL_ATTR_MFRD_A, PHCS_BFLOPCTL_VAL_RF106K, ...) and 
                                                                configuration of the SigInOut using SetAttrib(PHCS_BFLOPCTL_ATTR_SIGINOUT, 
                                                                PHCS_BFLOPCTL_VAL_READER, PHCS_BFLOPCTL_VAL_SIGINOUT). \n
                                                                Thereafter the communiation could be switched using PHCS_BFLOPCTL_VAL_COMM_SIGINOUT 
                                                                and PHCS_BFLOPCTL_VAL_COMM_ANTENNA.  \n
                                                                Deactivation is done using SetAttrib(.. PHCS_BFLOPCTL_VAL_NOSIGINOUT ..) */
/*@}*/

/*! \name Timer settings
    \brief Definitions for timer relevant settings.
*/
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_GROUP_TMR                   (0x06)    /*!< \brief Timer relevant settings. */
 #define PHCS_BFLOPCTL_ATTR_TMR                   (0x00)    /*!< \brief Sets all timer parameters at once. \n
                                                                param_a: specifies the timer auto function, if set to 1, the automatical operation mode is chosen. \n
                                                                param_b: specifies the Auto restart feature, if set to one this feature is activated. \n
                                                                buffer[0], [1]: specifies the prescaler value (only 12 bit are valid); buffer[0] specifies low value, buffer[1] specifies upper 4 bits. \n
                                                                buffer[2], [3]: specifies the reload value; buffer[2] specifies low value, buffer[3] specifies the upper 8 bits. */
 #define PHCS_BFLOPCTL_ATTR_TMR_AUTO              (0x01)    /*!< \brief Sets timer to automatic operation mode or resets it. */
 #define PHCS_BFLOPCTL_ATTR_TMR_RESTART           (0x02)    /*!< \brief Activates automatic restart of timer or dectivates this feature. */
 #define PHCS_BFLOPCTL_ATTR_TMR_GATED             (0x03)    /*!< \brief param_a: value of TGated (max. 3) */
 #define PHCS_BFLOPCTL_ATTR_TMR_PRESCALER         (0x04)    /*!< \brief param_a: TPrescaler_LO, \n
                                                                param_b: TPrescaler_HI (max. 0x0F) */
 #define PHCS_BFLOPCTL_ATTR_TMR_RELOAD            (0x05)    /*!< \brief param_a: TReloadVal_LO \n
                                                                param_b: TReloadVal_HI      */
 #define PHCS_BFLOPCTL_ATTR_TMR_COUNTER           (0x06)    /*!< param_a: TCounterVal_LO, \n param_b: TCounterVal_HI */
/*@}*/

/*! \name CRC
    \brief Definitions for CRC calculation.
*/
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_GROUP_CRC                   (0x07)    /*!< \brief CRC relevant settings. */
 #define PHCS_BFLOPCTL_ATTR_CRC_PRESET            (0x00)    /*!< \brief param_a: use only bits 0 and 1, \n
                                                                value 00 -> Preset: 0x00, 0x00 \n
                                                                value 01 -> Preset: 0x63, 0x63 \n
                                                                value 10 -> Preset: 0xA6, 0x71 \n
                                                                value 11 -> Preset: 0xFF, 0xFF \n */
 #define PHCS_BFLOPCTL_ATTR_CRC_RESULT            (0x01)    /*!< \brief param_a: Result_LSB, param_b: Result_MSB */
 #define PHCS_BFLOPCTL_ATTR_CRC_MSB_FIRST         (0x02)    /*!< \brief Activates CRC caluclation for external CRC operation
                                                                with MSB first. During protocol operation this bit is 
                                                                not used. The calculation mode is depndent from the 
                                                                chosen communication mode. */
 #define PHCS_BFLOPCTL_ATTR_CRC_CALC              (0x03)    /*!< \brief [in]  param_a: length of data (max 64), data: start of data buffer
                                                                out: param_a: result LSB, param_b: result MSB */
/*@}*/

/*! \name Protocoll settings
    \brief Definitions for register settings for protocol operation e.g. FelicaLen, CAOn, ... .
*/
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_GROUP_PROTOCOL              (0x08)    /*!< \brief Protocol (data exchange) settings. */
 #define PHCS_BFLOPCTL_ATTR_NFCID                 (0x09)    /*!< \brief param_a: 00: SENS_RES, nfcid1,SEL_RES (6 bytes); 
                                                              01: nfcid2 (IDm, PMm, systemcode) (18 bytes);
                                                              02: nfcid3 (10 byte)
                                                              FF: nfcid is random data
                                                     param_b: length of data
                                                     data: pointer to data */
  #define PHCS_BFLOPCTL_VAL_NFCID1                (0x00)    /* */
  #define PHCS_BFLOPCTL_VAL_NFCID2                (0x01)    /* */
  #define PHCS_BFLOPCTL_VAL_NFCID3                (0x02)    /* */
  #define PHCS_BFLOPCTL_VAL_NFCID_RANDOM          (0xFF)    /* */
/*@}*/

/*! \name General used values
    \brief Definitions for multiple used defines such as ON or OFF.
*/
/*@{*/
/*! \ingroup opctl */
  #define PHCS_BFLOPCTL_VAL_OFF                   (0x00)    /*<! \brief Switches the chosen functionality off. */
  #define PHCS_BFLOPCTL_VAL_ON                    (0x01)    /*<! \brief Switches the chosen functionality on. */
/*@}*/

#endif /* PHCSBFLOPCTL_HW1ORDINALS_H */

/* E.O.F. */
