/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) NXP Semiconductors
//
//                       (C)NXP Electronics N.V.2012
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    NXP reserves the right to make changes without notice at any time.
//   NXP makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. NXP must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */

/*! \file phcsBflRegCtl_SpiHw1.c
 *
 * Projekt: Object Oriented Reader Library Framework RegCtl component.
 *
 *  Source: phcsBflRegCtl_SpiHw1.c
 *
 * Comment:
 *  SPI communication between LPC17xx and RC523
 *
 * History:
 *  SP: Adoppted from sample for LPC17xx (Pegoda 2 Project) 22. October 2009
 */

//#include <FreeRTOS.h>
//#include <task.h>
//#include <mlsTypes.h>

/* BFL related */
//#include "includes.h"

#include <phcsBflHw1Reg.h>
#include <phcsBflRegCtl.h>
/**************************************************************************************************/
#include "pn512_m3_spi.h"
/*
typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}Bool;
*/
/**************************************************************************************************/
#define CS_ENABLE           0
#define CS_DISABLE          (!CS_ENABLE)

/* Temporary buffer used for exchange
 *
 * NOTE: If the reader chip will support more then 64 bytes in FIFO
 *       then the size of the buff should be changed accordingly
 */
#define BUFFER_SIZE 65

phcsBfl_Status_t phcsBflRegCtl_SpiHw1GetReg(phcsBflRegCtl_GetRegParam_t *getreg_param);
phcsBfl_Status_t phcsBflRegCtl_SpiHw1SetReg(phcsBflRegCtl_SetRegParam_t *setreg_param);

static void CS_Force(int32_t state);

uint_fast8_t ICACHE_FLASH_ATTR
phcsBflRegCtl_SpiHw1_xFer (uint8_t * tx_buff, uint8_t * rx_buff, const uint16_t len)
{
    int32_t tx_cnt=0;//, rx_cnt=0;

    CS_Force(CS_ENABLE);
    // TX
    while (tx_cnt < len)
    {
        /* Get the received data */
         *rx_buff++ = TxSeri1ByteData(*tx_buff++);

         tx_cnt++;
    }
    CS_Force(CS_DISABLE);
    return 1;
}
phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflRegCtl_SpiHw1ModReg(phcsBflRegCtl_ModRegParam_t *modify_param)
{
    phcsBflRegCtl_GetRegParam_t   getreg_param;
    phcsBflRegCtl_SetRegParam_t   setreg_param;
    uint8_t                       reg_data;
    phcsBfl_Status_t              status = PH_ERR_BFL_SUCCESS;

    getreg_param.address = modify_param->address;
    getreg_param.self    = modify_param->self;
    setreg_param.address = modify_param->address;
    setreg_param.self    = modify_param->self;

    status = phcsBflRegCtl_SpiHw1GetReg(&getreg_param);
    if (status == PH_ERR_BFL_SUCCESS)
    {
        reg_data = getreg_param.reg_data;

        if (modify_param->set)
        {
            /* The bits of the mask, set to one are set in the new data: */
            reg_data |= modify_param->mask;
        }
        else
        {
            /* The bits of the mask, set to one are cleared in the new data: */
            reg_data &= (~modify_param->mask);
        }

        setreg_param.reg_data = reg_data;
        status = phcsBflRegCtl_SpiHw1SetReg(&setreg_param);
    }
    else
    {
        status = PH_ERR_BFL_INTERFACE_ERROR;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_REGCTL)
    return status;
}



phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflRegCtl_SpiHw1GetReg(phcsBflRegCtl_GetRegParam_t *getreg_param)
{
    uint8_t rx_bufff[2];
    uint8_t tx_bufff[2];


    tx_bufff[0] = (uint8_t)((getreg_param->address << 1) | 0x80);
    tx_bufff[1] = (uint8_t)(0x80);

    if (!phcsBflRegCtl_SpiHw1_xFer (&tx_bufff[0], &rx_bufff[0], 2))
    {
        return PH_ERR_BFL_IO;
    }

    getreg_param->reg_data = rx_bufff[1];

    return PH_ERR_BFL_SUCCESS;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflRegCtl_SpiHw1SetReg(phcsBflRegCtl_SetRegParam_t *setreg_param)
{
    uint8_t rx_bufff[2];
    uint8_t tx_bufff[2];

    uint8_t *rx_buff = rx_bufff;
    uint8_t *tx_buff = tx_bufff;

    /*
    The MSB of the first byte defines the mode used. To read data from the PN512 the MSB is
    set to logic 1. To write data to the PN512 the MSB must be set to logic 0. Bits 6 to 1 define
    the address and the LSB is set to logic 0.
    */
    tx_bufff[0] = (uint8_t)((setreg_param->address << 1) & 0x7F);
    tx_bufff[1] = setreg_param->reg_data;

    if (!phcsBflRegCtl_SpiHw1_xFer (&tx_buff[0], &rx_buff[0], 2))
    {
        return PH_ERR_BFL_IO;
    }

    return PH_ERR_BFL_SUCCESS;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflRegCtl_SpiHw1SetMultiReg(phcsBflRegCtl_SetMultiRegParam_t *setmultireg_param)
{
    int             i;
    uint8_t         tx_buff[BUFFER_SIZE];
    uint8_t         rx_buff[BUFFER_SIZE];
    uint_fast16_t   len;

    /* Check if length exceeds BUFFER_SIZE bytes. HW1 only supports length of 64 byte in the FIFO! */
    if ((setmultireg_param->length>BUFFER_SIZE) && (setmultireg_param->address == PHCS_BFL_JREG_FIFODATA))
    {
        return (PH_ERR_BFL_INVALID_PARAMETER);
    }
    else
    {
        len = setmultireg_param->length+1;
        tx_buff[0] = (uint8_t)((setmultireg_param->address<<1) & 0x7F);
        for (i=1; i<len; i++)
        {
            tx_buff[i] = setmultireg_param->buffer[i-1];
        }

        if (!phcsBflRegCtl_SpiHw1_xFer(&tx_buff[0], &rx_buff[0], len))
        {
            return PH_ERR_BFL_IO;
        }
    }

    return PH_ERR_BFL_SUCCESS;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflRegCtl_SpiHw1GetMultiReg(phcsBflRegCtl_GetMultiRegParam_t *getmultireg_param)
{
    int              i;
    uint8_t          tx_buff[BUFFER_SIZE];
    uint8_t          rx_buff[BUFFER_SIZE];
//    uint_fast16_t    len;


    /* Check if length exceeds BUFFER_SIZE bytes. HW1 only supports length of 64 byte in the FIFO! */
    if (getmultireg_param->length > BUFFER_SIZE && getmultireg_param->address == PHCS_BFL_JREG_FIFODATA)
    {
        return (PH_ERR_BFL_INVALID_PARAMETER);
    }
    else
    {
        for (i = 0; i < (getmultireg_param->length); i++)
        {
            tx_buff[i] = (uint8_t)((getmultireg_param->address << 1) | 0x80);
        }
        tx_buff[getmultireg_param->length] = 0x80;  // Avoid reading FIFO to avoid loosing one byte

        if (!phcsBflRegCtl_SpiHw1_xFer (&tx_buff[0], &rx_buff[0], (getmultireg_param->length + 1)))
        {
            return PH_ERR_BFL_IO;
        }

        for (i = 1; i < (getmultireg_param->length + 1); i++)
        {
            getmultireg_param->buffer[(i - 1)] = rx_buff[i];
        }
    }

    return PH_ERR_BFL_SUCCESS;
}

static void ICACHE_FLASH_ATTR
CS_Force(int32_t state)
{
    if (state)
    {
        PN512_CS_set();
    }
    else
    {
        PN512_CS_reset();
    }
}

void ICACHE_FLASH_ATTR
phcsBflRegCtl_SpiHw1Init(phcsBflRegCtl_t *cif, void *p_params, phcsBflBal_t *p_lower)
{
    cif->mp_Members     = p_params;
    cif->mp_Lower       = p_lower;

    /* Init the function pointers: */
    cif->GetRegister            = &phcsBflRegCtl_SpiHw1GetReg;
    cif->GetRegisterMultiple    = &phcsBflRegCtl_SpiHw1GetMultiReg;
    cif->SetRegister            = &phcsBflRegCtl_SpiHw1SetReg;
    cif->SetRegisterMultiple    = &phcsBflRegCtl_SpiHw1SetMultiReg;
    cif->ModifyRegister         = &phcsBflRegCtl_SpiHw1ModReg;

    CS_Force(CS_DISABLE);
}



/* E.O.F. */

