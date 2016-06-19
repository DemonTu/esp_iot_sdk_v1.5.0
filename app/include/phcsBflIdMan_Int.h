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
 * \file phcsBflIdMan_Int.h
 *
 * Project: ID-Manager functionality implementation.
 *
 * Workfile: phcsBflIdMan_Int.h
 * $Author: mha $
 * $Revision: 1.1 $
 * $Date: Mon Oct 24 11:22:12 2005 $
 * $KeysEnd$
 *
 * Comment:
 *  
 *
 * History:
 *  MHa:  Generated 29. 09. 2005
 * \endif
 *
 */

#ifndef PHCSBFLIDMAN_INT_H
#define PHCSBFLIDMAN_INT_H




#define PHCSBFLIDMAN_MAX_ID_NUM          ((uint8_t)14) /*!< \brief Maximum number of allowed IDs. */
#define PHCSBFLIDMAN_EXCLUSIVE_ID        ((uint8_t)0)  /*!< \brief Exclusive ID number as specified in ISO. */



/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Idman GetFreeID
*/
/*!
* \ingroup id_man
* 
* \param[in] *idman_param       Parameter structure for ID Manager actions.
*                               The ID member of the structure contains the free ID number.
*                               The instance and ID members can be left dangling.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_NO_ID_AVAILABLE        No ID is available, all are occupied.
* \retval  PH_ERR_BFL_ID_ALREADY_IN_USE      ID cannot be assigned because it is already used.
* 
* The "Get Free ID" function returns the first free ID in its parameter idman_param. First ID 1..14 are
* searched, finally the function examines ID 0 if all other IDs are occupied. 
*
*/
phcsBfl_Status_t phcsBflIdMan_GetFreeID(phcsBflIdMan_Param_t *idman_param);



/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Idman IsFreeID
*/
/*!
* \ingroup id_man
* 
* \param[in,out] *idman_param       Parameter structure for ID Manager actions.
*                                   The ID to examine must be supplied by the caller in the ID member.
*                                   The instance member can be left dangling.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_NO_ID_AVAILABLE        No ID is available, all are occupied.
* \retval  PH_ERR_BFL_ID_ALREADY_IN_USE      ID cannot be assigned because it is already used.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
*
* This function checks whether a specified ID is free.
*
*/
phcsBfl_Status_t phcsBflIdMan_IsFreeID(phcsBflIdMan_Param_t *idman_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Idman AssignID
*/
/*!
* \ingroup id_man
* 
* \param[in] *idman_param       Parameter structure for ID Manager actions.
*                               The caller has to specify the address of the protocol object operating
*                               under a certain ID. Both members, instance and ID must be initialised
*                               by the caller.
*
* \retval  PH_ERR_BFL_SUCCESS                    Operation successful.
* \retval  PH_ERR_BFL_INSTANCE_ALREADY_IN_USE    Instance allready in use and could not be allocated twice.
*
* This function binds an ID to the corresponding address of the object operating unter this logical number.
*
*/
phcsBfl_Status_t phcsBflIdMan_AssignID(phcsBflIdMan_Param_t *idman_param);





/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Idman FreeIDByNumber
*/
/*!
* \ingroup id_man
* 
* \param[in] *idman_param   Parameter structure for ID Manager actions.
*                           The caller must specify the ID to clear in the ID member of the parameter.
*                           The instance member can be left dangling.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_ID_NOT_IN_USE          Specified ID is not in use.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
*
* By specifying an ID number the function deletes the corresponding protocol object entry.
*
*/

phcsBfl_Status_t phcsBflIdMan_FreeIDByNumber(phcsBflIdMan_Param_t *idman_param);



/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Idman FreeIDByInstance
*/
/*!
* \ingroup id_man
* 
* \param[in] *idman_param   Parameter structure for ID Manager actions.
*                           The caller must specify the instance to clear in the instance member of the parameter.
*                           The ID member can be left dangling.
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_ID_NOT_IN_USE          Specified ID is not in use.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
*
* The ID under which the object (instance address) is listed becomes available again.
*
*/

phcsBfl_Status_t phcsBflIdMan_FreeIDByInstance(phcsBflIdMan_Param_t *idman_param);
               


#endif  /* PHCSBFLIDMAN_INT_H */

/* E.O.F. */
