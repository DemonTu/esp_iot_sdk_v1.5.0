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
 * \file phcsBflI3P4AAct_Internal.h
 *
 * Project: ISO14443-4 basic functionality and protocol flow implementation for activation.
 *
 * Workfile: phcsBflI3P4AAct_Internal.h
 * $Author: mha $
 * $Revision: 1.1 $
 * $Date: Mon Oct 24 11:22:00 2005 $
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
 *
 */


#ifndef PHCSBFLI3P4AACT_INT_H
#define PHCSBFLI3P4AACT_INT_H


/* R/ATS Defines */
#define PHCS_BFLI3P4AACT_RATS_START_BYTE              (0xE0)                  /* */
#define PHCS_BFLI3P4AACT_RATS_CID_MASK                (0x0F)                  /* */
#define PHCS_BFLI3P4AACT_RATS_FSDI_SHL_VAL            (0x04)                  /* */
#define PHCS_BFLI3P4AACT_RATS_MAX_FSDI                (0x08)                  /* */
#define PHCS_BFLI3P4AACT_RATS_MAX_FSCI                PHCS_BFLI3P4AACT_RATS_MAX_FSDI    /* */
#define PHCS_BFLI3P4AACT_RATS_DEF_FORMAT              (0x02)                  /* */
#define PHCS_BFLI3P4AACT_RATS_FSCI_MASK               (0x0F)                  /* */
#define PHCS_BFLI3P4AACT_ATS_MIN_LENGTH               (0x01)                  /* */
#define PHCS_BFLI3P4AACT_ATS_LEN_BYTE_POS             (0x00)                  /* */
#define PHCS_BFLI3P4AACT_ATS_FORM_BYTE_POS            (0x01)                  /* */
#define PHCS_BFLI3P4AACT_ATS_TA1_PRESENT              (0x10)                  /* */
#define PHCS_BFLI3P4AACT_ATS_TB1_PRESENT              (0x20)                  /* */
#define PHCS_BFLI3P4AACT_ATS_TC1_PRESENT              (0x40)                  /* */
#define PHCS_BFLI3P4AACT_ATS_FWI_MASK                 (0x0F)                  /* */
#define PHCS_BFLI3P4AACT_ATS_FWI_SHR_MASK             (0x04)                  /* */

/* PPS Defines: */
#define PHCS_BFLI3P4AACT_PPS_CID_MASK                 PHCS_BFLI3P4AACT_RATS_CID_MASK    /* */
#define PHCS_BFLI3P4AACT_PPS_PPSS_TEMPL               (0xD0)                  /* */
#define PHCS_BFLI3P4AACT_PPS_DSI_SHL_VAL              (0x02)                  /* */
#define PHCS_BFLI3P4AACT_PPS_PARAMETER0               (0x11)                  /* */
#define PHCS_BFLI3P4AACT_PPS_RESPLEN                  (0x01)                  /* */
#define PHCS_BFLI3P4AACT_PPS_PPSS_POS                 (0x00)                  /* */
#define PHCS_BFLI3P4AACT_PPS_MAX_DSI                  (0x03)                  /* */
#define PHCS_BFLI3P4AACT_PPS_MAX_DRI                  PHCS_BFLI3P4AACT_PPS_MAX_DSI      /* */




/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL RATS:
*/
/*!
* \ingroup iso14443_4A_entry
*
* \param[in,out] *rats_param        Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* This function issues an ISO 14443-4 compliant RATS command. The parameter structure contain all the
* data necessary to be transferred.
*/
phcsBfl_Status_t phcsBflI3P4AAct_Rats(phcsBflI3P4AAct_RatsParam_t *rats_param);


/* //////////////////////////////////////////////////////////////////////////////////////////////////
// T=CL PPS:
*/
/*!
* \ingroup iso14443_4A_entry
*
* \param[in,out] *pps_param         Pointer to the I/O parameter structure
*
* \retval  PH_ERR_BFL_SUCCESS                Operation successful.
* \retval  PH_ERR_BFL_INVALID_FORMAT         Received data is not the one expected.
* \retval  PH_ERR_BFL_INVALID_PARAMETER      Input parameter is out of defined range.
* \retval  Other                            phcsBfl_Status_t values depending on the underlying components.
*
* This function issues an ISO 14443-4 compliant PPS command. This command is optional. The caller has to
* take care of the received parameters to chose the right one which are adjustable.
*
*/
phcsBfl_Status_t phcsBflI3P4AAct_Pps(phcsBflI3P4AAct_PpsParam_t *pps_param);


#endif /* PHCSBFLI3P4AACT_INT_H */

/* E.O.F. */
