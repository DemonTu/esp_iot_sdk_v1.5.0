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
 * \file phcsBflIo.h
 *
 * Project: Object Oriented Library Framework I/O Component.
 *
 *  Source: phcsBflIo.h
 * $Author: mha $
 * $Revision: 1.7 $
 * $Date: Fri Jun 30 11:05:19 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  This is the file to glue together all I/O implementations for all different hardware variants
 *  for the C++ wrapper.
 *
 * History:
 *  GK:  Generated 12. March 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLIO_H
#define PHCSBFLIO_H


/* Type defintions */
#include <phcsBflStatus.h>

#include <phcsBflRegCtl.h>
#include <phcsBflAux.h>


/*! \name Operation mode
 *  \brief Operation mode to be specified for I/O: */
/*@{*/
/*! \ingroup rcio */
/*! \brief NFC-Initiator and Reader functionality configured. */
#define PHCS_BFLIO_INITIATOR  0x01
/*! \brief Target and Card functionality only configured. */
#define PHCS_BFLIO_TARGET     0x00
/*@}*/


/*! \ingroup rcio
 *  \brief Parameter structure for Transceive functionality.
 *
 *  The members of this structure reflect a simple
 *  transparent I/O channel through lower layers and finally the chip. The Transceive function (which
 *  takes this structure as its parameter) always waits for I/O completion (all uint8_ts received or
 *  error encountered (e.g. timeout). No overlapped Transceive operation is possible. The send- and
 *  receive-buffers may overlap or even be the same, no double-buffering is required.
 */
typedef struct
{
    uint8_t  *tx_buffer;          /*!< \brief [in] References data to transmit via the UART. The location
                                                  referenced must contain an initialised buffer holding the data
                                                  to transmit. The maximum size of the buffer depends on the
                                                  underlying system's capabilities regarding how many uint8_ts it can
                                                  transmit in one frame. */
    uint16_t  tx_buffer_size;     /*!< \brief [in] Number of uint8_ts to transmit. */
    uint8_t  *rx_buffer;          /*!< \brief [in] Already allocated memory space sufficiently large to receive
                                                  incoming data. \n
                                             [out] Data copied to the provided buffer. */
    uint16_t  rx_buffer_size;     /*!< \brief [in] Maximum return buffer size in uint8_ts. This member informs the
                                                  function about the size of the buffer for incoming data. The function
                                                  checkes the buffer boundaries and returns an error in case of too many
                                                  uint8_ts being returned by the underlying features. \n
                                             [out] Yields the number of uint8_ts actually returned. */
    uint16_t  rx_bits;            /*!< \brief [out] Number of bits returned (additional to uint8_ts).        */
    void     *self;               /*!< \brief [in] Pointer to the C-interface in order to let this
                                                  member function find its "object". Should be left
                                                  dangling when calling the C++ wrapper. */
} phcsBflIo_TransceiveParam_t;

/*! \ingroup rcio
 *  \brief Parameter structure for Transmit functionality. This structure reflects a transparent Transmit
 *  channel. The Transmit function returns after all uint8_ts have been sent.
 */
typedef struct
{
    uint8_t  *tx_buffer;          /*!< \brief [in] References data to transmit via the UART. The location
                                                  referenced must contain an initialised buffer holding the data
                                                  to transmit. The maximum size of the buffer depends on the
                                                  underlying system's capabilities regarding how many uint8_ts it can
                                                  transmit in one frame. */
    uint16_t  tx_buffer_size;     /*!< \brief [in] Number of uint8_ts to transmit. */
    void     *self;               /*!< \brief [in] Pointer to the C-interface in order to let this
                                                  member function find its "object". Should be left
                                                  dangling when calling the C++ wrapper. */
} phcsBflIo_TransmitParam_t;


/*! \ingroup rcio
 *  \brief Parameter structure for Receive functionality.
 *
 *  This structure reflects a transparent Receive
 *  channel. The Receive function enters wait mode and finally returns after all uint8_ts have been received
 *  or an error (e.g. timeout) occurs.
 */
typedef struct
{
    uint8_t  *rx_buffer;          /*!< \brief [in] Already allocated memory space sufficiently large to receive
                                                  incoming data. \n
                                             [out] Data copied to the provided buffer. */
    uint16_t  rx_buffer_size;     /*!< \brief [in] Maximum return buffer size in uint8_ts. This member informs the
                                                  function about the size of the buffer for incoming data. The function
                                                  checkes the buffer boundaries and returns an error in case of too many
                                                  uint8_ts being returned by the underlying features. \n
                                             [out] Yields the number of uint8_ts actually returned. */
    uint16_t  rx_bits;            /*!< \brief [out] Number of bits returned (additional to uint8_ts).        */
    void     *self;               /*!< \brief [in] Pointer to the C-interface in order to let this
                                                  member function find its "object". Should be left
                                                  dangling when calling the C++ wrapper. */
} phcsBflIo_ReceiveParam_t;


/*! \ingroup rcio
 *  \brief Parameter structure used to initialise the CB address and the user reference pointer in Components
 *  such as phcsBflIo_t.
 */
typedef struct
{
    pphcsBflAux_WaitEventCb_t  wait_event_cb;   /*!< \brief [in] Pointer to the system (user)-defined callback (CB) function
                                                  used for event detection. */
    void              *user_ref;        /*!< \brief [in] Pointer to a system (user)-defined location. Can be used
                                                  to let the CB-internal code access events, signals and the
                                                  like. */
    void              *self;            /*!< \brief [in] Pointer to the C-interface in order to let this
                                                  member function find its "object". Should be left
                                                  dangling when calling the C++ wrapper.  */
} phcsBflIo_SetWecParam_t;


/*! \ingroup rcio
 *  \brief
 *  Parameter structure for MF-Authent helper is equal to Transmit parameter. Since the
 *  Mifare (R)  function for Authentication is part of the RF chip's I/O capabilities, it must
 *  be handled here. The function takes the authentication parameters in the tx_buffer member.
 *  Composition of the data is done in the Mifare component.
 */
typedef phcsBflIo_TransmitParam_t phcsBflIo_MfAuthentParam_t;


/*  C-interface member function pointer types: */
/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIo_Wrapper< phcsBflIOcp >::Transceive(phcsBflIo_TransceiveParam_t *transceive_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIo_t::Transceive
 * \endif
 *
 * \ingroup rcio
 *  \par Parameters:
 *  \li \ref phcsBflIo_TransceiveParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This function directly uses the auxiliary function phcsBflAux_Hw1Command for initiator mode with
 * the PN51x command Transceive (b1100) in the command register. \n
 * For target mode this function is disabled and replaced by Transmit and Receive
 * which also use the transceive command for bit syncronicity. \n
 * The rx_buffer_size parameter sets the maximum receive data length in uint8_ts. Even if more data is
 * received, no buffer overflow may occure. \n
 * In combination with a transparent register read and write command set, this function enables a
 * completely transparent communication between two devices. The evalution of occured transmission
 * errors is done internally.
 *
 * \note TX and RX buffers may overlap or even be the same.
 * \note In case of an error, the appropriate error code is set. Nevertheless all received data
 *       during the RF-Communication is returned. This is done for debugging reasons.
 * \note Neither framing and speed nor timeout and CRC are modified by this function. These
 *       parameters should be set in advance.
 */
typedef phcsBfl_Status_t (*pphcsBflIo_Transceive_t) (phcsBflIo_TransceiveParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIo_Wrapper< phcsBflIOcp >::Transmit(phcsBflIo_TransmitParam_t *transmit_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIo_t::Transmit
 * \endif
 *
 * \ingroup rcio
 *  \par Parameters:
 *  \li \ref phcsBflIo_TransmitParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This function is devided into two parts. One for the initiator mode and one for the target mode. \n
 * In initiator mode the function phcsBflAux_Hw1Command is called and the transmit command of PN51x
 * is executed. No response is expected. \n
 * In target mode there is first a check if the transceive command is active, because sending without any
 * previous reception is forbidden in any protocol used. Then the function phcsBflAux_Hw1Command is
 * called using the transceive command of PN51x. The transmit command for target operation is never used!
 *
 * \note Neither framing and speed nor timeout and CRC are modified by this function. These
 *       parameters should be set in advance.
 */
typedef phcsBfl_Status_t (*pphcsBflIo_Transmit_t)   (phcsBflIo_TransmitParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIo_Wrapper< phcsBflIOcp >::Receive(phcsBflIo_ReceiveParam_t *receive_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIo_t::Receive
 * \endif
 *
 * \ingroup rcio
 *  \par Parameters:
 *  \li \ref phcsBflIo_ReceiveParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This function is devided into two parts. One for the initiator mode and one for the target mode. \n
 * In initiator mode the function phcsBflAux_Hw1Command is called and the receive command of PN51x
 * is executed. If no timeout is set and nothing is received this function never returns! \n
 * In target mode the function phcsBflAux_Hw1Command is called and the transceive command of PN51x
 * is executed. This functionis also re-setup so that reception is possible in any case. \n
 * In both modes the rx_buffer_size parameter sets the maximum receive data length in uint8_ts. Even if
 * more data is received, no buffer overflow may occure. \n
 *
 * \note In case of an error, the appropriate error code is set. Nevertheless all received data
 *       during the RF-Communication is returned. This is done for debugging reasons.
 * \note Neither framing and speed nor timeout and CRC are modified by this function. These
 *       parameters should be set in advance.
 */
typedef phcsBfl_Status_t (*pphcsBflIo_Receive_t)    (phcsBflIo_ReceiveParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIo_Wrapper< phcsBflIOcp >::MfAuthent(phcsBflIo_MfAuthentParam_t *mfauthent_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIo_t::MfAuthent
 * \endif
 *
 * \ingroup rcio
 *  \par Parameters:
 *  \li \ref phcsBflIo_MfAuthentParam_t [in]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This function is directly called from Mifare Authentication function where the data is prepared
 * and handled over using the tx_buffer. The function calls phcsBflAux_Hw1Command where the
 * Authentication command of PN51x is performed. If used separated from the Mifare (R) protocol
 * the data structure as defined in the data sheet of the PN51x has to be used!
 *
 * \note Neither framing and speed nor timeout and CRC are modified by this function. These
 *       parameters should be set in advance.
 */
typedef phcsBfl_Status_t (*pphcsBflIo_MfAuthent_t)  (phcsBflIo_MfAuthentParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP
 * \fn void phcs_BFL::phcsBflIo_Wrapper< phcsBflIOcp >::SetWaitEventCb(phcsBflIo_SetWecParam_t *set_wec_param)
 * \else
 * \fn void phcsBflIo_t::SetWaitEventCb
 * \endif
 *
 * \ingroup rcio
 *  \par Parameters:
 *  \li \ref phcsBflI3P3A_SetWecParam_t [in]: Pointer to the I/O parameter structure.
 *
 * \brief This function initializes the user defined callback (interrupt handler) which might be used
 * in the functions of this component.
 *
 */
typedef void (*pphcsBflIo_SetWaitEventCb_t) (phcsBflIo_SetWecParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 *
 *  \ingroup rcio
 *  \brief C - Component interface: Structure to call RC/IO functionality:
 *
 *  The I/O component is responsible for all transparent I/O operations and other I/O commands of
 *  the RF chip. The layer of this component is the topmost of the hardware-depending layers. This
 *  component is required by protocols (e.g. NFC, Mifare, ISO 14443.4) to perform I/O operations.
 *
 * \endif
 */
typedef struct
{
    /* Methods: */
    pphcsBflIo_SetWaitEventCb_t   SetWaitEventCb;     /* Event detection CB address submission. */

    pphcsBflIo_Transceive_t       Transceive;         /* Transceive member function pointer. */
    pphcsBflIo_Transmit_t         Transmit;           /* Transmit member function pointer. */
    pphcsBflIo_Receive_t          Receive;            /* Receive member function pointer. */
    pphcsBflIo_MfAuthent_t        MfAuthent;          /* MfAuthent member function pointer. */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */

    #ifdef PHFL_BFL_CPP
        void *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                     C++ object, calling into the C-interface. */
    #endif

    /* User reference. RCL_WAIT_EVENT_CB receives this value with the parameter
       structure RCL_WAIT_EVENT_CB_PARAM. */
    void *mp_UserRef;
    /* Pointer to the event-detecting callback function, to be implemented by the embedding SW. */
    pphcsBflAux_WaitEventCb_t mp_WaitEventCB;
    /* Lower edge. */
    phcsBflRegCtl_t *mp_Lower;
} phcsBflIo_t;






/* //////////////////////////////////////////////////////////////////////////////////////////////
// Initialise functions for PN51x:
*/
/*! \ingroup rcio
 *  \brief Control variable structure to differ between Initiator and Target access.
 */
typedef struct
{
    /*! \brief Differs between Initiator/Reader or Target/Card access. */
    uint8_t  m_InitiatorNotTarget;
} phcsBflIo_Hw1Params_t;

/*!
* \ingroup rcio
* \param[in] *cif                   C object interface structure.
* \param[in] *rp                    Pointer to the internal control variables structure.
* \param[in] *p_lower               Pointer to the underlying layer, in this case register control.
* \param[in]  initiator__not_target Specifier for mode whether to support Initiator or Target.
*
* This function shall be called first to initialise the IO component. There the C-Layer, the internal
* variables, the underlaying layer and the device mode are initialised.
*
*/
void          phcsBflIo_Hw1Init(phcsBflIo_t     *cif,
                              void          *rp,
                              phcsBflRegCtl_t *p_lower,
                              uint8_t        initiator__not_target);



#endif /* PHCSBFLIO_H */

/* E.O.F. */
