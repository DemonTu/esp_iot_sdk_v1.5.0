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
 * \file phcsBflIdMan.h
 *
 * Project: ID-Manager functionality implementation.
 *
 * Workfile: phcsBflIdMan.h
 * $Author: mha $
 * $Revision: 1.2 $
 * $Date: Thu Oct 27 13:27:15 2005 $
 * $KeysEnd$
 *
 * Comment:
 *  
 *
 * History:
 *  BS:  Generated 24. 03. 2004
 *  MHa: Migrated to MoReUse September 2005
 * \endif
 *
 */


#ifndef PHCSBFLIDMAN_H
#define PHCSBFLIDMAN_H

#include <phcsBflRefDefs.h>
#include <phcsBflStatus.h>


/*! \ingroup id_man
 *  \brief Maximum counter value of IDs. */
#define PHCS_BFLIDMAN_MAX_ID_COUNT        ((uint8_t)15) 

/*!
 *  \ingroup id_man
 *  Parameter structure for all ID manager functions:
 */
typedef struct 
{
    uint8_t     ID;          /*!< \brief [in,out] ID, ranging from 0..14. */
    void       *instance;    /*!< \brief [in,out] Adress/location of the instance (object) of NFC/ISO14443.4 
                                              to be assigned. */
    void       *self;        /*!< \brief [in] Pointer to the C-interface of this component in   
                                          order to let this member function find its "object". 
                                          Should be left dangling when calling the C++ wrapper.*/

} phcsBflIdMan_Param_t;


/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIdMan_Wrapper::GetFreeID(phcsBflIdMan_Param_t *idman_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIdMan_t::GetFreeID
 * \endif
 *
 * \ingroup id_man
 *  \par Parameters:
 *  \li \ref phcsBflIdMan_Param_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 * The "Get Free ID" function returns the first free ID in its parameter idman_param. First ID 1..14 are
 * searched, finally the function examines ID 0 if all other IDs are occupied. 
 *
 */
typedef phcsBfl_Status_t (*pphcsBflIdMan_GetFreeID_t)        (phcsBflIdMan_Param_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIdMan_Wrapper::IsFreeID(phcsBflIdMan_Param_t *idman_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIdMan_t::IsFreeID
 * \endif
 *
 * \ingroup id_man
 *  \par Parameters:
 *  \li \ref phcsBflIdMan_Param_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief This function checks whether a specified ID is free.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflIdMan_IsFreeID_t)         (phcsBflIdMan_Param_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIdMan_Wrapper::AssignID(phcsBflIdMan_Param_t *idman_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIdMan_t::AssignID
 * \endif
 *
 * \ingroup id_man
 *  \par Parameters:
 *  \li \ref phcsBflIdMan_Param_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 *  This function binds an ID to the corresponding address of the object operating unter this logical number.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflIdMan_AssignID_t)         (phcsBflIdMan_Param_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIdMan_Wrapper::FreeIDByNumber(phcsBflIdMan_Param_t *idman_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIdMan_t::FreeIDByNumber
 * \endif
 *
 * \ingroup id_man
 *  \par Parameters:
 *  \li \ref phcsBflIdMan_Param_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 *  By specifying an ID number the function deletes the corresponding protocol object entry.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflIdMan_FreeIDByNumber_t)   (phcsBflIdMan_Param_t*);
/*!
 * \if sec_PHFL_BFL_CPP 
 * \fn phcsBfl_Status_t phcs_BFL::phcsBflIdMan_Wrapper::FreeIDByInstance(phcsBflIdMan_Param_t *idman_param)
 * \else
 * \fn phcsBfl_Status_t phcsBflIdMan_t::FreeIDByInstance
 * \endif
 *
 * \ingroup id_man
 *  \par Parameters:
 *  \li \ref phcsBflIdMan_Param_t [in,out]: Pointer to the I/O parameter structure.
 *
 *  \retval  PH_ERR_BFL_SUCCESS                Operation successful.
 *  \retval  Other                             Depending on implementation and underlaying component.
 *
 *  \brief 
 *  The ID under which the object (instance address) is listed becomes available again.
 *
 */
typedef phcsBfl_Status_t (*pphcsBflIdMan_FreeIDByInstance_t) (phcsBflIdMan_Param_t*);


/*! \ifnot sec_PHFL_BFL_CPP
 * 
 *  \ingroup id_man
 *  C-interface structure for ID-Manager: It allows to control different device IDs or card IDs
 *  automatically. 
 * 
 * \endif
 */
typedef struct
{
    /* Methods: */
    pphcsBflIdMan_GetFreeID_t        GetFreeID;          /* Get the first free ID available.              */
    pphcsBflIdMan_IsFreeID_t         IsFreeID;           /* Check whether the ID is available.            */
    pphcsBflIdMan_AssignID_t         AssignID;           /* Enter new instance into the ID manager list.  */
    pphcsBflIdMan_FreeIDByNumber_t   FreeIDByNumber;     /* Free ID by specifying the number.             */
    pphcsBflIdMan_FreeIDByInstance_t FreeIDByInstance;   /* Free ID by specifying the object instance.    */

    void *mp_Members;   /* Internal variables of the C-interface. Usually a structure is behind
                           this pointer. The type of the structure depends on the implementation
                           requirements. */
    #ifdef PHFL_BFL_CPP
        void       *mp_CallingObject;   /* Used by the "Glue-Class" to reference the wrapping
                                           C++ object, calling into the C-interface. */
    #endif
} phcsBflIdMan_t;


/*! \ingroup id_man
 *  \brief Parameter structure for ID-manager. Currently used to hold the number of instances.
 */
typedef struct 
{
    /*! \brief
     *  Array of n void pointers containing the used instances.
     */
    void    *mp_Instances[PHCS_BFLIDMAN_MAX_ID_COUNT];     
} phcsBflIdMan_InitParams_t;


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Idman Initialise
*/
/*!
* \ingroup id_man
* 
* \param[in] *cif       C object interface structure
* \param[in] *p_td      Pointer to the IDMAN_PARAMETERS Parameters structure (pre-allocated).
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
*
* This function initialises the C-object of the IdMan Component and expects valid pointers to the
* internal parameters structure as well as to a pre-allocated Component structure.
*
*/
phcsBfl_Status_t phcsBflIdMan_Init(phcsBflIdMan_t *cif, phcsBflIdMan_InitParams_t *p_td);



#endif  /* PHCSBFLIDMAN_H */

/* E.O.F. */
