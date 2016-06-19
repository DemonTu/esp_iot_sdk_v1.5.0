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
 * \file phcsBflI3P4.h
 *
 * Project: ISO14443-4 basic functionality and protocol flow implementation.
 *
 * Workfile: phcsBflI3P4.h
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Tue Feb 14 10:43:14 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  The functios declared here are used within the C and C++ of the implementation. When used by the
 *  C variant these functions, providing already full protocol facilities, are called directly. When
 *  used within a C++ environment the functions are encapsulated by a class which provides
 *  additional features such as multi-device capabilities per application.
 *
 * History:
 *  GK:  Generated 9. Sept. 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLI3P4_H
#define PHCSBFLI3P4_H

#include <phcsBflI3P4_Common.h>
#include <phcsBflIo.h>

/*! \ingroup iso14443_4
 *  \brief This bit instructs the EXCHANGE function to accept a further filled user buffer for data
 *         transfer to the PICC.
 *
 *  If set, the function continues chaining to the PICC with the
 *  following call. This makes two or more subsequent calls appear as one from the PICC's
 *  point of view. OR this value into the flags member of EXCHANGE_PARAM.
 */
#define PHCS_BFLI3P4_CONT_CHAINING    (0x01)                                       /* */
/*! \ingroup iso14443_4
 *  \brief This bit indicates an error during waiting time extension handling. Seems the callback
 *         has not been set before!
 */
#define PHCS_BFLI3P4_FLAG_CB          (0x02)                                       /* */




/*! \ingroup iso14443_4
 *  \brief Parameter structure for Data Exchange functionality.
 *  The function for data exchange in the ISO/IEC 14443.4 protocol takes a pointer to
 *  this structure. The buffers for sending and receiving may overlap or even be the same.
 */
typedef struct
{
    uint8_t  *nad_send;       /*!< \brief [in] NAD to send from the PCD to the PICC. */
    uint8_t  *snd_buf;        /*!< \brief [in] A large (64k) buffer which must be
                                              pre-allocated and initialised with the data to send. */
    uint32_t  snd_buf_len;    /*!< \brief [in] Number of data bytes to send to the PICC. */
    uint8_t  *nad_receive;    /*!< \brief [out] NAD received from the Target, if used. */
    uint8_t  *rec_buf;        /*!< \brief [out] A large (64k) buffer which must be
                                               pre-allocated is the destination for data to receive. */
    uint32_t  rec_buf_len;    /*!< \brief [in] Number of bytes available in the receive buffer: The
                                              function checks this value against the number of bytes being
                                              received. In case of insufficient space the function breaks
                                              and returns an error. \n
                                         [out] Number of bytes received from the PICC. */
    uint8_t   flags;          /*!< \brief [in,out] Contains flags to control the behaviour of Exchange. The
                                                  flag masks are defined in phcsBflI3P4.h. */
    void     *self;           /*!< \brief [in] Pointer to the C-interface of this component in
                                                  order to let this member function find its "object".
                                                  Should be left dangling when calling the C++ wrapper.*/
} phcsBflI3P4_ExchangeParam_t;


/*! \ingroup iso14443_4
 *  \brief Parameter structure for Deselect functionality.
 *  This function issues a DESELECT command.
 */
typedef struct
{
    void          *self;   /*!< \brief [in] Pointer to the C-interface of this component in
                                     order to let this member function find its "object".
                                     Should be left dangling when calling the C++ wrapper.*/
} phcsBflI3P4_DeselectParam_t;


/*! \ingroup iso14443_4
 *  \brief Parameter structure for Presence Check functionality. Has the same parameters as the DESELECT
 *  command, only the self pointer.
 */
typedef struct
{
    void          *self;   /*!< \brief [in] Pointer to the C-interface of this component in
                                     order to let this member function find its "object".
                                     Should be left dangling when calling the C++ wrapper.*/
} phcsBflI3P4_PresCheckParam_t;



/*! \ingroup iso14443_4
 *  \brief Parameter structure for the SET WTX CALLBACK function.
 *  The callback function expects a pointer to this parameter.
 */
typedef struct
{
    void       *object;      /*!< \brief [in] Pointer to reference the current object: \n
                                          Using pure C only, this parameter holds the address
                                          of the C-structure instance of the protocol. \n
                                          Using C++ however, this parameter points to the C++
                                          object, embedding the C-part. */
    void       *p_user_wtx;  /*!< \brief [in] Pointer to a location which is user (embedding-software) defined.
                                          See interface function SetPUser .*/
    uint8_t     set;         /*!< \brief [in] uint_fast8_t value: If set it indicates that the value specified
                                          in the <em>wtxm</em> parameter member shall be applied.\n
                                          If zero, the timeout shall be reset to the default and
                                          the value in <em>wtxm</em> is not valid. */
    uint8_t     wtxm;        /*!< \brief [in] WTXM, encoded as specified by  ISO 14443-4. This value
                                          has to be applied according to ISO 14443-4 to the reader
                                          by the application-defined callback function. */
    uint8_t     fwi;         /*!< \brief [in] Frame waiting time-i: This value has to be applied according
                                          to ISO 14443-4 to the reader by the application-defined
                                          callback function. */
} phcsBflI3P4_CbSetWtxParam_t;


/*! \ingroup iso14443_4
 *  \brief Parameter structure for SET BITRATE CALLBACK functionality.
 *  The callback function expects a pointer to this parameter.
 */
typedef struct
{
    void       *object;               /*!< \brief [in] Pointer to reference the current object: \n
                                                   Using pure C only, this parameter holds the address
                                                   of the C-structure instance of the protocol. \n
                                                   Using C++ however, this parameter points to the C++
                                                   object, embedding the C-part. */
    void       *p_user_reader_param; /*!< \brief [in] Pointer to a location which is user (embedding-software) defined.
                                                   See interface function SetPUser .*/
    uint8_t     dri;                  /*!< \brief [in] Divisor Recive Integer, as specified by ISO/IEC 14443-4,
                                                   determining the bitrates used from PICC to PCD. */
    uint8_t     dsi;                  /*!< \brief [in] Divisor Send Integer, as specified by ISO/IEC 14443-4,
                                                   determining the bitrates used from PCD to PICC. */
    uint8_t     fwi;                  /*!< \brief [in] Frame waiting time-i: This value has to be applied according
                                                   to ISO 14443-4 to the reader by the application-defined
                                                   callback function. */

} phcsBflI3P4_CbSetReaderParam_t;

/*! \ingroup iso14443_4
 *  \brief Parameter structure for SET_P_USER functionality.
 *  The function for setting user pointer in the ISO/IEC 14443.4 protocol takes a pointer to
 *  this structure.
 */
typedef struct
{
    void           *p_user_wtx;           /*!< \brief [in] Pointer to a location which is user (embedding-software) defined.
                                                    See interface function SetPUser .*/
    void           *p_user_reader_param;  /*!< \brief [in] Pointer to a location which is user (embedding-software) defined.
                                                    See interface function SetPUser .*/
    void           *self;                 /*!< \brief [in] Pointer to the C-interface of this component in
                                                    order to let this member function find its "object".
                                                    Should be left dangling when calling the C++ wrapper.*/

}phcsBflI3P4_SetPUserParam_t;


/*! \ingroup iso14443_4
 *  \brief Parameter structure for SET PROTOCOL functionality.
 *  The function for setting protocol parameters in the ISO/IEC 14443.4 protocol takes a pointer to
 *  this structure.
 */
typedef struct
{
	uint8_t                    max_retry;         /*!< \brief [in] Maximum retry to get the right data from the PICC
                                                            until a response to the caller is done. */
    phcsBflI3P4_ProtParam_t   *p_protocol_param;  /*!< \brief [in] Pointer to I3P4_PROTOCOL_PARAM structure */
    void					  *self;              /*!< \brief [in] Pointer to the C-interface of this component in
                                                            order to let this member function find its "object".
                                                            Should be left dangling when calling the C++ wrapper.*/

} phcsBflI3P4_SetProtParam_t;



/*! \ingroup iso14443_4
 *  \brief Parameter structure for RESET PROTOCOL functionality.
 *  The function for resetting protocol parameters in the ISO/IEC 14443.4 protocol takes a pointer to
 *  this structure.
 */
typedef struct
{

	void					  *self;  /*!< \brief [in] Pointer to the C-interface of this component in
                                                order to let this member function find its "object".
                                                Should be left dangling when calling the C++ wrapper.*/
} phcsBflI3P4_ResetProtParam_t;


/*!
 * \fn phcsBfl_Status_t phcsBflI3P4_SetCbParam_t::set_reader_params_cb
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_CbSetReaderParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  This function is defined by the embedding-software (caller) and called by the protocol when there
 *  is the need to change the bitrate of the PCD, e.g. when using higher data rates. In general, this
 *  function is called before each communication cycle. The action taken upon this depends on the application's
 *  implementation.\n
 *  If the pointer is set to NULL, the protocol ignores the call and continues operation. In any
 *  other case the pointer is considered valid.
 *
 *  \note A callback is used instead of directly changements to keep the functionality free of hardware
 *        dependent code.
 */
typedef phcsBfl_Status_t (*pphcsBflI3P4_CbSetReader_t) (phcsBflI3P4_CbSetReaderParam_t*);

/*!
 * \fn phcsBfl_Status_t phcsBflI3P4_SetCbParam_t::set_wtx_cb
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_CbSetWtxParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  This function is defined by the embedding-software (caller) and called by the protocol when there
 *  is the need to change the timeout (WTX received) in order to allow the card to do more time-consuming
 *  processing before returning data.\n
 *  If the pointer is set to NULL, the protocol ignores the call and continues operation. In any
 *  other case the pointer is considered valid.
 *
 *  \note A callback is used instead of directly changements to keep the functionality free of hardware
 *        dependent code.
 */
typedef phcsBfl_Status_t (*pphcsBflI3P4_CbSetWtx_t) (phcsBflI3P4_CbSetWtxParam_t*);



/*! \ingroup iso14443_4
 *  \brief Parameter structure for the function for setting the callback addresses.
 */
typedef struct
{
    pphcsBflI3P4_CbSetWtx_t     set_wtx_cb;             /* see functionpointer documentation above */
    pphcsBflI3P4_CbSetReader_t  set_reader_params_cb;   /* see functionpointer documentation above */
    void                       *self;                   /*!< \brief [in] Pointer to the C-interface of this component in
                                                                    order to let this member function find its "object".
                                                                    Should be left dangling when calling the C++ wrapper.*/
} phcsBflI3P4_SetCbParam_t;




/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P4_Wrapper::Exchange(phcsBflI3P4_ExchangeParam_t *exchange_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P4_t::Exchange
 * \endif
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_ExchangeParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief
 * Data exchange protocol according to ISO 14443-4.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflI3P4_Exchange_t)          (phcsBflI3P4_ExchangeParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P4_Wrapper::PresenceCheck(phcsBflI3P4_PresCheckParam_t *presence_check_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P4_t::PresenceCheck
 * \endif
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_PresCheckParam_t [in]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief
 * This function checks the presence of a PICC in the field without altering its state. This
 * is done by exploiting the scenarios in response to a R(NACK) block.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflI3P4_PresCheck_t)     (phcsBflI3P4_PresCheckParam_t*); /* c*/
/*!
 * \if sec_PHFL_BFL_CPP
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P4_Wrapper::Deselect(phcsBflI3P4_DeselectParam_t *deselect_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P4_t::Deselect
 * \endif
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_DeselectParam_t [in]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief
 * Deselect functionality according to ISO 14443-4 specification.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflI3P4_Deselect_t)          (phcsBflI3P4_DeselectParam_t*);       /* x*/
/*!
 * \if sec_PHFL_BFL_CPP
 * \fn void phcs_BFL::phcsBflI3P4_Wrapper::SetProtocol(phcsBflI3P4_SetProtParam_t *set_protocol_param)
 * \else
 * \fn void phcsBflI3P4_t::SetProtocol
 * \endif
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_SetProtParam_t [in]: Pointer to the I/O parameter structure.
 *
 *  \brief
 * This function applies the session parameters (determined during activation) to the communication
 * protocol. The same struct as used during activation can be used also here.
 *
 */
typedef void      (*pphcsBflI3P4_SetProt_t)       (phcsBflI3P4_SetProtParam_t*);   /* x*/
/*!
 * \if sec_PHFL_BFL_CPP
 * \fn void phcs_BFL::phcsBflI3P4_Wrapper::ResetProt(phcsBflI3P4_ResetProtParam_t *reset_protocol_param)
 * \else
 * \fn void phcsBflI3P4_t::ResetProt
 * \endif
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_ResetProtParam_t [in]: Pointer to the I/O parameter structure.
 *
 * \brief This function sets the protocol to initial state (reset).
 * It shall be used for protocol-reinitialisation only. This might be the case when ended up in an
 * unrecoverable communication error condition. Additionally this might be called at the end of a
 * communication to be prepared to start a new one. \n
 * It is called implicitely during initialisation procedure (phcsBflI3P4_Init).
 *
 */
typedef void      (*pphcsBflI3P4_ResetProt_t)     (phcsBflI3P4_ResetProtParam_t*); /* x*/
/*!
 * \if sec_PHFL_BFL_CPP
 * \fn void phcs_BFL::phcsBflI3P4_Wrapper::SetPUser(phcsBflI3P4_SetPUserParam_t *set_p_user_param)
 * \else
 * \fn void phcsBflI3P4_t::SetPUser
 * \endif
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_ResetProtParam_t [in]: Pointer to the I/O parameter structure.
 *
 * \brief This function set the user pointer.
 */
typedef void      (*pphcsBflI3P4_SetPUser_t)          (phcsBflI3P4_SetPUserParam_t*);     /* x*/
/*!
 * \if sec_PHFL_BFL_CPP
 * \fn void phcs_BFL::phcsBflI3P4_Wrapper::SetCallbacks(phcsBflI3P4_SetCbParam_t *set_cb_param)
 * \else
 * \fn void phcsBflI3P4_t::SetCallbacks
 * \endif
 *
 * \ingroup iso14443_4
 *  \par Parameters:
 *  \li \ref phcsBflI3P4_SetCbParam_t [in]: Pointer to the I/O parameter structure.
 *
 * \brief
 * The function initialises the callback pointers to embedding software-defined functionality responsible
 * for the application of reader settings. The function pointers in the set_cb_param structure must
 * point to valid locations for the adequate operation or to NULL.
 */
typedef void (*pphcsBflI3P4_SetCb_t)  (phcsBflI3P4_SetCbParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 *
 *  \ingroup iso14443_4
 *  C - Component interface of ISO 14443.4 data exchange protocol: This component does only
 *  data exchange, card presence check and deselecting of a PICC. This is because only these
 *  three types of functionality reflect the commonalities between ISO/IEC 14443.4 PICC type
 *  A and B. Protocol entry functionality is only provided for PICC type A, however by a
 *  different interface.
 *
 * \endif
 */
typedef struct
{
    /* Methods: */
    pphcsBflI3P4_Exchange_t        Exchange;           /* Data exchange protocol */
    pphcsBflI3P4_PresCheck_t       PresenceCheck;      /* PICC presence check function. */
    pphcsBflI3P4_Deselect_t        Deselect;           /* Deselect function. */
    pphcsBflI3P4_SetProt_t         SetProtocol;        /* Apply session settings. */
    pphcsBflI3P4_ResetProt_t       ResetProt;          /* Reset to initial state. */
    pphcsBflI3P4_SetPUser_t        SetPUser;           /* */

    /* Protocol Callbacks: */
    pphcsBflI3P4_CbSetWtx_t        CbSetWtx;           /* Callback for WTX handling. */
    pphcsBflI3P4_CbSetReader_t     CbSetReaderParams;  /* Callback for bitrate changing. */

    /* Callback initialisation: */
    pphcsBflI3P4_SetCb_t               SetCallbacks;       /* Callback initialiser interface. */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
        void       *mp_CppWrapper;      /* Used, if wrapped by the C++ feature. This
                                           location stores the C++ object's address in order
                                           to be available for the C-only feature for handing it
                                           over to a callback or the like.*/
    #else
        void       *mp_UserRef;         /* Member pointer variable for general purpose
                                           (user-defined). Available only when RCL_CPP is
                                           undefined (pure C operation). When using C++ (RCL_CPP
                                           is defined, a pointer variable with the same name
                                           and purpose becomes available in the C++ interface class.*/
    #endif
    phcsBflIo_t *mp_Lower;                  /* Point to the lower I/O device: */
} phcsBflI3P4_t;


/*! \ingroup iso14443_4
 *  \brief Structure with general internal operation parameters:
 */
typedef struct
{
    uint8_t  *mp_TRxBuffer;        /*!< \brief System-mamaged TRx buffer reference, used for
                                              internal APDU composition / analysis.             */
    uint16_t  m_MaxRxBufferSize;   /*!< \brief Size of Rx buffer.                                */
    uint16_t  m_MaxTxBufferSize;   /*!< \brief Size of Tx buffer.                                */
    uint8_t   m_DSI;               /*!< \brief Selected divisor integer from PICC to PCD.        */
    uint8_t   m_DRI;               /*!< \brief Selected divisor integer from PCD to PICC.        */
    uint8_t   m_CID;               /*!< \brief CID used for the current session.                 */
    uint8_t  *m_NAD;               /*!< \brief NAD for chaining (only first block).              */
    uint8_t   m_Flags;             /*!< Contains information about CID/NAD support by the
                                              PCD/PICC as well as their usage. Furthermore this
                                              byte controls the internal flow in connection with
                                              e.g. timeout or chaining.                         */
    uint8_t   m_State;             /*!< \brief Stores the current and previous state of data
                                              exchange.                                         */
    uint8_t   m_BlockNumber;       /*!< \brief Block Number used by Data Exchange function.      */
    uint8_t  *mp_CopyBytes;        /*!< \brief Pointer used for chaining and copying of data.    */
    uint8_t  *mp_StartOfBlock;     /*!< \brief Pointer to indicate the protocol where the current
                                              block to transfer starts.                         */
    uint8_t   m_MaxRetryCounter;   /*!< \brief Maximum number of attempts to re-establish a
                                              connection after errors.                          */

    uint8_t   m_FWI;               /*!< \brief Frame Waiting Time                                */

    uint8_t   m_Index;             /*!< \brief Index for APDU Picc chaining if userbuffer full   */

    uint8_t   m_Size;              /*!< \brief current TRX buffer size (for PICC APDU chaining)  */

	uint8_t   m_HistoryFlag;	   /*!< \brief Contains information for the presence check
										      function. For this function it is important to
										      know if an echange was done or not.				*/
    void     *mp_UserWTX;          /*!< \brief User pointer for WTX callback.                    */

    void     *mp_UserReaderParam;  /*!< \brief User pointer for Reader Parameter Callback.       */

} phcsBflI3P4_CommParam_t;


/* //////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Initialise:
*/
/*!
* \ingroup iso14443_4
* \param[in] *cif       C object interface structure
* \param[in] *p_td      Pointer to the Communication Parameters structure.
* \param[in] *p_lower   Pointer to the underlying layer's TRx function.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
*
* This function binds the pre-allocated interface Component structure to its internal variables -
* containing structure. Additionally the lower-edge interface pointer is required by this function.
* This function has to be called first to operable.
*/
phcsBfl_Status_t phcsBflI3P4_Init(phcsBflI3P4_t               *cif,
                                  phcsBflI3P4_CommParam_t     *p_td,
                                  phcsBflIo_t                 *p_lower);

#endif /* PHCSBFLI3P4_H */

/* E.O.F. */
