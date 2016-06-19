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
 * \file phcsBflRegCtl_I2CHw1Int.h
 *
 * Project: Object Oriented Library Framework RegCtl Component for I2C on 8xC66x.
 *
 *  Source: phcsBflRegCtl_I2CHw1Int.h
 * $Author: mha $
 * $Revision: 1.3 $
 * $Date: Fri Jun 30 11:01:49 2006 $
 *
 * Comment:
 *  Internal definitons.
 *
 * History:
 *  MHa: Generated September 2005
 *
 */


#ifndef PHCSBFLREGCTL_I2CHW1INT_H
#define PHCSBFLREGCTL_I2CHW1INT_H

#include <phcsBflRegCtl.h>


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Reg-Ctl Set Register for PN51x:
*/
/*!
* \ingroup i2c
* \param[in] *setreg_param      Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_IO_TIMEOUT             Timeout generated during communication.
*
* This function handles the write access to the PN51x as defined in the hardware specification.
* Bus handling (acknowledge, ...) is handled internally.
*/
phcsBfl_Status_t phcsBflRegCtl_I2CHw1SetReg(phcsBflRegCtl_SetRegParam_t *setreg_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Reg-Ctl Get Register for PN51x:
*/
/*!
* \ingroup i2c
* \param[in,out] *getreg_param  Pointer to the parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_IO_TIMEOUT             Timeout generated during communication.
*
* This function handles the read access to the PN51x as defined in the hardware specification.
* Bus handling (acknowledge, ...) is handled internally.
*/
phcsBfl_Status_t phcsBflRegCtl_I2CHw1GetReg(phcsBflRegCtl_GetRegParam_t *getreg_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Reg-Ctl Modify Register for PN51x:
*/
/*!
* \ingroup i2c
* \param[in] *modify_param      Pointer to the parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_IO_TIMEOUT             Timeout generated during communication.
*
* This function handles bit modifications on the PN51x. Therefore a GetRegister command is performed 
* before a SetRegister. In between the modification is done. 
* The bits which are 1 in the mask byte are set or cleared according to the parameter set. If set is 0, 
* the bits are cleared, otherwise they are set.
* \note Be aware that some register content may change in between read and write. Fot these regsters use an
*       apropriate way of bitwise modification.
*/
phcsBfl_Status_t phcsBflRegCtl_I2CHw1ModReg(phcsBflRegCtl_ModRegParam_t *modify_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Reg-Ctl Set Register Multiple for PN51x:
*/
/*!
* \ingroup i2c
* \param[in] *setmultireg_param     Pointer to the parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_IO_TIMEOUT             Timeout generated during communication.
*
* This function handles the write access for multiple data bytes from one register of the PN51x. The 
* function SetRegister is called as aften as required to write all data.
*/
phcsBfl_Status_t phcsBflRegCtl_I2CHw1SetMultiReg(phcsBflRegCtl_SetMultiRegParam_t *setmultireg_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// Reg-Ctl Get Register Multiple for PN51x:
*/
/*!
* \ingroup i2c
* \param[in,out] *getmultireg_param     Pointer to the parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_IO_TIMEOUT             Timeout generated during communication.
*
* This function handles the read access for multiple data bytes from one register from the PN51x. The 
* function GetRegister is called as aften as required to read all data.
*/
phcsBfl_Status_t phcsBflRegCtl_I2CHw1GetMultiReg(phcsBflRegCtl_GetMultiRegParam_t *getmultireg_param);


#endif /* PHCSBFLREGCTL_I2CHW1INT_H */

/* E.O.F. */
