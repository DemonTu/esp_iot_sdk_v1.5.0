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

/*! \file phcsBflOpCtl_Hw1.c
 *
 * Project: Object Oriented Library Framework OperationControl Component.
 *
 *  Source: phcsBflOpCtl_Hw1.c
 * $Author: mha $
 * $Revision: 1.10 $
 * $Date: Fri Jul 21 09:43:22 2006 $
 *
 * Comment:
 *  PN51x-specific variant of OperationControl.
 *
 * Functions:
 *
 *
 * History:
 *  MHa:    Generated 13. May 2003
 *  MHa:    Migrated to MoReUse September 2005
 *
 */

#include <phcsBflRefDefs.h>
#include <phcsBflHw1Reg.h>
#include <phcsBflRegCtl.h>
#include <phcsBflAux.h>

#include <phcsBflOpCtl.h>
#include <phcsBflOpCtl_Hw1Ordinals.h>
#include "phcsBflOpCtl_Hw1Int.h"

/* Header file which contains all the information about the register configuration to be
   able to operate in all communication modes and data rates. */
#include "phcsBflOpCtl_Hw1Cfg.h"


/*! \name Internal macro definition: */
/*@{*/
#define RCOPCTL_HW1_BREAK_ON_ERROR(rcl_status) if((rcl_status) != PH_ERR_BFL_SUCCESS) break /* */
/*@}*/

/*! \name Internal Definitions of register operating parameters
 *  \ingroup opctl
 *  These are internal definitions to maintain settings more easy. */
/*@{*/
#define HW1_RS232_9600BPS            0xEB /* */
#define HW1_RS232_19200BPS           0xCB /* */
#define HW1_RS232_38400BPS           0xAB /* */
#define HW1_RS232_57600BPS           0x9A /* */
#define HW1_RS232_115200BPS          0x79 /* */
#define HW1_RS232_230400BPS          0x5A /* */
#define HW1_RS232_460800BPS          0x3A /* */
#define HW1_RS232_921600BPS          0x1C /* */
#define HW1_RS232_1288000BPS         0x14 /* */

#define HW1_SPEED_SHL_VALUE          0x04 /* */
#define HW1_TGATED_SHL_VALUE         0x05 /* */
/*@}*/



/* Implementation: */
void ICACHE_FLASH_ATTR
phcsBflOpCtl_Hw1Init(phcsBflOpCtl_t     *cif,
                        void             *km,
                        phcsBflRegCtl_t    *p_lower)
{
    /* Glue together and init the operation parameters: */
    cif->mp_Members                 = km;
    cif->mp_Lower                   = p_lower;
    cif->mp_WaitEventCB             = NULL;
    cif->mp_UserRef                 = NULL;
    /* Initialize the function pointers: */
    cif->SetAttrib                  = phcsBflOpCtl_Hw1SetAttribute;
    cif->GetAttrib                  = phcsBflOpCtl_Hw1GetAttribute;
    cif->SetWaitEventCb             = phcsBflOpCtl_Hw1SetWaitEventCb;

    /* Initialize parameter for hardware type information to undefined. */
    ((phcsBflOpCtl_Hw1Params_t*)(cif->mp_Members))->hardware_type = PHCS_BFLOPCTL_HW1VAL_UNDEF_HW;
}


void ICACHE_FLASH_ATTR
phcsBflOpCtl_Hw1SetWaitEventCb(phcsBflOpCtl_SetWecParam_t *set_wec_param)
{
    phcsBflOpCtl_t *cif      = (phcsBflOpCtl_t*)(set_wec_param->self);
    cif->mp_WaitEventCB   = set_wec_param->wait_event_cb;
    cif->mp_UserRef       = set_wec_param->user_ref;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflOpCtl_Hw1SetAttribute(phcsBflOpCtl_AttribParam_t *attrib_param)
{
    phcsBflRegCtl_t           *lower  = ((phcsBflOpCtl_t*)(attrib_param->self))->mp_Lower;
    phcsBflOpCtl_Hw1Params_t  *ocp = (phcsBflOpCtl_Hw1Params_t*)(((phcsBflOpCtl_t*)(attrib_param->self))->mp_Members);
    C_HW1_RC_OP_CTL_CONFIGS  current_config;   /*!< Specifies the current hardware configuration which
                                                             is used for communication setup.                   */
    phcsBfl_Status_t                   status = PH_ERR_BFL_SUCCESS;
    phcsBflRegCtl_SetRegParam_t        srp;
    phcsBflRegCtl_GetRegParam_t        grp;
    phcsBflRegCtl_ModRegParam_t        mrp;
    phcsBflOpCtl_AttribParam_t         attp;
    phcsBflAux_CommandParam_t          scp;
    uint8_t                          buffer[25];
    int16_t                          i;
    const uint8_t (*ActualUsed)[PHCS_BFLOPCTL_VAL_ARRAY_WIDTH];                     /*  */

    srp.self  = lower;
    grp.self  = lower;
    mrp.self  = lower;
    attp.self = attrib_param->self;

    scp.lower           = lower;
    scp.wait_event_cb   = ((phcsBflOpCtl_t*)(attrib_param->self))->mp_WaitEventCB;
    scp.user_ref        = ((phcsBflOpCtl_t*)(attrib_param->self))->mp_UserRef;


    /* read out version register and store hardware type in variable if not set before */
    if(ocp->hardware_type == PHCS_BFLOPCTL_HW1VAL_UNDEF_HW)
    {
        grp.address = PHCS_BFL_JREG_VERSION;
        status = lower->GetRegister(&grp);
        PHCS_BFLAUX_CHECK_SUCCESS(status)

        /* set internal variable to current harware version */
        ocp->hardware_type = grp.reg_data;
    }

    switch(ocp->hardware_type)
    {
        case PHCS_BFLOPCTL_HW1VAL_VERS1:
            /* configure harware configuration pointers to chosen */
            current_config.Active_106                = PHCS_BFLOPCTL_VAL_HW1ACT_106;
            current_config.Active_212_424            = PHCS_BFLOPCTL_VAL_HW1ACT_212424;
            current_config.Passive_Initiator_106     = PHCS_BFLOPCTL_VAL_HW1PASI_106;
            current_config.Passive_Initiator_212_424 = PHCS_BFLOPCTL_VAL_HW1PASI_212424;
            current_config.Passive_Target            = PHCS_BFLOPCTL_VAL_HW1PAST;
            break;

        case PHCS_BFLOPCTL_HW1VAL_VERS2:
        case PHCS_BFLOPCTL_HW1VAL_VERS3:
            /* configure harware configuration pointers to chosen */
            current_config.Active_106                = PHCS_BFLOPCTL_VAL_HW2ACT_106;
            current_config.Active_212_424            = PHCS_BFLOPCTL_VAL_HW2ACT_212424;
            current_config.Passive_Initiator_106     = PHCS_BFLOPCTL_VAL_HW2PASI_106;
            current_config.Passive_Initiator_212_424 = PHCS_BFLOPCTL_VAL_HW2PASI_212424;
            current_config.Passive_Target            = PHCS_BFLOPCTL_VAL_HW2PAST;
            break;

        case PHCS_BFLOPCTL_HW1VAL_VERS4:
        case PHCS_BFLOPCTL_HW1VAL_VERS5:
        default:
            /* configure harware configuration pointers to chosen */
            current_config.Active_106                = PHCS_BFLOPCTL_VAL_HW4ACT_106;
            current_config.Active_212_424            = PHCS_BFLOPCTL_VAL_HW4ACT_212424;
            current_config.Passive_Initiator_106     = PHCS_BFLOPCTL_VAL_HW4PASI_106;
            current_config.Passive_Initiator_212_424 = PHCS_BFLOPCTL_VAL_HW4PASI_212424;
            current_config.Passive_Target            = PHCS_BFLOPCTL_VAL_HW4PAST;
            break;
    }

    switch (attrib_param->group_ordinal)
    {
        case PHCS_BFLOPCTL_GROUP_RCMETRICS:
            /* No metrics to set @ PN51x! */
            status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
            break;

        case PHCS_BFLOPCTL_GROUP_MODE:
            /* disable Crypto1 bit if authentication has been performed bevore for all modes */
            mrp.address = PHCS_BFL_JREG_STATUS2;
            mrp.mask    = PHCS_BFL_JBIT_CRYPTO1ON;
            mrp.set     = 0;
            status = lower->ModifyRegister(&mrp);
            RCOPCTL_HW1_BREAK_ON_ERROR(status);

            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_ACTIVE:
                    /* check if parameter are correct */
                    if(attrib_param->param_a > PHCS_BFLOPCTL_VAL_RF424K || attrib_param->param_b > PHCS_BFLOPCTL_VAL_RF424K)
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    else
                    {
                        /* do not touch bits: InvMod, TXMix in register TxMode */
                        grp.address  = PHCS_BFL_JREG_TXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_INVMOD | PHCS_BFL_JBIT_TXMIX));

                        srp.address  = PHCS_BFL_JREG_TXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_a << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_NFC);
                        /* TxCRCEn = 1; TxSpeed = x; InvMod, TXMix = 0; TxFraming = 1 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* do not touch bits: InvMod, TXMix in register RxMode */
                        grp.address  = PHCS_BFL_JREG_RXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_RXNOERR | PHCS_BFL_JBIT_RXMULTIPLE));

                        srp.address  = PHCS_BFL_JREG_RXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_b << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_NFC);
                        /* RxCRCEn = 1; RxSpeed = x; RxNoErr, RxMultiple = 0; RxFraming = 1 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* set AutoRfOf and CAOn bit */
                        mrp.address  = PHCS_BFL_JREG_TXAUTO;
                        mrp.mask     = (uint8_t)(PHCS_BFL_JBIT_AUTORFOFF | PHCS_BFL_JBIT_CAON);
                        mrp.set      = 1;
                        status = lower->ModifyRegister(&mrp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* set current configuration pointer to the one according to the data rate chosen */
                        if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_RF106K)
                        {
                            ActualUsed = current_config.Active_106;
                        } else
                        {   /* 212 or 424 kbps */
                            ActualUsed = current_config.Active_212_424;
                        }

                        /* do configuration according to the data rate */
                        i = 0;
                        while(ActualUsed[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS] != PHCS_BFLOPCTL_VAL_MATRIX_END)
                        {
                            /* set first both address parameters for comming register access */
                            grp.address = ActualUsed[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS];
                            srp.address = grp.address;

                            /* get current register value */
                            status = lower->GetRegister(&grp);
                            RCOPCTL_HW1_BREAK_ON_ERROR(status);

                            srp.reg_data = (grp.reg_data & ActualUsed[i][PHCS_BFLOPCTL_VAL_AND_POS]) | ActualUsed[i][PHCS_BFLOPCTL_VAL_OR_POS];
                            status = lower->SetRegister(&srp);
                            RCOPCTL_HW1_BREAK_ON_ERROR(status);
                            i++;
                        }
                    }
                    break;
                case PHCS_BFLOPCTL_ATTR_MFRD_A:
                    /* check if parameter are correct */
                    if(attrib_param->param_a > PHCS_BFLOPCTL_VAL_RF848K || attrib_param->param_b > PHCS_BFLOPCTL_VAL_RF848K)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    } else
                    {
                        /* do not touch bits: InvMod, TXMix in register TxMode */
                        grp.address  = PHCS_BFL_JREG_TXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_INVMOD | PHCS_BFL_JBIT_TXMIX));

                        srp.address  = PHCS_BFL_JREG_TXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_a << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_MIFARE);
                        /* TxCRCEn = 1; TxSpeed = x; InvMod, TXMix = 0; TxFraming = 0 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* do not touch bits: InvMod, TXMix in register RxMode */
                        grp.address  = PHCS_BFL_JREG_RXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_RXNOERR | PHCS_BFL_JBIT_RXMULTIPLE));

                        srp.address  = PHCS_BFL_JREG_RXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_b << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_MIFARE);
                        /* RxCRCEn = 1; RxSpeed = x; RxNoErr, RxMultiple = 0; TxFraming = 0 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* set AutoRfOf and CAOn bit */
                        mrp.address  = PHCS_BFL_JREG_TXAUTO;
                        mrp.mask     = (uint8_t)(PHCS_BFL_JBIT_AUTORFOFF | PHCS_BFL_JBIT_CAON);
                        mrp.set      = 0;
                        status = lower->ModifyRegister(&mrp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

						/* init register address outside. it is used as 1st in all statements! */
                        srp.address  = PHCS_BFL_JREG_MODWIDTH;
                        switch(attrib_param->param_a)
                        {
                            case PHCS_BFLOPCTL_VAL_RF212K:
                                srp.reg_data = 0x15;            /* Modwidth = 0x15 */
                                status = lower->SetRegister(&srp);
                                RCOPCTL_HW1_BREAK_ON_ERROR(status);
                                break;
                            case PHCS_BFLOPCTL_VAL_RF424K:
                                srp.reg_data = 0x0A;            /* Modwidth = 0x0A */
                                status = lower->SetRegister(&srp);
                                RCOPCTL_HW1_BREAK_ON_ERROR(status);
                                break;
                            case PHCS_BFLOPCTL_VAL_RF848K:
                                srp.reg_data = 0x05;            /* Modwidth = 0x05 */
                                status = lower->SetRegister(&srp);
                                RCOPCTL_HW1_BREAK_ON_ERROR(status);
                                break;
                            default:
                                srp.reg_data = 0x26;            /* Modwidth = 0x26 (reset) */
                                status = lower->SetRegister(&srp);
                                RCOPCTL_HW1_BREAK_ON_ERROR(status);
                                /* do configuration according to the data rate */
                                i = 0;
                                while(current_config.Passive_Initiator_106[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS] != PHCS_BFLOPCTL_VAL_MATRIX_END)
                                {
                                    /* set first both address parameters for comming register access */
                                    grp.address = current_config.Passive_Initiator_106[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS];
                                    srp.address = grp.address;

                                    /* get current register value */
                                    status = lower->GetRegister(&grp);
                                    RCOPCTL_HW1_BREAK_ON_ERROR(status);

                                    srp.reg_data = (grp.reg_data & current_config.Passive_Initiator_106[i][PHCS_BFLOPCTL_VAL_AND_POS]) |
                                                    current_config.Passive_Initiator_106[i][PHCS_BFLOPCTL_VAL_OR_POS];
                                    status = lower->SetRegister(&srp);
                                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                                    i++;
                                }
                        }
                    }
                    break;
                case PHCS_BFLOPCTL_ATTR_PAS_POL:
                    /* check if parameter are correct */
                    if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_RF106K || attrib_param->param_b == PHCS_BFLOPCTL_VAL_RF106K ||
                       attrib_param->param_a > PHCS_BFLOPCTL_VAL_RF424K || attrib_param->param_b > PHCS_BFLOPCTL_VAL_RF424K)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        /* do not touch bits: InvMod, TXMix in register TxMode */
                        grp.address  = PHCS_BFL_JREG_TXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_INVMOD | PHCS_BFL_JBIT_TXMIX));

                        srp.address  = PHCS_BFL_JREG_TXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_a << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_FELICA);
                        /* TxCRCEn = 1; TxSpeed = x; InvMod, TXMix = 0; TxFraming = 2 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* do not touch bits: InvMod, TXMix in register RxMode */
                        grp.address  = PHCS_BFL_JREG_RXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_RXNOERR | PHCS_BFL_JBIT_RXMULTIPLE));

                        srp.address  = PHCS_BFL_JREG_RXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_b << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_FELICA);
                        /* RxCRCEn = 1; RxSpeed = x; RxNoErr, RxMultiple = 0; RxFraming = 2 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        mrp.address = PHCS_BFL_JREG_TXAUTO;
                        mrp.mask    = PHCS_BFL_JBIT_AUTORFOFF | PHCS_BFL_JBIT_FORCE100ASK | PHCS_BFL_JBIT_CAON;
                        mrp.set     = 0;
                        /* AutoRfOff, CAOn = 0; Force100ASK, AutoWakeUp, InitialRFOn, TxXRfAutoEn = X */
                        status = lower->ModifyRegister(&mrp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);


                        /* do configuration according to the data rate */
                        i = 0;
                        while(current_config.Passive_Initiator_212_424[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS] != PHCS_BFLOPCTL_VAL_MATRIX_END)
                        {
                            /* set first both address parameters for comming register access */
                            grp.address = current_config.Passive_Initiator_212_424[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS];
                            srp.address = grp.address;

                            /* get current register value */
                            status = lower->GetRegister(&grp);
                            RCOPCTL_HW1_BREAK_ON_ERROR(status);

                            srp.reg_data = (grp.reg_data & current_config.Passive_Initiator_212_424[i][PHCS_BFLOPCTL_VAL_AND_POS]) |
                                            current_config.Passive_Initiator_212_424[i][PHCS_BFLOPCTL_VAL_OR_POS];
                            status = lower->SetRegister(&srp);
                            RCOPCTL_HW1_BREAK_ON_ERROR(status);
                            i++;
                        }
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_PASSIVE:
                    /* check if parameter are correct */
                    if(attrib_param->param_a > PHCS_BFLOPCTL_VAL_RF848K || attrib_param->param_b > PHCS_BFLOPCTL_VAL_RF848K)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    } else
                    {
                        /* disable automatically driver settings if activated bfore */
                        mrp.address  = PHCS_BFL_JREG_TXAUTO;
                        mrp.mask     = PHCS_BFL_JBIT_AUTORFOFF | PHCS_BFL_JBIT_FORCE100ASK | PHCS_BFL_JBIT_CAON | PHCS_BFL_JBIT_INITIALRFON | PHCS_BFL_JBIT_TX2RFAUTOEN | PHCS_BFL_JBIT_TX1RFAUTOEN;
                        mrp.set      = 0;
                        status = lower->ModifyRegister(&mrp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* disable drivers if activated bfore */
                        mrp.address  = PHCS_BFL_JREG_TXCONTROL;
                        mrp.mask     = PHCS_BFL_JBIT_TX2RFEN | PHCS_BFL_JBIT_TX1RFEN;
                        mrp.set      = 0;
                        status = lower->ModifyRegister(&mrp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* do not touch bits: InvMod, TXMix in register TxMode */
                        grp.address  = PHCS_BFL_JREG_TXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_INVMOD | PHCS_BFL_JBIT_TXMIX));

                        srp.address  = PHCS_BFL_JREG_TXMODE;
                        if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_RF106K)
                            srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | PHCS_BFL_JBIT_106KBPS | PHCS_BFL_JBIT_MIFARE);
                        else if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_RF212K)
                            srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | PHCS_BFL_JBIT_212KBPS | PHCS_BFL_JBIT_FELICA);
                        else
                            srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | PHCS_BFL_JBIT_424KBPS | PHCS_BFL_JBIT_FELICA);
                        /* TxCRCEn = 1; TxSpeed = 0; InvMod = 0; TXMix = 0; TxFraming = 0 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* do not touch bits: InvMod, TXMix in register RxMode */
                        grp.address  = PHCS_BFL_JREG_RXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_RXNOERR | PHCS_BFL_JBIT_RXMULTIPLE));

                        srp.address  = PHCS_BFL_JREG_RXMODE;
                        if(attrib_param->param_b == PHCS_BFLOPCTL_VAL_RF106K)
                            srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | PHCS_BFL_JBIT_106KBPS | PHCS_BFL_JBIT_MIFARE);
                        else if(attrib_param->param_b == PHCS_BFLOPCTL_VAL_RF212K)
                            srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | PHCS_BFL_JBIT_212KBPS | PHCS_BFL_JBIT_FELICA);
                        else
                            srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | PHCS_BFL_JBIT_424KBPS | PHCS_BFL_JBIT_FELICA);
                        /* RxCRCEn = 1; RxSpeed = 0; RxNoErr, RxMultiple = 0; RxFraming = 0 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);


                        /* do configuration according to the data rate */
                        i = 0;
                        while(current_config.Passive_Target[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS] != PHCS_BFLOPCTL_VAL_MATRIX_END)
                        {
                            /* set first both address parameters for comming register access */
                            grp.address = current_config.Passive_Target[i][PHCS_BFLOPCTL_VAL_ADDRESS_POS];
                            srp.address = grp.address;

                            /* get current register value */
                            status = lower->GetRegister(&grp);
                            RCOPCTL_HW1_BREAK_ON_ERROR(status);

                            srp.reg_data = (grp.reg_data & current_config.Passive_Target[i][PHCS_BFLOPCTL_VAL_AND_POS]) |
                                            current_config.Passive_Target[i][PHCS_BFLOPCTL_VAL_OR_POS];
                            status = lower->SetRegister(&srp);
                            RCOPCTL_HW1_BREAK_ON_ERROR(status);
                            i++;
                        }
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_MFCARD:
                    /* check if parameter are correct */
                    if(attrib_param->param_a > PHCS_BFLOPCTL_VAL_RF848K || attrib_param->param_b > PHCS_BFLOPCTL_VAL_RF848K)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    } else
                    {
                        /* Set Tx/RxMode as defined from external */
                        /* do not touch bits: InvMod, TXMix in register TxMode */
                        grp.address  = PHCS_BFL_JREG_TXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_INVMOD | PHCS_BFL_JBIT_TXMIX));

                        srp.address  = PHCS_BFL_JREG_TXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_a << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_MIFARE);
                        /* TxCRCEn = 1; TxSpeed = 0; InvMod = 0; TXMix = 0; TxFraming = 0 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* do not touch bits: InvMod, TXMix in register RxMode */
                        grp.address  = PHCS_BFL_JREG_RXMODE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_RXNOERR | PHCS_BFL_JBIT_RXMULTIPLE));

                        srp.address  = PHCS_BFL_JREG_RXMODE;
                        srp.reg_data = (uint8_t)(grp.reg_data | PHCS_BFL_JBIT_CRCEN | (attrib_param->param_b << HW1_SPEED_SHL_VALUE) | PHCS_BFL_JBIT_MIFARE);
                        /* RxCRCEn = 1; RxSpeed = 0; RxNoErr, RxMultiple = 0; RxFraming = 0 */
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* Reset the configuration of general passive mode. */
                        grp.address = PHCS_BFL_JREG_TXBITPHASE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        srp.address  = PHCS_BFL_JREG_TXBITPHASE;
                        srp.reg_data = (uint8_t)((grp.reg_data & PHCS_BFL_JBIT_RCVCLKCHANGE) | 0x07);

                        grp.address = PHCS_BFL_JREG_MIFARE;
                        status = lower->GetRegister(&grp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        grp.reg_data = (uint8_t)(grp.reg_data & (PHCS_BFL_JBIT_MFHALTED | PHCS_BFL_JMASK_TXWAIT));
                        /* choose right settings according to RxSpeed */

                        srp.address  = PHCS_BFL_JREG_MIFARE;
                        switch(attrib_param->param_b)
                        {
                            case PHCS_BFLOPCTL_VAL_RF212K:
                                /* SensMiller = 4; TauMiller = 1; MFHalted = X; TxWait = X */
                                srp.reg_data = (uint8_t)(grp.reg_data | 0x80 | 0x08);
                                break;

                            case PHCS_BFLOPCTL_VAL_RF424K:
                                /* SensMiller = 5; TauMiller = 2; MFHalted = X; TxWait = X */
                                srp.reg_data = (uint8_t)(grp.reg_data | 0xA0 | 0x10);
                                break;

                            case PHCS_BFLOPCTL_VAL_RF848K:
                                /* SensMiller = 7; TauMiller = 3; MFHalted = X; TxWait = X */
                                srp.reg_data = (uint8_t)(grp.reg_data | 0xD0 | 0x18);
                                break;

                            default:
                                /* SensMiller = 3; TauMiller = 0; MFHalted = X; TxWait = X */
                                srp.reg_data = (uint8_t)(grp.reg_data | 0x60 | 0x00);
                                break;
                        }
                        status = lower->SetRegister(&srp);
                    }
                    break;

                default:
                    /* Other values are not valid: */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_COMM:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_RS232BAUD:
                    srp.address = PHCS_BFL_JREG_SERIALSPEED;
                    status = phcsBflOpCtl_Hw1MapSerSpeedSet(attrib_param->param_a,
                                                                 &(srp.reg_data));
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    /* OK, we can set the rate: */
                    status = lower->SetRegister(&srp);
                    break;

                default:
                    /* Other values are not valid: */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_SIGNAL:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_SIGINOUT:
                    if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_READER)
                    {
                        /*
 						 * PN51x acts as reader
						 */
                        if (attrib_param->param_b == PHCS_BFLOPCTL_VAL_NOSIGINOUT)
                        {
                            /* Use the RF link */
                            grp.address = PHCS_BFL_JREG_TXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_TXSEL;
                            srp.reg_data = (uint8_t)  ( ( grp.reg_data & 0xC0 ) | 0x10 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            grp.address = PHCS_BFL_JREG_RXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_RXSEL;
                            srp.reg_data = (uint8_t)  ( ( (grp.reg_data) & 0x3F ) | 0x80 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Reset the LargeBWPLL bit */
                            mrp.address = PHCS_BFL_JREG_MANUALRCV;
                            mrp.mask = PHCS_BFL_JBIT_LARGEBWPLL;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Settings only relevant for SAM mode */
                            srp.address  = PHCS_BFL_JREG_TESTSEL1;
                            srp.reg_data = 0x00;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Disable TxMix */
                            mrp.address = PHCS_BFL_JREG_TXMODE;
                            mrp.mask = PHCS_BFL_JBIT_TXMIX;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Disable RxMultiple */
                            mrp.address = PHCS_BFL_JREG_RXMODE;
                            mrp.mask = PHCS_BFL_JBIT_RXMULTIPLE;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

						} else if (attrib_param->param_b == PHCS_BFLOPCTL_VAL_SIGINOUT)
                        {
                            /* Use SIGIN/SIGOUT */
                            grp.address  = PHCS_BFL_JREG_TXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address  = PHCS_BFL_JREG_TXSEL;
                            srp.reg_data = (uint8_t)  ( (grp.reg_data & 0xC0) | 0x0D );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            grp.address = PHCS_BFL_JREG_RXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_RXSEL;
                            srp.reg_data = (uint8_t)  ( ( (grp.reg_data) & 0x3F ) | 0x40 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Disable Large BW PLL, improves performance */
                            mrp.address = PHCS_BFL_JREG_MANUALRCV;
                            mrp.mask    = PHCS_BFL_JBIT_LARGEBWPLL;
                            mrp.set     = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Settings only relevant for SAM mode */
                            srp.address  = PHCS_BFL_JREG_TESTSEL1;
                            srp.reg_data = 0x18;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Disable TxMix */
                            mrp.address = PHCS_BFL_JREG_TXMODE;
                            mrp.mask = PHCS_BFL_JBIT_TXMIX;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Disable RxMultiple */
                            mrp.address = PHCS_BFL_JREG_RXMODE;
                            mrp.mask = PHCS_BFL_JBIT_RXMULTIPLE;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));
						} else if (attrib_param->param_b == PHCS_BFLOPCTL_VAL_COMM_SIGINOUT)
                        {   /* Communicate with secure IC */
                            grp.address  = PHCS_BFL_JREG_TXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address  = PHCS_BFL_JREG_TXSEL;
                            srp.reg_data = (uint8_t)  ( (grp.reg_data & 0xC0) | 0x1D );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            grp.address = PHCS_BFL_JREG_RXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_RXSEL;
                            srp.reg_data = (uint8_t)  ( ( (grp.reg_data) & 0x3F ) | 0x40 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

						} else if (attrib_param->param_b == PHCS_BFLOPCTL_VAL_COMM_ANTENNA)
                        {   /* Communicate over RF field */
                            /* Use the RF link */
                            grp.address = PHCS_BFL_JREG_TXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_TXSEL;
                            srp.reg_data = (uint8_t)  ( ( grp.reg_data & 0xC0 ) | 0x14 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            grp.address = PHCS_BFL_JREG_RXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_RXSEL;
                            srp.reg_data = (uint8_t)  ( ( (grp.reg_data) & 0x3F ) | 0x80 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));
                        } else
                        {
                            status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                        }

                    } else if (attrib_param->param_a == PHCS_BFLOPCTL_VAL_CARD)
                    {   /* PN51x acts as card */
                        if (attrib_param->param_b == PHCS_BFLOPCTL_VAL_NOSIGINOUT)
                        {
                            /* Disable link to SAM */
                            grp.address = PHCS_BFL_JREG_TXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_TXSEL;
                            srp.reg_data = (uint8_t)  ( ( grp.reg_data & 0xC0 ) | 0x10 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Set the UartSel */
                            grp.address = PHCS_BFL_JREG_RXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_RXSEL;
                            srp.reg_data = (uint8_t)  ( ( (grp.reg_data) & 0x3F ) | 0x80 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Set the LargeBWPLL bit */
                            mrp.address = PHCS_BFL_JREG_MANUALRCV;
                            mrp.mask = PHCS_BFL_JBIT_LARGEBWPLL;
                            mrp.set = 1;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Settings only relevant for SAM mode */
                            srp.address  = PHCS_BFL_JREG_TESTSEL1;
                            srp.reg_data = 0x00;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Disable TxMix */
                            mrp.address = PHCS_BFL_JREG_TXMODE;
                            mrp.mask = PHCS_BFL_JBIT_TXMIX;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Disable RxMultiple */
                            mrp.address = PHCS_BFL_JREG_RXMODE;
                            mrp.mask = PHCS_BFL_JBIT_RXMULTIPLE;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Setting this bit is only reasonable if only passive mode is used. */
                            mrp.address = PHCS_BFL_JREG_FELICANFC2;
                            mrp.mask = 0x80;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

						} else if (attrib_param->param_b == PHCS_BFLOPCTL_VAL_SIGINOUT)
                        {
                            /*
                             * Enable link to SAM and to PN51x
                             */
                            grp.address = PHCS_BFL_JREG_TXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_TXSEL;
                            srp.reg_data = (uint8_t)  ( ( grp.reg_data & 0xC0 ) | 0x1E );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Set UartSel to 2 and  */
                            grp.address = PHCS_BFL_JREG_RXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_RXSEL;
                            srp.reg_data = (uint8_t)  ( ( (grp.reg_data) & 0x3F ) | 0x80 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Reset the LargeBWPLL bit */
                            mrp.address = PHCS_BFL_JREG_MANUALRCV;
                            mrp.mask = PHCS_BFL_JBIT_LARGEBWPLL;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Settings only relevant for SAM mode */
                            srp.address  = PHCS_BFL_JREG_TESTSEL1;
                            srp.reg_data = 0x00;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Enable TxMix */
                            mrp.address = PHCS_BFL_JREG_TXMODE;
                            mrp.mask = PHCS_BFL_JBIT_TXMIX;
                            mrp.set = 1;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Disable RxMultiple */
                            mrp.address = PHCS_BFL_JREG_RXMODE;
                            mrp.mask = PHCS_BFL_JBIT_RXMULTIPLE;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Setting this bit is only reasonable if only passive mode is used. */
                            mrp.address = PHCS_BFL_JREG_FELICANFC2;
                            mrp.mask = 0x80;
                            mrp.set = 1;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                        } else if (attrib_param->param_b == PHCS_BFLOPCTL_VAL_ONLYSIGINOUT)
						{   /* Exclusive link to SAM */
                            grp.address = PHCS_BFL_JREG_TXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_TXSEL;
                            srp.reg_data = (uint8_t)  ( ( grp.reg_data & 0xC0 ) | 0x2E );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Set the UartSel */
                            grp.address = PHCS_BFL_JREG_RXSEL;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                            srp.address = PHCS_BFL_JREG_RXSEL;
                            srp.reg_data = (uint8_t)  ( ( (grp.reg_data) & 0x3F ) | 0x80 );
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Reset the LargeBWPLL bit */
                            mrp.address = PHCS_BFL_JREG_MANUALRCV;
                            mrp.mask = PHCS_BFL_JBIT_LARGEBWPLL;
                            mrp.set = 0;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Settings only relevant for SAM mode */
                            srp.address  = PHCS_BFL_JREG_TESTSEL1;
                            srp.reg_data = 0x00;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));

                            /* Enable TxMix */
                            mrp.address = PHCS_BFL_JREG_TXMODE;
                            mrp.mask = PHCS_BFL_JBIT_TXMIX;
                            mrp.set = 1;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Enable RxMultiple */
                            mrp.address = PHCS_BFL_JREG_RXMODE;
                            mrp.mask = PHCS_BFL_JBIT_RXMULTIPLE;
                            mrp.set = 1;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                            /* Setting this bit is only reasonable if only passive mode is used. */
                            mrp.address = PHCS_BFL_JREG_FELICANFC2;
                            mrp.mask = 0x80;
                            mrp.set = 1;
                            RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

						} else
                        {
                            status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                        }
                    } else
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    break;

                default:
                    /* Other values are not valid: */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_TMR:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_TMR:
                    if(attrib_param->param_a   > 1 ||
                       attrib_param->param_b   > 1 ||
                       attrib_param->buffer[1] > PHCS_BFL_JMASK_TPRESCALER_HI)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        /* Activate / Deactivate Auto mode of timer */
                        mrp.address = PHCS_BFL_JREG_TMODE;
                        mrp.mask    = PHCS_BFL_JBIT_TAUTO;
                        mrp.set     = attrib_param->param_a;
                        RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                        /* Activate / Deactivate Auto restart mode of timer */
                        mrp.address = PHCS_BFL_JREG_TMODE;
                        mrp.mask    = PHCS_BFL_JBIT_TAUTORESTART;
                        mrp.set     = attrib_param->param_b;
                        RCOPCTL_HW1_BREAK_ON_ERROR(lower->ModifyRegister(&mrp));

                        /* Set prescaler value */
                        srp.address = PHCS_BFL_JREG_TPRESCALER;
                        srp.reg_data = attrib_param->buffer[0];
                        RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));
                        status = phcsBflOpCtl_Hw1ManipulateRegs(lower,
                                                              PHCS_BFL_JREG_TMODE,
                                                              PHCS_BFL_JMASK_TPRESCALER_HI,
                                                              0,
                                                              attrib_param->buffer[1]);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);

                        /* set reload value */
                        srp.address = PHCS_BFL_JREG_TRELOADLO;
                        srp.reg_data = attrib_param->buffer[2];
                        RCOPCTL_HW1_BREAK_ON_ERROR(lower->SetRegister(&srp));
                        srp.address = PHCS_BFL_JREG_TRELOADHI;
                        srp.reg_data = attrib_param->buffer[3];
                        status = lower->SetRegister(&srp);
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_TMR_AUTO:
                    /* check parameter range first */
                    if(attrib_param->param_a > 1)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        mrp.set     = (uint8_t)((attrib_param->param_a == PHCS_BFLOPCTL_VAL_ON) ? 1 : 0);
                        mrp.address = PHCS_BFL_JREG_TMODE;
                        mrp.mask    = PHCS_BFL_JBIT_TAUTO;
                        status = lower->ModifyRegister(&mrp);
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_TMR_RESTART:
                    /* check parameter range first */
                    if(attrib_param->param_a > 1)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        mrp.set     = (uint8_t)((attrib_param->param_a == PHCS_BFLOPCTL_VAL_ON) ? 1 : 0);
                        mrp.address = PHCS_BFL_JREG_TMODE;
                        mrp.mask    = PHCS_BFL_JBIT_TAUTORESTART;
                        status = lower->ModifyRegister(&mrp);
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_TMR_GATED:
                    /* check parameter range first */
                    if(attrib_param->param_a > (PHCS_BFL_JMASK_TGATED >> HW1_TGATED_SHL_VALUE))
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        status = phcsBflOpCtl_Hw1ManipulateRegs(lower,
                                                                PHCS_BFL_JREG_TMODE,
                                                                PHCS_BFL_JMASK_TGATED,
                                                                HW1_TGATED_SHL_VALUE,
                                                                attrib_param->param_a);
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_TMR_PRESCALER:
                    /* check parameter range first */
                    if(attrib_param->param_b > PHCS_BFL_JMASK_TPRESCALER_HI)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        srp.address = PHCS_BFL_JREG_TPRESCALER;
                        srp.reg_data = attrib_param->param_a;
                        status = lower->SetRegister(&srp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        status = phcsBflOpCtl_Hw1ManipulateRegs(lower,
                                                                PHCS_BFL_JREG_TMODE,
                                                                PHCS_BFL_JMASK_TPRESCALER_HI,
                                                                0,
                                                                attrib_param->param_b);
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_TMR_RELOAD:
                    /* set timer reload values */
                    srp.address = PHCS_BFL_JREG_TRELOADLO;
                    srp.reg_data = attrib_param->param_a;
                    status = lower->SetRegister(&srp);
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    srp.address = PHCS_BFL_JREG_TRELOADHI;
                    srp.reg_data = attrib_param->param_b;
                    status = lower->SetRegister(&srp);
                    break;

                case PHCS_BFLOPCTL_ATTR_TMR_COUNTER:
                    status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                    break;

                default:
                    /* Invalid system attribute. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_CRC:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_CRC_PRESET:
                    if(attrib_param->param_a > PHCS_BFL_JMASK_CRCPRESET)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        /* set CRC preset values */
                        grp.address = PHCS_BFL_JREG_MODE;
                        RCOPCTL_HW1_BREAK_ON_ERROR(lower->GetRegister(&grp));
                        srp.address = PHCS_BFL_JREG_MODE;
                        srp.reg_data = (uint8_t)((grp.reg_data & ~PHCS_BFL_JMASK_CRCPRESET) | attrib_param->param_a);
                        status = lower->SetRegister(&srp);
                    }
                    break;

                case PHCS_BFLOPCTL_ATTR_CRC_RESULT:
                    status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                    break;

                case PHCS_BFLOPCTL_ATTR_CRC_MSB_FIRST:
                    mrp.set     = (uint8_t)((attrib_param->param_a == PHCS_BFLOPCTL_VAL_ON) ? 1 : 0);
                    mrp.address = PHCS_BFL_JREG_MODE;
                    mrp.mask    = PHCS_BFL_JBIT_MSBFIRST;
                    status = lower->ModifyRegister(&mrp);
                    break;

                case PHCS_BFLOPCTL_ATTR_CRC_CALC:
                    if(attrib_param->param_a > 64)
                    {
                        status = PH_ERR_BFL_INVALID_PARAMETER;
                    }
                    else
                    {
                        scp.cmd       = PHCS_BFL_JCMD_CALCCRC;
                        scp.tx_buffer = attrib_param->buffer;
                        scp.tx_bytes  = attrib_param->param_a;
                        status = phcsBflAux_Hw1Command(&scp);
                        RCOPCTL_HW1_BREAK_ON_ERROR(status);
                        /* call internal function to get CRC value */
                        attp.group_ordinal    = PHCS_BFLOPCTL_GROUP_CRC;
                        attp.attrtype_ordinal = PHCS_BFLOPCTL_ATTR_CRC_RESULT;
                        status = phcsBflOpCtl_Hw1GetAttribute(&attp);
                        attrib_param->param_a = attp.param_a;
                        attrib_param->param_b = attp.param_b;
                    }
                    break;

                default:
                    /* Invalid system attribute. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_PROTOCOL:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_NFCID:
                    scp.cmd       = PHCS_BFL_JCMD_CONFIG;
                    scp.rx_buffer = buffer;
                    scp.rx_bytes  = 25;
                    scp.tx_bytes  = 0;
                    scp.tx_buffer = 0;
                    status = phcsBflAux_Hw1Command(&scp);
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    if((scp.rx_bytes == 25) || ((scp.rx_bytes == 24) && (scp.rx_bits != 0)))
                    {
                        if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_NFCID1 && attrib_param->param_b == 6)
                        {   /* nfcid1 */
                            /* modify relevant data */
                            for(i=0; i<attrib_param->param_b; i++)
                            {
                                buffer[i] = attrib_param->buffer[i];
                            }

                            /* store dat in PN51x */
                            scp.tx_buffer = buffer;
                            scp.tx_bytes  = 25;
                            scp.rx_bytes  = 0;
                            status = phcsBflAux_Hw1Command(&scp);
                        } else if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_NFCID2 && attrib_param->param_b == 18)
                        {   /* nfcid2 */
                            /* modify relevant data */
                            for(i=0;i<18;i++)
                            {
                                buffer[6+i] = attrib_param->buffer[i];
                            }

                            /* store dat in PN51x */
                            scp.tx_buffer = buffer;
                            scp.tx_bytes  = 25;
                            scp.rx_bytes  = 0;
                            status = phcsBflAux_Hw1Command(&scp);
                        } else if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_NFCID3 && attrib_param->param_b == 10)
                        {   /* nfcid3 */
                            /* modify relevant data */
                            for(i=0;i<3;i++)
                            {
                                buffer[2+i] = attrib_param->buffer[i];
                            }
                            for(i=0;i<6;i++)
                            {
                                buffer[8+i] = attrib_param->buffer[3+i];
                            }
                            buffer[24] = attrib_param->buffer[9];

                            /* store dat in PN51x */
                            scp.tx_buffer = buffer;
                            scp.tx_bytes  = 25;
                            scp.rx_bytes  = 0;
                            status = phcsBflAux_Hw1Command(&scp);
                        } else if(attrib_param->param_a == PHCS_BFLOPCTL_VAL_NFCID_RANDOM)
                        {   /* generate random ID */
                            scp.cmd = PHCS_BFL_JCMD_RANDOMIDS;
                            scp.rx_buffer = buffer;
                            scp.rx_bytes = 0;
                            scp.tx_buffer = buffer;
                            scp.tx_bytes = 0;
                            status = phcsBflAux_Hw1Command(&scp);
                        } else
                        {
                            status = PH_ERR_BFL_INVALID_PARAMETER;
                        }
                    } else
                    {
                        status = PH_ERR_BFL_OTHER_ERROR;
                    }
                    break;

                default:
                    /* Invalid system attribute. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        default:
            /* Invalid group ordinal! */
            status = PH_ERR_BFL_INVALID_PARAMETER;
            break;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_OPCLT)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflOpCtl_Hw1GetAttribute(phcsBflOpCtl_AttribParam_t *attrib_param)
{
    phcsBflRegCtl_t              *lower  = ((phcsBflOpCtl_t*)(attrib_param->self))->mp_Lower;
    phcsBfl_Status_t              status;
    phcsBflRegCtl_GetRegParam_t   grp;
    phcsBflAux_CommandParam_t     scp;
    uint8_t                     buffer[25];
    uint8_t                     i;

    scp.lower           = lower;
    scp.wait_event_cb   = ((phcsBflOpCtl_t*)(attrib_param->self))->mp_WaitEventCB;
    scp.user_ref        = ((phcsBflOpCtl_t*)(attrib_param->self))->mp_UserRef;
    grp.self            = lower;

    switch (attrib_param->group_ordinal)
    {
        case PHCS_BFLOPCTL_GROUP_RCMETRICS:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_VERSION:
                    grp.address = PHCS_BFL_JREG_VERSION;
                    status = lower->GetRegister(&grp);
                    if (status == PH_ERR_BFL_SUCCESS)
                    {
                        attrib_param->param_a = grp.reg_data;
                        attrib_param->param_b = 0;
                    }
                    break;

                default:
                    /* We have no other metrics. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_MODE:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_ACTIVE:
                case PHCS_BFLOPCTL_ATTR_PASSIVE:
                case PHCS_BFLOPCTL_ATTR_MFRD_A:
                case PHCS_BFLOPCTL_ATTR_PAS_POL:
                case PHCS_BFLOPCTL_ATTR_MFCARD:
                    status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                    break;

                default:
                    /* Other values are not valid: */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_COMM:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_RS232BAUD:
                    status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                    break;

                default:
                    /* Other values are not valid: */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_SIGNAL:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_SIGINOUT:
                    status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                    break;

                default:
                    /* Other values are not valid: */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_TMR:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_TMR:
                case PHCS_BFLOPCTL_ATTR_TMR_AUTO:
                case PHCS_BFLOPCTL_ATTR_TMR_RESTART:
                case PHCS_BFLOPCTL_ATTR_TMR_GATED:
                case PHCS_BFLOPCTL_ATTR_TMR_PRESCALER:
                case PHCS_BFLOPCTL_ATTR_TMR_RELOAD:
                    status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                    break;

                case PHCS_BFLOPCTL_ATTR_TMR_COUNTER:
                    grp.address = PHCS_BFL_JREG_TCOUNTERVALHI;
                    status = lower->GetRegister(&grp);
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    attrib_param->param_b = grp.reg_data;

                    grp.address = PHCS_BFL_JREG_TCOUNTERVALLO;
                    status = lower->GetRegister(&grp);
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    attrib_param->param_a = grp.reg_data;
                    break;

                default:
                    /* We have no other metrics. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_CRC:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_CRC_RESULT:
                    grp.address = PHCS_BFL_JREG_CRCRESULT1;
                    status = lower->GetRegister(&grp);
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    attrib_param->param_b = grp.reg_data;

                    grp.address = PHCS_BFL_JREG_CRCRESULT2;
                    status = lower->GetRegister(&grp);
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    attrib_param->param_a = grp.reg_data;
                    break;

                case PHCS_BFLOPCTL_ATTR_CRC_PRESET:
                case PHCS_BFLOPCTL_ATTR_CRC_MSB_FIRST:
                case PHCS_BFLOPCTL_ATTR_CRC_CALC:
                    status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                    break;

                default:
                    /* We have no other metrics. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        case PHCS_BFLOPCTL_GROUP_PROTOCOL:
            switch (attrib_param->attrtype_ordinal)
            {
                case PHCS_BFLOPCTL_ATTR_NFCID:
                    scp.cmd       = PHCS_BFL_JCMD_CONFIG;
                    scp.rx_buffer = buffer;
                    scp.rx_bytes  = 25;
                    scp.tx_bytes  = 0;
                    status = phcsBflAux_Hw1Command(&scp);
                    RCOPCTL_HW1_BREAK_ON_ERROR(status);
                    switch(attrib_param->param_a)
                    {
                        case PHCS_BFLOPCTL_VAL_NFCID1: /* nfcid1 */
                            attrib_param->param_b = 6;

                            /* read relevant data */
                            for(i=0; i<attrib_param->param_b; i++)
                            {
                                attrib_param->buffer[i] = buffer[i];
                            }
                            break;

                        case PHCS_BFLOPCTL_VAL_NFCID2: /* nfcid2 */
                            attrib_param->param_b = 18;

                            /* read relevant data */
                            for(i=0;i<18;i++)
                            {
                                attrib_param->buffer[i] = buffer[6+i];
                            }
                            break;

                        case PHCS_BFLOPCTL_VAL_NFCID3: /* nfcid3 */
                            attrib_param->param_b = 10;

                            /* read relevant data */
                            for(i=0;i<3;i++)
                            {
                                attrib_param->buffer[i] = buffer[2+i];
                            }
                            for(i=0;i<6;i++)
                            {
                                attrib_param->buffer[3+i] = buffer[8+i];
                            }
                            attrib_param->buffer[9] = buffer[24];

                            break;

                        case PHCS_BFLOPCTL_VAL_NFCID_RANDOM:
                            status = PH_ERR_BFL_UNSUPPORTED_PARAMETER;
                            break;

                        default:
                            /* Invalid parameter. */
                            status = PH_ERR_BFL_INVALID_PARAMETER;
                            break;
                    }
                    break;

                default:
                    /* Invalid system attribute. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                    break;
            }
            break;

        default:
            /* We do not return other information! */
            status = PH_ERR_BFL_INVALID_PARAMETER;
            break;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_OPCLT)
    return status;
}




/* Internals: */

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflOpCtl_Hw1MapSerSpeedSet(uint8_t setting, uint8_t *mapped_value)
{
    phcsBfl_Status_t status = PH_ERR_BFL_SUCCESS;

    switch (setting)
    {
        case PHCS_BFLOPCTL_VAL_SER9600:
            *mapped_value = HW1_RS232_9600BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER19200:
            *mapped_value = HW1_RS232_19200BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER38400:
            *mapped_value = HW1_RS232_38400BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER57600:
            *mapped_value = HW1_RS232_57600BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER115200:
            *mapped_value = HW1_RS232_115200BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER230400:
            *mapped_value = HW1_RS232_230400BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER460800:
            *mapped_value = HW1_RS232_460800BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER921600:
            *mapped_value = HW1_RS232_921600BPS;
            break;

        case PHCS_BFLOPCTL_VAL_SER1288000:
            *mapped_value = HW1_RS232_1288000BPS;
            break;

        default:
            /* Other values are not valid: */
            status = PH_ERR_BFL_INVALID_PARAMETER;
            break;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_OPCLT)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflOpCtl_Hw1ManipulateRegs(phcsBflRegCtl_t  *lower,
                                            uint8_t         address,
                                            uint8_t         mask,
                                            uint8_t         shl,
                                            uint8_t         new_value)
{
    phcsBfl_Status_t             status;
    phcsBflRegCtl_GetRegParam_t  grp;
    phcsBflRegCtl_SetRegParam_t  srp;

    grp.self    = lower;
    grp.address = address;
    status = lower->GetRegister(&grp);
    if (status == PH_ERR_BFL_SUCCESS)
    {
        srp.self     =  lower;
        srp.address  =  address;
        /* Clear bits to be set to new value: */
        srp.reg_data =  (uint8_t)(grp.reg_data & (~mask));
        /* OR' in the new value, limited to the mask in order not to impact other bits: */
        srp.reg_data |= ((new_value << shl) & mask);
        status = lower->SetRegister(&srp);
    } else { /* Do nothing. */ }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_OPCLT)
    return status;
}

/* E.O.F. */
