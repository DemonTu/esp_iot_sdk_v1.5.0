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
 * \file phcsBflPolAct.h
 *
 * Project: Object Oriented Library Framework Passive Polling Activation Component.
 *
 *  Source: phcsBflPolAct.h
 * $Author: mha $
 * $Revision: 1.4 $
 * $Date: Fri Jun 30 11:05:23 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  This is the file to glue together all Passive Polling implementations for all different hardware 
 *  variants for the C++ wrapper.
 *
 * History:
 *  MHa: Generated 07. May 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */

#ifndef PHCSBFLPOLACT_H
#define PHCSBFLPOLACT_H


/* Type defintions */
#include <phcsBflStatus.h>

#include <phcsBflRegCtl.h>
#include <phcsBflAux.h>


/*! \name Operation mode 
 * Operation mode to be specified for Passive Polling Activation: */
/*@{*/
/*! \ingroup felpol */
/*! \brief Support NFC-Initiator and Reader functionality. */
#define PHCS_BFLPOLACT_INITIATOR  0x01 
/*! \brief Target and Card functionality only supported.   */
#define PHCS_BFLPOLACT_TARGET     0x00 
/*@}*/


/*! \ingroup felpol
 *  \brief Parameter structure for FeliCa Polling functionality.
 */
typedef struct 
{
    uint8_t  *tx_buffer;       /*!< \brief [in] References data to transmit in following order: \n
                                               bytes 0,1: System Code information (0xFFFF: all system codes) \n
                                               byte  2:   Reserved (0x01 indicated System Code requested) \n
                                               byte  3:   Time Slot number.                                  */
    uint16_t  tx_buffer_size;  /*!< \brief [in] Number of bytes to transmit.                      */
    uint8_t  *rx_buffer;       /*!< \brief [out] References data received from Target(s) in following order: \n
                                                byte  00   : length byte of first received polling response data stream (either 17 or 19 bytes)\n
                                                bytes 01-08: IDm of Target 0 \n
                                                bytes 09-16: PMm of Target 0 \n
                                                bytes 17-18: System Code (if responded (length will be 21))          */
    uint16_t  rx_buffer_size;  /*!< \brief [in] Maximum Length of receive buffer. For outside buffer handling 
                                               buffer size shall be at least 21 bytes 
                                               (length, command, IDm, PMm, (system code), error byte from PN51x) \n
                                          [out] Number of correct bytes received (always multiple of 18). */
    void     *self;            /*!< \brief [in] Pointer to the C-interface in order to let this
                                               member function find its "object". Should be left
                                               dangling when calling the C++ wrapper.               */
} phcsBflPolAct_PollingParam_t;


/*! \ingroup felpol
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
} phcsBflPolAct_SetWecParam_t;


/* C-interface member function pointer types: */

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflPolAct_Wrapper< phcsBflFPcp >::Polling(phcsBflPolAct_PollingParam_t *polling_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflPolAct_t::Polling
 * \endif
 *
 * \ingroup felpol
 *  \par Parameters:
 *  \li \ref phcsBflPolAct_PollingParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This function handles the felica polling procedure which is the same as the activation procedure for passive 
 * communication mode at 212 and 424 kbps as specified in ISO18092. \n First RxMultiple functionality of PN51x is 
 * activated. Then the data is prepared as described in the specification and sent using phcsBflAux_Hw1Command.\n
 * The response data is analysed. If anythig was correct, the complete data steam is handed over to the 
 * calling procedure (even length and command). So the response data stream looks like LEN, CMD, Data. Only the CRC
 * information is skipped. \n
 * If there was an error, the errorneous frame is not handed over to the caller but the error status is 
 * returned to the calling procedure. If there is more than one valid response all correct data frames are handed
 * over to the caller. 
 * 
 * \note Timeout handling has to be done by the caller!! The timer shall always be set in the way that all possible 
 *       responses may be done during the specified time. In any other case only the first response might be read.
 */
typedef phcsBfl_Status_t (*pphcsBflPolAct_Polling_t) (phcsBflPolAct_PollingParam_t*);


/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflPolAct_Wrapper< phcsBflFPcp >::SetWaitEventCb(phcsBflPolAct_SetWecParam_t *set_wec_param)
 * \else
 * \fn void phcsBflPolAct_t::SetWaitEventCb
 * \endif
 *
 * \ingroup felpol
 *  \par Parameters:
 *  \li \ref phcsBflPolAct_SetWecParam_t [in]: Pointer to the I/O parameter structure.
 *
 * \brief This function initializes the user defined callback (interrupt handler) which might be used
 * in the functions of this component.
 *
 */
typedef void (*pphcsBflPolAct_SetWaitEventCb_t) (phcsBflPolAct_SetWecParam_t*);



/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup felpol
 *  \brief C - Component interface: Structure to call FeliCa Polling functionality.
 *  This component provides FeliCa card / NFC FeliCa mode Target acquisition.
 * 
 * \endif
 */
typedef struct 
{
    /* Methods: */
    pphcsBflPolAct_SetWaitEventCb_t  SetWaitEventCb;  /* Event detection CB address submission. */
    pphcsBflPolAct_Polling_t         Polling;         /* Polling member function pointer. */
    
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
    /* Lower edge: */
    phcsBflRegCtl_t *mp_Lower;
} phcsBflPolAct_t;





/* //////////////////////////////////////////////////////////////////////////////////////////////
// Initialise functionality for PN51x:
*/
/*! \ingroup felpol
 *  \brief Control variable structure to differ between Initiator and Target access.
 */
typedef struct 
{
    /*! \brief Differs between Initiator/Reader or Target/Card access. */
    uint8_t  m_InitiatorNotTarget; 
} phcsBflPolAct_Hw1Params_t;

/*!
* \ingroup felpol
* \param[in,out] *cif                   C object interface structure
* \param[in,out] *rp                    Pointer to the internal control variables structure.
* \param[in]     *p_lower               Pointer to the underlying layer's function.
* \param[in]      initiator__not_target Specifier for mode whether to support Initiator or Target.
* 
* This function shall be called first to initialise the FeliCa Polling component. There the 
* C-Layer, the internal variables, the underlaying layer and the device mode are initialised.
* An own function pointer is typedef'ed for this function to enable the call within a generic 
* C++ I/O wrapper. For the C++ wrapper see the file INCFELICAPOLLING.H .
*
*/
void          phcsBflPolAct_Hw1Init(phcsBflPolAct_t     *cif,
                                  void              *rp,
                                  phcsBflRegCtl_t     *p_lower,
                                  uint8_t            initiator__not_target);



#endif /* PHCSBFLPOLACT_H */

/* E.O.F. */
