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


/*! \file NfcInitiatorInitialiseC.c
 *
 * $Author: mha $
 * $Revision: 1.19 $
 * $Date: Fri Jun 30 11:02:45 2006 $
 */

/* Include all necessary component headers for Mifare Reader operation and additional definitions.
   (BAL, RegisterControl, Iso14443Part3, OperationControl, RcIo, Mifare)                 */

/* For debugging purposes (printf) */
#include <stdio.h>

/* Common headers */
#include <phcsBflRefDefs.h>
#include <phcsBflHw1Reg.h>

/* Needed for any Operating Mode */
#include <phcsBflBal.h>
#include <phcsBflRegCtl.h>
#include <phcsBflOpCtl.h>
#include <phcsBflOpCtl_Hw1Ordinals.h>

/* For SetTimeOut */
//#include "ExampleUtilsC.h"
//#include "ExampleGlobals.h"

#include "reader_ISO_14443_P4.h"

/* Needed for NFC Initiator mode */
#include <phcsBflNfc.h>
#include <phcsBflNfcCommon.h>

/* Needed for activation in passive communication mode at 106kbps protocol */
#include <phcsBflI3P3A.h>
/* Needed for activation in passive communication mode at 212 & 424kbps protocol */
#include <phcsBflPolAct.h>

/*
 * If needed uncomment this line to suppress the warning
 * #pragma warning (disable: 4505)
 */
//#pragma warning (disable: 4505)

/*
 * Initialise function for active and passive communication
 */
phcsBfl_Status_t NfcInitiatorInitialise(	uint8_t	 passive,
									    uint8_t	 speed,
									    phcsBflRegCtl_t    *rc_reg_ctl,
									    phcsBflOpCtl_t		*rc_op_ctl,
									    uint8_t	*trxbuffer,
                                        uint8_t   *nfcid12)
{
    /* Register control parameters */
    phcsBflRegCtl_SetRegParam_t     srp;
    phcsBflRegCtl_GetRegParam_t     grp;
    phcsBflRegCtl_ModRegParam_t     mrp;

    /* Operation control parameters */
    phcsBflOpCtl_AttribParam_t      opp;

	/* Generic variables */
	phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    uint8_t i;

	mrp.self = grp.self = srp.self = rc_reg_ctl;
	opp.self = rc_op_ctl;

	/* Either passive or active at 106kBit/s */
	if (passive)
	{
		/*********************************************************************
		* P A S S I V E   M O D E                                            *
		*********************************************************************/
        if (speed == PHCS_BFLOPCTL_VAL_RF106K)
        {
		    /*********************************************************************
		    * 1 0 6   k b p s                                                    *
		    *********************************************************************/

            phcsBflI3P3A_t                iso14443_3;
            phcsBflI3P3A_Hw1Params_t   iso_14443_3_params;

            /* ISO14443-3A parameters       */
            phcsBflI3P3A_ReqAParam_t           req_p;
            phcsBflI3P3A_AnticollSelectParam_t sel_p;

            /* setting of self pointer */
	        req_p.self = sel_p.self = &iso14443_3;

		    /* Initialise ISO14443-3 component (choose Initiator == PCD behaviour) */
		    phcsBflI3P3A_Hw1Initialise(&iso14443_3, &iso_14443_3_params, rc_reg_ctl, PHCS_BFLI3P3A_INITIATOR);

		    /* Configuration for 106kBit/s passive (=Mifare Reader) */
		    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
		    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_MFRD_A;
		    opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
		    opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
		    status = rc_op_ctl->SetAttrib(&opp);
		    if(status != PH_ERR_BFL_SUCCESS)
			    printf("*** ERROR performing Mifare Reader configuration! Status = %04X \n", status);

		    /* activate Initiator mode of PN51x (Reader) */
		    srp.address = PHCS_BFL_JREG_CONTROL;
		    srp.reg_data = PHCS_BFL_JBIT_INITIATOR;
		    status = rc_reg_ctl->SetRegister(&srp);
		    if(status != PH_ERR_BFL_SUCCESS)
			    printf("*** ERROR! SetRegister: Address=%02X, Data=%02X, Status = %04X \n",
					    srp.address, srp.reg_data, status);

		    /* Set timeout for RF-on before first command to 5ms */
		    status = SetTimeOut(rc_reg_ctl, rc_op_ctl, 5000, 0);
	        if (status != PH_ERR_BFL_SUCCESS)
            {
		        printf("[E] Could not set the timer of the PN51x.\n");
            } else
            {
		        printf("[I] Timer of the PN51x reconfigured.\n");
            }

		    /* Activate field (driver in auto mode) */
		    mrp.address = PHCS_BFL_JREG_TXAUTO;
		    mrp.mask    = PHCS_BFL_JBIT_INITIALRFON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
		    mrp.set     = 1;
		    status = rc_reg_ctl->ModifyRegister(&mrp);
		    if(status)
			    printf("*** ERROR! Status = %04X \n", status);

		    /* Activate Receiver */
		    mrp.set     = 0;
		    mrp.address = PHCS_BFL_JREG_COMMAND;
		    mrp.mask    = PHCS_BFL_JBIT_RCVOFF;
		    status = rc_reg_ctl->ModifyRegister(&mrp);
		    if(status)
			    printf("*** ERROR! Status = %04X \n", status);

		    /* Wait until RF field is switched on and timer has passed */
            grp.address = PHCS_BFL_JREG_COMMIRQ;
            do
            {
                status = rc_reg_ctl->GetRegister(&grp);
            } while(!(grp.reg_data & PHCS_BFL_JBIT_TIMERI) && status == PH_ERR_BFL_SUCCESS);
	        /* Check status and display error if something went wrong */
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! GetRegister: Address=%02X, Data=%02X, Status = %04X \n",
                        grp.address, grp.reg_data, status);

            /* Clear the interrupt request flag afterwards */
            srp.address  = PHCS_BFL_JREG_COMMIRQ;
            srp.reg_data = PHCS_BFL_JBIT_TIMERI;
            status = rc_reg_ctl->SetRegister(&srp);
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! SetRegister: Address=%02X, Data=%02X, Status = %04X \n",
                        srp.address, srp.reg_data, status);

		    /* Set timeout for REQA, ANTICOLL, SELECT to 200us */
		    status = SetTimeOut(rc_reg_ctl, rc_op_ctl, 200, 0);
	        if (status != PH_ERR_BFL_SUCCESS)
            {
		        printf("[E] Could not set the timer of the PN51x.\n");
            } else
            {
		        printf("[I] Timer of the PN51x reconfigured.\n");
            }

		    /*
		    * Do activation according to ISO14443A Part3
		    * Prepare the Request command
		    */
		    trxbuffer[0] = 0;
		    trxbuffer[1] = 0;
		    req_p.req_code = PHCS_BFLI3P3A_REQIDL;    /* Set Request command code (or Wakeup: ISO14443_3_REQALL) */
		    req_p.atq      = trxbuffer;                     /* Let Request use the defined return buffer */
		    status = iso14443_3.RequestA(&req_p);   /* Start Request command */
		    /*
		    * In normal operation this command returns one of the following three return codes:
		    * - PH_ERR_BFL_SUCCESS				At least one card has responded
		    * - PH_ERR_BFL_COLLISION_ERROR		At least two cards have responded
		    * - PH_ERR_BFL_IO_TIMEOUT			No response at all
		    */
		    if(status == PH_ERR_BFL_SUCCESS || (status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
		    {
			    /* There was at least one response */
			    if((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
				    printf("[W] Multiple Cards detected!! Collision occurred during Request!\n");
			    printf("[I] ATQA = %02X %02X \n", req_p.atq[0],req_p.atq[1]);
		    } else
		    {
			    /* No response at all */
			    printf("*** ERROR! RequestA: RequestCode=%02X, ATQA=%02X %02X, Status = %04X \n",
					    req_p.req_code, req_p.atq[0],req_p.atq[1], status);
				/* It doesn't make sense to go further */
				return PH_ERR_BFL_IO_TIMEOUT;
		    }

		    /* Prepare data for combined anticollision/select command to get one complete ID */
		    for(i=0;i<12;i++)
			    trxbuffer[i]      = 0x00;

		    sel_p.uid          = trxbuffer;                 /* Put serial buffer for ID */
		    sel_p.uid_length   = 0;                      /* No bits are known from ID, so set to 0 */
		    /* Start Anticollision/Select command */
		    status = iso14443_3.AnticollSelect(&sel_p);
		    /* Check return code: If the command was ok, is always returns PH_ERR_BFL_SUCCESSS.
		    * If nothing was received, parameter uid_length is 0. */
		    if(status == PH_ERR_BFL_SUCCESS)
		    {
			    /* Display UID and SAK if everything was OK */
			    printf("[I] UID: ");
			    for(i=0;i<(sel_p.uid_length/8);i++)
				    printf("%02X", sel_p.uid[i]);
			    printf("\tSAK: %02X \n",sel_p.sak);
                /* Copy data to return buffer for further usage */
                for(i=0; i<sel_p.uid_length/8; i++)
                    nfcid12[i] = sel_p.uid[i];
		    } else
		    {
			    printf("*** ERROR! Anticollision/Select: Status = %04X \n", status);
				/* It doesn't make sense to go further */
				return PH_ERR_BFL_IO_TIMEOUT;
		    }

		    /*
		    * Up to this point the ISO14443-3A protocol has been used
		    * Now part 3 is finished and one has to choose between the several branches. Here is the
		    * example how to use the Mifare Standard specific command set of the BFL
		    */

		    /*
		    * Configure PN51x to cut and add Protocol byte(s) at 106kbps
		    * This is not valid for higher data rates!
		    */
		    mrp.set     = 1;
		    mrp.address = PHCS_BFL_JREG_MODE;
		    mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
		    status = rc_reg_ctl->ModifyRegister(&mrp);
		    if(status)
			    printf("*** ERROR! Status = %04X \n", status);

        } else if (speed == PHCS_BFLOPCTL_VAL_RF212K || speed == PHCS_BFLOPCTL_VAL_RF424K)
        {
		    /*********************************************************************
		    * 2 1 2   o r   4 2 4   k b p s                                                    *
		    *********************************************************************/

            phcsBflPolAct_t              fp;
            phcsBflPolAct_Hw1Params_t fp_params;

	        /* FeliCa Polling parameters    */
            phcsBflPolAct_PollingParam_t pol_p;

	        /* Variable to calculate timeout value for timeslots */
            uint32_t    timeout;
            uint8_t     index, length;

            /* init self pointer */
            pol_p.self = &fp;

            /* Initialise FeliCa Polling component */
            phcsBflPolAct_Hw1Init(&fp, &fp_params, rc_reg_ctl, PHCS_BFLPOLACT_INITIATOR);

            /*
	        * Configure PN51x to act as a Initator in passive communication mode above 106 kbps (Felica Reader).
            * Remark: operation control function do not perform a softreset and does not know anything
            *         about the antenna configuration, so these things have to be handled here!
	        */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_PAS_POL;
            if(speed == PHCS_BFLOPCTL_VAL_RF212K)
            {
                opp.param_a          = PHCS_BFLOPCTL_VAL_RF212K;
                opp.param_b          = PHCS_BFLOPCTL_VAL_RF212K;
            } else
            {
                opp.param_a          = PHCS_BFLOPCTL_VAL_RF424K;
                opp.param_b          = PHCS_BFLOPCTL_VAL_RF424K;
            }
	        /* Apply the settings above */
            status = rc_op_ctl->SetAttrib(&opp);
            /* Check status and display error if something went wrong */
            if(status)
                printf("*** ERROR performing Initiator configuration for passive mode at 212 or 424 kbps! Status = %04X \n", status);

		    /* activate Initiator mode of PN51x (Reader) */
            srp.address = PHCS_BFL_JREG_CONTROL;
            srp.reg_data = PHCS_BFL_JBIT_INITIATOR;
            status = rc_reg_ctl->SetRegister(&srp);
		    if(status != PH_ERR_BFL_SUCCESS)
			    printf("*** ERROR! SetRegister: Address=%02X, Data=%02X, Status = %04X \n",
					    srp.address, srp.reg_data, status);

            /* Set the timer to auto mode, 2s using operation control commands before HF is switched
	        * on to guarantee ECMA-340 compliance of Polling procedure for 212kbps. */

            /*
	        * Activate automatic start/stop of timer
	        */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;         /* Set operation control group parameter to Timer */
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_AUTO;     /* Set function for Timer automatic mode */
            opp.param_a          = PHCS_BFLOPCTL_VAL_ON;              /* Use parameter a to switch it on */
            status = rc_op_ctl->SetAttrib(&opp);
            /* Check status and display error if something went wrong */
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! OperationControl settings Timer auto mode. Status = %04X \n", status);

            /*
	        * Set prescaler steps to 100us
	        */
	        opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;				/* Set operation control group parameter to Timer */
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_PRESCALER;    /* Prescaler */
	        /* Calculation of the values: 100us = 6.78MHz/681 = 6.78MHz/0x2A9 */
            opp.param_b          = 0x02;                        /* High value (max 0x0F) */
            opp.param_a          = 0xA9;                        /* Low value */
            status = rc_op_ctl->SetAttrib(&opp);
            /* Check status and display error if something went wrong */
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! OperationControl settings Timer Prescaler. Status = %04X \n", status);

            /* Set the reload value */
            timeout = 20000;            /* set to 20000 for 2s (startup time definition of ECMA)  */
	        opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_TMR;				/* Set operation control group parameter to Timer */
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_TMR_RELOAD;
            opp.param_a          = (uint8_t)timeout;           /* Low value */
            opp.param_b          = (uint8_t)(timeout >> 8);    /* High value (max 0xFF) */
            status = rc_op_ctl->SetAttrib(&opp);
            /* Check status and display error if something went wrong */
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! OperationControl settings Timer Reload. Status = %04X \n", status);

            /* Activate the field (automatically if there is no external field detected) */
            mrp.address = PHCS_BFL_JREG_TXAUTO;
            mrp.mask    = PHCS_BFL_JBIT_INITIALRFON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
            mrp.set     = 1;
            status = rc_reg_ctl->ModifyRegister(&mrp);

            /* After switching on the drivers wait until the timer Interrupt occurs, so that
            the field is on and the 5ms delay have been passed. This 5ms are defined in
	        the ECMA 340 standard.
	        */
            grp.address = PHCS_BFL_JREG_COMMIRQ;
            do
            {
                status = rc_reg_ctl->GetRegister(&grp);
            } while(!(grp.reg_data & PHCS_BFL_JBIT_TIMERI) && status == PH_ERR_BFL_SUCCESS);
	        /* Check status and display error if something went wrong */
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! GetRegister: Address=%02X, Data=%02X, Status = %04X \n",
                        grp.address, grp.reg_data, status);

            /* Clear the interrupt request flag afterwards */
            srp.address  = PHCS_BFL_JREG_COMMIRQ;
            srp.reg_data = PHCS_BFL_JBIT_TIMERI;
            status = rc_reg_ctl->SetRegister(&srp);
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! SetRegister: Address=%02X, Data=%02X, Status = %04X \n",
                        srp.address, srp.reg_data, status);

		    /* Set timeout for Polling request with one timeslot to 3.6ms */
		    status = SetTimeOut(rc_reg_ctl, rc_op_ctl, 3600, 0);
	        if (status != PH_ERR_BFL_SUCCESS)
            {
		        printf("[E] Could not set the timer of the PN51x.\n");
            } else
            {
		        printf("[I] Timer of the PN51x reconfigured.\n");
            }

            /* activate Receiver for Communication
            The RcvOff bit and the PowerDown bit are cleared, the command is not changed. */
            srp.address  = PHCS_BFL_JREG_COMMAND;
            srp.reg_data = PHCS_BFL_JCMD_NOCMDCHANGE;
            status = rc_reg_ctl->SetRegister(&srp);
            /* check status and display error if something went wrong */
            if(status != PH_ERR_BFL_SUCCESS)
                printf("*** ERROR! SetRegister: Address=%02X, Data=%02X, Status = %04X \n",
                        srp.address, srp.reg_data, status);

            /*
	        * Now the timing conditions are met and the settings are appropriate for activation in passive mode.
	        * The commands can be configured now.
	        */

	        /*
	        * Perform activation in passive communication mode above 106lbps (FeliCa Polling)
	        */

	        /* Prepare data for FeliCa polling */
            trxbuffer[0] = 0xFF;				/* Systemcode 0 */
            trxbuffer[1] = 0xFF;				/* Systemcode 1 */
            trxbuffer[2] = 0x00;				/* If systemcode is requested from card: 01, else: 00 */
            trxbuffer[3] = 0x01;				/* Timeslotnumber (00, 01, 03, 07, 0F) */
            pol_p.tx_buffer      = trxbuffer;   /* Let Polling use the initialised buffer */
            pol_p.tx_buffer_size = 4;           /* Number of bytes initialised in TxBuffer */
            pol_p.rx_buffer      = trxbuffer + 4;                   /* Let Polling use the defined return buffer */
            pol_p.rx_buffer_size = EXAMPLE_TRX_BUFFER_SIZE - 4;     /* Maximum rx buffer size */
            /* Start the POLLING_REQUEST command */
            status = fp.Polling(&pol_p);
            /* Check return code:
            * If PH_ERR_BFL_SUCCESS is received, display IDm, PMm and Systemcode
            * else display error code.
	        */
            if(status)
		        /* Error has occured */
                printf("*** ERROR! FeliCaPolling: Status = %04X \n", status);
            else
		        /* No Error, display the values from the target */
            {
                index = 0;
				while(pol_p.rx_buffer_size)
				{
					length = pol_p.rx_buffer[index];
                    index++;
					printf("[I] Length: 0x%02X\t IDm: 0x", length);
					/* Skip command byte */
					index++;
					/* Display the IDm */
					for(i=0;i<8;i++)
                    {
						printf("%02X",pol_p.rx_buffer[index]);
                        index++;
                    }
					printf("\tPMm: 0x");
					/* Display the PMm */
					for(i=0;i<8;i++)
                    {
						printf("%02X",pol_p.rx_buffer[index]);
                        index++;
                    }
					/* Display System Code if applicable, otherwise not */
					if(length == 0x14) {
						printf("\tSystemcode: 0x%02X%02X \n", pol_p.rx_buffer[index], pol_p.rx_buffer[index+1]);
						index+=2;
					} else {
						printf("\n");
					}
					pol_p.rx_buffer_size = (uint8_t)(pol_p.rx_buffer_size - length);
				}
                /* Copy first NFCID2 received to return buffer for further use */
                for(i=0;i<pol_p.rx_buffer[0] - 2; i++)
                    nfcid12[i] = pol_p.rx_buffer[2+i];
            }
        } else
        {
            status = PH_ERR_BFL_INVALID_PARAMETER;
        }
	} else
	{
		/*********************************************************************
		* A C T I V E   M O D E                                              *
		**********************************************************************/
		opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
		opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;
		if(speed == PHCS_BFLOPCTL_VAL_RF106K)
		{
			opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
			opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
		} else if (speed == PHCS_BFLOPCTL_VAL_RF212K)
		{
			opp.param_a          = PHCS_BFLOPCTL_VAL_RF212K;
			opp.param_b          = PHCS_BFLOPCTL_VAL_RF212K;
		} else if (speed == PHCS_BFLOPCTL_VAL_RF424K)
		{
			opp.param_a          = PHCS_BFLOPCTL_VAL_RF424K;
			opp.param_b          = PHCS_BFLOPCTL_VAL_RF424K;
		} else
        {
            return PH_ERR_BFL_INVALID_PARAMETER;
        }
		status = rc_op_ctl->SetAttrib(&opp);
		if(status != PH_ERR_BFL_SUCCESS)
			printf("*** ERROR performing Mifare Reader configuration! Status = %04X \n", status);

		/* activate Initiator mode of PN51x (Reader) */
		srp.address = PHCS_BFL_JREG_CONTROL;
		srp.reg_data = PHCS_BFL_JBIT_INITIATOR;
		status = rc_reg_ctl->SetRegister(&srp);
		if(status != PH_ERR_BFL_SUCCESS)
			printf("*** ERROR! SetRegister: Address=%02X, Data=%02X, Status = %04X \n",
					srp.address, srp.reg_data, status);

		/* Set timeout for Inital RF guard time (Tirfg) */
		status = SetTimeOut(rc_reg_ctl, rc_op_ctl, 5000, 0);
	    if (status != PH_ERR_BFL_SUCCESS)
        {
		    printf("[E] Could not set the timer of the PN51x.\n");
        } else
        {
		    printf("[I] Timer of the PN51x reconfigured.\n");
        }

		/* activate field (driver in auto mode) */
		mrp.address = PHCS_BFL_JREG_TXAUTO;
		mrp.mask    = PHCS_BFL_JBIT_INITIALRFON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
		mrp.set     = 1;
		status = rc_reg_ctl->ModifyRegister(&mrp);
		if(status)
			printf("*** ERROR! Status = %04X \n", status);

		/* Activate Receiver */
		mrp.set     = 0;
		mrp.address = PHCS_BFL_JREG_COMMAND;
		mrp.mask    = PHCS_BFL_JBIT_RCVOFF;
		status = rc_reg_ctl->ModifyRegister(&mrp);
		if(status)
			printf("*** ERROR! Status = %04X \n", status);

		/*
		* Configure PN51x to cut and add Protocol byte(s) at 106kbps
		* This is not valid for higher data rates!
		*/
		mrp.set     = 1;
		mrp.address = PHCS_BFL_JREG_MODE;
		mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
		status = rc_reg_ctl->ModifyRegister(&mrp);
		if(status)
			printf("*** ERROR! Status = %04X \n", status);

        /* After switching on the drivers wait until the timer Interrupt occures, so that
        the field is on and the 5ms delay have been passed. This 5ms are defined in
	    the ECMA 340 standard.
	    */
        grp.address = PHCS_BFL_JREG_COMMIRQ;
        do
        {
            status = rc_reg_ctl->GetRegister(&grp);
        } while(!(grp.reg_data & PHCS_BFL_JBIT_TIMERI) && status == PH_ERR_BFL_SUCCESS);
	    /* Check status and display error if something went wrong */
        if(status != PH_ERR_BFL_SUCCESS)
            printf("*** ERROR! GetRegister: Address=%02X, Data=%02X, Status = %04X \n",
                    grp.address, grp.reg_data, status);

        /* Clear the interrupt request flag afterwards */
        srp.address  = PHCS_BFL_JREG_COMMIRQ;
        srp.reg_data = PHCS_BFL_JBIT_TIMERI;
        status = rc_reg_ctl->SetRegister(&srp);
        if(status != PH_ERR_BFL_SUCCESS)
            printf("*** ERROR! SetRegister: Address=%02X, Data=%02X, Status = %04X \n",
                    srp.address, srp.reg_data, status);

        /* Reset collision avoidence function after having switched on the RF for further communication */
        mrp.address = PHCS_BFL_JREG_TXAUTO;
        mrp.mask    = PHCS_BFL_JBIT_CAON;
        mrp.set     = 0;
        status = rc_reg_ctl->ModifyRegister(&mrp);
        if(status)
            printf("*** ERROR! ModifyRegister: Address=%02X, Mask=%02X, Status = %04X \n",
                    mrp.address, mrp.mask, status);

	}

    return status;
}


phcsBfl_Status_t NfcInitiatorWakeupTarget(uint8_t	        passive,
									    uint8_t		    speed,
									    phcsBflRegCtl_t	       *rc_reg_ctl,
									    phcsBflOpCtl_t		   *rc_op_ctl,
                                        phcsBflNfc_Initiator_t  *nfc_i,
									    uint8_t	       *trxbuffer,
                                        uint8_t          *nfcidi_buffer,
                                        uint8_t          *nfcid12)

{
    /* Register control parameters  */
    phcsBflRegCtl_ModRegParam_t          mrp;

    /* Operation control parameters */
    phcsBflOpCtl_AttribParam_t           opp;

    /* Generic variables */
	phcsBfl_Status_t     status = PH_ERR_BFL_SUCCESS;
    uint8_t i;

	mrp.self = rc_reg_ctl;
	opp.self = rc_op_ctl;

	/* Either passive or active at 106kBit/s */
	if (passive)
	{
		/*********************************************************************
		* P A S S I V E   M O D E                                            *
		*********************************************************************/
        if (speed == PHCS_BFLOPCTL_VAL_RF106K)
        {
		    /*********************************************************************
		    * 1 0 6   k b p s                                                    *
		    *********************************************************************/

            phcsBflI3P3A_t                  iso14443_3;
            phcsBflI3P3A_Hw1Params_t iso_14443_3_params;

            /* ISO14443-3A parameters       */
            phcsBflI3P3A_ReqAParam_t           req_p;
            phcsBflI3P3A_AnticollSelectParam_t sel_p;

            /* setting of self pointer */
	        req_p.self = sel_p.self = &iso14443_3;

		    /* Initialise ISO14443-3 component (choose Initiator == PCD behaviour) */
		    phcsBflI3P3A_Hw1Initialise(&iso14443_3, &iso_14443_3_params, rc_reg_ctl, PHCS_BFLI3P3A_INITIATOR);

		    /* Configuration for 106kBit/s passive (=Mifare Reader) */
		    opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
		    opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_MFRD_A;
		    opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
		    opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
		    status = rc_op_ctl->SetAttrib(&opp);
		    if(status != PH_ERR_BFL_SUCCESS)
			    printf("*** ERROR performing Mifare Reader configuration! Status = %04X \n", status);

		    /*
		    * Deactivate protocol specific preamble handling if used before.
		    */
		    mrp.set     = 0;
		    mrp.address = PHCS_BFL_JREG_MODE;
		    mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
		    status = rc_reg_ctl->ModifyRegister(&mrp);
		    if(status)
			    printf("*** ERROR! Status = %04X \n", status);

            /* Set timeout for REQA, ANTICOLL, SELECT to 200us */
		    status = SetTimeOut(rc_reg_ctl, rc_op_ctl, 200, 0);
	        if (status != PH_ERR_BFL_SUCCESS)
            {
		        printf("[E] Could not set the timer of the PN51x.\n");
            } else
            {
		        printf("[I] Timer of the PN51x reconfigured.\n");
            }

		    /*
		    * Do activation according to ISO14443A Part3
		    * Prepare the Request command
		    */
		    trxbuffer[0] = 0;
		    trxbuffer[1] = 0;
		    req_p.req_code = PHCS_BFLI3P3A_REQALL;    /* Set Wakeup command code */
		    req_p.atq      = trxbuffer;                     /* Let Request use the defined return buffer */
		    status = iso14443_3.RequestA(&req_p);   /* Start Request command */
		    /*
		    * In normal operation this command returns one of the following three return codes:
		    * - PH_ERR_BFL_SUCCESS				At least one card has responded
		    * - PH_ERR_BFL_COLLISION_ERROR		At least two cards have responded
		    * - PH_ERR_BFL_IO_TIMEOUT			No response at all
		    */
		    if(status == PH_ERR_BFL_SUCCESS || (status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
		    {
			    /* There was at least one response */
			    if((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_COLLISION_ERROR)
				    printf("[W] Multiple Cards detected!! Collision occurred during Request!\n");
			    printf("[I] ATQA = %02X %02X \n", req_p.atq[0],req_p.atq[1]);
		    } else
			    /* No response at all */
			    printf("*** ERROR! RequestA: RequestCode=%02X, ATQA=%02X %02X, Status = %04X \n",
					    req_p.req_code, req_p.atq[0],req_p.atq[1], status);

		    /* Prepare data for combined anticollision/select command to get one complete ID */
		    for(i=0;i<12;i++)
			    trxbuffer[i]      = 0x00;

		    sel_p.uid          = trxbuffer;              /* Put serial buffer for ID */
		    sel_p.uid_length   = 0;                      /* No bits are known from ID, so set to 0 */
		    /* Start Anticollision/Select command */
		    status = iso14443_3.AnticollSelect(&sel_p);
		    /* Check return code: If the command was ok, is always returns PH_ERR_BFL_SUCCESSS.
		    * If nothing was received, parameter uid_length is 0. */
		    if(status == PH_ERR_BFL_SUCCESS)
		    {
			    /* Display UID and SAK if everything was OK */
			    printf("[I] UID: ");
			    for(i=0;i<(sel_p.uid_length/8);i++)
				    printf("%02X", sel_p.uid[i]);
			    printf("\tSAK: %02X \n",sel_p.sak);
                /* Copy data to return buffer for further usage */
                for(i=0; i<sel_p.uid_length/8; i++)
                    nfcid12[i] = sel_p.uid[i];
		    } else
		    {
			    printf("*** ERROR! Anticollision/Select: Status = %04X \n", status);
		    }

        } else if (speed == PHCS_BFLOPCTL_VAL_RF212K || speed == PHCS_BFLOPCTL_VAL_RF424K)
        {
		    /*********************************************************************
		    * 2 1 2   o r   4 2 4   k b p s                                                    *
		    *********************************************************************/

            phcsBflPolAct_t                  fp;
            phcsBflPolAct_Hw1Params_t     fp_params;

	        /* FeliCa Polling parameters    */
            phcsBflPolAct_PollingParam_t     pol_p;

	        /* Variable to calculate timeout value for timeslots */
            uint8_t   index, length;

            /* init self pointer */
            pol_p.self = &fp;

            /* Initialise FeliCa Polling component */
            phcsBflPolAct_Hw1Init(&fp, &fp_params, rc_reg_ctl, PHCS_BFLPOLACT_INITIATOR);

            /*
	        * Configure PN51x to act as a Initator in passive communication mode above 106 kbps (Felica Reader).
            * Remark: operation control function do not perform a softreset and does not know anything
            *         about the antenna configuration, so these things have to be handled here!
	        */
            opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
            opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_PAS_POL;
            if(speed == PHCS_BFLOPCTL_VAL_RF212K)
            {
                opp.param_a          = PHCS_BFLOPCTL_VAL_RF212K;
                opp.param_b          = PHCS_BFLOPCTL_VAL_RF212K;
            } else
            {
                opp.param_a          = PHCS_BFLOPCTL_VAL_RF424K;
                opp.param_b          = PHCS_BFLOPCTL_VAL_RF424K;
            }
	        /* Apply the settings above */
            status = rc_op_ctl->SetAttrib(&opp);
            /* Check status and diplay error if something went wrong */
            if(status)
                printf("*** ERROR performing Initiator configuration for passive mode at 212 or 424 kbps! Status = %04X \n", status);

		    /* Set timeout for Polling request with one timeslot to 3.6ms */
		    status = SetTimeOut(rc_reg_ctl, rc_op_ctl, 3600, 0);
	        if (status != PH_ERR_BFL_SUCCESS)
            {
		        printf("[E] Could not set the timer of the PN51x.\n");
            } else
            {
		        printf("[I] Timer of the PN51x reconfigured.\n");
            }

	        /*
	        * Perform activation in passive communication mode above 106kbps (FeliCa Polling)
	        */

	        /* Prepare data for FeliCa polling */
            trxbuffer[0] = 0xFF;				/* Systemcode 0 */
            trxbuffer[1] = 0xFF;				/* Systemcode 1 */
            trxbuffer[2] = 0x00;				/* If systemcode is requested from card: 01, else: 00 */
            trxbuffer[3] = 0x01;				/* Timeslotnumber (00, 01, 03, 07, 0F) */
            pol_p.tx_buffer      = trxbuffer;   /* Let Polling use the initialised buffer */
            pol_p.tx_buffer_size = 4;           /* Number of bytes initialised in TxBuffer */
            pol_p.rx_buffer      = trxbuffer + 4;                   /* Let Polling use the defined return buffer */
            pol_p.rx_buffer_size = EXAMPLE_TRX_BUFFER_SIZE - 4;     /* Maximum rx buffer size */
            /* Start the POLLING_REQUEST command */
            status = fp.Polling(&pol_p);
            /* Check return code:
            * If PH_ERR_BFL_SUCCESS is received, display IDm, PMm and Systemcode
            * else display error code.
	        */
            if(status)
		        /* Error has occured */
                printf("*** ERROR! FeliCaPolling: Status = %04X \n", status);
            else
		        /* No Error, display the values from the target */
            {
                index = 0;
				while(pol_p.rx_buffer_size)
				{
					length = pol_p.rx_buffer[index];
                    index++;
					printf("[I] Length: 0x%02X\t IDm: 0x", length);
					/* Skip command byte */
					index++;
					/* Display the IDm */
					for(i=0;i<8;i++)
                    {
						printf("%02X",pol_p.rx_buffer[index]);
                        index++;
                    }
					printf("\tPMm: 0x");
					/* Display the PMm */
					for(i=0;i<8;i++)
                    {
						printf("%02X",pol_p.rx_buffer[index]);
                        index++;
                    }
					/* Display System Code if applicable, otherwise not */
					if(length == 0x14) {
						printf("\tSystemcode: 0x%02X%02X \n", pol_p.rx_buffer[index], pol_p.rx_buffer[index+1]);
						index+=2;
					} else {
						printf("\n");
					}
					pol_p.rx_buffer_size = (uint8_t)(pol_p.rx_buffer_size - length);
				}
                /* Copy first NFCID2 received to return buffer for further use */
                for(i=0;i<pol_p.rx_buffer[0] - 2; i++)
                    nfcid12[i] = pol_p.rx_buffer[2+i];
            }
        } else
        {
            status = PH_ERR_BFL_INVALID_PARAMETER;
        }
	} else
	{
		/*********************************************************************
		* A C T I V E   M O D E                                              *
		**********************************************************************/
		opp.group_ordinal    = PHCS_BFLOPCTL_GROUP_MODE;
		opp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_ACTIVE;
		if(speed == PHCS_BFLOPCTL_VAL_RF106K)
		{
			opp.param_a          = PHCS_BFLOPCTL_VAL_RF106K;
			opp.param_b          = PHCS_BFLOPCTL_VAL_RF106K;
		} else if (speed == PHCS_BFLOPCTL_VAL_RF212K)
		{
			opp.param_a          = PHCS_BFLOPCTL_VAL_RF212K;
			opp.param_b          = PHCS_BFLOPCTL_VAL_RF212K;
		} else if (speed == PHCS_BFLOPCTL_VAL_RF424K)
		{
			opp.param_a          = PHCS_BFLOPCTL_VAL_RF424K;
			opp.param_b          = PHCS_BFLOPCTL_VAL_RF424K;
		} else
        {
            return PH_ERR_BFL_INVALID_PARAMETER;
        }
		status = rc_op_ctl->SetAttrib(&opp);
		if(status != PH_ERR_BFL_SUCCESS)
			printf("*** ERROR performing Mifare Reader configuration! Status = %04X \n", status);
	}

	/*
	* Configure PN51x to cut and add Protocol byte(s) at 106kbps
	* This is not valid for higher data rates!
	*/
	mrp.set     = 1;
	mrp.address = PHCS_BFL_JREG_MODE;
	mrp.mask    = PHCS_BFL_JBIT_DETECTSYNC;
	status = rc_reg_ctl->ModifyRegister(&mrp);
	if(status)
		printf("*** ERROR! Status = %04X \n", status);

    if (passive)
    {
        /* NFC Initiator parameters */
        phcsBflNfc_InitiatorAtrReqParam_t       atr_p;
        /* initialize self pointer */
        atr_p.self = nfc_i;

	    /* Set the timeout value to maximum, this simplifies debugging. */
	    status = SetTimeOut(rc_reg_ctl, rc_op_ctl, 5000000, PN51X_STEPS_500US);
	    if (status != PH_ERR_BFL_SUCCESS)
        {
		    printf("[E] Could not set the timer of the PN51x.\n");
        } else
        {
		    printf("[I] Timer of the PN51x reconfigured.\n");
        }

        if(speed != PHCS_BFLOPCTL_VAL_RF106K)
        {
            for(i=0; i<PHCS_BFLNFCCOMMON_ID_LEN; i++)
                nfcidi_buffer[i] = nfcid12[i];
        }

        /* Perform ATR_REQ (define ATR_REQ parameter and activate command) */
        atr_p.nfcid_it = nfcidi_buffer;
        atr_p.did      = 0x01;          /* DID 1 should be used for 1st device */
        atr_p.bs_it    = 0x00;          /* No data rates above 424kbps are available for sending */
        atr_p.br_it    = 0x00;          /* No data rates above 424kbps are available for receive */
        /* Max. length 63 bytes valid, general bytes added, NAD is used */
        atr_p.pp_it    = PHCS_BFLNFCCOMMON_ATR_PP_LR_64 | PHCS_BFLNFCCOMMON_ATR_PP_NAD_PRES | PHCS_BFLNFCCOMMON_ATR_PP_GI_PRES;
        atr_p.g_it     = NULL;
        atr_p.glen_i   = 0;

        /* Issue the ATR_REQ command */
	    status = nfc_i->AtrRequest(&atr_p);
        if(status)
            printf("*** ERROR! ATR_REQ: Status = %04X \n", status);
        else
        {
		    /* There was a response from a target */
            printf("[I] ATR_REQ successful. NFCIDt: ");
            for(i=0;i<10;i++)
                printf("%02X",atr_p.nfcid_it[i]);
            printf("  DIDt: %02X  BSt: %02X  BRt: %02X",atr_p.did, atr_p.bs_it, atr_p.br_it);
            printf("  TO: %02X  PPt: %02X\n", atr_p.to_t, atr_p.pp_it);
        }
    } else
    {
        /* NFC Initiator parameters */
        phcsBflNfc_InitiatorWupReqParam_t       wup_p;
        /* initialize self pointer */
        wup_p.self = nfc_i;

        /* Perform WUP_REQ */
	    /* Activate same device with new DID */
	    wup_p.did = 0x02;
	    status = nfc_i->WupRequest(&wup_p);
	    if(status)
		    printf("*** ERROR! Status = %04X \n", status);
	    else
		    printf("[I] Got a WakeUp Response.\n");
    }

    return status;
}
