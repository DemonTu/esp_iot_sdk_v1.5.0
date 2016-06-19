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


/*! \file NfcTargetC.c
 *
 * Projekt: Object Oriented Reader Library Framework BAL component.
 *
 * $Author: bs $
 * $Revision: 1.66 $
 * $Date: Wed Jul  5 14:37:36 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

/* For debugging purposes (printf) */
#include <stdio.h>

/* Common headers */
#include <phcsBflRefDefs.h>
#include <phcsBflHw1Reg.h>

/* Needed for any Operating Mode */
#include <phcsBflBal.h>
#include <phcsBflRegCtl.h>
#include <phcsBflIo.h>
#include <phcsBflOpCtl.h>
#include <phcsBflOpCtl_Hw1Ordinals.h>

/* Needed for this Operating Mode */
#include "NfcTargetC.h"
#include <phcsBflI3P3A.h>
#include <phcsBflNfc.h>
#include <phcsBflNfcCommon.h>

#include "reader_ISO_14443_P4.h"

//#include "ExampleGlobals.h"
//#include "ExampleUtilsC.h"

/* Globals variables */
static uint8_t g_TxSpeed = 0xFF, g_RxSpeed = 0xFF;

/* ISO14443-3 relevant settings */
static uint8_t g_atqa[]   = {0x08, 0x00};
static uint8_t g_nfcid1[] = {0x12, 0x34, 0x56};
static uint8_t g_sak       = 0xC0;

/* NFCID2 settings */
static uint8_t g_nfcid2[] = {0x01, 0xFE, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define RETURN_STATUS(status) if((status) != PH_ERR_BFL_SUCCESS) return (status) /* */
#define NFC_TARGET_CTRL_1ST_REQ_AFT_ATR  0x0008
#define NFC_TARGET_ATR_DONE_FLAG  0x01

/* Structure which is necessary for the endPoint callback propagated from Target implementation. */
typedef struct
{
    phcsBflRegCtl_t         *p_rc_reg_ctl;
    phcsBflOpCtl_t		    *p_rc_op_ctl;
    phcsBflI3P3A_t    *p_iso14443_3;
    uint8_t           *p_nfcidt;
	uint8_t            p_Flag;
} TARGET_ENDPOINT_PARAM;

/*
 * The target endpoint implementation is split into three parts:
 * - uint16_t ActivateNfcTarget(void *comHandle)
 *   Configures the registers on the PN51x
 * - phcsBfl_Status_t TargetEndpointCallback(phcsBflNfc_TargetEndpointParam_t *endpoint_param)
 *   Interprets the commands (coming from the initiator)
 * - phcsBfl_Status_t SetIDs(phcsBflOpCtl_t *p_op_ctl)
 *   Defines the target settings and configures the PN51x
 */


/* Enpoint implementation comments:
 * --------------------------------
 *
 * ATR_REQ:  Received data is displayed in debug window.
 *           NFCID3 is '12 23 34 45 56 67 78 89 9A AB BC CD DE EF' but choosen in RunTest's nfcid_t.
 *           BST: 00, BRT:00, TOT: 08, PP: 01, no general bytes
 * PSL_REQ:  Error message to debug window.
 *           Evaluation and Reconfiguration (only receive side) of chosen datarate.
 * PSL_RES:  Error message to debug window.
 *           Reconfiguration (only transmit side) of chosen datarate.
 * RLS_REQ:  Error message to debug window.
 * RLS_RES:  Error message to debug window. Reset protocol for further operations. ...
 * DSL_REQ:  Error message to debug window.
 * DSL_RES:  Error message to debug window. Activation of automatic Anticollision inpassive mode. ...
 * WUP_REQ:  Error message to debug window.
 * REQ_TOX:  Response to TEST_COMMAND3 -> TEST_RESPONSE3 is sent.
 * REQ_NACK: Error message to debug window.
 * REQ_ACK:  Previous part of chaining has been received by the Initiator, next part is ready to send.
 */
phcsBfl_Status_t TargetEndpointCallback(phcsBflNfc_TargetEndpointParam_t *endpoint_param)
{
    phcsBfl_Status_t               status               = PH_ERR_BFL_SUCCESS;
    /* Register control parameters */
    phcsBflRegCtl_SetRegParam_t         srp;
    phcsBflRegCtl_GetRegParam_t         grp;
    phcsBflRegCtl_ModRegParam_t         mrp;
    /* Operation control parameters */
    phcsBflOpCtl_AttribParam_t      opp;
    /* ISO14443-3 parameters */
    phcsBflI3P3A_HaltAParam_t hlt_p;
    uint8_t           index                = 0;
    uint8_t           hlt_buffer[2]        = {0x50, 0x00};
    uint8_t           nfcid_buffer[18];
    static uint8_t    psl_brs, psl_fsl;

	grp.self = srp.self = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl;
	mrp.self = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl;
    opp.self = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_op_ctl;
    hlt_p.self = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_iso14443_3;

    /* Choose right action according to calling function */
    switch (endpoint_param->action_type)
    {
        case PHCS_BFLNFCCOMMON_ATR_REQ:
            /* Get parameters of the Initiator: The protocol omits not relevant info at the beginning: */
			printf("[I] Target Endpoint:\n    ATR_REQ: BSI = %02X, BRI = %02X, PPI = %02X, GI = ",
                   (endpoint_param->p_req_data)[index],      /* BSI */
                   (endpoint_param->p_req_data)[index + 1],  /* BRI */
                   (endpoint_param->p_req_data)[index + 2]); /* PPI - NAD usable, no generics. */
            index = 3;
            while (index < endpoint_param->req_data_length)
            {
                printf("%02X ", (endpoint_param->p_req_data)[index]);
                index++;
            }
            printf("\n");

            /* In passive mode 212 & 424 kbps check if the NFCID3 is equal to NFCID2. If not do not respond. */
            if(!(endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE))
            {
                grp.address = PHCS_BFL_JREG_RXMODE;
                status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->GetRegister(&grp);
                if((grp.reg_data & PHCS_BFL_JMASK_SPEED) != PHCS_BFL_JBIT_106KBPS)
                {
                    /* Check if data received is same as send during passive activation before */
                    for(index=0; index<PHCS_BFLNFCCOMMON_ID_LEN; index++)
                    {
                        if(g_nfcid2[index] != ((phcsBflNfc_TargetCommParam_t*)((phcsBflNfc_Target_t*)(endpoint_param->target))->mp_Members)->mp_NfcIDi[index])
                        {
                            status = PH_ERR_BFL_TARGET_NFCID_MISMATCH;
                            break;
                        }
                    }
                }
            }

            /* deactivate time jitter after successful reception */
            if (status == PH_ERR_BFL_SUCCESS)
            {
                mrp.address = PHCS_BFL_JREG_TXAUTO;
                mrp.mask    = PHCS_BFL_JBIT_CAON;
                mrp.set     = 0;
                status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->ModifyRegister(&mrp);
            }

            /* Fill in the parameters of the Target: */
            index = 0;
            (endpoint_param->p_res_data)[index] = PHCS_BFLNFCCOMMON_ATR_BS_DS_8 | PHCS_BFLNFCCOMMON_ATR_BS_DS_16 | PHCS_BFLNFCCOMMON_ATR_BS_DS_32; /* BST */
            index++;
            (endpoint_param->p_res_data)[index] = PHCS_BFLNFCCOMMON_ATR_BR_DR_8 | PHCS_BFLNFCCOMMON_ATR_BR_DR_16 | PHCS_BFLNFCCOMMON_ATR_BR_DR_32; /* BRT */
            index++;
            (endpoint_param->p_res_data)[index] = 0x0E;   /* TOT */
            index++;
            /* LR: 128, PPT - NAD not usable, no generics present. */
            (endpoint_param->p_res_data)[index] = PHCS_BFLNFCCOMMON_ATR_PP_LR_128;
            index++;

            /* We return something thus we have to set the size: */
            endpoint_param->res_data_length = index;

			/* After successfull ATR set AtrDoneFlag*/
			if (status == PH_ERR_BFL_SUCCESS)  ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_Flag |= NFC_TARGET_ATR_DONE_FLAG;
            break;

        case PHCS_BFLNFCCOMMON_PSL_REQ:
            /* Change RxSpeed and Framing here. Must be already active when "Start-Send" is done!
               Store settings internally for usage in PSL_RES to. */
            psl_brs = (endpoint_param->p_req_data)[index];      /* store BRS value internally */
            psl_fsl = (endpoint_param->p_req_data)[index + 1];  /* store FSL value internally */
            printf("    PSL_REQ - Getting new parameters: [0x] BRS = %02X, FSL = %02X.\n", psl_brs, psl_fsl);

            /* check if there is a difference between already configured data rate and new one
               Note: DSI parameters are receive parameters for Target. */
            if(g_RxSpeed != (psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK) >> 3)
            {
                /* Change data-rate for Rx-path of Target bevore sending!
                   Note: differentiation between active and passive mode has to be done! */
                printf("    PSL_REQ - applying new parameters for receive!\n");

                /* Adjust parameters according to previous settings and to new one
                 * Differentiation between active and passive mode has to be done! */
                if(endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE)
                {
					/* Active mode */
                    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
                    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;
                    opp.param_a          = g_TxSpeed;
                    opp.param_b          = (uint8_t)((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK) >> 3);
                    /* Start set attribute command */
                    status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_op_ctl->SetAttrib(&opp);
                    /* Check status and diplay error if something went wrong */
                    if(status)
                    {
                        printf("*** ERROR! Reconfiguration of hardware failed. Status = %04X \n", status);
                    } else
                    {
                        printf("    Hardware reconfiguration sucessfully. Mode: %s, TxSpeed: %d, RxSpeed: %d \n",
                            (endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE) ? "active" : "passive",
                            opp.param_a, opp.param_b);
				        /* Store new data rate in global for further use */
                        g_RxSpeed = (uint8_t)((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK) >> 3);
                    }
                } else
                {
					/* Passive mode
					 * Change only Speed and framing, all other configuration is ok. */
                    grp.address = PHCS_BFL_JREG_RXMODE;
                    status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->GetRegister(&grp);
                    if(status != PH_ERR_BFL_SUCCESS)
                        printf("[E] ERROR! Reading register failed. Status = %04X. ", status);
                    grp.reg_data = (grp.reg_data & ~(PHCS_BFL_JMASK_SPEED | PHCS_BFL_JMASK_FRAMING));

                    srp.address = PHCS_BFL_JREG_RXMODE;
                    if(psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK)
                    {
                        srp.reg_data = grp.reg_data | (((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK) << 1) | PHCS_BFL_JBIT_FELICA);
                    } else
                    {
                        srp.reg_data = grp.reg_data | (((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK) << 1) | PHCS_BFL_JBIT_MIFARE);
                    }
                    status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->SetRegister(&srp);
                    /* Check status and diplay error if something went wrong */
                    if(status)
                    {
                        printf("*** ERROR! Reconfiguration of hardware failed. Status = %04X \n", status);
                    } else
                    {
                        printf("    Hardware reconfiguration sucessfully. Mode: %s, TxSpeed: %d, RxSpeed: %d \n",
                            (endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE) ? "active" : "passive",
                            g_TxSpeed, (srp.reg_data & PHCS_BFL_JMASK_SPEED)>>4);
				        /* Store new data rate in global for further use */
                        g_RxSpeed = (uint8_t)((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK) >> 3);
                    }
                }
            }
            break;

        case PHCS_BFLNFCCOMMON_PSL_RES:
            printf("    PSL_RES: \n");
            if(g_TxSpeed != (psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DR_MASK))
            {
                /* Change TxSpeed and Framing here. Get information out of static bytes.
                   Tx parameters must be set after transmission of response!             */
                printf("    PSL_RES - applying new parameters for transmit!\n");

                /* Adjust parameters according to previous settings and to new one
                 * Differentiation between active and passive mode has to be done! */
                if(endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE)
                {
					/* Active mode */
                    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
                    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;
                    opp.param_a          = (uint8_t)(psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DR_MASK);
                    opp.param_b          = g_RxSpeed;
                    /* Start set attribute command */
                    status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_op_ctl->SetAttrib(&opp);
                    if(status)
                    {
                        printf("*** ERROR! Reconfiguration of hardware failed. Status = %04X \n", status);
                    } else
                    {
                        printf("    Hardware reconfiguration sucessfully. Mode: %s, TxSpeed: %d, RxSpeed: %d \n",
                            (endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE) ? "active" : "passive",
                            opp.param_a, opp.param_b);
                        /* store new data rate in global for further use */
                        g_TxSpeed = (uint8_t)(psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DR_MASK);
                    }
                } else
                {
					/* Passive mode
					 * Change only Speed and framing, all other configuration is ok. */
                    grp.address = PHCS_BFL_JREG_TXMODE;
                    status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->GetRegister(&grp);
                    if(status != PH_ERR_BFL_SUCCESS)
                        printf("[E] ERROR! Reading register failed. Status = %04X. ", status);
                    grp.reg_data = (grp.reg_data & ~(PHCS_BFL_JMASK_SPEED | PHCS_BFL_JMASK_FRAMING));

                    srp.address = PHCS_BFL_JREG_TXMODE;
                    if(psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DR_MASK)
                    {
                        srp.reg_data = grp.reg_data | (((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DR_MASK) << 4) | PHCS_BFL_JBIT_FELICA);
                    } else
                    {
                        srp.reg_data = grp.reg_data | (((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DR_MASK) << 4) | PHCS_BFL_JBIT_MIFARE);
                    }
                    status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->SetRegister(&srp);
                    /* Check status and diplay error if something went wrong */
                    if(status)
                    {
                        printf("*** ERROR! Reconfiguration of hardware failed. Status = %04X \n", status);
                    } else
                    {
                        printf("    Hardware reconfiguration sucessfully. Mode: %s, TxSpeed: %d, RxSpeed: %d \n",
                            (endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE) ? "active" : "passive",
                            (srp.reg_data & PHCS_BFL_JMASK_SPEED)>>4, g_RxSpeed);
				        /* Store new data rate in global for further use */
                        g_RxSpeed = (uint8_t)((psl_brs & PHCS_BFLNFCCOMMON_PSL_BRS_DS_MASK) >> 3);
                    }
                }
            }
            break;

        case PHCS_BFLNFCCOMMON_RLS_REQ:
            /*  No bytes to return - the Target protocol does that automatically: */
            printf("    RLS_REQ: \n");
            break;

        case PHCS_BFLNFCCOMMON_RLS_RES:
            /* do anything necessary to be prepared for further actions */
            printf("    RLS_RES: \n");
            /* generate new ID for response, shall be randomly */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_PROTOCOL;      /* set operation control group protocol */
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_NFCID;          /* signal to set ID parameters */
            opp.param_a          = PHCS_BFLOPCTL_VAL_NFCID_RANDOM;    /* Randomize IDs */
            opp.param_b          = 0;                       /* must be 0 */
            status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_op_ctl->SetAttrib(&opp);
            if(status != PH_ERR_BFL_SUCCESS)
            {
                printf("*** ERROR! Setting random IDs. Status = %04X \n", status);
            } else
            {
                printf("New random numbers generated sucessfully. \n");
            }

            /* Store new NFCID1 in global variables again to be prepared for next activation */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_PROTOCOL;      /* set operation control group protocol */
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_NFCID;          /* signal to set ID parameters */
            opp.param_a          = PHCS_BFLOPCTL_VAL_NFCID1;          /* Get NFCID1 */
            opp.param_b          = 0;                       /* must be 0 */
            opp.buffer           = nfcid_buffer;            /* local buffer to store temporarily the NFCID */
            status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_op_ctl->GetAttrib(&opp);
            if(status != PH_ERR_BFL_SUCCESS)
            {
                printf("*** ERROR! Getting NFCID1. Status = %04X \n", status);
            } else
            {
                index = 0;
                g_atqa[0]   = opp.buffer[index];
                index++;
                g_atqa[1]   = opp.buffer[index];
                index++;
                g_nfcid1[0] = opp.buffer[index];
                index++;
                g_nfcid1[1] = opp.buffer[index];
                index++;
                g_nfcid1[2] = opp.buffer[index];
                index++;
                g_sak     = opp.buffer[index];
                index++;
            }

            /* Store new NFCID2 in global variables again to be prepared for next activation */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_PROTOCOL;      /* set operation control group protocol */
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_NFCID;          /* signal to set ID parameters */
            opp.param_a          = PHCS_BFLOPCTL_VAL_NFCID2;          /* Get NFCID1 */
            opp.buffer           = nfcid_buffer;            /* local buffer to store temporarily the NFCID */
            status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_op_ctl->GetAttrib(&opp);
            if(status != PH_ERR_BFL_SUCCESS)
            {
                printf("*** ERROR! Getting NFCID2. Status = %04X \n", status);
            } else
            {
                for(index=0; index<opp.param_b; index++)
                    g_nfcid2[index] = opp.buffer[index];
            }
            status = PH_ERR_BFL_TARGET_RELEASED;
            break;

        case PHCS_BFLNFCCOMMON_DSL_REQ:
            /*  No bytes to return - the Target protocol does that automatically: */
            printf("    DSL_REQ: \n");
            break;

        case PHCS_BFLNFCCOMMON_DSL_RES:
            /*  Do necessary settings after sending DSL_RES here: */
            printf("    DSL_RES: \n");
            /* Deactivate DetectSync functionality if activated before! If not there is no correct receiving possible! */
            mrp.address = PHCS_BFL_JREG_MODE;
            mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
            mrp.set     = 0;
            status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->ModifyRegister(&mrp);
            if (status != PH_ERR_BFL_SUCCESS)
            {
                printf("*** ERROR! Re-setting Detect-Sync. Status = %04X \n", status);
                break;
            }

            /* Reset the Speed settings before re-activatiing the Mode detector */
            mrp.address = PHCS_BFL_JREG_TXMODE;
            mrp.mask    = PHCS_BFL_JMASK_SPEED || PHCS_BFL_JMASK_FRAMING;
            mrp.set     = 0;
            status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->ModifyRegister(&mrp);
            if (status != PH_ERR_BFL_SUCCESS)
            {
                printf("*** ERROR! Re-setting TxSpeed and TxFraming. Status = %04X \n", status);
                break;
            }
            mrp.address = PHCS_BFL_JREG_RXMODE;
            mrp.mask    = PHCS_BFL_JMASK_SPEED || PHCS_BFL_JMASK_FRAMING;
            mrp.set     = 0;
            status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->ModifyRegister(&mrp);
            if (status != PH_ERR_BFL_SUCCESS)
            {
                printf("*** ERROR! Re-setting RxSpeed and TxFraming. Status = %04X \n", status);
                break;
            }

            /*
             * For Passive mode do Halt procedure to be ready for passive activation again.
             * This function also activate the automatic anticollision procedure of the hardware.
             */
            if(!(endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_ACTIVE))
            {
                hlt_p.buffer        = hlt_buffer;
                hlt_p.buffer_length = 2;
                status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_iso14443_3->HaltA(&hlt_p);
            } else
            {   /* Setup Mode detector and automatic receive */
                srp.address  = PHCS_BFL_JREG_COMMAND;
                srp.reg_data = PHCS_BFL_JCMD_AUTOCOLL;
                status = ((TARGET_ENDPOINT_PARAM*)(endpoint_param->p_user))->p_rc_reg_ctl->SetRegister(&srp);
                if(status)
                    printf("*** ERROR! Re-activation of Modedetector. Status = %04X \n", status);
            }
            status = PH_ERR_BFL_TARGET_DESELECTED;
            break;

        case PHCS_BFLNFCCOMMON_WUP_REQ:
            /*  No bytes to return - the Target protocol does that automatically.
                Check of NFCID is done in the Target protocol implementation. */
            printf("    WUP_REQ:\n");
            break;

        case PHCS_BFLNFC_TARGET_REQ_TOX:   /* Target has received a TOX response. */
            /*
             * The Initiator has confirmed a request for more processing time (TEST_COMMAND3).
             * Extended timeout has started and is running.
             */

            /*
             * Attention: Do not check the flags in here! All flags are set / reset before
             *            the next call of ..DEP_REQ or ..REQ_INF.
             */
            printf("TOX_REQ received, send TEST_RESPONSE3\n");

            /* reset the res_data length, actual value contains the maximum number of bytes to insert */
            (endpoint_param->res_data_length) = 0;
            /* Write data into the buffer to be returned. */

            /* We tell the protocol to have a DEP_RES (INF) ready. */
            /* check if manual ot automatic chaining mode */
            if(((phcsBflNfc_TargetCommParam_t*)((phcsBflNfc_Target_t*)(endpoint_param->target))->mp_Members)->m_MaxTargetBufferSize == 0 ||
               ((phcsBflNfc_TargetCommParam_t*)((phcsBflNfc_Target_t*)(endpoint_param->target))->mp_Members)->mp_TargetBuffer == NULL)
            {
                endpoint_param->action_type = PHCS_BFLNFC_TARGET_RES_INF;
            } else
            {
                endpoint_param->action_type = PHCS_BFLNFCCOMMON_DEP_RES;
            }
            break;

        case PHCS_BFLNFC_TARGET_REQ_NACK:  /* A NACK has been received. */
            /*  Send if the previous data has not been received correctly by the Initiator:
                The previous sent data shall be replied! */
            printf(" REQ_NAK \n");
            break;

        case PHCS_BFLNFC_TARGET_REQ_ACK:   /* An ACK has been received. */
            /*  Send if Chaining from Target to Initiator is active and one packet has been
                received by the Initiator: */
            printf(" REQ_ACK \n");
            /* reset the res_data length, actual value contains the maximum number of bytes to insert */
            (endpoint_param->res_data_length) = 0;
            /* Write data into the buffer to be returned. */
            break;

        case PHCS_BFLNFC_TARGET_REQ_INF:   /* An INF block has been received. */
            /*
             * Manual Chaining (either m_MaxTargetBufferSize==0 or mp_TargetBuffer==0):
             * If no external buffer is supplied, the Target endpoint must handle chaining
             * itself, with the following to take care of within endpoint_param:
             * a. flags & mi_present == 1: An ACK or TOX frame has to be returned. Expect more info to
             *    arrive from the Initiator. Otherwise, continue with sending a TOX or INF frame.
             * b. nad: The first byte of the trx_buffer is the NAD itself! Data starts at
             *    the second byte!
             */
            printf("[I] REQ_INF: DEP 'Manual' chaining. Input size = %02i.\n", endpoint_param->req_data_length);
            /*
             * We confirm the first I-block with an ACK since MI is set:
             */
            if(endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_MI_PRES)
            {
                /* The first packet has arrived: */
                printf("DEP_REQ received, do 'Manual' chaining - send ACK.\n");

                /* Tell the system the type of response: */
                endpoint_param->action_type = PHCS_BFLNFC_TARGET_RES_ACK;
                endpoint_param->res_data_length = index;
            } else  /* no chaining is active */
            {
                /* Respond with fixed data */
				printf("Respond with fixed data.\n");
				endpoint_param->p_res_data[0] = 0x90;
				endpoint_param->p_res_data[1] = 0x00;
				endpoint_param->res_data_length = 2;
                endpoint_param->action_type = PHCS_BFLNFC_TARGET_RES_INF;
            }
            break;

        case PHCS_BFLNFCCOMMON_DEP_REQ:   /* In case of auto-chaining the application buffer contains the entire data. */
            /*
             * Auto-chaining (either m_MaxTargetBufferSize!=0 or mp_TargetBuffer!=0):
             * The Target has a buffer large enough to perform chaining automatically:
             * The entire buffer is handed over to this function.
             */

            printf("DEP_REQ: DEP Auto chaining active; input size = %02i.\n", endpoint_param->req_data_length);

			if (endpoint_param->flags & PHCS_BFLNFC_TARGET_EP_FLAGS_RX_OVL)
			{
				printf("Target Buffer is full, endpoint is called.\n");
			} else
			{
                /* Respond with fixed data */
				printf("Respond with fixed data.\n");
				endpoint_param->p_res_data[0] = 0x90;
				endpoint_param->p_res_data[1] = 0x00;
				endpoint_param->res_data_length = 2;
				endpoint_param->action_type = PHCS_BFLNFCCOMMON_DEP_RES;
			}
            break;

        default:
            printf(" Anything else called which is not implemented! \n");

            endpoint_param->res_data_length = index;
            status = PH_ERR_BFL_UNSUPPORTED_COMMAND;
            break;
    }
    return status;
}


/*
 *  Funtion to set Mifare and FeliCa parameters.
 *  Retuns the error code.
 */
phcsBfl_Status_t SetIDs(phcsBflOpCtl_t *p_op_ctl)
{
    /* Declaration of status variable */
    phcsBfl_Status_t             status = PH_ERR_BFL_SUCCESS;
    /* Operation control parameters */
    phcsBflOpCtl_AttribParam_t  opp;

    /* Buffer and index byte for byte stream generation */
    uint8_t buffer[20];
    uint8_t index = 0;

    /* Initialize self parameter of used structure */
    opp.self = p_op_ctl;

    /* Mifare ID (resp. ISO14443-4 ID) and response data */
    buffer[index] = g_atqa[0];    /* ATQA byte 0 */
    index++;
    buffer[index] = g_atqa[1];    /* ATQA byte 1 */
    index++;
    buffer[index] = g_nfcid1[0];  /* SNR byte 1, byte 0 fixed to 0x08 */
    index++;
    buffer[index] = g_nfcid1[1];  /* SNR byte 2 */
    index++;
    buffer[index] = g_nfcid1[2];  /* SNR byte 3 */
    index++;
    buffer[index] = g_sak;        /* SAK byte  */
    index++;

    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_PROTOCOL;  /* Set operation control group protocol */
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_NFCID;      /* Signal to set ID parameters */
    opp.param_a          = PHCS_BFLOPCTL_VAL_NFCID1;      /* Mifare settings */
    opp.param_b          = index;               /* Length of data in buffer */
    opp.buffer           = buffer;              /* Let operation control use the defined buffer */
    status = p_op_ctl->SetAttrib(&opp);
    if (status)
    {
		printf("[E] Error: Last status = %04X \n", status);
        return status;
    }
    else
    {
        printf("[I] NFCIDs and configuration data set.\n");
    }


    /*
     * FeliCa ID and System code (NFCID2)
     * Fixed (bytes 0 & 1); IDm (bytes 2 to 7); PMm (bytes 8 to 15); systemcode (bytes 16 & 17)
     */
    for(index=0; index<18; index++)
        buffer[index] = g_nfcid2[index];

    opp.param_a          = PHCS_BFLOPCTL_VAL_NFCID2;  /* Felica settings */
    opp.param_b          = index;           /* Length of data in buffer */
    opp.buffer           = buffer;          /* Let operation control use the defined buffer */
    status = p_op_ctl->SetAttrib(&opp);
    if (status)
        printf("*** ERROR! Status = %04X \n", status);
    else
        printf("Felica ID and systemcode set.\n");

    return status;
}


/*
 * Main function for Target settings.
 */
uint16_t ActivateNfcTarget(void *comHandle, uint32_t aSettings)
{
	/* BAL either for Windows xor for Linux */
    phcsBflBal_t                bal;
    phcsBflRegCtl_t             rc_reg_ctl;
    phcsBflIo_t                 rcio;
    phcsBflOpCtl_t              rc_op_ctl;
    phcsBflI3P3A_t        iso14443_3;
    phcsBflNfc_Target_t          nfc_t;
    phcsBflNfc_TargetEndpoint_t target_endpoint;

	/*
	 * Declaration of internal Component Structures:
	 */
#if 0
#ifdef WIN32
    phcsBflBal_Hw1SerWinInitParams_t   bal_params;
#else
    phcsBflBal_Hw1SerLinInitParams_t         bal_params;
#endif
#endif
    phcsBflRegCtl_SerHw1Params_t         rc_reg_ctl_params;
    phcsBflIo_Hw1Params_t                    rc_io_params;
    phcsBflOpCtl_Hw1Params_t                 rc_op_ctl_params;
    phcsBflI3P3A_Hw1Params_t           iso_14443_3_params;
    phcsBflNfc_TargetCommParam_t       target_params;

	/*
	 * Declaration of the used parameter structures
	 */

    /* Register control parameters */
    phcsBflRegCtl_SetRegParam_t         srp;
    phcsBflRegCtl_GetRegParam_t         grp;
    phcsBflRegCtl_ModRegParam_t         mrp;

    /* Operation control parameters */
    phcsBflOpCtl_AttribParam_t          opp;

    /* RCIO transceive parameter */
    phcsBflIo_ReceiveParam_t            rcv_p;

    /* ISO14443-3 parameters */
    phcsBflI3P3A_HaltAParam_t     hlt_p;

    /* User pointer parameters */
    phcsBflNfc_TargetSetPUserParam_t    nfct_sup_param;
	/* Structure for parameter propagation to the endpoint */
    TARGET_ENDPOINT_PARAM               endp_param;

    /* Endpoint structure */
    phcsBflNfc_TargetDispatchParam_t    nfct_dispatch_param;

	/* Declaration of generic variables */
    phcsBfl_Status_t     istatus = PH_ERR_BFL_SUCCESS, status = PH_ERR_BFL_SUCCESS;
    /* Buffer for data transfer (send/receive) */
    uint8_t buffer[EXAMPLE_TRX_BUFFER_SIZE + PHCS_BFLNFCCOMMON_MAX_PREAM_LEN];
	/* Target Buffer */
	uint8_t targetbuffer[EXAMPLE_TARGET_BUFFER_SIZE];
    /* Buffer to store received ID of the initiator */
    uint8_t nfcidi_buffer[PHCS_BFLNFCCOMMON_ID_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    /* Buffer to store received ID of the target */
    uint8_t nfcidt_buffer[PHCS_BFLNFCCOMMON_ID_LEN] = { 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA };
    /* Counter variable */
    uint8_t i;
    /* Variables to check incoming data speed after automatic anticollision */
    uint16_t length            = 0;
    /* Variable to sign dedicated modes (value different to 0 indicate mode) */
    uint8_t activate_NFCTarget = 0, activate_MifareCard = 0, activate_FelicaCard = 0, passive = 0;

	/* Initialisation of self parameters */
    srp.self   = &rc_reg_ctl;
    grp.self   = &rc_reg_ctl;
    mrp.self   = &rc_reg_ctl;
    opp.self   = &rc_op_ctl;
    rcv_p.self = &rcio;
    hlt_p.self = &iso14443_3;
    nfct_dispatch_param.self = &nfc_t;
    target_endpoint.TargetEndpointCallback = TargetEndpointCallback;

    nfct_sup_param.self = &nfc_t;

    /*
	 * Build up stack for serial communication.
     * Start with the lowest layer, so that the upper ones may refer to this already.
	 */

#if 0  // las083
#ifdef WIN32
    bal_params.com_handle = (void*)(comHandle);
    bal_params.com_port   = 0;
    phcsBflBal_Hw1SerWinInit(&bal, &bal_params);
#else
    bal_params.com_handle = (void*)(comHandle);
    bal_params.com_port   = 0;
    phcsBflBal_Hw1SerLinInit(&bal, &bal_params);
    /* Initialise Register Control Layer component */
    phcsBflRegCtl_SerHw1Init(&rc_reg_ctl, &rc_reg_ctl_params, &bal);
#endif
#else
    phcsBflRegCtl_SpiHw1Init(&rc_reg_ctl, &rc_reg_ctl_params, &bal);
#endif
    /* Initialise RcIo component */
    phcsBflIo_Hw1Init(&rcio, &rc_io_params, &rc_reg_ctl, PHCS_BFLIO_TARGET);
    /* Initialise Operation Control component */
    phcsBflOpCtl_Hw1Init(&rc_op_ctl, &rc_op_ctl_params, &rc_reg_ctl);
    /* Initialise ISO14443-3 component */
    phcsBflI3P3A_Hw1Initialise(&iso14443_3, &iso_14443_3_params, &rc_reg_ctl, PHCS_BFLI3P3A_TARGET);
    /* Initialise NFCTarget only if needed, because of different preamble types used */

	/*
	 * Appropriate settings for the Target Mode
	 */
    /* Driver settings for passive communication */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_PASSIVE;
    opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
    opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
    status = rc_op_ctl.SetAttrib(&opp);
    if(status)
        printf("*** ERROR! Status = %04X \n", status);

	/* Set Mifare and FeliCa IDs (predefined in function above) */
    status = SetIDs(&rc_op_ctl);
    if(status)
        printf("*** ERROR! Status = %04X \n", status);

    /* Note that this is only needed if the SIGIN/SIGOUT feature is used! */
	if (aSettings)
	{
		if ( aSettings == 1 )
		{
			opp.param_b = PHCS_BFLOPCTL_VAL_NOSIGINOUT;
			printf("[I] SIGIN/SIGOUT is disabled.\n");
		}
		else if ( aSettings == 2 )
		{
			opp.param_b = PHCS_BFLOPCTL_VAL_ONLYSIGINOUT;
			printf("[I] SIGIN/SIGOUT only.\n");
		}
		else
		{
			opp.param_b = PHCS_BFLOPCTL_VAL_SIGINOUT;
			printf("[I] Both Target and SIGIN/SIGOUT.\n");
		}

		opp.group_ordinal = PHCS_BFLOPCTL_GROUP_SIGNAL;
		opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_SIGINOUT;
		opp.param_a = PHCS_BFLOPCTL_VAL_CARD;
		status = rc_op_ctl.SetAttrib(&opp);
		if(status != PH_ERR_BFL_SUCCESS)
			printf("[E] Failed to set SIGINOUT functionality. Status = %04X \n", status);
	}

	/* Activate automatic Single Device Detection */
    srp.address  = PHCS_BFL_JREG_COMMAND;
    srp.reg_data = PHCS_BFL_JCMD_AUTOCOLL;
    status = rc_reg_ctl.SetRegister(&srp);
    if(status)
        printf("*** ERROR! Status = %04X \n", status);
    else
        printf("[I] Automatic SDD of PN51x activated and selected. Waiting for a command...\n");

	/* Activate automatic RfOn for active mode */
    mrp.address = PHCS_BFL_JREG_TXAUTO;
    mrp.mask    = PHCS_BFL_JBIT_AUTORFOFF | PHCS_BFL_JBIT_CAON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
    mrp.set     = 1;
    /* AutoRfOff = 1; Force100ASK, AutoWakeUp = X; CAOn = 1; InitialRFOn = X; TxXRfAutoEn = 1; */
    status = rc_reg_ctl.ModifyRegister(&mrp);
    if(status)
        printf("*** ERROR! Status = %04X \n", status);

	/* Call the transceive function to receive incomming data */
    rcv_p.rx_buffer = buffer;
	rcv_p.rx_buffer_size = EXAMPLE_TRX_BUFFER_SIZE + PHCS_BFLNFCCOMMON_MAX_PREAM_LEN;
    status = rcio.Receive(&rcv_p);
    if(status)
        printf("*** ERROR! Receive command returned with status %04X \n", status);
    else
    {   /* Data has been received */
        printf("[I] Received data: ");
        for(i=0; i<rcv_p.rx_buffer_size; i++)
            printf("%02X ", buffer[i]);
        printf("\n");
        length = rcv_p.rx_buffer_size;
    }

    /* After that the mode (which is forced by the initiator) can be determined.
     * This is done by looking at the RF speed and incoming data.
     * Store TxSpeed and RxSpeed for further investigations.
     */
    grp.address = PHCS_BFL_JREG_TXMODE;
    status = rc_reg_ctl.GetRegister(&grp);
    if (status)
        printf("*** ERROR! Reading register %02X. Status = %04X \n", grp.address, status);
    /* Mask out bits for speed selection and use receive speed for further decissions. */
    g_TxSpeed = (uint8_t)((grp.reg_data & PHCS_BFL_JMASK_SPEED)>>4);

    grp.address = PHCS_BFL_JREG_RXMODE;
    status = rc_reg_ctl.GetRegister(&grp);
    if (status)
        printf("*** ERROR! Reading register %02X. Status = %04X \n", grp.address, status);
    /* Mask out bits for speed selection and use receive speed for further decissions. */
    g_RxSpeed = (uint8_t)((grp.reg_data & PHCS_BFL_JMASK_SPEED)>>4);

    if(status == PH_ERR_BFL_SUCCESS && g_RxSpeed <= PHCS_BFLOPCTL_VAL_RF424K)
    {
        /* Check if passive mode has been used (PN51x statemachine has been processed) */
        grp.address = PHCS_BFL_JREG_STATUS2;
        status = rc_reg_ctl.GetRegister(&grp);
        if(status)
            printf("*** ERROR! Reading register %02X. Status = %04X \n", grp.address, status);

        /* In passive mode operation this bit is set */
        if(grp.reg_data & PHCS_BFL_JBIT_MFSELECTED)
        {
            /* deactivate bit AutoRFOn & AutoRFOff for passive communication. Not needed here. */
            mrp.address = PHCS_BFL_JREG_TXAUTO;
            mrp.mask    = PHCS_BFL_JBIT_AUTORFOFF | PHCS_BFL_JBIT_CAON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
            mrp.set     = 0;
            /* AutoRfOff = 1; Force100ASK, AutoWakeUp = X; CAOn = 1; InitialRFOn = X; TxXRfAutoEn = 1; */
            status = rc_reg_ctl.ModifyRegister(&mrp);
            if(status)
                printf("*** ERROR! Modify Register: Status = %04X \n", status);


            /* Check if data was received with 106kbps */
            if(g_RxSpeed == PHCS_BFLOPCTL_VAL_RF106K)
            {
                printf("[I] 106kbps Passive mode detected.\n");

                /* The HALT command is the only implemented one */
                hlt_p.buffer        = rcv_p.rx_buffer;
                hlt_p.buffer_length = (uint8_t)rcv_p.rx_buffer_size;
                status = iso14443_3.HaltA(&hlt_p);
                if(status == PH_ERR_BFL_SUCCESS)
                {
                    printf("Halt command received.\n");
                } else
                {
                    /* Check preamble byte for NFC 106kbps */
                    if((buffer[0] == PHCS_BFLNFCCOMMON_PREAM_1BYTE) && (buffer[1] + 1 == length))
                    {
                        printf("[I] Frame format according to ECMA 340.\n");
                        activate_NFCTarget = 1;
                        passive = 1;
						status = PH_ERR_BFL_SUCCESS;
                        nfct_dispatch_param.buffer = &buffer[1];      /* Use position=1 because we want to skip 0xF0 at beginning */
                        nfct_dispatch_param.length = (uint16_t)(length - 1); /* Skip CRC and 0xF0 for NFC protocol */
                    } else
                    {
                        printf("Frame format not according to ECMA 340, try Mifare Card mode.\n");
                        activate_MifareCard = 1;
                    }
                }
            } else  /* Speed is either 212kbps or 424kbps */
            {
                /* Check if framing is according to NFC */
                if(buffer[0] == length && buffer[1] == PHCS_BFLNFCCOMMON_REQ)
                {
					/* Activate NFC Target protocol in passive mode */
                    activate_NFCTarget = 1;
                    passive = 1;
                } else
                {
					/* Activate FeliCa Card mode */
                    activate_FelicaCard = 1;
                }
                nfct_dispatch_param.buffer = buffer;
                nfct_dispatch_param.length = length;
            }
        } else
        {
			/* Obviously active mode since the PN51x state machine hasn't been passed */
            if(status == PH_ERR_BFL_SUCCESS && g_RxSpeed == PHCS_BFLOPCTL_VAL_RF106K)
            {
				/* Check active configuration data at 106kBit/s */
				/* Since CRC is not handled automatically in active mode we have two additional bytes in the FIFO */
                if((buffer[0] == PHCS_BFLNFCCOMMON_PREAM_1BYTE) && (buffer[1] + 3 == length))
                {
                    phcsBflRegCtl_GetRegParam_t grp_1;
                    grp_1.self = &rc_reg_ctl;

                    printf("Frame format according to ECMA 340.\n");

                    /* Check CRC, status is set from last operation */
                    grp.address = PHCS_BFL_JREG_CRCRESULT1;
                    status = rc_reg_ctl.GetRegister(&grp);
                    grp_1.address = PHCS_BFL_JREG_CRCRESULT2;
                    status = rc_reg_ctl.GetRegister(&grp_1);

                    if((status != PH_ERR_BFL_SUCCESS) || ((grp.reg_data | grp_1.reg_data) != 0x00))
                    {
                        printf("*** ERROR! Status of CRC calculation = %04X or CRC wrong.\n", status);
                    } else
                    {
                        /* Perform all PN51x settings for Active mode */
                        opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
                        opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;
                        opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
                        opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
                        status = rc_op_ctl.SetAttrib(&opp);
                        if(status)
                            printf("*** ERROR! Status = %04X \n", status);

                        /* Activate NFC protocol */
                        activate_NFCTarget = 1;
                         /* Prepare data for protocol operation */
                        nfct_dispatch_param.buffer = &buffer[1];      /* Use position=1 because we want to skip 0xF0 at beginning */
                        nfct_dispatch_param.length = (uint16_t)(length - 3); /* Skip CRC and 0xF0 for NFC protocol */
                    }
                } else
                {
					printf("[E] Active Mode / 106kbps : Wrong preamble or incorrect size (%X != %X)!\n", buffer[1]+3, length);
                }
            } else if(status == PH_ERR_BFL_SUCCESS)
            {
				/*
				 * Speed is either 212kbps or 424kbps, RF is on
                 * Do all the PN51x settings for active mode
				 */
                opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
                opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;
                if(g_RxSpeed == PHCS_BFLOPCTL_VAL_RF212K)
                {
					/* Parameter for 212kbps active */
                    opp.param_a          = PHCS_BFLOPCTL_VAL_RF212K;
                    opp.param_b          = PHCS_BFLOPCTL_VAL_RF212K;
                } else
                {
					/* Parameter for 424kbps active */
                    opp.param_a          = PHCS_BFLOPCTL_VAL_RF424K;
                    opp.param_b          = PHCS_BFLOPCTL_VAL_RF424K;
                }
                status = rc_op_ctl.SetAttrib(&opp);
                if(status)
                    printf("*** ERROR! Status = %04X \n", status);

                /* Activate NFC protocol */
                activate_NFCTarget = 1;
                nfct_dispatch_param.buffer = buffer;
                nfct_dispatch_param.length = length;
            } else
            {
                printf("Driver is not active for active mode! Configuration seems to be wrong!\n");
            }
        }
    } else
    {   /* Speed higher than 424kbps */
        printf("*** ERROR! Initiator/Reader's speed is higher than 424kbps. This is not supported.\n");
    }

    /*
	 * NFCTarget
	 */
    if(activate_NFCTarget)
    {
        if(status)
        {
            printf("*** ERROR! Reading register %02X = %04X \n", grp.address, status);
        } else
        {
			/* Initialise NFC Target component */
            mrp.address = PHCS_BFL_JREG_MODE;
            mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
            mrp.set     = 1;
            status = rc_reg_ctl.ModifyRegister(&mrp);
            if (status != PH_ERR_BFL_SUCCESS)
            {
                printf("Error! Modify register: %02X, Status: %04X", mrp.address, status);
            } else
            {
                phcsBflNfc_TargetInit(  &nfc_t,
								        &target_params,
								        PHCS_BFLNFCCOMMON_PREAM_OMIT,
                                        PHCS_BFLNFCCOMMON_PREAM_OMIT,
                                        targetbuffer,
                                        EXAMPLE_TARGET_BUFFER_SIZE,
                                        nfcidi_buffer,
                                        nfcidt_buffer,
                                        (uint8_t)(passive ? 1 : 0),
                                        &target_endpoint,
                                        &rcio);
            }

            /* Initialise parameter structure */
            endp_param.p_iso14443_3 = &iso14443_3;
            endp_param.p_rc_op_ctl  = &rc_op_ctl;
            endp_param.p_rc_reg_ctl = &rc_reg_ctl;
            endp_param.p_nfcidt     =  nfcidt_buffer;
            /* Initialise user pointer to endpoint implementation */
            nfct_sup_param.p_user = &endp_param;
            nfc_t.SetPUser(&nfct_sup_param);
        }

		/* Reset Flag.*/
		endp_param.p_Flag = 0x00;

        /*
		 * Loop until the status is not PH_ERR_BFL_SUCCESS
		 */
        do
        {
			/* Dispatch the APDU */
            if(status == PH_ERR_BFL_SUCCESS)
			{
				istatus = nfc_t.Dispatch(&nfct_dispatch_param);
				if(istatus == PH_ERR_BFL_SUCCESS || (istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_DESELECTED)
				{
					printf("    Endpoint operation successful! \n");
				} else if ((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_RELEASED)
				{ /* break if status returned from enpoint callback is Released */
					break;
				} else
				{
					printf("[E] Error in endpoint operation (Status = %04X)! \n", istatus);
					grp.address = PHCS_BFL_JREG_COMMAND;
					status = rc_reg_ctl.GetRegister(&grp);
					if(status == PH_ERR_BFL_SUCCESS)
					{
						srp.address = PHCS_BFL_JREG_COMMAND;
						srp.reg_data = ((grp.reg_data & PHCS_BFL_JMASK_COMMAND_INV) | PHCS_BFL_JCMD_TRANSCEIVE);
						status = rc_reg_ctl.SetRegister(&srp);
						if(status != PH_ERR_BFL_SUCCESS)
							printf("    Error during Write of Addr: %02X, Status: %04X.\n", srp.address, status);
					} else
					{
						printf("    Error during Read of Addr: %02X, Status: %04X.\n", grp.address, status);
					}
				}
			}

            /* Check if drivers are on, if so switch them off! */
            grp.address = PHCS_BFL_JREG_TXCONTROL;
            status = rc_reg_ctl.GetRegister(&grp);
            if (status == PH_ERR_BFL_SUCCESS && (grp.reg_data & (PHCS_BFL_JBIT_TX2RFEN | PHCS_BFL_JBIT_TX1RFEN)))
            {
                srp.address = PHCS_BFL_JREG_TXCONTROL;
                srp.reg_data = (uint8_t)(grp.reg_data & ~(PHCS_BFL_JBIT_TX2RFEN | PHCS_BFL_JBIT_TX1RFEN));
                status = rc_reg_ctl.SetRegister(&srp);
                if(status != PH_ERR_BFL_SUCCESS)
                    printf("    Error during Write of Addr: %02X, Status: %04X.\n", srp.address, status);
            } else
			{
				if (status != PH_ERR_BFL_SUCCESS)
				{
					printf("    Error during Read of Addr: %02X, Status: %04X.\n", grp.address, status);
				}
			}

            /* Call transceive function to receive incoming data. */
            rcv_p.rx_buffer = buffer;
            rcv_p.rx_buffer_size = 64;
            printf("\n[I] Waiting for incoming data...\n");
            status = rcio.Receive(&rcv_p);
            if(status)
                printf("[E] ERROR! Receive command returned with status %04X \n", status);

			// If PSL receive status != 0 and ATR was sent before, the target shall not
			// answer to a twice PSL request.
			if ((status != PH_ERR_BFL_SUCCESS) && (endp_param.p_Flag  & NFC_TARGET_ATR_DONE_FLAG))
			{
				((phcsBflNfc_TargetCommParam_t*)nfc_t.mp_Members)->m_CtrlWord &= ~NFC_TARGET_CTRL_1ST_REQ_AFT_ATR;
			}

            /* Display data even if error occured */
            printf("[I] Received data: ");
            for(i=0; i<rcv_p.rx_buffer_size; i++)
                printf("%02X ", buffer[i]);
            printf("\n");
            length = rcv_p.rx_buffer_size;

            /* differ between re-activation in 106 passive and all other modes */
            if((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_DESELECTED)
            {
                printf("[I] Target has been deselected before. Do special check here. \n");

                grp.address = PHCS_BFL_JREG_RXMODE;
                status = rc_reg_ctl.GetRegister(&grp);
                if(status)
                    printf("[E] ERROR! reading register caused status %04X. \n", status);

                /* change configuration if speed differs from previous or active mode was chosen. */
                if((uint8_t)((grp.reg_data & PHCS_BFL_JMASK_SPEED)>>4) != g_RxSpeed || !passive)
                {
                    g_RxSpeed = g_TxSpeed = (uint8_t)((grp.reg_data & PHCS_BFL_JMASK_SPEED)>>4);
                    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
                    if(passive)
                        opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_PASSIVE;
                    else
                        opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;

                    opp.param_a = g_TxSpeed;
                    opp.param_b = g_RxSpeed;
                    status = rc_op_ctl.SetAttrib(&opp);
                    /* Check status and display error if something went wrong */
                    if(status != PH_ERR_BFL_SUCCESS)
	                {
                        printf("[E] Error reconfiguring hardware! Status = %04X \n", status);
		                return status;
	                } else
	                {
	                    printf("[I] Hardware reconfigured for Mode %d. New value for TxSpeed = %d, RxSpeed = %d. \n", passive, opp.param_a, opp.param_b);
	                }

                }

                if(g_RxSpeed == PHCS_BFLOPCTL_VAL_RF106K)
                {
                    if(passive)
                        nfct_dispatch_param.length = rcv_p.rx_buffer_size - 1;
                    else
                        nfct_dispatch_param.length = rcv_p.rx_buffer_size - 3;
                    nfct_dispatch_param.buffer = &(rcv_p.rx_buffer[1]);

                    /* activate DetectSync */
                    mrp.address = PHCS_BFL_JREG_MODE;
                    mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
                    mrp.set     = 1;
                    status = rc_reg_ctl.ModifyRegister(&mrp);
                    if (status != PH_ERR_BFL_SUCCESS)
                        printf("    Error! ModifyRegister: Status = %04X", status);
                } else
                {
                    nfct_dispatch_param.length = rcv_p.rx_buffer_size;
                    nfct_dispatch_param.buffer = rcv_p.rx_buffer;
                }
            } else
            {
                nfct_dispatch_param.length = rcv_p.rx_buffer_size;
                nfct_dispatch_param.buffer = rcv_p.rx_buffer;
            }
        } while((istatus & PH_ERR_BFL_ERR_MASK) != PH_ERR_BFL_TARGET_RELEASED);

    } else if(activate_MifareCard)
    {
		printf("[W] ISO14443-4 PICC is not implemented.\n");

    } else if(activate_FelicaCard)
    {
		/* No implementation yet */
        printf("[I] Sorry, no implementation for FeliCa.\n");
    } else
    {   /* Wrong path */
        printf("[E] Any other configuration found, invalid path.\n");
    }
    printf("[I] Target operation is finished.\n");

    return PH_ERR_BFL_SUCCESS;
}
