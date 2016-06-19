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
 * \file phcsBflI3P4AAct.h
 *
 * Project: ISO14443-4 basic functionality and protocol flow implementation for activation.
 *
 * Workfile: phcsBflI3P4AAct.h
 * $Author: mha $
 * $Revision: 1.2 $
 * $Date: Thu Oct 27 13:27:14 2005 $
 * $KeysEnd$
 *
 * Comment:
 *  The functios declared here are used within the C and C++ of the implementation. When used by the
 *  C variant these functions, providing already full protocol facilities, are called directly. When
 *  used within a C++ environment the functions are encapsulated by a class which provides
 *  additional features such as multi-device capabilities per application.
 *
 * History:
 *  GK:  Generated 14. Okt. 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLI3P4AACT_H
#define PHCSBFLI3P4AACT_H

#include <phcsBflStatus.h>
#include <phcsBflI3P4_Common.h>
#include <phcsBflIo.h>


/* //////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Initialise:
*/

/*! \ingroup iso14443_4A_entry
 *  \brief Parameter structure for RATS command (Get Ats of the PICC).
 */
typedef struct 
{
    uint8_t               cid;           /*!< \brief [in] CID assigned to the PICC. The function only checks the
                                                         range, but not the rules in the context of the value 0. */
    uint8_t               fsi;           /*!< \brief [in] FSDI, the PCD frame size integer, according to ISO/IEC 14443.4. \n
                                                    [out] FSDI, the PCD frame size integer, according to ISO/IEC 14443.4. */
    uint8_t              *ats;           /*!< \brief [in,out] Pointer to a pre-allocated buffer receiving the ATS. The
                                                             size of the buffer has to comply to the ISO/IEC 14443.4 - defined
                                                             maximum size of the ATS. */
    uint16_t              ats_len;       /*!< \brief [out] Length of the ATS, received from the PICC. */
    uint8_t              *ta1;           /*!< \brief [out] Pointer to the TA(1) location within the ATS.*/
    uint8_t              *tb1;           /*!< \brief [out] Pointer to the TB(1) location within the ATS.*/
    uint8_t              *tc1;           /*!< \brief [out] Pointer to the TC(1) location within the ATS.*/
    uint8_t              *app_inf;       /*!< \brief [out] Pointer to the Historical byte begin within the ATS.*/
    uint8_t               app_inf_len;   /*!< \brief [out] Length of the Historical Bytes within the ATS. */
    void                 *self;          /*!< \brief [in] Pointer to the C-interface of this component in   
                                                         order to let this member function find its "object". 
                                                         Should be left dangling when calling the C++ wrapper.*/
} phcsBflI3P4AAct_RatsParam_t;

/*! \ingroup iso14443_4A_entry
 *  \brief Parameter structure for Protocol Parameter Select functionality.
 */
typedef struct 
{
    uint8_t  cid;     /*!< \brief [in] CID assigned to the PICC. The function checks the range. */
    uint8_t  dsi;     /*!< \brief [in] Dividor Send Integer: ISO/IEC 14443.4 reference value for baud rate
                                      used when sending data to the PICC. */
    uint8_t  dri;     /*!< \brief [in] Dividor Receive Integer: ISO/IEC 14443.4 reference value for baud rate
                                      used when receiving data from the PICC. */
    void    *self;    /*!< \brief [in] Pointer to the C-interface of this component in   
                                      order to let this member function find its "object". 
                                      Should be left dangling when calling the C++ wrapper.*/
} phcsBflI3P4AAct_PpsParam_t;




/*  C-interface member function pointer types: */

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P4AAct_Wrapper::Rats(phcsBflI3P4AAct_RatsParam_t *rats_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P4AAct_t::Rats
 * \endif
 *
 * \ingroup iso14443_4A_entry
 *  \par Parameters:
 *  \li \ref phcsBflI3P4AAct_RatsParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 * This function issues an ISO 14443-4 compliant RATS command. The parameter structure contain all the 
 * data necessary to be transferred.
 *
 */
typedef phcsBfl_Status_t       (*pphcsBflI3P4AAct_Rats_t)           (phcsBflI3P4AAct_RatsParam_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflI3P4AAct_Wrapper::Pps(phcsBflI3P4AAct_PpsParam_t *pps_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflI3P4AAct_t::Pps
 * \endif
 *
 * \ingroup iso14443_4A_entry
 *  \par Parameters:
 *  \li \ref phcsBflI3P4AAct_PpsParam_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 * This function issues an ISO 14443-4 compliant PPS command. This command is optional. The caller has to 
 * take care of the received parameters to chose the right one which are adjustable.
 *
 */
typedef phcsBfl_Status_t       (*pphcsBflI3P4AAct_Pps_t)            (phcsBflI3P4AAct_PpsParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup iso14443_4A_entry
 *  \brief C - Component interface of ISO 14443.4 PICC type A protocol entry.
 *
 *  In the context of ISO 14443.4 this library exposes the PICC type A protocol entry with RATS and
 *  PPS. This component comes without the ISO 14443.3 part (Initialisation and Anticollision)
 *  since that part is covered by a different component. Moreover, this part initialises the
 *  phcsBflI3P4_ProtParam_t to be handed over to the ISO 14443.4 transmission protocol.
 * 
 * \endif
 */
typedef struct 
{
    /* Methods: */
    pphcsBflI3P4AAct_Rats_t             Rats;               /* Request for Answer to Select.     */
    pphcsBflI3P4AAct_Pps_t              Pps;                /* Protocol and Parameter Selection. */
    
    uint16_t m_trx_buffer_size;               /* Buffer size for frames composed by the
                                                       activation. Typical values are 64, 128
                                                       or 256[Bytes] */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
    #endif

    phcsBflIo_t *mp_Lower;                  /* Point to the lower I/O device: */
} phcsBflI3P4AAct_t;



/*!
* \ingroup iso14443_4A_entry
* 
* \param[in] *cif   C object interface structure
* \param[in] *p_td  Pointer to the Communication Parameters structure. This structure is filled
*                   by RATS and PPS and to be handed over to the exchange protocol (phcsBfl_I3P4)
*                   object.
* \param[in] *p_trxbuffer 
*                   Pointers to TX and RX buffer, used by the protocol for intermediate storage.
* \param[in]  trxbuffersize 
*                   Size of TX and RX buffer, used by the protocol for intermediate storage.
* \param[in] *p_lower
*                   Pointer to the underlying layer's TRx function.
* 
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
*
* This function shall be called first to glue together the C-interface, its internal members parameter 
* and to establish a link to the next lower layer. Moreover, the initialisation of operation parameters 
* is done. After this function the Component is operational.
*/
phcsBfl_Status_t phcsBflI3P4AAct_Init(phcsBflI3P4AAct_t           *cif,
                                  phcsBflI3P4_ProtParam_t     *p_td,
                                  uint8_t                   *p_trxbuffer,
                                  uint16_t                   trxbuffersize,
                                  phcsBflIo_t                 *p_lower);


#endif /* PHCSBFLI3P4AACT_H */

/* E.O.F. */
