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
 * \file phcsBflOpCtl_Hw1Int.h
 *
 * Project: Object Oriented Library Framework OperationControl Component for PN51x.
 *
 *  Source: phcsBflOpCtl_Hw1Int.h
 * $Author: mha $
 * $Revision: 1.5 $
 * $Date: Fri Jun 30 11:01:52 2006 $
 *
 * Comment:
 *  None.
 *
 * History:
 *  MHa: Generated September 2005
 *
 */


#ifndef PHCSBFLOPCTL_HW1INT_H
#define PHCSBFLOPCTL_HW1INT_H

#include <phcsBflOpCtl.h>


/*! \name Definitions for RCOPCTL parameter matrix:
    \ingroup rcopctl 
    These defines are used to build up the parameter Matrixes for initilising the different hardware types
    and antenna configurations. \n
 */
/*@{*/
/* Command defined for operation of Matrix values. */
#define PHCS_BFLOPCTL_VAL_MATRIX_END           0xC3    /*!< Defines the end of the current record to be read. */

/* Comon defines for arrays */
#define PHCS_BFLOPCTL_VAL_ARRAY_WIDTH          0x03
/* defines for getting right position out of the array */
#define PHCS_BFLOPCTL_VAL_ADDRESS_POS          0x00
#define PHCS_BFLOPCTL_VAL_AND_POS              0x01
#define PHCS_BFLOPCTL_VAL_OR_POS               0x02
/*@}*/

/* Entry types for configuration tables (arrays) */
#define PHCS_BFLOPCTL_VAL_BEG_PARAM_SET(param_set)          /* code */ const uint8_t (param_set)[][PHCS_BFLOPCTL_VAL_ARRAY_WIDTH] = {
#define PHCS_BFLOPCTL_VAL_END_PARAM_SET                       {PHCS_BFLOPCTL_VAL_MATRIX_END, 0x00, 0x00}};
#define PHCS_BFLOPCTL_VAL_PARAM_ENTRY(param1, param2, param3) {(param1), (param2), (param3)},

/*! \ingroup opctl 
 *  Current hardware configuration container: \n
 *  Pointer to arrays that contain the configuration for the communication modes and data rates. The
 *  initialisation and the hardware types shall not be changed. Instead of this the arrays which contain
 *  the configuration shall be modified for own purposes. \n
 *  The configuration is done in file phcsBflOpCtl_Hw1Cfg.h. There are tables which are defined for different
 *  hardware types. 
 *  \ note Be awere of not including the file phcsBflOpCtl_Hw1Cfg.h anywhere else!! This will cause linker errors
 *         which may not be directly related to the problem!!
 */
typedef struct
{
    const uint8_t (*Active_106)[PHCS_BFLOPCTL_VAL_ARRAY_WIDTH];                     /*!< Data structure for antenna 
                                                                                 related configuration for 106 kbps
                                                                                 in active communication mode. See
                                                                                 PHCS_BFLOPCTL_ATTR_ACTIVE for additional configurations 
                                                                                 and settings. */
    const uint8_t (*Active_212_424)[PHCS_BFLOPCTL_VAL_ARRAY_WIDTH];                 /*!< Data structure for antenna 
                                                                                 related configuration for 212 and
                                                                                 424 kbps in active communication mode.
                                                                                 The speed is set independently. See
                                                                                 PHCS_BFLOPCTL_ATTR_ACTIVE for additional configurations 
                                                                                 and settings. */
    const uint8_t (*Passive_Initiator_106)[PHCS_BFLOPCTL_VAL_ARRAY_WIDTH];          /*!< Data structure for antenna 
                                                                                 related configuration for 106 kbps
                                                                                 in passive communication mode. See
                                                                                 PHCS_BFLOPCTL_ATTR_MFRD_A for additional configurations 
                                                                                 and settings. */
    const uint8_t (*Passive_Initiator_212_424)[PHCS_BFLOPCTL_VAL_ARRAY_WIDTH];      /*!< Data structure for antenna 
                                                                                 related configuration for 212 and
                                                                                 424 kbps in passive communication mode.
                                                                                 The speed is set independently. See
                                                                                 PHCS_BFLOPCTL_ATTR_PAS_POL for additional 
                                                                                 configurations and settings. */
    const uint8_t (*Passive_Target)[PHCS_BFLOPCTL_VAL_ARRAY_WIDTH];                 /*!< Data structure for antenna 
                                                                                 related configuration for all data
                                                                                 rates in passive communication mode. 
                                                                                 See PHCS_BFLOPCTL_ATTR_PASSIVE for additional 
                                                                                 configurations and settings. */
} C_HW1_RC_OP_CTL_CONFIGS;




/* //////////////////////////////////////////////////////////////////////////////////////////////
// OperationControl Initialise for PN51x:
*/

/* //////////////////////////////////////////////////////////////////////////////////////////////////
// OperationControl Set Attribute for PN51x:
*/
/*!
* \ingroup opctl
* \param[in] *attrib_param    Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS               Operation successful.
* \retval  PH_ERR_BFL_INTERFACE_ERROR       Error on input parameters.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* This function does various hardware dependent settings, e.g. store NFCIDs in the hardware for passive 
* communication mode, sets the timer or calculates the CRC of a data stream without sending it. \n
* The definitions for the function calls can be found in the file rcopctl_ordinals.
*/
phcsBfl_Status_t phcsBflOpCtl_Hw1SetAttribute(phcsBflOpCtl_AttribParam_t *attrib_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// OperationControl Get Attribute for PN51x:
*/
/*!
* \ingroup opctl
* \param[in,out] *attrib_param  Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INTERFACE_ERROR        Error on input parameters.
* \retval  Other                             phcsBfl_Status_t values depending on the underlying components.
*
* This function reads out various hardware dependent settings, e.g. read IDs, get actual hardware version. \n
* The definitions for the function calls can be found in the file rcopctl_ordinals.
*/
phcsBfl_Status_t phcsBflOpCtl_Hw1GetAttribute(phcsBflOpCtl_AttribParam_t *attrib_param);
                                      

/*! \name Internal helper functions prototypes
 *  \ingroup opctl
 *  Internal functions for multiple used functionalities. */
/*@{*/
phcsBfl_Status_t phcsBflOpCtl_Hw1MapSerSpeedSet(uint8_t setting, uint8_t *mapped_value);
phcsBfl_Status_t phcsBflOpCtl_Hw1ManipulateRegs(phcsBflRegCtl_t  *lower,     /*<! Handle of the lower instance. */
                                            uint8_t         address,   /*<! PN51x register address. */
                                            uint8_t         mask,      /*<! Select only the bits required. */
                                            uint8_t         shl,       /*<! How far to shift left. */
                                            uint8_t         new_value);/*<! Value to be written. */
/*@}*/




#endif /* PHCSBFLOPCTL_HW1INT_H */

/* E.O.F. */
