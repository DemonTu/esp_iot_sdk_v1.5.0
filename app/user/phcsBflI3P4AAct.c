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

/*! \file phcsBflI3P4AAct.c
 *
 * Project: ISO 14443.4.
 *
 * Workfile: phcsBflI3P4AAct.c
 * $Author: mha $
 * $Revision: 1.4 $
 * $Date: Fri Jun 23 07:20:49 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  None.
 *
 * History:
 *  GK:  Generated 25. Oct. 2002
 *  MHa: Migrated to MoReUse September 2005
 *
 */

#include <phcsBflI3P4AAct.h>
#include "phcsBflI3P4AAct_Int.h"
//#include "includes.h"
#include "ets_sys.h"
#include "osapi.h"

/* ////////////////////////////////////////////////////////////////////////////////////////////// */
/* "Public" functions, visible for the user:                                                      */
/* ////////////////////////////////////////////////////////////////////////////////////////////// */


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P4AAct_Init(phcsBflI3P4AAct_t            *cif,
                                  phcsBflI3P4_ProtParam_t      *p_td,
                                  uint8_t                    *p_trxbuffer,
                                  uint16_t                    trxbuffersize,
                                  phcsBflIo_t                  *p_lower)
{
    p_td->trx_buffer_size     = trxbuffersize;
    p_td->trx_buffer          = p_trxbuffer;
    cif->m_trx_buffer_size    = trxbuffersize;
    cif->mp_Members           = p_td;
    cif->mp_Lower             = p_lower;
    cif->Rats                 = phcsBflI3P4AAct_Rats;
    cif->Pps                  = phcsBflI3P4AAct_Pps;

    return PH_ERR_BFL_SUCCESS;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P4AAct_Rats(phcsBflI3P4AAct_RatsParam_t *rats_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_ProtParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_ProtParam_t*)(((phcsBflI3P4AAct_t*)(rats_param->self))->mp_Members);

    static uint16_t FsiTable[9] = {16, 24, 32, 40, 48, 64, 96, 128, 256};
    phcsBfl_Status_t       PHCS_BFL_MEMLOC_REM   status        = PH_ERR_BFL_SUCCESS;
    uint8_t   PHCS_BFL_MEMLOC_COUNT    index         = 0;
    uint8_t   PHCS_BFL_MEMLOC_REM   parameter;
    uint8_t   PHCS_BFL_MEMLOC_REM   ats_format_byte;
    uint8_t   PHCS_BFL_MEMLOC_REM   int_fsci, int_fsdi;
    phcsBflIo_TransceiveParam_t PHCS_BFL_MEMLOC_REM transceive_param;

    /* Apply default values for exchange protocol (phcsBfl_I3P4) initialisation struct: */
    p_td->nad_supported = 0;
    p_td->cid_supported = 1;
    p_td->cid           = rats_param->cid;
    p_td->tx_baud_int   = PHCS_BFLI3P4_COMMON_BAUD_106_INT;
    p_td->rx_baud_int   = PHCS_BFLI3P4_COMMON_BAUD_106_INT;
    p_td->fwi           = 4;
    /* p_td->fsi is already initialised by the Initialize function. */
    /* p_td->trx_buffer is already initialised by the Initialize function. */

    /* Issue a RATS command and return the response! */
    /* Range check for CID and FSD */
    int_fsdi = rats_param->fsi;
    if ((int_fsdi <= PHCS_BFLI3P4AACT_RATS_MAX_FSDI) &&
        (((phcsBflI3P4AAct_t*)rats_param->self)->m_trx_buffer_size >= FsiTable[int_fsdi]) &&
        (rats_param->cid <= PHCS_BFLI3P4_COMMON_MAX_CID))
    {
        /* Parameterbyte */
        parameter = (uint8_t)((rats_param->cid & PHCS_BFLI3P4AACT_RATS_CID_MASK) |
                              (int_fsdi << PHCS_BFLI3P4AACT_RATS_FSDI_SHL_VAL));

        /* Build buffer */
        p_td->trx_buffer[index] = PHCS_BFLI3P4AACT_RATS_START_BYTE;
        index++;
        p_td->trx_buffer[index] = parameter;
        index++;

        /* Assignments */
        transceive_param.tx_buffer      = p_td->trx_buffer;
        transceive_param.tx_buffer_size = index;
        transceive_param.rx_buffer      = p_td->trx_buffer;
        transceive_param.rx_buffer_size = ((phcsBflI3P4AAct_t*)rats_param->self)->m_trx_buffer_size;
        transceive_param.self = ((phcsBflI3P4AAct_t*)rats_param->self)->mp_Lower;

        /* Transceive Function from lower layer, return status */
        status = ((phcsBflI3P4AAct_t*)rats_param->self)->mp_Lower->Transceive(&transceive_param);

        if (status == PH_ERR_BFL_SUCCESS)
        {
            /* Copy ATS into user buffer: */
            for (index = 0; index < transceive_param.rx_buffer_size; index++)
            {
                rats_param->ats[index] = transceive_param.rx_buffer[index];
            }

            /* Length according to Length Byte: */
            rats_param->ats_len = rats_param->ats[PHCS_BFLI3P4AACT_ATS_LEN_BYTE_POS];

            if (rats_param->ats_len == transceive_param.rx_buffer_size)
            {

                /* OK: Analyse: ATS from user buffer: */
                index = PHCS_BFLI3P4AACT_ATS_MIN_LENGTH;

                if (rats_param->ats_len == PHCS_BFLI3P4AACT_ATS_MIN_LENGTH)
                {
                    ats_format_byte = PHCS_BFLI3P4AACT_RATS_DEF_FORMAT;
                }
                else
                {
                    ats_format_byte =  rats_param->ats[PHCS_BFLI3P4AACT_ATS_FORM_BYTE_POS];
                }
                index++;

                /* FSCI */
                int_fsci = (uint8_t) (ats_format_byte & PHCS_BFLI3P4AACT_RATS_FSCI_MASK);
               // printf("FSCI = %d\r\n",int_fsci);
               // printf("FSDI= %d\r\n",int_fsdi);

                if (int_fsci > PHCS_BFLI3P4AACT_RATS_MAX_FSCI)
                {
                    /* interpret FSCI greater than RATS_MAX_FSCI as RATS_MAX_FSCI */
                    int_fsci = PHCS_BFLI3P4AACT_RATS_MAX_FSCI;
                }

                p_td->fsci = int_fsci;
                p_td->fsdi = int_fsdi;

                /* TA1 ? */
                if ((ats_format_byte & PHCS_BFLI3P4AACT_ATS_TA1_PRESENT) != 0)
                {
                    rats_param->ta1 =&(rats_param->ats[index]);
                    index++;
                } else
                {
                    rats_param->ta1 = NULL;
                }

                /* TB1 ? */
                if ((ats_format_byte & PHCS_BFLI3P4AACT_ATS_TB1_PRESENT) != 0)
                {

                    rats_param->tb1 = &(rats_param->ats[index]);
                    index++;

                    p_td->fwi = (uint8_t)((*rats_param->tb1 >> PHCS_BFLI3P4AACT_ATS_FWI_SHR_MASK) &
                                           PHCS_BFLI3P4AACT_ATS_FWI_MASK);

                } else
                {
                    rats_param->tb1 = NULL;
                }

                /* TC1 ? */
                if ((ats_format_byte & PHCS_BFLI3P4AACT_ATS_TC1_PRESENT) != 0)
                {
                    rats_param->tc1 = &(rats_param->ats[index]);
                    index++;
                    /* We set internal controls according to CID/NAD support status of the PICC: */
                    if (((*rats_param->tc1) & PHCS_BFLI3P4_COMMON_CID_SUP_MASK) != 0)
                    {
                        /* CID supported: */
                        /* Parameter cid_supported & Cid value set as default */
                    } else
                    {
                        /* CID not supported: */
                        p_td->cid_supported = 0;
                        p_td->cid           = 0;
                    }

                    if (((*rats_param->tc1) & PHCS_BFLI3P4_COMMON_NAD_SUP_MASK) != 0)
                    {
                        /* NAD supported: */
                        p_td->nad_supported = 1;
                    } else
                    {
                        /* NAD not supported: */
                        /* Do nothing since defaults are already initialised. */
                    }
                } else
                {
                    rats_param->tc1 = NULL;
                    /* Defaults of CID / NAD support are already applied. */
                }


                /* Historicals ? */
                if (rats_param->ats_len > index)
                {
                    rats_param->app_inf = &(rats_param->ats[index]);
                    rats_param->app_inf_len = (uint8_t)(rats_param->ats_len - index);
                } else
                {
                    rats_param->app_inf = NULL;
                    rats_param->app_inf_len = 0;
                }


            } else
            {
                /* Length byte and actual number of RX bytes mismatch! */
                status = PH_ERR_BFL_INVALID_FORMAT;
            }
        } else
        {
            /* TRx Error: No action, merely return status (already set).*/
        }
    } else
    {
        /* Input param check failed. */
        status = PH_ERR_BFL_INVALID_PARAMETER;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4AACT)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P4AAct_Pps(phcsBflI3P4AAct_PpsParam_t *pps_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_ProtParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_ProtParam_t*)(((phcsBflI3P4AAct_t*)(pps_param->self))->mp_Members);

    phcsBfl_Status_t               PHCS_BFL_MEMLOC_REM   status            = PH_ERR_BFL_SUCCESS;
    uint8_t           PHCS_BFL_MEMLOC_COUNT    index             = 0;
    phcsBflIo_TransceiveParam_t   PHCS_BFL_MEMLOC_REM   transceive_param;
    uint8_t           PHCS_BFL_MEMLOC_REM   cid = (uint8_t)
                                                        ((p_td->cid_supported != 0) ? pps_param->cid : 0);
    uint8_t           PHCS_BFL_MEMLOC_REM   int_dri = pps_param->dri;
    uint8_t           PHCS_BFL_MEMLOC_REM   int_dsi = pps_param->dsi;
    uint8_t           PHCS_BFL_MEMLOC_REM   ppss;

    if ((cid <= PHCS_BFLI3P4_COMMON_MAX_CID) &&
        (int_dsi <= PHCS_BFLI3P4AACT_PPS_MAX_DSI) &&
        (int_dri <= PHCS_BFLI3P4AACT_PPS_MAX_DRI))
    {
        /* OK, issue a PPS */
        /* Startbyte, Parameter 0, Parameter 1*/
        ppss = (uint8_t)((pps_param->cid & PHCS_BFLI3P4AACT_PPS_CID_MASK) |
                          PHCS_BFLI3P4AACT_PPS_PPSS_TEMPL);
        p_td->trx_buffer[index] = ppss;
        index++;
        p_td->trx_buffer[index] = PHCS_BFLI3P4AACT_PPS_PARAMETER0;
        index++;
        p_td->trx_buffer[index] = (uint8_t)((int_dsi << PHCS_BFLI3P4AACT_PPS_DSI_SHL_VAL) | int_dri);
        index++;

        /* Assignments */
        transceive_param.tx_buffer      = p_td->trx_buffer;
        transceive_param.tx_buffer_size = index;
        transceive_param.rx_buffer      = p_td->trx_buffer;
        transceive_param.rx_buffer_size = p_td->trx_buffer_size;
        transceive_param.self = ((phcsBflI3P4AAct_t*)pps_param->self)->mp_Lower;

        /* Transceive function from lower layer, return status */
        status = ((phcsBflI3P4AAct_t*)pps_param->self)->mp_Lower->Transceive(&transceive_param);

        if (status == PH_ERR_BFL_SUCCESS)
        {
            if ((transceive_param.rx_buffer_size == PHCS_BFLI3P4AACT_PPS_RESPLEN) &&
                (transceive_param.rx_buffer[PHCS_BFLI3P4AACT_PPS_PPSS_POS] == ppss))
            {
                /* OK: We can apply the new parameters! */
                p_td->tx_baud_int = int_dsi;
                p_td->rx_baud_int = int_dri;
            } else
            {
                /* Error: Response is not what we expected. */
                status = PH_ERR_BFL_INVALID_FORMAT;
            }
        } else
        {
            /* TRx error! Merely return status. */
        }
    } else
    {
      status = PH_ERR_BFL_INVALID_PARAMETER;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4AACT)
    return status;
}


/* EOF */
