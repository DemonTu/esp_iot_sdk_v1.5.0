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
 * \file phcsBflI3P3A.h
 *
 * Project: Object Oriented Library Framework phcsBfl_I3P3A Component.
 *
 *  Source: phcsBflI3P3A.h
 * $Author: mha $
 * $Revision: 1.4 $
 * $Date: Fri Jun 30 11:05:18 2006 $
 *
 * Comment:
 *  This is the file to glue together all phcsBfl_I3P3A implementations for all different hardware 
 *  variants for the C++ wrapper.
 *
 * History:
 *  MHa: Generated 13. May 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */

#ifndef PHCSBFLI3P3A_H
#define PHCSBFLI3P3A_H

#include <phcsBflRegCtl.h>
#include <phcsBflAux.h>


/*! \name Operation modes
 *  \brief Operation mode to be specified for phcsBfl_I3P3A: */
/*@{*/
/*! \ingroup iso14443_3 */
/*! \brief Support NFC-Initiator and Reader functionality. */
#define PHCS_BFLI3P3A_INITIATOR  0x01 
/*! \brief Target and Card functionality only supported.   */
#define PHCS_BFLI3P3A_TARGET     0x00 
/*@}*/
                                       

/*! \name Request codes */
/*@{*/
/*! \ingroup iso14443_3 */
/*! \brief Request code for all devices. */
#define PHCS_BFLI3P3A_REQALL     0x52 
/*! \brief Request code only for idle devices. */
#define PHCS_BFLI3P3A_REQIDL     0x26 
/*@}*/

/*! \ingroup iso14443_3 
    \brief 
    Specifies the LSB in the flags byte for collision detection.
    This flag is set if there is a colission durng activation.
*/
#define PHCS_BFLI3P3A_COLL_DETECTED		0x01


/*! \ingroup iso14443_3
 *  \brief Parameter structure for Request functionality. The RequestA function issues a REQA or WUPA request,
 *  depending on the req_code parameter.
 */
typedef struct 
{
    uint8_t  req_code;            /*!< \brief [in] Request code according to ISO 14443-3. Possible
                                                  values are ISO14443_3_REQIDL (0x26) and ISO14443_3_REQALL (0x52). */
    uint8_t *atq;                 /*!< \brief [in] Already allocated buffer of two bytes (16 bits). \n 
                                              [out] ATQA (Answer to Request type A), received from PICC (16 bit). 
                                                   atq[0] ... LSB, atq[1] ... MSB */
    void          *self;          /*!< \brief [in] Pointer to the C-interface of this component in order 
                                                  to let this member function find its "object". 
                                                  Remark: Should be left dangling when calling the 
                                                  C++ wrapper. */
} phcsBflI3P3A_ReqAParam_t;


/*! \ingroup iso14443_3
 *  \brief Parameter structure for Anticollission/Select functionality. The AnticollSelect function which
 *  takes this parameter performs the acquisition loop according to ISO 14443-3.
 */
typedef struct 
{
    uint8_t *uid;           /*!< \brief [in] Buffer for up to 3 cascade levels of serial number. 
                                             \n <b>Note:</b> All unused bytes have to be initialised with 0 for correct results. \n 
                                        [out] received serial number from PICC. */
    uint8_t  uid_length;    /*!< \brief [in] number of known (valid) bits to send. \n 
                                        [out] length of (U)ID in bits. Also the cascaded byte (0x88) is included. */
    uint8_t  sak;           /*!< \brief [out] Select Acknowledge byte from the Card/Target 
                                             (if cascaded, only last SAK byte is returned). */
	uint8_t  flags;			/*!< \brief [out] contains additional information about the anticollision process
							                  bit0: If 1, a collision has been detected (but resolved) */
    void    *self;          /*!< \brief [in] Pointer to the C-interface of this component in order 
                                             to let this member function find its "object". 
                                             \n <b>Note:</b> Should be left dangling when calling the C++ wrapper.  */
} phcsBflI3P3A_AnticollSelectParam_t;

/*! \ingroup iso14443_3
 *  \brief Parameter structure for Halt functionality.
 *
 *  The HaltA function moves an ISO 14443-3 Type A PICC out of protocol in Initator/Reader mode and checks if the 
 *  incoming data according to Halt format in Target/Card mode.
 */
typedef struct 
{
    uint8_t  *buffer;             /*!< \brief [in] Pointer to data buffer for Target/Card function of Halt.
                                                  For Initiator/Reader mode this parameter shall be set to NULL. */
    uint16_t  buffer_length;      /*!< \brief [in] Length of incoming buffer to check for Target/Card mode.
                                                  For Initiator/Reader mode this parameter shall be set to 0. */
    void     *self;               /*!< \brief [in] Pointer to the C-interface of this component in order 
                                                  to let this member function find its "object". 
                                                  Remark: Should be left dangling when calling the C++ wrapper.  */
} phcsBflI3P3A_HaltAParam_t;

/*! \ingroup iso14443_3 
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
} phcsBflI3P3A_SetWecParam_t;

/*! \ingroup iso14443_3 
 *  \brief Parameter structures for Anticollission/Select and Select only are the same. */
typedef phcsBflI3P3A_AnticollSelectParam_t phcsBflI3P3A_SelectParam_t;

/* C-interface member function pointer types: */
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P3A_Wrapper< phcsBflIsoAcp >::RequestA(phcsBflI3P3A_ReqAParam_t *request_a_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P3A_t::RequestA
 * \endif
 *
 * \ingroup iso14443_3 
 *  \par Parameters:
 *  \li \ref phcsBflI3P3A_ReqAParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This command handles the Request procedure of ISO14443-3. Depending on the Request Code (ISO14443_3_REQALL or 
 * ISO14443_3_REQIDL) and the state of the cards in the field all cards reply with their Tag-Type synchronously. 
 * The time between end of the Request command and start of reply of the card is exactly 8 * 9.44 us long. The 
 * Tag-Type field is 16 bits long and only one bit out of 16 is set. \n
 * Double and Triple serial numbers are possible. \n
 *
 * Relevant bit positions LSByte:  \n
 *	 [8..7] UID size   \n
 *	 \n 00 standard 32 bit long UID \n
 *	 \n 01 UID size double (56 bit long) \n
 *	 \n 10 UID size triple (80 bit long) \n 
 *	 [5..1] if any bit is set, frame anticollision is supported;  tag type recognition \n
 * The complete MSByte is RFU. \n
 *
 * \note In case of an error, the appropriate error code is set. Nevertheless all received data 
 *       during the RF-Communication is returned for debugging reasons.
 * \note Future cards will work also with other request codes.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflI3P3A_RequestA_t)            (phcsBflI3P3A_ReqAParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P3A_Wrapper< phcsBflIsoAcp >::AnticollSelect(phcsBflI3P3A_AnticollSelectParam_t *AnticollSelect_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P3A_t::AnticollSelect
 * \endif
 *
 * \ingroup iso14443_3 
 *  \par Parameters:
 *  \li \ref phcsBflI3P3A_AnticollSelectParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This command combines the ISO14443-3 functions of anticollision and select. The functionality is split up into two 
 * independent internal procedures. One to perform the anticollision, the other one to perform the select.
 * The cascade level is automatically increased if the Cascade Tag for a further level is received.
 * The checkbyte is verified, but not stored in the buffer. \n
 * The sel_lv1_code contains the select code of cascade level 1. This is 0x93 for ISO14443-3 compatible devices. The 
 * select codes for cascade level 2 and 3 are chosen automatically according to the information in the SAK byte received. \n
 * The uid references a buffer which may contain the known part of the uid when the function is called.
 * When the function returns to the calling procedure, this buffer contains the received serial number of 
 * the target device. The length is according to the cascade level of the ID. The indicator of another 
 * cascade level (0x88) is not removed and also stored in the buffer. \n
 * The uid_length parameter is the bit count (!) of the ID. As input it is the number of known bits,
 * as output it shows the ID length, in bits, which is always a multiple of 32. \n
 * At the end this function selects a card by the specified serial number. All other cards in the field fall back 
 * into the idle mode and they are not longer involved during the further communication. 
 *
 * \note In case of an error, the appropriate error code is set. Nevertheless all received data 
 *       during the RF-Communication is returned for debugging reasons.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflI3P3A_AnticollSelect_t) (phcsBflI3P3A_AnticollSelectParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P3A_Wrapper< phcsBflIsoAcp >::Select(phcsBflI3P3A_SelectParam_t *select_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P3A_t::Select
 * \endif
 *
 * \ingroup iso14443_3 
 *  \par Parameters:
 *  \li \ref phcsBflI3P3A_SelectParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This command only performs the select of a known device. For detailed information on the 
 * parameters see the decription of \ref phcsBflI3P3A_SelectParam_t command. \n
 * The sel_lv1_code is the first command to be sent to the passive device.  \n
 * The uid contains the ID of the device. All 3 cascade levels can be transferred at once to the 
 * function. The uid_length parameter defines whether the ID is single, double or triple. \n
 * At the end this function selects a card by the specified serial number. All other cards in the field fall back 
 * into the idle mode and they are not longer involved during the further communication. 
 *
 * \note In case of an error, the appropriate error code is set. Nevertheless all received data 
 *       during the RF-Communication is returned. This is done for debugging reasons.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflI3P3A_Select_t)              (phcsBflI3P3A_SelectParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P3A_Wrapper< phcsBflIsoAcp >::HaltA(phcsBflI3P3A_HaltAParam_t *halt_a_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P3A_t::HaltA
 * \endif
 *
 * \ingroup iso14443_3 
 *  \par Parameters:
 *  \li \ref phcsBflI3P3A_HaltAParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 * This function handles the halt command for ISO14443-3 in both the reader and the card mode. \n
 * In reader operating mode, this function sets a MIFARE (R) Classic compatible card into the halt state. 
 * Having send the command to the card, the function does not expect a cards response. Only in case of any 
 * error the card sends back a NACK. If the command was successful, the card does not return with an ACK. 
 * Thus, the function is successful, if a timeout is indicated. This is handled internally.\n
 * In card operating mode this functions evaluates the data received, sets the Halt flag of the PN51x and
 * starts again the automatic anticolission procedure.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflI3P3A_HaltA_t)               (phcsBflI3P3A_HaltAParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflI3P3A_Wrapper< phcsBflIsoAcp >::SetWaitEventCb(phcsBflI3P3A_SetWecParam_t *set_wec_param)
 * \else
 * \fn void phcsBflI3P3A_t::SetWaitEventCb
 * \endif
 *
 * \ingroup iso14443_3 
 *  \par Parameters:
 *  \li \ref phcsBflI3P3A_SetWecParam_t [in]: Pointer to the I/O parameter structure.
 *
 * \brief This function initializes the user defined callback (interrupt handler) which might be used
 * in the functions of this component.
 *
 */
typedef void (*pphcsBflI3P3A_SetWaitEventCb_t) (phcsBflI3P3A_SetWecParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup iso14443_3
 *  \brief C - Component interface: Structure to call ISO 14443 Part 3 Type A functionality.
 *
 *  ISO 14443.3 functionality represents PICC Initialisation and Anticollision as well as Halt.
 * 
 * \endif
 */
typedef struct 
{
    /* Methods: */
    pphcsBflI3P3A_RequestA_t               RequestA;               /* RequestA member function pointer. */
    pphcsBflI3P3A_AnticollSelect_t         AnticollSelect;    /* AnticollSelect member function pointer. */
    pphcsBflI3P3A_Select_t                 Select;                 /* Select member function pointer. */
    pphcsBflI3P3A_HaltA_t                  HaltA;                  /* HaltA member function pointer. */

    pphcsBflI3P3A_SetWaitEventCb_t         SetWaitEventCb;         /* Event detection CB address submission. */

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
    /* Point to the lower I/O device: */
    phcsBflRegCtl_t *mp_Lower;
} phcsBflI3P3A_t;





/* //////////////////////////////////////////////////////////////////////////////////////////////
// Initialise functions for PN51x:
*/
/*! \ingroup iso14443_3
 *  \brief Control variable structure to differ between Initiator and Target access.
 */
typedef struct 
{
    /*! \brief Differs between Initiator/Reader or Target/Card access. */
    uint8_t  m_InitiatorNotTarget;
} phcsBflI3P3A_Hw1Params_t;

/*!
* \ingroup iso14443_3
* \param[in,out] *cif                   C object interface structure
* \param[in,out] *rp                    Pointer to the internal control variables structure.
* \param[in]     *p_lower               Pointer to the underlying layer's function.
* \param[in]      initiator__not_target Specifier for mode whether to support Initiator or Target.
* 
* This function shall be called first to initialise the ISO14443-3 component. There the C-Layer, 
* the internal variables, the underlaying layer and the device mode are initialised.
* An own function pointer is typedef'ed for this function to enable the call within a generic C++ 
* phcsBfl_I3P3A wrapper. For the C++ wrapper see the file INCISO14443_3.H.
*
*/
void          phcsBflI3P3A_Hw1Initialise(phcsBflI3P3A_t    *cif,
                                       void            *rp,
                                       phcsBflRegCtl_t   *p_lower,
                                       uint8_t          initiator__not_target);



#endif /* PHCSBFLI3P3A_H */

/* E.O.F. */
