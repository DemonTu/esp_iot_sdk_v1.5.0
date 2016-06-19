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
 * \file phcsBflOpCtl.h
 *
 * Project: Object Oriented Library Framework OperationControl Component.
 *
 *  Source: phcsBflOpCtl.h
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 30 11:05:22 2006 $
 *
 * Comment:
 *  This is the file to glue together all OperationControl implementations for all different 
 *  hardware variants for the C++ wrapper.
 *
 * History:
 *  MHa: Generated 13. May 2003
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */

#ifndef PHCSBFLOPCTL_H
#define PHCSBFLOPCTL_H

/* Type defintions */
#include <phcsBflStatus.h>

#include <phcsBflRegCtl.h>
#include <phcsBflAux.h>


/*! \ingroup opctl
 *  \brief Parameter structure for SetAttrib/GetAttrib functionality.
 */
typedef struct 
{
    uint8_t     group_ordinal;       /*!< \brief [in,out] Ordinal of the command group. */
    uint8_t     attrtype_ordinal;    /*!< \brief [in,out] Ordinal of the attribute within a group. */
    uint8_t     param_a;             /*!< \brief [in,out] Parameter A of the attribute. */
    uint8_t     param_b;             /*!< \brief [in,out] Parameter B of the attribute. */
    uint8_t    *buffer;              /*!< \brief [in,out] Pointer to data buffer. */
    void       *self;                /*!< \brief [in] Pointer to the C-interface in order to let this
                                                  member function find its "object". Should be left
                                                  dangling when calling the C++ wrapper.  */
} phcsBflOpCtl_AttribParam_t;


/*! \ingroup opctl
 *  \brief Parameter structure used to initialise the CB address and the user reference pointer in Components
 *  such as phcsBflIo_t.
 */
typedef struct 
{
    pphcsBflAux_WaitEventCb_t wait_event_cb;   /*!< \brief [in] Pointer to the system (user)-defined callback (CB) 
                                                    function used for event detection. */
    void              *user_ref;        /*!< \brief [in] Pointer to a system (user)-defined location. Can be used
                                                  to let the CB-internal code access events, signals and the
                                                  like. */
    void              *self;            /*!< \brief [in] Pointer to the C-interface (\ref phcsBflOpCtl_t) in order to let this
                                                  member function find its "object". Should be left
                                                  dangling when calling the C++ wrapper.  */
} phcsBflOpCtl_SetWecParam_t;


/*  C-interface member function pointer types: */
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflOpCtl_Wrapper< phcsBflOCcp >::SetAttrib(phcsBflOpCtl_AttribParam_t *attrib_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflOpCtl_t::SetAttrib
 * \endif
 *
 * \ingroup opctl
 *  \par Parameters:
 *  \li \ref phcsBflOpCtl_AttribParam_t [in]: Pointer to the parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS               Operation successful.
 *  \retval  PH_ERR_BFL_INTERFACE_ERROR       Error on input parameters.
 *  \retval  Other                            Depending on implementation and underlaying component.
 *
 *  This function does various hardware dependent settings, e.g. store NFCIDs in the hardware for passive 
 *  communication mode, sets the timer or calculates the CRC of a data stream without sending it. \n
 *  The definitions for the function calls can be found in the file rcopctl_ordinals.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflOpCtl_SetAttrib_t)  (phcsBflOpCtl_AttribParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflOpCtl_Wrapper< phcsBflOCcp >::GetAttrib(phcsBflOpCtl_AttribParam_t *attrib_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflOpCtl_t::GetAttrib
 * \endif
 *
 * \ingroup opctl
 *  \par Parameters:
 *  \li \ref phcsBflOpCtl_AttribParam_t [in,out]: Pointer to the parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS               Operation successful.
 *  \retval  PH_ERR_BFL_INTERFACE_ERROR       Error on input parameters.
 *  \retval  Other                            Depending on implementation and underlaying component.
 *
 *  This function reads out various hardware dependent settings, e.g. read IDs, get actual hardware version. \n
 *  The definitions for the function calls can be found in the file rcopctl_ordinals.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflOpCtl_GetAttrib_t)  (phcsBflOpCtl_AttribParam_t*);

/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn void phcs_BFL::phcsBflOpCtl_Wrapper< phcsBflOCcp >::SetWaitEventCb(phcsBflOpCtl_SetWecParam_t *set_wec_param)
 * \else
 * \fn void phcsBflOpCtl_t::SetWaitEventCb
 * \endif
 *
 * \ingroup opctl
 *  \par Parameters:
 *  \li \ref phcsBflOpCtl_SetWecParam_t [in]: Pointer to the parameter structure.
 *
 *  In order to let Components such as e.g. phcsBflIo_t use the \ref pphcsBflAux_WaitEventCb_t 
 *  callback function as an event detector the address of the embedding system-defined CB has to submitted
 *  to the Component.
 */
typedef void (*pphcsBflOpCtl_SetWaitEventCb_t) (phcsBflOpCtl_SetWecParam_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup opctl
 *  \brief C - Component interface: Structure reflecting operation control functionality. This functionality
 *  allows configuration of the RF-chip on a higher level than the register access functions.
 * 
 * \endif
 */
typedef struct 
{
    /* Methods: */
    pphcsBflOpCtl_SetWaitEventCb_t   SetWaitEventCb;     /* Event detection CB address submission. */

    pphcsBflOpCtl_SetAttrib_t        SetAttrib;          /* Set Attrib member function. */
    pphcsBflOpCtl_GetAttrib_t        GetAttrib;          /* Get Attrib member function. */
    
    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                     C++ object, calling into the C-interface. */
    #endif

    /* User reference. pphcsBflAux_Hw1WaitEventCb_t receives this value with the parameter 
       structure phcsBflAux_Hw1WaitEventCbParam_t. */
    void *mp_UserRef;
    /* Pointer to the event-detecting callback function, to be implemented by the embedding SW. */
    pphcsBflAux_WaitEventCb_t mp_WaitEventCB;
    /* Lower edge: */
    phcsBflRegCtl_t *mp_Lower;
} phcsBflOpCtl_t;







/* //////////////////////////////////////////////////////////////////////////////////////////////
// OperationControl Initialise functions for PN51x:
*/
/*! \name Definitions for RCOPCTL parameter matrix:
 *  These defines are used to build up the parameter list(s) for initilising the different hardware types
 *  and antenna configurations. The unused parameters can be removed for the final implementation to save 
 *  memory space. \n
 *  Current configuration for all modes can be found in the file phcsBflOpCtl_Hw1Cfg.h. There also the 
 *  changements shall take place.
 *  \note Be sure the file phcsBflOpCtl_Hw1Cfg.h is not include twice!! Otherwise a lot of linker errors 
 *        will occure and the software will not be compilable!!
 *  \note Be aware that some register configuration is also done without having influence. These settings 
 *        can if necessary be overwritten by the parameter list defined.
 */
/*@{*/
/*! \ingroup opctl */
#define PHCS_BFLOPCTL_HW1VAL_UNDEF_HW        0xB1    /*!< \brief No known hardware version, 
                                                        first set of parameters is used for default. */
#define PHCS_BFLOPCTL_HW1VAL_VERS1           0x28    /*!< \brief Uses 1st set of parameters defined. Value is corresponding to a
                                                        version register enty of the hardware. */
#define PHCS_BFLOPCTL_HW1VAL_VERS2           0x27    /*!< \brief Uses 2nd set of parameters defined. Value is corresponding to a
                                                        version register enty of the hardware. */
#define PHCS_BFLOPCTL_HW1VAL_VERS3           0x26    /*!< \brief Uses same set of parameters as defined for HW_2. Value is 
                                                        corresponding to a version register enty of the hardware. */
#define PHCS_BFLOPCTL_HW1VAL_VERS4           0xA0    /*!< \brief Uses 4th set of parameters defined. Value is corresponding to a
                                                        version register enty of the hardware. */
#define PHCS_BFLOPCTL_HW1VAL_VERS5           0x80    /*!< \brief Uses 5th set of parameters defined. Value is corresponding to a
                                                        version register enty of the hardware. This configuration uses the same
                                                        setting as VERS4. */

#define PHCS_BFLOPCTL_HW_VERSIONS           {PHCS_BFLOPCTL_HW1VAL_VERS1, PHCS_BFLOPCTL_HW1VAL_VERS2, \
                                             PHCS_BFLOPCTL_HW1VAL_VERS3, PHCS_BFLOPCTL_HW1VAL_VERS4, \
                                             PHCS_BFLOPCTL_HW1VAL_VERS5, PHCS_BFLOPCTL_HW1VAL_UNDEF_HW}
/*@}*/

/*! \ingroup opctl 
 *  \brief This internal structure keeps the information about hte currently used hardware.
 */
typedef struct 
{
    uint8_t     hardware_type;                        /*!< \brief Specifies currently used type of Hardware. Default 
                                                             value is 0xB1 (\ref PHCS_BFLOPCTL_HW1VAL_UNDEF_HW)
                                                             which means no dedicated hardware version is selected.  */
} phcsBflOpCtl_Hw1Params_t;

/*!
* \ingroup opctl
* \param[in] *cif       Pointer to an instance of the C object interface structure
* \param[in] *km        Pointer to the internal control variables structure instance.
* \param[in] *p_lower   Pointer to the underlying functionality.
* 
* This function shall be called first to initialise the operation control component. There
* the C-Layer, the internal variables and the underlaying layer are initialised.
* An own function pointer is typedef'ed for this function to enable the call within
* a generic C++ OperationControl wrapper. 
*
*/
void phcsBflOpCtl_Hw1Init(phcsBflOpCtl_t     *cif,
                        void             *km,
                        phcsBflRegCtl_t    *p_lower);
/*!
 * \ingroup opctl
 * \param[in] *set_wec_param
 *                  Pointer to the phcsBflOpCtl_SetWecParam_t structure containing data
 *                  to initialise the underlying callback function for event detection.
 *
 * This function initializes the user defined callback (interrupt handler) which might be used
 * in the functions of this component.
 */
void phcsBflOpCtl_Hw1SetWaitEventCb(phcsBflOpCtl_SetWecParam_t *set_wec_param);



#endif /* PHCSBFLOPCTL_H */

/* E.O.F. */
