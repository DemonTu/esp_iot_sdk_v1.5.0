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

/*! \file phcsBflAux_Hw1.c
 *
 * Project: Object Oriented Library Framework Auxilliary Component.
 *
 *  Source: phcsBflAux_Hw1.c
 * $Author: mha $
 * $Revision: 1.9 $
 * $Date: Tue Aug 22 09:36:13 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  PN51x-specific variant of Auxilliary.
 *
 * Functions:
 *
 *
 * History:
 *  MHa: Generated 13. May 2003
 *  MHa: Migrated to MoReUse September 2005
 *
 */




#include <phcsBflRefDefs.h>
#include <phcsBflAux.h>
#include <phcsBflHw1Reg.h>

//#include "includes.h"
#include "ets_sys.h"
#include "osapi.h"




phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflAux_Hw1Command(phcsBflAux_CommandParam_t *command_param)
{
    phcsBfl_Status_t             PHCS_BFL_MEMLOC_REM status  = PH_ERR_BFL_SUCCESS;  /* general status information */
    phcsBfl_Status_t             PHCS_BFL_MEMLOC_REM istatus = PH_ERR_BFL_SUCCESS;  /* communication status information about */

    phcsBflRegCtl_SetRegParam_t       PHCS_BFL_MEMLOC_REM setreg_param;
    phcsBflRegCtl_GetRegParam_t       PHCS_BFL_MEMLOC_REM getreg_param;
    phcsBflRegCtl_GetRegParam_t       PHCS_BFL_MEMLOC_REM getreg_param2;
    phcsBflRegCtl_ModRegParam_t       PHCS_BFL_MEMLOC_REM modify_param;
    phcsBflRegCtl_SetMultiRegParam_t  PHCS_BFL_MEMLOC_REM setmulti_param;
    phcsBflRegCtl_GetMultiRegParam_t  PHCS_BFL_MEMLOC_REM getmulti_param;

    uint16_t        PHCS_BFL_MEMLOC_REM maxRxBytes;
    uint8_t         PHCS_BFL_MEMLOC_REM commIrqEn_Int   = 0;
    uint8_t         PHCS_BFL_MEMLOC_REM divIrqEn_Int    = 0;
    uint8_t         PHCS_BFL_MEMLOC_REM waitForComm_Int = PHCS_BFL_JBIT_ERRI | PHCS_BFL_JBIT_TXI;
    uint8_t         PHCS_BFL_MEMLOC_REM waitForDiv_Int  = 0;
    uint8_t         PHCS_BFL_MEMLOC_REM doReceive   = 0;
    uint8_t         PHCS_BFL_MEMLOC_REM currentCmd  = 0;
    uint_fast32_t tmot = 0;
    /* Wait For Event Callback var's: */
    phcsBflAux_WaitEventCbParam_t PHCS_BFL_MEMLOC_REM wait_event_cb_param;

    /* self is set only once. Not modified during operation!! */
    setreg_param.self   = command_param->lower;
    getreg_param.self   = command_param->lower;
    getreg_param2.self  = command_param->lower;
    modify_param.self   = command_param->lower;
    setmulti_param.self = command_param->lower;
    getmulti_param.self = command_param->lower;

    /* store incoming buffer length for receive function check */
    maxRxBytes = command_param->rx_bytes;
    command_param->rx_bytes = 0;     /* set response length to 0 if timeout occures */
    /* deactivate parameter .target_send if initiator is activated */
    if (command_param->initiator == PHCS_BFLAUX_INITIATOR)
    {
        command_param->target_send = 0;
    }

    /* disable command or set to transceive */
    getreg_param.address = PHCS_BFL_JREG_COMMAND;
    PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))
    currentCmd = getreg_param.reg_data;

    setreg_param.address = PHCS_BFL_JREG_COMMAND;
    if((command_param->cmd == PHCS_BFL_JCMD_TRANSCEIVE) || (command_param->cmd == PHCS_BFL_CMD_TRANSCEIVE_TO))
    {
        /* current command is not transceive and current command is not autocoll */
        if(((currentCmd & PHCS_BFL_JMASK_COMMAND) != PHCS_BFL_JCMD_TRANSCEIVE) &&
           ((currentCmd & PHCS_BFL_JMASK_COMMAND) != PHCS_BFL_JCMD_AUTOCOLL))
        {
            setreg_param.reg_data = (uint8_t)((currentCmd & PHCS_BFL_JMASK_COMMAND_INV) | PHCS_BFL_JCMD_TRANSCEIVE);
            PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))
        }
       // printf("not transceive\r\n");
    }
    else
    {
        /* clear current command */
        setreg_param.reg_data = (uint8_t)(currentCmd & PHCS_BFL_JMASK_COMMAND_INV);
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))

        /* reset the FIFO in case there has been something received upfront */
        setreg_param.address  = PHCS_BFL_JREG_FIFOLEVEL;
        setreg_param.reg_data = PHCS_BFL_JBIT_FLUSHBUFFER;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))

       // printf("clear fifo\r\n");
    }

   // printf("cmd = %ld\r\n",command_param->cmd);
    switch(command_param->cmd)
    {
        case PHCS_BFL_JCMD_IDLE:         /* values are 00, so return immediately after all bytes written to FIFO */
            waitForComm_Int = 0;
            waitForDiv_Int  = 0;
            break;
        case PHCS_BFL_JCMD_CONFIG:       /*  */
            commIrqEn_Int = PHCS_BFL_JBIT_IDLEI;
            waitForComm_Int = PHCS_BFL_JBIT_IDLEI;

            if(command_param->tx_bytes == 0)
            {
                doReceive = 1;
            }
            break;
        case PHCS_BFL_JCMD_RANDOMIDS:    /*  */
            commIrqEn_Int = PHCS_BFL_JBIT_IDLEI;
            waitForComm_Int = PHCS_BFL_JBIT_IDLEI;
            break;
        case PHCS_BFL_JCMD_CALCCRC:      /* values are 00, so return immediately after all bytes written to FIFO */
            waitForComm_Int = 0;
            waitForDiv_Int  = 0;
            break;
        case PHCS_BFL_JCMD_TRANSMIT:     /*  */
            commIrqEn_Int   = PHCS_BFL_JBIT_TXI | PHCS_BFL_JBIT_TIMERI;
            waitForComm_Int = PHCS_BFL_JBIT_TXI;
            break;
        case PHCS_BFL_JCMD_RECEIVE:      /*  */
            commIrqEn_Int   = PHCS_BFL_JBIT_RXI | PHCS_BFL_JBIT_TIMERI | PHCS_BFL_JBIT_ERRI;
            waitForComm_Int = PHCS_BFL_JBIT_RXI | PHCS_BFL_JBIT_TIMERI;
            doReceive = 1;
            break;
        case PHCS_BFL_JCMD_TRANSCEIVE:   /*  */
            if(command_param->initiator == PHCS_BFLAUX_INITIATOR)
            {
                command_param->target_send = 0;
                commIrqEn_Int = PHCS_BFL_JBIT_RXI | PHCS_BFL_JBIT_TIMERI | PHCS_BFL_JBIT_ERRI;
                waitForComm_Int = PHCS_BFL_JBIT_RXI | PHCS_BFL_JBIT_TIMERI;
                doReceive = 1;
            }
            else
            {
                if(command_param->target_send != 0)
                {
                    commIrqEn_Int = PHCS_BFL_JBIT_TXI;
                    waitForComm_Int = PHCS_BFL_JBIT_TXI;
                }
                else
                {
                    commIrqEn_Int = PHCS_BFL_JBIT_RXI | PHCS_BFL_JBIT_TIMERI | PHCS_BFL_JBIT_ERRI;
                    waitForComm_Int = PHCS_BFL_JBIT_RXI | PHCS_BFL_JBIT_TIMERI;
                    doReceive = 1;
                }
            }
            break;
        case PHCS_BFL_JCMD_AUTOCOLL:     /*  */
            commIrqEn_Int   = PHCS_BFL_JBIT_IDLEI | PHCS_BFL_JBIT_TIMERI | PHCS_BFL_JBIT_ERRI;
            divIrqEn_Int    = PHCS_BFL_JBIT_MODEI;
            waitForComm_Int = PHCS_BFL_JBIT_IDLEI | PHCS_BFL_JBIT_TIMERI | PHCS_BFL_JBIT_ERRI;
            waitForDiv_Int  = PHCS_BFL_JBIT_MODEI;
            break;
        case PHCS_BFL_JCMD_AUTHENT:      /*  */
            commIrqEn_Int = PHCS_BFL_JBIT_IDLEI | PHCS_BFL_JBIT_TIMERI | PHCS_BFL_JBIT_ERRI;
            waitForComm_Int = PHCS_BFL_JBIT_IDLEI | PHCS_BFL_JBIT_TIMERI;
            break;
        case PHCS_BFL_JCMD_SOFTRESET:    /* values are 0x00 for IrqEn and for waitFor, nothing to do */
            waitForComm_Int = 0;
            waitForDiv_Int  = 0;
            break;
        case PHCS_BFL_CMD_TRANSCEIVE_TO: /* used for FeliCa polling command only */
            command_param->target_send = 0;
            commIrqEn_Int = PHCS_BFL_JBIT_RXI | PHCS_BFL_JBIT_TXI | PHCS_BFL_JBIT_TIMERI | PHCS_BFL_JBIT_ERRI;
            waitForComm_Int = PHCS_BFL_JBIT_TIMERI;
            doReceive = 1;
            break;
        default:
            status = PH_ERR_BFL_UNSUPPORTED_COMMAND;
    }

    if (status == PH_ERR_BFL_SUCCESS)
    {
        /* activate necessary communication Irq's */
        getreg_param.address = PHCS_BFL_JREG_COMMIEN;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))
        setreg_param.address = PHCS_BFL_JREG_COMMIEN;
        setreg_param.reg_data = (uint8_t)(getreg_param.reg_data | commIrqEn_Int);
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))

        /* activate necessary other Irq's */
        getreg_param.address = PHCS_BFL_JREG_DIVIEN;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))
        setreg_param.address = PHCS_BFL_JREG_DIVIEN;
        setreg_param.reg_data = (uint8_t)(getreg_param.reg_data | divIrqEn_Int);
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))

        /* Check if data has to be transferred to the FIFO. If not, just skip it */
        if (command_param->tx_bytes > 0)
        {
#ifdef  DEBUG_YYF
            uint_fast8_t i, num;
            uint8_t *tx_buf;
            num = command_param->tx_bytes;
            tx_buf = command_param->tx_buffer;
            printf("send%u:", num);
            for (i=0; i<num; i++)
            {
                printf("%02X ", tx_buf[i]);
            }
            printf("\r\n");
#endif
            /* write data to FIFO using multiple register access */
            setmulti_param.address = PHCS_BFL_JREG_FIFODATA;
            setmulti_param.buffer  = command_param->tx_buffer;
            setmulti_param.length  = command_param->tx_bytes;
            PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegisterMultiple(&setmulti_param))
		}

        /* Here we distinguish between the local polling (serves mainly as an example) or the event-triggered,
           application-defined callback. */
        if (command_param->wait_event_cb != NULL)
        {
            /* Jump into app-CB and do there whatever the app has defined for us to do...: */

            /*
             * The only thing which has to be known by this event sensing function is the COM port handle.
             * This handle is handed over by the user_ref variable.
             */
            wait_event_cb_param.user_ref        = command_param->user_ref;

            /*
             * Additionally set all necessary internal variables to be used in the callback.
             */
            wait_event_cb_param.commIrqEn            = &commIrqEn_Int;
            wait_event_cb_param.divIrqEn             = &divIrqEn_Int;
            wait_event_cb_param.waitForComm          = &waitForComm_Int;
            wait_event_cb_param.waitForDiv           = &waitForDiv_Int;
            wait_event_cb_param.command              = command_param->cmd;
            wait_event_cb_param.initiator_not_target = command_param->initiator;
            wait_event_cb_param.target_send          = command_param->target_send;
            wait_event_cb_param.lower                = command_param->lower;
            wait_event_cb_param.cb_data              = NULL;
            wait_event_cb_param.cb_data_length       = 0;

            /*
             * Now call the event sensing function and return until the IRQ pin has generated an event
             */
            istatus = command_param->wait_event_cb(&wait_event_cb_param);

            /* Read out the interrupt registers */
            getreg_param2.address = PHCS_BFL_JREG_COMMIRQ;
            command_param->lower->GetRegister(&getreg_param2);

            getreg_param.address = PHCS_BFL_JREG_DIVIRQ;
            command_param->lower->GetRegister(&getreg_param);

        }
        else
        {
            /* do seperate action if command to be executed is transceive */
            if ((command_param->cmd == PHCS_BFL_JCMD_TRANSCEIVE) || (command_param->cmd == PHCS_BFL_CMD_TRANSCEIVE_TO))
            {
                /* check if Initiator mode or Target Transmit mode */
                if ((command_param->target_send != 0) || (command_param->initiator == PHCS_BFLAUX_INITIATOR))
                {
                    /* TRx is always an endless loop, Initiator and Target must set STARTSEND. */
                    modify_param.address = PHCS_BFL_JREG_BITFRAMING;
                    modify_param.set = 1;
                    modify_param.mask = PHCS_BFL_JBIT_STARTSEND;
                    PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->ModifyRegister(&modify_param))
                }
            }
            else
            {
                setreg_param.address = PHCS_BFL_JREG_COMMAND;
                setreg_param.reg_data = (uint8_t)((currentCmd & PHCS_BFL_JMASK_COMMAND_INV) | command_param->cmd);
                PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))
            }

            /* Poll for IRQ: */
            getreg_param2.address = PHCS_BFL_JREG_COMMIRQ;    /* setreg_param used to save RAM */
            getreg_param.address = PHCS_BFL_JREG_DIVIRQ;
            getreg_param2.reg_data = 0x00;           /* setreg_param used to save RAM */
            getreg_param.reg_data = 0x00;

            tmot = 6000;
            while ((!(waitForComm_Int & getreg_param2.reg_data) && !(waitForDiv_Int  & getreg_param.reg_data) )
                 && (tmot--))
            {
                PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param2))
                PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))
            }
#ifdef  DEBUG_YYF
            if (0 == tmot)
            {
                printf("tmot--\ncommInt:%#02x,r:%#02x\nDivInt:%#02X R:%#02x\r\n",
                    waitForComm_Int, getreg_param2.reg_data,
                    waitForDiv_Int, getreg_param.reg_data);
            }
#endif
        }

        /* Store IRQ bits for clearance afterwards */
        waitForComm_Int = (uint8_t)(waitForComm_Int & getreg_param2.reg_data);
        waitForDiv_Int  = (uint8_t)(waitForDiv_Int & getreg_param.reg_data);

        /* Check if timeout has occurred */
       // printf ("reg 2 = %lx\r\n",getreg_param2.reg_data);

        if ((getreg_param2.reg_data & PHCS_BFL_JBIT_TIMERI) == PHCS_BFL_JBIT_TIMERI)
        {
            istatus = PH_ERR_BFL_IO_TIMEOUT;
            //printf(" timeout \r\n");
            /* restart transceive for next loop */
            if ((command_param->cmd == PHCS_BFL_JCMD_TRANSCEIVE) || (command_param->cmd == PHCS_BFL_CMD_TRANSCEIVE_TO))
            {
                setreg_param.address  = PHCS_BFL_JREG_COMMAND;
                setreg_param.reg_data = (uint8_t)((currentCmd & PHCS_BFL_JMASK_COMMAND_INV) | PHCS_BFL_JCMD_TRANSCEIVE);
                PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))
            }
        }
    }

    /* disable all interrupt sources */
    modify_param.address = PHCS_BFL_JREG_COMMIEN;
    modify_param.set     = 0;
    modify_param.mask    = commIrqEn_Int;       /* keep IrqInv bit untouched */
    PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->ModifyRegister(&modify_param))

    /* parameters: set and mask are the same as above, so there is no need to set new!!
       keep IrqPushPull bit untouched */
    modify_param.address = PHCS_BFL_JREG_DIVIEN;
    modify_param.mask    = divIrqEn_Int;
    PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->ModifyRegister(&modify_param))

    if(doReceive && ( (istatus == PH_ERR_BFL_SUCCESS) || ((istatus == PH_ERR_BFL_IO_TIMEOUT) && (command_param->cmd == PHCS_BFL_CMD_TRANSCEIVE_TO) ) ) )
    {
        /* read number of bytes received (used for Error check and correct transaction) */
        getreg_param.address = PHCS_BFL_JREG_FIFOLEVEL;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))
        command_param->rx_bytes = getreg_param.reg_data;

        getreg_param.address = PHCS_BFL_JREG_CONTROL;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))
        command_param->rx_bits = (uint8_t)(getreg_param.reg_data & 0x07);

        getreg_param.address = PHCS_BFL_JREG_ERROR;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))

        /* set status information if error occurred */
        if(getreg_param.reg_data != 0x00)
        {
            if((getreg_param.reg_data & PHCS_BFL_JBIT_COLLERR) == PHCS_BFL_JBIT_COLLERR)
            {
                istatus = PH_ERR_BFL_COLLISION_ERROR;         /* Collision Error */
            }
            else if((getreg_param.reg_data & PHCS_BFL_JBIT_PARITYERR) == PHCS_BFL_JBIT_PARITYERR)
            {
                istatus = PH_ERR_BFL_PARITY_ERROR;            /* Parity Error */
            }else
            {
            }

            if((getreg_param.reg_data & PHCS_BFL_JBIT_PROTERR) == PHCS_BFL_JBIT_PROTERR)
            {
                istatus = PH_ERR_BFL_PROTOCOL_ERROR;          /* Protocoll Error */
            }
            else if((getreg_param.reg_data & PHCS_BFL_JBIT_BUFFEROVFL) == PHCS_BFL_JBIT_BUFFEROVFL)
            {
                istatus = PH_ERR_BFL_BUFFER_OVERFLOW;         /* BufferOverflow Error */
            }
            else if((getreg_param.reg_data & PHCS_BFL_JBIT_CRCERR) == PHCS_BFL_JBIT_CRCERR)
            {   /* CRC Error */
                if( (command_param->rx_bytes == 0x01) &&
                    (maxRxBytes >= 1) &&
                    ((command_param->rx_bits == 0x04) ||
                     (command_param->rx_bits == 0x00)) )
                {   /* CRC Error and only one byte received might be a Mifare (N)ACK */
                    getreg_param.address = PHCS_BFL_JREG_FIFODATA;
                    PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegister(&getreg_param))
                    command_param->rx_buffer[0] = getreg_param.reg_data;
                    command_param->rx_bytes = 1;
                    istatus = PH_ERR_BFL_ACK_SUPPOSED;        /* (N)ACK supposed */
                } else
                {
                    istatus = PH_ERR_BFL_CRC_ERROR;           /* CRC Error */
                }
            }
            else if((getreg_param.reg_data & PHCS_BFL_JBIT_RFERR) == PHCS_BFL_JBIT_RFERR)
            {
                istatus = PH_ERR_BFL_RF_ERROR;                /* RF Error */
            }
            else if((getreg_param.reg_data & PHCS_BFL_JBIT_TEMPERR) == PHCS_BFL_JBIT_TEMPERR)
            {
                istatus = PH_ERR_BFL_TEMP_ERROR;              /* Temperature Error */
            }
            else
            {
            }
            if((getreg_param.reg_data & PHCS_BFL_JBIT_WRERR) == PHCS_BFL_JBIT_WRERR)
            {
                istatus = PH_ERR_BFL_FIFO_WRITE_ERROR;        /* Error Writing to FIFO */
            }
            if(istatus == PH_ERR_BFL_SUCCESS)
            {
                istatus = PH_ERR_BFL_ERROR_NY_IMPLEMENTED;    /* Error not yet implemented, shall never occur! */
            }

            /* restart transceive for next loop */
            if ((command_param->cmd == PHCS_BFL_JCMD_TRANSCEIVE) || (command_param->cmd == PHCS_BFL_CMD_TRANSCEIVE_TO))
            {
                setreg_param.address  = PHCS_BFL_JREG_COMMAND;
                setreg_param.reg_data = (uint8_t)((currentCmd & PHCS_BFL_JMASK_COMMAND_INV) | PHCS_BFL_JCMD_TRANSCEIVE);
                PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))
            }
        }

        /* read data from FIFO and set response parameter */
        if((istatus & PH_ERR_BFL_ERR_MASK) != PH_ERR_BFL_ACK_SUPPOSED)
        {
            /* adjust length to read if buffer size is too small */
            if(maxRxBytes < command_param->rx_bytes)
            {
                command_param->rx_bytes = maxRxBytes;
                istatus = PH_ERR_BFL_BUF_2_SMALL;
            }

            /* get all data received from the FIFO */
            getmulti_param.address = PHCS_BFL_JREG_FIFODATA;
            getmulti_param.buffer  = command_param->rx_buffer;
            getmulti_param.length  = command_param->rx_bytes;
            PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->GetRegisterMultiple(&getmulti_param))

            /* in case of incomplete last byte reduce number of complete bytes by 1 */
            if((command_param->rx_bits != 0x00) && (command_param->rx_bytes != 0x00))
            {
                command_param->rx_bytes--;
            }
        }
    }

    /* If an error occured and the command used is transceive, setup transceive again. */
    if(((command_param->cmd & PHCS_BFL_JMASK_COMMAND) == PHCS_BFL_JCMD_TRANSCEIVE) && (istatus != PH_ERR_BFL_SUCCESS))
    {
        setreg_param.address  = PHCS_BFL_JREG_COMMAND;
        setreg_param.reg_data = PHCS_BFL_JCMD_TRANSCEIVE;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))
    }

    /* remove all Interrupt request flags that are used during function,
       keep all other like they are */
    /* timer is not cleard when entering the function, but cleard by leaving */
    setreg_param.address  = PHCS_BFL_JREG_COMMIRQ;
    setreg_param.reg_data = waitForComm_Int | PHCS_BFL_JBIT_TIMERI;
    PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))

    setreg_param.address  = PHCS_BFL_JREG_DIVIRQ;
    setreg_param.reg_data = waitForDiv_Int;
    PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))

    /* Flush FIFO */
    if(command_param->target_send == 0)
    {
        setreg_param.address  = PHCS_BFL_JREG_FIFOLEVEL;
        setreg_param.reg_data = PHCS_BFL_JBIT_FLUSHBUFFER;
        PHCS_BFLAUX_CHECK_SUCCESS(command_param->lower->SetRegister(&setreg_param))
    }

    PHCS_BFL_ADD_COMPCODE(istatus, PH_ERR_BFL_AUX)

    return status;
}

/* E.O.F. */
