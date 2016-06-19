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
 * \file phcsBflAux.h
 *
 * Project: Object Oriented Library Framework Auxilliary Component.
 *
 *  Source: phcsBflAux.h
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 30 11:05:18 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  MHa: Generated September 2005
 *
 * \endif
 */


#ifndef PHCSBFLAUX_H
#define PHCSBFLAUX_H

#include <phcsBflRegCtl.h>


/*! 
 *  \ingroup aux
 *   Macro definition for error handling of underlaying layer and exit.
 *
 */
#define PHCS_BFLAUX_CHECK_SUCCESS(result) { status = (result); if(status != PH_ERR_BFL_SUCCESS) {return status;} }


/*! \name Operation mode 
 *  \brief Operation mode to be specified for I/O: */
/*@{*/
/*! \ingroup rcio */
/*! \brief NFC-Initiator and Reader functionality configured. */
#define PHCS_BFLAUX_INITIATOR  0x01 
/*! \brief Target and Card functionality only configured. */
#define PHCS_BFLAUX_TARGET     0x00 
/*@}*/


/*! \ingroup aux
 *  This structure describes the interface to the callback function, used for interrupt event
 *  detection. Components such as e.g. phcsBflIo_t use the callback function as an underlying feature
 *  to avoid continuous polling for events.
 *  \note The phcsBflIo_t for PN51x does not directly use the CB. It is the underlying RcAux feature
 *        which consists of commonalities between e.g. phcsBflIo_t, phcsBflOpCtl_t, phcsBflI3P3A_t or
 *        phcsBflPolAct_t.
 */
typedef struct 
{
    uint8_t *cb_data;               /*!< \brief [in/out] Data to be handed over to the pphcsBflAux_WaitEventCb_t function
                                                         (embedding system-defined). The implementation of this
                                                         function strongly depends on the embedding system,
                                                         the hardware used (PN51x or a later successor) and
                                                         the implementation of RcAux. Therefore the encoding
                                                         of this parameter cannot be standardised. CAUTION: 
                                                         The encoding of this parameter is different for each
                                                         implementation of the RCL/BFL. */
    uint8_t   cb_data_length;       /*!< \brief [in/out] The length of the cb_data member buffer. Specified by the
                                                         calling RcAux function, modified by the system-defined CB. */
    uint8_t  *commIrqEn;            /*!< \brief [in] CommIEn register as set during configuration. */
    uint8_t  *divIrqEn;             /*!< \brief [in] DivIEn register as set during configuration. */
    uint8_t  *waitForComm;          /*!< \brief [in] CommIrq register bits to wait for abort condition. */
    uint8_t  *waitForDiv;           /*!< \brief [in] DivIrq register bits to wait for abort condition. */
    uint8_t   command;              /*!< \brief [in] Command as defined in Command register plus additional 
                                                     command PHCS_BFL_CMD_TRANSCEIVE_TO for phcsBflPolAct_Wrapper functionality. */    
    uint8_t   initiator_not_target; /*!< \brief [in] Differentiates between Initiator or Target(0) functionality. */    
    uint8_t   target_send;          /*!< \brief [in] Shows in target mode the intention to send data in transceive mode,
                                                     not data to be received. */    
    phcsBflRegCtl_t  *lower;        /*!< \brief [in] Pointer to the underlying RcRegCtl Component. */
    void          *user_ref;        /*!< \brief [in] Pointer to a system (user)-defined location. Can be used
                                                     to let the CB-internal code access events, signals and the
                                                     like. */
} phcsBflAux_WaitEventCbParam_t;
 

/*! \ingroup aux
 *  Components such as e.g. phcsBflIo_t use the callback function as an underlying feature
 *  to avoid polling. The address of the function (implemented by the embedding system but
 *  following the definition) must be handed over to the e.g. phcsBflIo_t Component. The internal
 *  functionality of the CB is nothing but waiting for interrupts/events triggerd by PN51x.
 */
typedef phcsBfl_Status_t (*pphcsBflAux_WaitEventCb_t) (phcsBflAux_WaitEventCbParam_t*);




/*! \ingroup aux
 *  \brief Struct definition for a communication channel between caller and help function 
 */
typedef struct 
{
  phcsBflRegCtl_t     *lower;            /*!< \brief [in] Pointer to lower component.                           */
  pphcsBflAux_WaitEventCb_t wait_event_cb;    /*!< \brief [in] Pointer to the Wait For Event Callback function.      */
  void             *user_ref;         /*!< \brief [in] Pointer to an application defined location required
                                                by the pphcsBflAux_WaitEventCb_t to access system API etc.    */
  uint8_t     cmd;              /*!< \brief [in] command code                                          */
  uint8_t     initiator;        /*!< \brief [in] Initiator mode configuration                          */
  uint8_t     target_send;      /*!< \brief [in] In target mode configuration differs between 
                                                send and receive mode for transceive.                 */
  uint8_t    *tx_buffer;        /*!< \brief [in] References data to transmit via the UART.             */
  uint16_t    tx_bytes;         /*!< \brief [in] Number of bytes to transmit. if there are only 
                                                bits to transmit, set length to full byte             */
  uint8_t    *rx_buffer;        /*!< \brief [in] Place where to put received bytes.\n [out] data.         */
  uint16_t    rx_bytes;         /*!< \brief [in] Maximum return buffer size[bytes]. \n
                                           [out] Number of bytes returned.                             */
  uint16_t    rx_bits;          /*!< \brief [out] Number of bits returned (additional to bytes).        */
} phcsBflAux_CommandParam_t;







/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Auxilliary functions for dedicated Hardware - first implemetation: PN51x:
*/

/*!
* \ingroup aux
* \param[in,out] *command_param       Pointer to the I/O parameter structure.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_ACK_SUPPOSED           Single byte received, CRC error detected, might be an ACK/NACK.
* \retval  PH_ERR_BFL_BUFFER_OVERFLOW        Buffer too small that all data can written.
* \retval  PH_ERR_BFL_COLLISION_ERROR        Collision detected during receive procedure.
* \retval  PH_ERR_BFL_CRC_ERROR              Hardware detected wrong CRC.
* \retval  PH_ERR_BFL_TEMP_ERROR             Hardware detected temperature error.
* \retval  PH_ERR_BFL_UNSUPPORTED_COMMAND    The device does not support the command chosen.
* \retval  PH_ERR_BFL_PARITY_ERROR           Hardware detected wrong Parity.
* \retval  PH_ERR_BFL_PROTOCOL_ERROR         Protocol error as defined in the hardware register settings.
* \retval  PH_ERR_BFL_RF_ERROR               Hardware detected wrong RF-field handling.
* \retval  PH_ERR_BFL_ERROR_NY_IMPLEMENTED   Path has been chosen which is currently not implemented.
* \retval  Other                             phcsBfl_Status_t values depending on the underlying components.
* 
* This module handles the common part of the send and receive functionality. It also does the filling
* of the FIFO of the PN51x and does all the error checking for the response data. It is used by all
* components of the hardware dependent layer of the BFL. \n
* For this module there is only the C-Interface existing. It shall be called directly if really necessary.
* For standard operation it shall not be necessary that this component has to be used by the caller!
*
* \note TX and RX buffers may overlap or even be the same.
* \note In case of an error, the appropriate error code is set. Nevertheless all received data 
*       during the RF-Communication is returned. This is done for debugging reasons.
* \note Neither framing and speed nor timeout and CRC are modified by this function. These 
*       parameters should be set in advance.
* 
*/
phcsBfl_Status_t phcsBflAux_Hw1Command(phcsBflAux_CommandParam_t *command_param);




#endif /* PHCSBFLAUX_H */

/* E.O.F. */
