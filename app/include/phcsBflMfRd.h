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
 * \file phcsBflMfRd.h
 *
 * Project: Object Oriented Library Framework Mifare Component.
 *
 *  Source: phcsBflMfRd.h
 * $Author: mha $
 * $Revision: 1.2 $
 * $Date: Thu Oct 27 13:27:13 2005 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated 29. April 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLMFRD_H
#define PHCSBFLMFRD_H

#include <phcsBflIo.h>

/* USER - relevant definitions, understood by this component: */

/*! \name Commands
 *  \brief Command definitions for Mifare operation. These command bytes comply to the MIFARE
 *  specification and serve as one parameter for the MIFARE transaction commands defined
 *  within the scope of this implementation.
 */
/*@{*/
/*!  \ingroup mfrd */
#define PHCS_BFLMFRD_AUTHENT_A            ((uint8_t)0x60)   /*!< \brief AUTHENT A command. */
#define PHCS_BFLMFRD_AUTHENT_B            ((uint8_t)0x61)   /*!< \brief AUTHENT B command. */
#define PHCS_BFLMFRD_READ                 ((uint8_t)0x30)   /*!< \brief READ command. */ 
#define PHCS_BFLMFRD_WRITE                ((uint8_t)0xA0)   /*!< \brief WRITE 16 bytes command. */
#define PHCS_BFLMFRD_WRITE4               ((uint8_t)0xA2)   /*!< \brief WRITE 4 bytes command. */
#define PHCS_BFLMFRD_INCREMENT            ((uint8_t)0xC1)   /*!< \brief INCREMENT command. */
#define PHCS_BFLMFRD_DECREMENT            ((uint8_t)0xC0)   /*!< \brief DECREMENT command.  */
#define PHCS_BFLMFRD_RESTORE              ((uint8_t)0xC2)   /*!< \brief RESTORE command. */
#define PHCS_BFLMFRD_TRANSFER             ((uint8_t)0xB0)   /*!< \brief TRANSFER command. */
#define PHCS_BFLMFRD_NOCOMMAND            ((uint8_t)0x00)   /* VOID command (no MIFARE command). */
/*@}*/


/*! \name Size Definitions
 *  \brief These definitions are important for the caller (user). The sizes are also internally checked 
 *  and used for request/response processing.
 */
/*@{*/
/*!  \ingroup mfrd */
/*! \brief Size of MF exchange buffer, used for underlying Transceive: */
#define PHCS_BFLMFRD_MAX_DATABUF_SIZE     ((uint8_t)32)             /* */
/*! \brief Size of a Mifare Standard card data block, in bytes. */
#define PHCS_BFLMFRD_STD_BLOCK_SIZE       ((uint8_t)16)             /* */
/*! \brief Size of a Mifare Ultra Light card/label data block, in bytes. */
#define PHCS_BFLMFRD_UL_PAGE_SIZE         ((uint8_t)4)              /* */
/*! \brief Data size of a Mifare value block, in bytes. */
#define PHCS_BFLMFRD_VALUE_DATA_SIZE      ((uint8_t)4)              /* */
/*@}*/



/*! \ingroup mfrd
 *  \brief Parameter structure for Mifare Reader functionality.
 */
typedef struct 
{
    /*! \brief [in] MIFARE specification-compliant command byte (according to MIFARE specification)
             or defined in file mifare.h. */
    uint8_t   cmd;

    /*! \brief [in] MIFARE block address: MIFARE operates block-oriented, although authentication is
             sector-based. */
    uint8_t   addr;

    /*! \brief [in] For sending bytes this must be a pre-allocated and initialised buffer. For receiving
             data, the buffer must be pre-allocated and of a size of PHCS_BFLMFRD_MAX_DATABUF_SIZE,
             defined in mifare.h. \n
        [out] Data copied into the buffer by this function e.g. as a result of a READ operation. */
    uint8_t  *buffer;

    /*! \brief [in] Length of data to send: Usually this value is 16 for a block to write. \n
             For a READ operation this value should be initialised with 0. \n
        [out] Receive buffer length: Number of data bytes received. */ 
    uint8_t   buffer_length;
                                             
    /*! \brief [in] When using the Transaction member of the MIFARE functionality this parameter should
             be left dangling.  \n
             When using the <em>generic</em> Mifare methods, this parameter specifies what type of
             response (none or timeout respectively, acknowledge or data) to expect.*/
    uint8_t   reply_type;
    void     *self;  /*!< \brief [in] Pointer to the C-interface of this component in   
                                     order to let this member function find its "object". 
                                     Should be left dangling when calling the C++ wrapper.        */
} phcsBflMfRd_CommandParam_t;


/*  C-interface member function pointer types: */

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflMfRd_Wrapper::Transaction(phcsBflMfRd_CommandParam_t *cmd_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflMfRd_t::Transaction
 * \endif
 *
 * \ingroup mfrd
 *  \par Parameters:
 *  \li \ref phcsBflMfRd_CommandParam_t [in]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief The main MIFARE reader protocol entry point. 
 *
 *  All MIFARE functionality is concentrated in this place. According to the cmd parameter all 
 *  possible Mifare (R) commands are handled. \n
 *  For a detailed description of all commands have a look at any Mifare documantation.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflMfRd_Transaction_t)   (phcsBflMfRd_CommandParam_t*); /* MIFARE TRANSACTION, wrapping the generics. */


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup mfrd
 *  \brief C - Component interface: Structure to call MIFARE Reader functionality.
 *
 *  This interface exposes the MIFARE Reader functionality without the ISO 14443.3 Initialisation
 *  and Anticollision part.
 * 
 * \endif
 */
typedef struct 
{
    /* Methods: */
    pphcsBflMfRd_Transaction_t   Transaction;      /* Mifare functionality according to spec. */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
    #endif

    phcsBflIo_t *mp_Lower;          /* Point to the lower I/O device: */
} phcsBflMfRd_t;




/*! \ingroup mfrd
 *  \brief Internal control variables: This structure holds all component-related variables
 *         and is referenced by the mp_Members element of phcsBflMfRd_t.
 */
typedef struct 
{
    /*! \brief
     * The TX/RX buffer used for underlying operations. Since the minimum size of this buffer
     * is specified to be 64 bytes no length information needs to be present because all Mifare
     * requests / responses are well below this boundary.
     */
    uint8_t  *m_TrxBuffer;
} phcsBflMfRd_InternalParam_t;



/* //////////////////////////////////////////////////////////////////////////////////////////////
// Mifare Initialise:
*/
/*!
* \ingroup mfrd
* \param[in] *cif           Pointer to an instance of the C object interface structure
* \param[in] *mp            Pointer to the alredy allocated internal control variables structure.
* \param[in] *p_lower       Pointer to the underlying layer I/O.
* \param[in] *p_trxbuffer   Pointer to the system-wide TRx buffer, allocated and managed by the embedding
*                           software. The buffer serves as the source/destination buffer for the
*                           underlying I/O Transceive functionality.
* 
* \brief This function shall be called first to initialise the Mifare Reader component. 
*
* There the C-Layer, the internal variables, the underlaying layer and the device mode are initialised.
* An own function pointer is typedef'ed for this function to enable the call within
* a generic C++ Mifare wrapper. 
*
*/
void phcsBflMfRd_Init(phcsBflMfRd_t  *cif,
                    void         *mp,
                    phcsBflIo_t    *p_lower,
                    uint8_t      *p_trxbuffer);


#endif /* PHCSBFLMFRD_H */

/* E.O.F. */
