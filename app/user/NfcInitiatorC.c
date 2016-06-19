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


/*! \file NfcInitiatorC.c
 *
 * $Author: mha $
 * $Revision: 1.37 $
 * $Date: Fri Jun 23 07:23:57 2006 $
 *
 * Comment:
 *  Example code for handling of BFL.
 *
 */

/* Calculation of the values: 100us = 6.78MHz/(677+1) = 6.78MHz/(0x2A5+1) */
#define PRESCALER_100_US            0x02A5
#define PRESCALER_500_US            0x0D3D

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
#include <phcsBflNfc.h>
#include <phcsBflNfcCommon.h>
#include "NfcInitiatorC.h"

/* Needed for some global definitions (buffer size) */
//#include "ExampleGlobals.h"
/* Needed for setting the timeout value */
//#include "ExampleUtilsC.h"

#include "reader_ISO_14443_P4.h"
/* Prototype for the NFC Initiator initialisation (according to ECMA340) */
phcsBfl_Status_t NfcInitiatorInitialise(	uint8_t									 passive,
									    uint8_t									 speed,
									    phcsBflRegCtl_t									*rc_reg_ctl,
									    phcsBflOpCtl_t									*rc_op_ctl,
									    uint8_t									*trxbuffer,
                                        uint8_t                                   *nfcid12);

/* Prototype for the WakeUp procedure handling after a deselect command (according to ECMA340) */
phcsBfl_Status_t NfcInitiatorWakeupTarget(uint8_t	        passive,
									    uint8_t		    speed,
									    phcsBflRegCtl_t	       *rc_reg_ctl,
									    phcsBflOpCtl_t		   *rc_op_ctl,
                                        phcsBflNfc_Initiator_t  *nfc_i,
									    uint8_t	       *trxbuffer,
                                        uint8_t          *nfcidi_buffer,
                                        uint8_t           *nfcid12);

/*
 * This example shows how to act as NFC Initiator
 */
phcsBfl_Status_t ActivateNfcInitiator(void *comHandle,
								    uint8_t isPassive,
								    uint8_t opSpeed )
{
	/* Declaration of RCL Component Structures (auto), C variant: */
    phcsBflBal_t            bal;
    phcsBflRegCtl_t         rc_reg_ctl;
    phcsBflIo_t             rcio;
    phcsBflOpCtl_t          rc_op_ctl;
    phcsBflNfc_Initiator_t   nfc_i;
	/* BAL either for Windows xor for Linux */
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
    phcsBflNfc_InitiatorCommParams_t            nfci_params;

	/*
	 * Declaration of used parameter structures
	 */
    /* Register control parameters */
    phcsBflRegCtl_ModRegParam_t     mrp;

    /* Operation control parameters */
    phcsBflOpCtl_AttribParam_t      opp;

    /* NFC Initiator parameters */
    phcsBflNfc_InitiatorAtrReqParam_t   atr_p;
    phcsBflNfc_InitiatorPslReqParam_t   psl_p;
    phcsBflNfc_InitiatorDepReqParam_t   dep_p;
    //phcsBflNfc_InitiatorWupReqParam_t   wup_p;
    phcsBflNfc_InitiatorAttReqParam_t   att_p;
    phcsBflNfc_InitiatorDslReqParam_t   dsl_p;
    phcsBflNfc_InitiatorRlsReqParam_t   rls_p;
    phcsBflNfc_InitiatorToxReqParam_t   tox_p;
    phcsBflNfc_InitiatorSetTRxPropParam_t   stp_p;

	/*
	 * Declaration of variables, buffer, ...
	 */
    /* Status variable */
    phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    /* Buffer for data transfer (send/receive) */
    uint8_t trxbuffer[EXAMPLE_TRX_BUFFER_SIZE];
    /* User buffer */
    uint8_t userbuffer[EXAMPLE_USER_BUFFER_SIZE];
    /* Buffer to store received ID of the initiator */
    uint8_t nfcidi_buffer[PHCS_BFLNFCCOMMON_ID_LEN] = { 0x0A, 0x09, 0x08, 0x07, 0x06, 0x06, 0x07, 0x08, 0x09, 0x0A };
    /* Buffer to store received ID of the target */
    uint8_t nfcidt_buffer[PHCS_BFLNFCCOMMON_ID_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    /* Buffer for NFCID1 or NFCID2, depending on the activation */
    uint8_t nfcid12_buffer[18];
    /* Length of general bytes */
    const uint8_t general_buffer_len = 4;
    /* Buffer to store general bytes */
    uint8_t general_buffer[4] = { 0xDD, 0xEE, 0xFF, 0x00};
    /* Variable for protocol */
    uint8_t my_to = 14;       /* Set local timeout value to maximum */
    /* Start timeout value for timeout calculation up to TO parameter of ATR_RES is equal to 7. */
    uint32_t  timeout = 302;   /* This value precalculated out of the ISO18092 */
    /* Counter variable */
    uint16_t i;

	/* Initialisation of self parameters */
	mrp.self   = &rc_reg_ctl;
    opp.self   = &rc_op_ctl;
    atr_p.self = &nfc_i;
    psl_p.self = &nfc_i;
    dep_p.self = &nfc_i;
    //wup_p.self = &nfc_i;
    att_p.self = &nfc_i;
    dsl_p.self = &nfc_i;
    rls_p.self = &nfc_i;
    tox_p.self = &nfc_i;
    stp_p.self = &nfc_i;

    /*
	 * Build up stack for serial communication.
     * Start with the lowest layer, so that the upper ones may refer to this already.
	 */
#if 0    // las083
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
    phcsBflIo_Hw1Init(&rcio, &rc_io_params, &rc_reg_ctl, PHCS_BFLIO_INITIATOR);
    /* Initialise Operation Control component */
    phcsBflOpCtl_Hw1Init(&rc_op_ctl, &rc_op_ctl_params, &rc_reg_ctl);


	/*
	 * Initialise NFC Initiator component (open protocol implementation of NFC)
	 * Needed in any case, independed of speed, passive/active.
	 */
	phcsBflNfc_InitiatorInit(&nfc_i,
					               &nfci_params,
					                PHCS_BFLNFCCOMMON_PREAM_OMIT,
					                PHCS_BFLNFCCOMMON_PREAM_OMIT,
					                trxbuffer,
					                EXAMPLE_TRX_BUFFER_SIZE,
					                nfcidt_buffer,
                                   &rcio);
	switch( opSpeed & 0x03 )
	{
	case 0x00:
		printf("[I] Data Rate: 106kBit/s ");
		break;
	case 0x01:
		printf("[I] Data Rate: 212kBit/s ");
		break;
	case 0x02:
		printf("[I] Data Rate: 424kBit/s ");
		break;
	default:
		printf("[E] Invalid data rate!\n");
		return PH_ERR_BFL_OTHER_ERROR;
	}

	if (isPassive )
	{
		printf("in Passive mode.\n");
		isPassive = 0x01;
	}
	else
	{
		printf("in Active mode.\n");
		isPassive = 0x00;
	}


    /* initialize hardware and do activation according to to selected mode and speed */
	status = NfcInitiatorInitialise( isPassive,         /* communication mode: either active or passive */
                                     opSpeed,           /* speed of communication: 106, 212 or 424 kbps */
                                    &rc_reg_ctl,        /* pointer to initialized register control component */
                                    &rc_op_ctl,         /* pointer to initialized operation control component */
                                     trxbuffer,         /* general communication buffer */
                                     nfcid12_buffer);   /* buffer to receive either nfcid1 or nfcid2 for
                                                           passive activation. To be used for reactivation. */
	if (status != PH_ERR_BFL_SUCCESS)
	{
		printf("[I] Error initialising Initiator.\n");
		return status;
	} else
	{
		printf("[I] PN51x has now been initialised.\n");
	}


	/* Set the timer to the default value. */
	status = SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 5000000, PN51X_STEPS_500US);
	if (status != PH_ERR_BFL_SUCCESS)
    {
		printf("[E] Could not set the timer of the PN51x.\n");
    } else
    {
		printf("[I] Timer of the PN51x reconfigured.\n");
    }

    /* Use NFCID2 as NFCID3 for passive communication mode at 212 and 424 kbps. */
    if(isPassive && (opSpeed != PHCS_BFLOPCTL_VAL_RF106K))
    {
        for(i=0; i<PHCS_BFLNFCCOMMON_ID_LEN; i++)
            nfcidi_buffer[i] = nfcid12_buffer[i];
    }

    /* Perform ATR_REQ (define ATR_REQ parameter and activate command) */
    atr_p.nfcid_it = nfcidi_buffer;
    atr_p.did      = 0x01;          /* DID 1 should be used for 1st device */
    atr_p.bs_it    = 0x00;          /* No data rates above 424kbps are available for sending */
    atr_p.br_it    = 0x00;          /* No data rates above 424kbps are available for receive */
    /* Max. length 63 bytes valid, general bytes added, NAD is not used */
    atr_p.pp_it    = PHCS_BFLNFCCOMMON_ATR_PP_LR_64 | PHCS_BFLNFCCOMMON_ATR_PP_GI_PRES;
    atr_p.g_it     = general_buffer;
    atr_p.glen_i   = general_buffer_len;

    /* Issue the ATR_REQ command */
	status = nfc_i.AtrRequest(&atr_p);
    if(status)
        printf("*** ERROR! ATR_REQ exits with status = %04X \n", status);
    else
    {
		/* There was a response from a target */
        printf("[I] NFCIDt: ");
        for(i=0;i<10;i++)
            printf("%02X",nfcidt_buffer[i]);
        printf("  DIDt: %02X  BSt: %02X  BRt: %02X",atr_p.did, atr_p.bs_it, atr_p.br_it);
        printf("  TO: %02X  PPt: %02X\n", atr_p.to_t, atr_p.pp_it);
        /* store value for further calculations locally */
        my_to = atr_p.to_t;
    }

    /* change timeout value according to received TO value */
    timeout   = (timeout << my_to) / 100;

    /* prepare buffer for Timer settings (Prescaler and Reload value) */
    userbuffer[0] = (uint8_t) ( PRESCALER_100_US & 0xFF );        /* Low value */
    userbuffer[1] = (uint8_t) ( (PRESCALER_100_US >> 8) & 0x0F ); /* High value (max 0x0F) */
    userbuffer[2] = (uint8_t) ( timeout & 0xFF );          /* Low value */
    userbuffer[3] = (uint8_t) ( (timeout >> 8) & 0xFF );   /* High value (max 0xFF) */

    /* do adjustments of timer with preset values */
    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;
    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR;
    opp.param_a          = 1;                   /* Timer Auto mode ON */
    opp.param_b          = 0;                   /* Timer auto reload OFF */
    opp.buffer           = userbuffer;          /* Buffer used for Prescaler and Reload value */
    status = rc_op_ctl.SetAttrib(&opp);
    /* Check status and display error if something went wrong */
    if(status != PH_ERR_BFL_SUCCESS)
	{
        printf("[E] Error setting timer! Status = %04X \n", status);
		return status;
	}



    /* Perform PSL_REQ if something to be changed from activation */
    if(opSpeed)
    {   /* Current setting is differnet to 106kbps, use for PPS 106kbps */
        psl_p.BRS = PHCS_BFLNFCCOMMON_PSL_BRS_DR_1 | PHCS_BFLNFCCOMMON_PSL_BRS_DS_1;    /* Choose 106kbps for further comunication */
    }else
    {   /* Current setting is 106kbps, use for PPS 212kbps */
        psl_p.BRS = PHCS_BFLNFCCOMMON_PSL_BRS_DR_2 | PHCS_BFLNFCCOMMON_PSL_BRS_DS_2;    /* Choose 212kbps for further comunication */
    }
    psl_p.FSL = PHCS_BFLNFCCOMMON_PSL_FSL_LR_64;                      /* Max. 63 bytes to send allowed */
    status = nfc_i.PslRequest(&psl_p);
    /* Check status and display error if something went wrong */
    if(status)
    {
        printf("*** ERROR! PSL_REQ exit with status = %04X \n", status);
    } else
    {
        printf("[I] PSL_REQ successful. BRS = %02X, FSL = %02X.\n", psl_p.BRS, psl_p.FSL);
        /* Reconfigure the IC according to the new data rate. */
        opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
        if(isPassive)
        {
            if(opSpeed)
            {
                opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_MFRD_A;
            } else
            {
                opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_PAS_POL;
            }
        } else
        {
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;
        }
        if(opSpeed)
        {
            opp.param_a = PHCS_BFLOPCTL_VAL_RF106K;
            opp.param_b = PHCS_BFLOPCTL_VAL_RF106K;
        } else
        {
            opp.param_a = PHCS_BFLOPCTL_VAL_RF212K;
            opp.param_b = PHCS_BFLOPCTL_VAL_RF212K;
        }
        status = rc_op_ctl.SetAttrib(&opp);
        /* Check status and display error if something went wrong */
        if(status != PH_ERR_BFL_SUCCESS)
	    {
            printf("[E] Error reconfiguring hardware! Status = %04X \n", status);
		    return status;
	    } else
	    {
	        printf("[I] Hardware reconfigured. \n    New value for Mode: %s, TxSpeed %d, RxSpeed %d. \n",
	            (isPassive) ? "passive" : "active",
	            opp.param_b, opp.param_a);
	    }

        /* Set Detect Sync after reconfiguration for frame conformance. */
        mrp.address = PHCS_BFL_JREG_MODE;
        mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
        mrp.set     = 1;
        status = rc_reg_ctl.ModifyRegister(&mrp);
        if(status != PH_ERR_BFL_SUCCESS)
	    {
            printf("[E] Error reconfiguring DetectSync! Status = %04X \n", status);
        }

        /* Set protocol specific data (if changes occur during PSL, this shall be called again)
        TrxBuffer = buffer, length = 64, no manual preamble: */
        stp_p.p_trxbuffer       = trxbuffer;
        stp_p.trxbuffersize     = EXAMPLE_TRX_BUFFER_SIZE;
        stp_p.tx_preamble_type  = PHCS_BFLNFCCOMMON_PREAM_OMIT;
        stp_p.rx_preamble_type  = PHCS_BFLNFCCOMMON_PREAM_OMIT;
        nfc_i.SetTRxProp(&stp_p);
    }


    /*
	 * Perform a DEP_REQ
	 */
	printf("[I] Sending a DEP_REQ... ");

	for (i = 0; i < 100; i++)
	{
		// This is just some dummy data...
        userbuffer[i] = (uint8_t) (i + 0x10);
	}

    dep_p.nad                = NULL;        /* NAD is not activated, so use NULL */
    dep_p.output_buffer      = userbuffer;  /* Data to send is in the buffer */
    dep_p.output_buffer_size = EXAMPLE_USER_BUFFER_SIZE;
    dep_p.input_buffer       = userbuffer;  /* response data in user buffer */
    dep_p.input_buffer_size  = i;
    /* Start DEP_REQ */
	status = nfc_i.DepRequest(&dep_p);
    do
    {
		if (status == PH_ERR_BFL_SUCCESS)
		{
			/* All is fine, just leave the loop */
			break;
		}
        if((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_SET_TOX)
        {
			/* The target needs some additional time, Initiator has to handle the TOX request */

			printf(" +++ Set extended waiting time (value = 0x%02X)!\n",
                /* If returned status is Target_Set_TOX, activate State machine again */
                nfc_i.RequestedToxValue(&tox_p));

            /* Set timeout value according to new value received. Timeout value is set right from above. */
            timeout = timeout * nfc_i.RequestedToxValue(&tox_p);

            /* prepare buffer for Timer settings (Prescaler and Reload value) */
            userbuffer[0] = (uint8_t) ( PRESCALER_100_US & 0xFF );        /* Low value */
            userbuffer[1] = (uint8_t) ( (PRESCALER_100_US >> 8) & 0x0F ); /* High value (max 0x0F) */
            userbuffer[2] = (uint8_t) ( timeout & 0xFF );          /* Low value */
            userbuffer[3] = (uint8_t) ( (timeout >> 8) & 0xFF );   /* High value (max 0xFF) */

            /* do adjustments of timer with preset values */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR;
            opp.param_a          = 1;                   /* Timer Auto mode ON */
            opp.param_b          = 0;                   /* Timer auto reload OFF */
            opp.buffer           = userbuffer;          /* Buffer used for Prescaler and Reload value */
            status = rc_op_ctl.SetAttrib(&opp);
            /* Check status and display error if something went wrong */
            if(status != PH_ERR_BFL_SUCCESS)
	        {
                printf("[E] Error setting timer! Status = %04X \n", status);
		        return status;
	        }

			// This has to be set to a value greather than zero
			dep_p.output_buffer_size = EXAMPLE_USER_BUFFER_SIZE;

			/* Re-Start DEP_REQ with same parameters as above */
            status = nfc_i.DepRequest(&dep_p);

        }
		if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_RESET_TOX)
        {
			/* After the TOX has been processed, it will be switched back to the default TO value */

            /* Reset the timeout to default values. */
            printf(" +++ Reset waiting time to default!\n");

            /* change timeout value according to received TO value */
            timeout   = (timeout << my_to) / 100;

            /* prepare buffer for Timer settings (Prescaler and Reload value) */
            userbuffer[0] = (uint8_t) ( PRESCALER_100_US & 0xFF );        /* Low value */
            userbuffer[1] = (uint8_t) ( (PRESCALER_100_US >> 8) & 0x0F ); /* High value (max 0x0F) */
            userbuffer[2] = (uint8_t) ( timeout & 0xFF );          /* Low value */
            userbuffer[3] = (uint8_t) ( (timeout >> 8) & 0xFF );   /* High value (max 0xFF) */

            /* do adjustments of timer with preset values */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR;
            opp.param_a          = 1;                   /* Timer Auto mode ON */
            opp.param_b          = 0;                   /* Timer auto reload OFF */
            opp.buffer           = userbuffer;          /* Buffer used for Prescaler and Reload value */
            status = rc_op_ctl.SetAttrib(&opp);
            /* Check status and display error if something went wrong. */
            if(status != PH_ERR_BFL_SUCCESS)
	        {
                printf("[E] Error setting timer! Status = %04X \n", status);
		        return status;
	        }

			/* This has to be set to a value greather than zero */
			dep_p.output_buffer_size = EXAMPLE_USER_BUFFER_SIZE;

            /* Re-Start DEP_REQ with same parameters as above */
            status = nfc_i.DepRequest(&dep_p);

        } else
		{
            /* Other DEP status: */
            printf(" +++ DEP-status = 0x%04X.\n", status);
		}

    } while ( ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_RESET_TOX) ||
              ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_TARGET_SET_TOX) );

	if (status == PH_ERR_BFL_SUCCESS)
    {
        printf(" Success, received data: ");
        for(i=0;i<dep_p.output_buffer_size;i++)
            printf("%02X ", dep_p.output_buffer[i]);
		printf("\n");
    }


    /* Perform DEP_REQ (Attention) (no parameters have to be set) */
	printf("[I] Sending an Attention Request... ");
    status = nfc_i.AttRequest(&att_p);
    if(status)
        printf(" Error. Response failed, status = %04X \n", status);
	else
		printf(" Successful. Got an Attention Response.\n");

    /* Perform DSL_REQ (no parameters have to be set) */
    status = nfc_i.DslRequest(&dsl_p);
    if(status)
        printf("*** ERROR! Status = %04X \n", status);
	else
		printf("[I] Got a Deselect Response.\n");

	/* Set the timer to the default value. */
	status = SetTimeOut(&rc_reg_ctl, &rc_op_ctl, 5000000, PN51X_STEPS_500US);
	if (status != PH_ERR_BFL_SUCCESS)
    {
		printf("[E] Could not set the timer of the PN51x.\n");
    } else
    {
		printf("[I] Timer of the PN51x reconfigured.\n");
    }

    /*
       Start re-activation of Target after Deselect dependent on the commonication mode chosen before.
       A new initial data rate for communication might be chosen. According to the communication mode
       and data rate a different flow has to be followed.
     */
    status = NfcInitiatorWakeupTarget(	isPassive,          /* Communication mode must be the same! */
								        opSpeed,            /* Speed may be one out of the 3 possible
                                                               for activation. */
								       &rc_reg_ctl,         /* Pointer to initialized register control component */
								       &rc_op_ctl,          /* Pointer to initialized operation control component */
                                       &nfc_i,              /* Pointer to initialized Initiator channel */
								        trxbuffer,          /* buffer for communication */
                                        nfcidi_buffer,      /* buffer for nfcid of the Initiator */
                                        nfcid12_buffer);    /* buffer for nfcid1 or nfcid2. */


	/* Perform RLS_REQ (no parameters have to be set) */
	status = nfc_i.RlsRequest(&rls_p);
	if(status)
		printf("*** ERROR! Status = %04X \n", status);
	else
		printf("[I] Got a Release Response.\n");

    return status;
}
