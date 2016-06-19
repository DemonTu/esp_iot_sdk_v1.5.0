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

/*! \file phcsBflI3P4.c
 *
 * Project: ISO 14443.4.
 *
 * Workfile: phcsBflI3P4.c
 * $Author: bs $
 * $Revision: 1.9 $
 * $Date: Tue Jul  4 16:43:42 2006 $
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

#include <phcsBflI3P4.h>
#include "phcsBflI3P4_Int.h"
#include "phcsBflI3P4_PcdInt.h"

//#include "includes.h"
#include "ets_sys.h"
#include "osapi.h"

/* ////////////////////////////////////////////////////////////////////////////////////////////// */
/* "Private" functions with headers, not visible for the user:                                                      */
/* ////////////////////////////////////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Build S Block:
// *p_td    Pointer to the Communication Parameters structure.
// *p_cid   Pointer to the CID, procided by the caller. If NULL, no CID is used.
//  type    Determines the type of the S-block (see header file).
//  Return: Size of the S-Block just built. Block itself resides in the TRx buffer.
*/
static uint8_t BuildS(phcsBflI3P4_CommParam_t     *p_td,
                      uint8_t                   *p_cid,
                      uint8_t                    type,
                      uint8_t                    info);

/* //////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Build R Block:
// *p_td    Pointer to the Communication Parameters structure.
// *p_cid   Pointer to the CID, procided by the caller. If NULL, no CID is used.
//  type    Determines the type of the R-block (see header file).
//  blockNr ...
//  Return: Size of the R-Block just built. Block itself resides in the TRx buffer.
*/
static uint8_t BuildR(phcsBflI3P4_CommParam_t     *p_td,
                      uint8_t                   *p_cid,
                      uint8_t                    type,
                      uint8_t                    blockNr);


/* //////////////////////////////////////////////////////////////////////////////////////////////
// T=CL Build I Block:
// *p_td    Pointer to the Communication Parameters structure.
// *p_cid   Pointer to the CID, procided by the caller. If NULL, no CID is used.
// *p_nad   Pointer to the NAD, procided by the caller. If NULL, no NAD is used.
// *usrbuf  Pointer to the large user buffer.
//  usrbuf_size  ...
//  Return: Size of the I-Block just built. Block itself resides in the TRx buffer.
//
//  Comment: Initially, at the beginning of EXCHANGE this function expects
//           p_td->mp_StartOfBlock and p_td->mp_CopyBytes to be set to the
//           address of element [0] of the usrbuf. After a successful exchange,
//           it is within the responsibility of the EXCHANGE function to set
//           p_td->mp_StartOfBlock to the value of p_td->mp_CopyBytes. After an
//           unsuccessful exchange, the EXCHANGE function must set the value of
//           p_td->mpCopyBytes to p_td->mp_StartOfBlock.
*/
static uint16_t BuildI(phcsBflI3P4_CommParam_t    *p_td,
                       uint8_t                  *p_cid,
                       uint8_t                  *p_nad,
                       uint8_t                  *usrbuf,
                       uint16_t                  usrbuf_size,
                       uint8_t                   override_mi);

static phcsBfl_Status_t ReceiveI(phcsBflI3P4_CommParam_t    *p_td,
                               uint16_t                  current_rx_buffer_size,
                               uint8_t                  *usrbuf,
                               uint32_t                  usrbuf_size,
                               uint16_t                 *current_usrbuf_size);


static uint8_t ReceiveSWtx(phcsBflI3P4_CommParam_t *p_td);

static phcsBfl_Status_t ReqReaderParameterCb(phcsBflI3P4_t  *cif, uint8_t  dri, uint8_t  dsi, uint8_t fwi, void *p_user);


static phcsBfl_Status_t ReqWtxCb(phcsBflI3P4_t  *cif, uint8_t  set, uint8_t  wtxm, uint8_t fwi, void *p_user);


static void SetState(phcsBflI3P4_CommParam_t *p_td,
                     uint8_t current,
                     uint8_t state);

static uint8_t GetState(phcsBflI3P4_CommParam_t *p_td, uint8_t current);

static phcsBfl_Status_t CheckRAck(phcsBflI3P4_CommParam_t *p_td,
                                uint16_t               current_rx_buffer_size);

static phcsBfl_Status_t CheckSWtx(phcsBflI3P4_CommParam_t *p_td,
                                uint16_t               current_rx_buffer_size);

static phcsBfl_Status_t CheckI(phcsBflI3P4_CommParam_t *p_td,
                             uint16_t               current_rx_buffer_size);

/*static void BranchIntoErrorState(phcsBflI3P4_CommParam_t *p_td,
                                 phcsBfl_Status_t status); */

static phcsBfl_Status_t CheckCid(phcsBflI3P4_CommParam_t *p_td);

/* End of internal prototypes. ********************************************************************/

static uint8_t ICACHE_FLASH_ATTR
BuildS(phcsBflI3P4_CommParam_t     *p_td,
                      uint8_t                   *p_cid,
                      uint8_t                    type,
                      uint8_t                    info)
{
    uint8_t PHCS_BFL_MEMLOC_COUNT   index    = I3P4_S_BLOCK_MIN_LEN;
    uint8_t PHCS_BFL_MEMLOC_REM  pcb      = I3P4_S_BLOCK_TEMPLATE;

    pcb |= type;

    if (p_cid != NULL)
    {
        pcb |= I3P4_IRS_CID_PRESENT_MASK;
        p_td->mp_TRxBuffer[index] = *p_cid;
        index++;
    }


    p_td->mp_TRxBuffer[I3P4_PCB_POS] = pcb;

    if(type != I3P4_S_BLOCK_DESELECT)
    {
        p_td->mp_TRxBuffer[index] = info;
        index++;
    }
    return index;

}


static uint8_t ICACHE_FLASH_ATTR
BuildR(phcsBflI3P4_CommParam_t     *p_td,
                      uint8_t                   *p_cid,
                      uint8_t                    type,
                      uint8_t                    blockNr)
{

    uint8_t PHCS_BFL_MEMLOC_COUNT   index    =  I3P4_R_BLOCK_MIN_LEN;
    uint8_t PHCS_BFL_MEMLOC_REM  pcb      =  I3P4_R_BLOCK_TEMPLATE;
    uint8_t PHCS_BFL_MEMLOC_REM  bNr      =  0;

    bNr = (uint8_t)(blockNr & I3P4_R_BLOCKNR_MASK);

    pcb |= type |= bNr;


    if (p_cid != NULL)
    {
        pcb |= I3P4_IRS_CID_PRESENT_MASK;
        p_td->mp_TRxBuffer[index] = *p_cid;
        index++;
    }

    p_td->mp_TRxBuffer[I3P4_PCB_POS ] = pcb;

    return index;
}


static uint16_t ICACHE_FLASH_ATTR
BuildI(phcsBflI3P4_CommParam_t    *p_td,
                       uint8_t                  *p_cid,
                       uint8_t                  *p_nad,
                       uint8_t                  *usrbuf,
                       uint16_t                  usrbuf_size,
                       uint8_t                   override_mi)
{
    uint16_t PHCS_BFL_MEMLOC_COUNT  index         = 1;
    uint8_t  PHCS_BFL_MEMLOC_REM bNr           = 0;
    uint8_t  PHCS_BFL_MEMLOC_REM pcb           = I3P4_I_BLOCK_TEMPLATE;



    if ((p_td->mp_StartOfBlock <= (usrbuf + usrbuf_size)) &&
        (p_td->mp_StartOfBlock >= usrbuf) &&
        (p_td->mp_CopyBytes <= (usrbuf + usrbuf_size)) &&
        (p_td->mp_CopyBytes >= usrbuf))
    {

        /* PCB preparation: */
        bNr = (uint8_t)(p_td->m_BlockNumber & I3P4_I_BLOCKNR_MASK);
        pcb |= bNr;

        /* CID/NAD to build into this frame? */
        if (p_cid != NULL)
        {

            pcb |= I3P4_IRS_CID_PRESENT_MASK;
            p_td->mp_TRxBuffer[index] = *p_cid;
            index++;
        }
        if (p_nad != NULL)
        {

            pcb |= I3P4_I_NAD_PRESENT_MASK;
            p_td->mp_TRxBuffer[index] = *p_nad;
            index++;

            p_td->m_Flags |= PHCS_BFLI3P4_FLAG_INCLUDE_NAD;
			/* This flag is need for NAD handling during META chaining */
			p_td->m_Flags |= PHCS_BFLI3P4_FLAG_INCLUDE_NAD_BEFORE;
        }

        p_td->mp_TRxBuffer[I3P4_PCB_POS] = pcb;


        /*
         * Copy data until the end of the internal TRx-buffer
         * or external user buffer is reached:
         */
        while ((p_td->mp_CopyBytes < (usrbuf + usrbuf_size)) &&
               (index < (p_td->m_MaxTxBufferSize)))
        {

            p_td->mp_TRxBuffer[index] = *p_td->mp_CopyBytes;
            index++;
            p_td->mp_CopyBytes ++;

        }

        /* Still not reached the end of the user buffer? Set MI flag! */
        if ((p_td->mp_CopyBytes < (usrbuf+usrbuf_size)) || override_mi)
        {
            pcb |= I3P4_CHAINING_BIT_MASK;
            p_td->mp_TRxBuffer[I3P4_PCB_POS] = pcb;
        }
    } else
    {
        /*
         * Error: The pointers to walk through the user buffer
         * have not been initialised! The value of index is still
         * zero thus we do nothing. This branch should never be
         * entered with a correct EXCHANGE implementation.
         */
    }

    /* Tell the calling function about how many bytes to send: */
    return index;
}

static phcsBfl_Status_t ICACHE_FLASH_ATTR
ReceiveI(phcsBflI3P4_CommParam_t    *p_td,
                               uint16_t                  current_rx_buffer_size,
                               uint8_t                  *usrbuf,
                               uint32_t                  usrbuf_size,
                               uint16_t                 *current_usrbuf_size)


{
    phcsBfl_Status_t      PHCS_BFL_MEMLOC_REM status        = PH_ERR_BFL_SUCCESS;
    uint16_t PHCS_BFL_MEMLOC_COUNT  index         = 1; /* PCB always present: Omit. */


    if ((p_td->mp_StartOfBlock >= usrbuf) &&
        (p_td->mp_CopyBytes >= usrbuf))
    {
        if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0)
        {
            index ++;
        }
        if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_INCLUDE_NAD) != 0)
        {
            index ++;
        }

        /* don't allow receiving of further NADs in subsequent blocks */
        p_td->m_Flags &= PHCS_BFLI3P4_FLAG_INCLUDE_NAD_I;

        /*
         * Copy data from exchange buffer to userbuffer until the end
         * of the exchange buffer
         */

        while (index < current_rx_buffer_size)
        {
            if (p_td->mp_CopyBytes == (usrbuf + usrbuf_size))
            {
                p_td->m_Size = (uint8_t)(current_rx_buffer_size);

                p_td->m_Index = (uint8_t)(index);

                status = PH_ERR_BFL_USERBUFFER_FULL;

                break;

            } else
            {
                *p_td->mp_CopyBytes = p_td->mp_TRxBuffer[index];
                index++;
                p_td->mp_CopyBytes++;

                (*current_usrbuf_size)++;
            }
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}


static uint8_t ICACHE_FLASH_ATTR
ReceiveSWtx(phcsBflI3P4_CommParam_t *p_td)

{
    uint8_t  PHCS_BFL_MEMLOC_COUNT  index = 1;
    uint8_t  PHCS_BFL_MEMLOC_REM inf;

    if(( p_td->m_Flags & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0)
    {
        index ++;
    }

    inf = p_td->mp_TRxBuffer[index];

    return inf;
}

static phcsBfl_Status_t ICACHE_FLASH_ATTR
ReqReaderParameterCb(phcsBflI3P4_t  *cif, uint8_t  dri, uint8_t  dsi, uint8_t fwi, void *p_user)
{
    phcsBfl_Status_t                PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;
    phcsBflI3P4_CbSetReaderParam_t  PHCS_BFL_MEMLOC_REM sbp;

    if (cif->CbSetReaderParams != NULL)
    {
        sbp.dri    = dri;
        sbp.dsi    = dsi;
        sbp.fwi    = fwi;
        sbp.p_user_reader_param = p_user;
        #ifdef PHFL_BFL_CPP
            sbp.object = cif->mp_CppWrapper;
        #else
            sbp.object = cif;
        #endif
        status = cif->CbSetReaderParams(&sbp);
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}


static phcsBfl_Status_t ICACHE_FLASH_ATTR
ReqWtxCb(phcsBflI3P4_t  *cif, uint8_t  set, uint8_t  wtxm, uint8_t fwi, void *p_user)
{
    phcsBfl_Status_t             PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_INTERFACE_NOT_ENABLED;
    phcsBflI3P4_CbSetWtxParam_t  PHCS_BFL_MEMLOC_REM swp;

    if (cif->CbSetWtx != NULL)
    {
        swp.set  = set;
        swp.wtxm = wtxm;
        swp.fwi  = fwi;
        swp.p_user_wtx = p_user;
        #ifdef PHFL_BFL_CPP
            swp.object = cif->mp_CppWrapper;
        #else
            swp.object = cif;
        #endif
        cif->CbSetWtx(&swp);
        status = PH_ERR_BFL_SUCCESS;
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}


static void ICACHE_FLASH_ATTR
SetState(phcsBflI3P4_CommParam_t *p_td,
                     uint8_t current,
                     uint8_t state)
{
    /* Clear half-byte to modify (use the other mask always). */
    p_td->m_State &= ((current != 0) ? PHCS_BFLI3P4_STATE_PREV_MASK :
                                            PHCS_BFLI3P4_STATE_CUR_MASK);
    p_td->m_State |= ((current != 0) ? (state & PHCS_BFLI3P4_STATE_CUR_MASK) :
                                            (state << 4));
}


static uint8_t ICACHE_FLASH_ATTR
GetState(phcsBflI3P4_CommParam_t *p_td, uint8_t current)
{
    return (uint8_t)((current != 0) ? (p_td->m_State & PHCS_BFLI3P4_STATE_CUR_MASK) :
                                          (p_td->m_State >> 4));
}


static phcsBfl_Status_t ICACHE_FLASH_ATTR
CheckRAck(phcsBflI3P4_CommParam_t *p_td,
                           uint16_t current_rx_buffer_size)
{
    phcsBfl_Status_t      PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_INVALID_FORMAT;
    uint8_t  PHCS_BFL_MEMLOC_REM pcb;

    if ((current_rx_buffer_size <= I3P4_R_BLOCK_MAX_LEN) &&
        (current_rx_buffer_size >= I3P4_R_BLOCK_MIN_LEN))
    {
        /* Frame OK [length]: */
        pcb = p_td->mp_TRxBuffer[I3P4_PCB_POS];

        if ((pcb & I3P4_RACK_PCB_CHKMASK) == I3P4_R_BLOCK_TEM_PICC)
        {
            /* Yes, it's a R(ACK): */
            if ((pcb & I3P4_RACK_PCB_BLOCKNR) == p_td->m_BlockNumber)
            {
                /* Block-# match. */
                status = CheckCid(p_td);
                if (status == PH_ERR_BFL_SUCCESS)
                {
                    /* OK, we needn't do anything. */
                } else
                {
                    /* CID (usage) mismatch. */
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                }
            } else
            {
                /* BL#-mismatch. */
                status = PH_ERR_BFL_BLOCKNR_NOT_EQUAL;
            }
        } else
        {
            /* Nope, the mask reveals something else than R(ACK). */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* Frame length NOK: */
        status = PH_ERR_BFL_INVALID_FORMAT;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}


static phcsBfl_Status_t ICACHE_FLASH_ATTR
CheckSWtx(phcsBflI3P4_CommParam_t *p_td,
                           uint16_t current_rx_buffer_size)
{

    phcsBfl_Status_t      PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_INVALID_FORMAT;
    uint8_t  PHCS_BFL_MEMLOC_REM pcb;

    if ((current_rx_buffer_size <= I3P4_S_BLOCK_MAX_LEN) &&
        (current_rx_buffer_size >= I3P4_S_BLOCK_MIN_LEN))
    {
        /* Frame OK [length]: */
        pcb = p_td->mp_TRxBuffer[I3P4_PCB_POS];

        if ((pcb & I3P4_SWTX_PCB_CHKMASK) ==
            (I3P4_S_BLOCK_TEMP_PICC | I3P4_S_BLOCK_WTX))
        {
            /* Yes, we have a S(WTX): */
            status = CheckCid(p_td);
            if (status == PH_ERR_BFL_SUCCESS)
            {
                /* OK, we do nothing. */
            } else
            {
                /* No CID (usage) match: */
                status = PH_ERR_BFL_INVALID_PARAMETER;
            }
        } else
        {
            /* No S(WTX): */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* Invalid block length: */
        status = PH_ERR_BFL_INVALID_FORMAT;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}


static phcsBfl_Status_t ICACHE_FLASH_ATTR
CheckI(phcsBflI3P4_CommParam_t *p_td,
                               uint16_t current_rx_buffer_size)
{
    phcsBfl_Status_t      PHCS_BFL_MEMLOC_REM status        = PH_ERR_BFL_INVALID_FORMAT;
    uint8_t  PHCS_BFL_MEMLOC_REM pcb;
    uint8_t  PHCS_BFL_MEMLOC_REM received_nad;
    uint8_t  PHCS_BFL_MEMLOC_REM index         = 0;

    if (current_rx_buffer_size >= I3P4_I_BLOCK_MIN_LEN)
    {
        /* Frame OK [length]: */
        pcb = p_td->mp_TRxBuffer[I3P4_PCB_POS];

        if ((pcb & I3P4_I_PCB_CHKMASK ) == I3P4_I_BLOCK_TEMPLATE)
        {
            if ((pcb & I3P4_I_PCB_BLOCKNR) == p_td->m_BlockNumber)
            {
                status = CheckCid(p_td);
                if (status == PH_ERR_BFL_SUCCESS)
                {
                    /* OK, we check NAD. */
                    if ((pcb & I3P4_I_NAD_PRESENT_MASK) != 0)
                    {
                        /* PCB indicates NAD presence; is that OK? */
                        if (((p_td->m_Flags & PHCS_BFLI3P4_COMMON_NAD_SUP_MASK) != 0) &&
                            ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_INCLUDE_NAD)) != 0)
                        {
                            /* OK, a NAD is allowed to be present, check value: */
                            /* The returned NAD must be half-byte-wise reversed: */
                            if ((pcb & I3P4_IRS_CID_PRESENT_MASK) != 0)
                            {
                                index ++;
                            }
                            received_nad = p_td->mp_TRxBuffer[index+1];

                            if ((received_nad & I3P4_NAD_MASK) == (I3P4_NAD_TEMPLATE))
                            {
                                /* In p_td->m_NAD we still have the sent NAD: */
                                if ((uint8_t)(((received_nad << 4) & 0xFF)
                                     | (received_nad >> 4)) == *p_td->m_NAD)
                                {
                                    /* OK, NAD values match! */
                                    status = PH_ERR_BFL_SUCCESS;

                                    /* p_td->m_NAD now gets the received NAD: */
                                    *p_td->m_NAD = received_nad;
                                } else
                                {
                                    /* NAD value mismatch! */
                                     status = PH_ERR_BFL_INVALID_PARAMETER;
                                }
                            } else
                            {
                                status = PH_ERR_BFL_INVALID_PARAMETER;
                            }

                        } else
                        {
                            if ((!p_td->m_Flags & PHCS_BFLI3P4_FLAG_NAD_SUP_MASK) &&
                            (p_td->m_Flags & PHCS_BFLI3P4_FLAG_INCLUDE_NAD))
                            {
                                /* No NAD allowed, error: */
                                status = PH_ERR_BFL_INVALID_PARAMETER;
                            }
                        }
                    } else
                    {
                        /* there is no NAD present, but it has to be */
                        if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_INCLUDE_NAD) != 0)
                        {
                            status = PH_ERR_BFL_INVALID_PARAMETER;
                        }
                    }
                } else
                {
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                }
            } else
            {
                status = PH_ERR_BFL_BLOCKNR_NOT_EQUAL;
            }
        } else
        {
            status = PH_ERR_BFL_INVALID_FORMAT;
        }

    } else
    {
        /*Length NOK: */
        status = PH_ERR_BFL_INVALID_FORMAT;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;

}


/*static void BranchIntoErrorState(phcsBflI3P4_CommParam_t *p_td,
                                 phcsBfl_Status_t status)
{
    const uint8_t CURRENT  = PHCS_BFLI3P4_STATE_CURRENT;
    const uint8_t PREVIOUS = PHCS_BFLI3P4_STATE_PREVIOUS;
    uint8_t       last_state;

    if (status != PH_ERR_BFL_SUCCESS)
    {
        last_state = GetState(p_td, CURRENT);
        SetState(p_td,
                 CURRENT,
                 p_td->m_Flags & PHCS_BFLI3P4_FLAG_PICC_CHAINING ? PHCS_BFLI3P4_STATE_EACK:
                 PHCS_BFLI3P4_STATE_ENAK);
        SetState(p_td, PREVIOUS, last_state);
    }
}*/


static phcsBfl_Status_t ICACHE_FLASH_ATTR
CheckCid(phcsBflI3P4_CommParam_t *p_td)
{
    phcsBfl_Status_t      PHCS_BFL_MEMLOC_REM status;
    uint8_t  PHCS_BFL_MEMLOC_REM pcb;
    uint8_t  PHCS_BFL_MEMLOC_REM int_cid;

    pcb = p_td->mp_TRxBuffer[I3P4_PCB_POS];

    if ((pcb & I3P4_IRS_CID_PRESENT_MASK) != 0)
    {
        /* Yes, we have a CID, is it allowed to have one? */
        if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0)
        {
            /* OK: CID is supported & PICC has sent one! */
            /* Check value: */
            int_cid = p_td->mp_TRxBuffer[I3P4_CID_POS];

            if ((int_cid & I3P4_CID_CHECK_MASK) == (I3P4_CID_TEMPLATE))
            {
                if ((p_td->m_CID) == (int_cid & I3P4_CID_MASK))
                {
                    status = PH_ERR_BFL_SUCCESS;
                } else
                {
                    status = PH_ERR_BFL_INVALID_PARAMETER;
                }
            }else
            {
                status = PH_ERR_BFL_INVALID_PARAMETER;
            }
        } else
        {
            /* Error: CID not supported but received. */
            status = PH_ERR_BFL_INVALID_PARAMETER;
        }
    } else
    {
        /* No CID in INF block; none required? */
        if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0)
        {
            /* Error: CID is supported but PICC has not sent one! */
            status = PH_ERR_BFL_INVALID_PARAMETER;
        } else
        {
            /* OK: CID not supported & not received. */
            status = PH_ERR_BFL_SUCCESS;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}






/* ////////////////////////////////////////////////////////////////////////////////////////////// */
/* "Public" functions, visible for the user:                                                      */
/* ////////////////////////////////////////////////////////////////////////////////////////////// */

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P4_Init(phcsBflI3P4_t               *cif,
                              phcsBflI3P4_CommParam_t     *p_td,
                              phcsBflIo_t                 *p_lower)
{
	phcsBflI3P4_ResetProtParam_t reset_protocol;
    phcsBflI3P4_SetPUserParam_t     set_p_user;

    phcsBfl_Status_t  PHCS_BFL_MEMLOC_REM status = PH_ERR_BFL_SUCCESS;
    cif->mp_Members           = p_td;
    cif->mp_Lower             = p_lower;
    cif->Exchange             = phcsBflI3P4_Exchange;
    cif->PresenceCheck        = phcsBflI3P4_PresCheck;
    cif->Deselect             = phcsBflI3P4_Deselect;
    cif->ResetProt            = phcsBflI3P4_ResetProt;
    cif->SetPUser             = phcsBflI3P4_SetPUser;
    cif->SetProtocol          = phcsBflI3P4_SetProt;
    cif->SetCallbacks         = phcsBflI3P4_SetCallbacks;
	/* Callbacks: */
    cif->CbSetWtx             = NULL;
    cif->CbSetReaderParams    = NULL;

    /* Reset Protocol */
	reset_protocol.self = cif;
	cif->ResetProt(&reset_protocol);

    /* User pointer */
    set_p_user.p_user_reader_param = NULL;
    set_p_user.p_user_wtx = NULL;
    set_p_user.self = cif;
    cif->SetPUser(&set_p_user);

    return status;
}


void ICACHE_FLASH_ATTR
phcsBflI3P4_SetCallbacks (phcsBflI3P4_SetCbParam_t *set_cb_param)
{
    phcsBflI3P4_t PHCS_BFL_MEMLOC_REM *cif = (phcsBflI3P4_t*) set_cb_param->self;
    cif->CbSetWtx           = set_cb_param->set_wtx_cb;
    cif->CbSetReaderParams  = set_cb_param->set_reader_params_cb;

}


void ICACHE_FLASH_ATTR
phcsBflI3P4_SetProt(phcsBflI3P4_SetProtParam_t *set_protocol_param)
{
    static uint16_t FsiTable[9] = {16, 24, 32, 40, 48, 64, 96, 128, 256};
	phcsBflI3P4_ProtParam_t PHCS_BFL_MEMLOC_REM *p_pp = set_protocol_param->p_protocol_param;

	 /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_CommParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_CommParam_t*)(((phcsBflI3P4_t*)(set_protocol_param->self))->mp_Members);

	phcsBflI3P4_ResetProtParam_t reset_protocol;
    reset_protocol.self = set_protocol_param->self;
    phcsBflI3P4_ResetProt(&reset_protocol);


    p_td->m_MaxRxBufferSize = (uint16_t)(FsiTable[p_pp->fsdi] - I3P4_CRC_LEN);

    p_td->m_MaxTxBufferSize = (uint16_t)(FsiTable[p_pp->fsci] - I3P4_CRC_LEN);

    p_td->mp_TRxBuffer        = p_pp->trx_buffer;

    p_td->m_CID               = (uint8_t)((p_pp->cid <= PHCS_BFLI3P4_COMMON_MAX_CID) ?
                                    p_pp->cid : PHCS_BFLI3P4_COMMON_MAX_CID);
    p_td->m_DRI               = p_pp->rx_baud_int;
    p_td->m_DSI               = p_pp->tx_baud_int;
    p_td->m_FWI               = p_pp->fwi;

    p_td->m_MaxRetryCounter   = set_protocol_param->max_retry;

    if ((p_pp->cid_supported) != 0)
    {
        p_td->m_Flags |= PHCS_BFLI3P4_FLAG_CID_SUP_MASK ;
    }

    if ((p_pp->nad_supported) != 0)
    {
        p_td->m_Flags |= PHCS_BFLI3P4_FLAG_NAD_SUP_MASK ;
    }
}


void ICACHE_FLASH_ATTR
phcsBflI3P4_ResetProt(phcsBflI3P4_ResetProtParam_t *reset_protocol_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_CommParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_CommParam_t*)(((phcsBflI3P4_t*)(reset_protocol_param->self))->mp_Members);

    p_td->m_BlockNumber        = PHCS_BFLI3P4_INIT_RD_BLNR;
    p_td->m_Flags              = PHCS_BFLI3P4_FLAG_RESET_VALUE;
    p_td->m_State              = PHCS_BFLI3P4_STATE_RESET_VALUE;
	p_td->m_HistoryFlag        = PHCS_BFLI3P4_FLAG_NO_HISTORY;
}

void ICACHE_FLASH_ATTR
phcsBflI3P4_SetPUser(phcsBflI3P4_SetPUserParam_t *set_p_user_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_CommParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_CommParam_t*)(((phcsBflI3P4_t*)(set_p_user_param->self))->mp_Members);

    p_td->mp_UserReaderParam = set_p_user_param->p_user_reader_param;
    p_td->mp_UserWTX = set_p_user_param->p_user_wtx;

}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P4_Exchange(phcsBflI3P4_ExchangeParam_t *exchange_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_CommParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_CommParam_t*)(((phcsBflI3P4_t*)(exchange_param->self))->mp_Members);
    /* State machine exit flag: */
    uint8_t           PHCS_BFL_MEMLOC_REM my_exit                        = 0;
    /* Temporarily disable default behaviour to TRx upon state transition (e.g. resend block):*/
    uint8_t           PHCS_BFL_MEMLOC_REM inhibit_trx                 = 0;
    /* Current number of bytes sent/received (in the small Tx/Rx buffer). */
    uint16_t          PHCS_BFL_MEMLOC_COUNT current_tx_buffer_size       = 0;
    uint16_t          PHCS_BFL_MEMLOC_COUNT current_rx_buffer_size       = 0;
    /* Current number of bytes in the large User (-receive) buffer. */
    uint16_t          PHCS_BFL_MEMLOC_COUNT current_user_rx_buffer_size  = 0;
    /* Protocol Control Byte, built upon parameters/settings/state. */
    uint8_t           PHCS_BFL_MEMLOC_REM pcb;
    /* Remember start of data transmitted in order to be able to re-send info in case of failure. */
    uint8_t           PHCS_BFL_MEMLOC_REM *last_StartofBlock          = 0;
    /* This variable codes the previous state in order to remember last action. */
    uint8_t           PHCS_BFL_MEMLOC_REM previous_state;
    /* In case of S(WTX) the INF block has a 1-byte-content: */
    uint8_t           PHCS_BFL_MEMLOC_REM wtx_value;
    /* The WTXM value masked from the wtx_value byte above. */
    uint8_t           PHCS_BFL_MEMLOC_REM int_wtxm;
    /* Retry counter, incremented each time a communication attempt is repeated. */
    uint8_t           PHCS_BFL_MEMLOC_COUNT  retry                       = 0;
    /* Points to the COMMUNICATION PARAMETER structure's NAD member. */
    uint8_t           PHCS_BFL_MEMLOC_REM *NAD                        = NULL;
    /* Parameter to pass over to the lower layer's TRx function. */
    phcsBflIo_TransceiveParam_t   PHCS_BFL_MEMLOC_REM transceive_param;

    phcsBfl_Status_t               PHCS_BFL_MEMLOC_REM status                       = PH_ERR_BFL_SUCCESS;
	phcsBfl_Status_t               PHCS_BFL_MEMLOC_REM istatus                      = PH_ERR_BFL_SUCCESS;


    /* Check NAD */
     if (exchange_param->nad_send != NULL)
    {
        if ((*exchange_param->nad_send & I3P4_NAD_MASK) == (I3P4_NAD_TEMPLATE))
        {
            p_td->m_NAD = exchange_param->nad_send;
            NAD = p_td->m_NAD;
            /* Clear NAD Include Bit */
            p_td->m_Flags &= PHCS_BFLI3P4_FLAG_INCLUDE_NAD_I;

        } else
        {
             return status = PH_ERR_BFL_INVALID_PARAMETER | PH_ERR_BFL_I3P4;
        }

    } else
    {
        p_td->m_NAD = exchange_param->nad_send;
        NAD = p_td->m_NAD;
        /* Clear NAD Include Bit */
        p_td->m_Flags &= PHCS_BFLI3P4_FLAG_INCLUDE_NAD_I;

    }

    /* Call Callback for Bitrate and frame waiting time */
    status = ReqReaderParameterCb((phcsBflI3P4_t*) exchange_param->self, p_td->m_DRI, p_td->m_DSI, p_td->m_FWI, p_td->mp_UserReaderParam);

    /* Added check of Callback here to ensure no transmission of data if anythig wrong in CB */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        return status;
    }

    /* Reset pointers: */
    p_td->mp_StartOfBlock = exchange_param->snd_buf;
    p_td->mp_CopyBytes = p_td->mp_StartOfBlock;

    /* Type of function entry
     * (EI has been set when exiting because of too many data received):
     */
    if ((GetState(p_td, PHCS_BFLI3P4_STATE_CURRENT) == PHCS_BFLI3P4_STATE_EI)
        && ((exchange_param->flags & PHCS_BFLI3P4_CONT_CHAINING)) != 0)
    {

        exchange_param->snd_buf_len = 0;
        exchange_param->flags &= PHCS_BFLI3P4_CONT_CHAINING_I;

        /* Reset Userbuffer Pointers*/
        p_td->mp_StartOfBlock = exchange_param->rec_buf;
        p_td->mp_CopyBytes = p_td->mp_StartOfBlock;


        while (p_td->m_Index < p_td->m_Size)
        {

            *p_td->mp_CopyBytes = p_td->mp_TRxBuffer[p_td->m_Index];

            p_td->mp_CopyBytes ++;

            p_td->m_Index ++;
        }

        if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_PICC_APDU_CHAIN) != 0)
        {
        /* Cont'd operation, in case of "meta" chaining PICC->PCD. */
            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_SA);
            SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_EI);

            /* Toggle the block Nr.: */
            p_td->m_BlockNumber =(uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);

        } else
        {
            /* Toggle the block Nr.: */
            p_td->m_BlockNumber =(uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);
			SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);

            /* Update receiver buffer length */
		    exchange_param->rec_buf_len = (uint32_t)(p_td->mp_CopyBytes - exchange_param->rec_buf);

            status = PH_ERR_BFL_SUCCESS;
        }

    } else
    {

        /* Check userbuffer length */
        if (exchange_param->snd_buf_len < I3P4_I_BLOCK_MIN_LEN)
        {
            return status = PH_ERR_BFL_INVALID_PARAMETER | PH_ERR_BFL_I3P4;
        }

        /* Start of exchange, protocol starts over or resumes sending data from
         * PCD to PICC:
         */
        SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_RS);
        SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RS);
        /* Clear PICC chaining bit: */
        p_td->m_Flags &= PHCS_BFLI3P4_FLAG_PICC_CHAINING_I;


    }
    /* Clear Timout-bit:*/
    p_td->m_Flags &= PHCS_BFLI3P4_FLAG_TIMEOUT_I;

    p_td->m_Index &= I3P4_CLEAR_INDEX_MASK;

    p_td->m_Size  &= I3P4_CLEAR_SIZE_MASK;


    do
    {

        inhibit_trx = 0; /* We allow Transceive by default for each pass. */
        switch (GetState(p_td, PHCS_BFLI3P4_STATE_CURRENT))
        {
            /* Error handling states: */
            case PHCS_BFLI3P4_STATE_RSR:

                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_RSR);

                /* Check Retry Counter */
                if (retry < p_td->m_MaxRetryCounter)
                {
                    retry ++;

                    /* Step back pointers in user buffer */
                    p_td->mp_StartOfBlock = last_StartofBlock;
                    p_td->mp_CopyBytes = p_td->mp_StartOfBlock;

                    if (p_td->mp_StartOfBlock == exchange_param->snd_buf)
                    {
                        /* It's the first block, reset NAD */
                        NAD = p_td->m_NAD;
                    }

                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RS);

                } else
                {
                    /* We do not apply a new status, instead we remember the status
                       code which lead to this condition and return it: */
                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                }

                inhibit_trx =1;
                break;

            case PHCS_BFLI3P4_STATE_EACK:

                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_EACK);

                /* Check Retry Counter */
                if (retry < p_td->m_MaxRetryCounter)
                {
                    retry++;
                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EI);

                    /* Send R(ACK) */
                    current_tx_buffer_size = BuildR(p_td,
                                                   (((p_td->m_Flags)
                                                    & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0) ?
                                                    &(p_td->m_CID) : NULL,
                                                    I3P4_R_BLOCK_ACK,
                                                    p_td->m_BlockNumber);
                } else
                {
                    /* We do not apply a new status, instead we remember the status
                       code which lead to this condition and return it: */
                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                    inhibit_trx =1;
                }
                break;

            case PHCS_BFLI3P4_STATE_ENAK:

                previous_state = GetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS);
                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_ENAK);

                /* Check Retry Counter */
                if (retry < p_td->m_MaxRetryCounter)
                {
                    retry++;

                    if (previous_state == PHCS_BFLI3P4_STATE_EA)
                    {
                        SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EA);
                    } else
                    {
                        SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EI);
                    }

                    /* Send R(NAK) */
                    current_tx_buffer_size = BuildR(p_td,
                                                   (((p_td->m_Flags)
                                                    & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0) ?
                                                    &(p_td->m_CID) : NULL,
                                                    I3P4_R_BLOCK_NAK,
                                                    p_td->m_BlockNumber);
                } else
                {
                    /* We do not apply a new status, instead we remember the status
                       code which lead to this condition and return it: */
                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                    inhibit_trx =1;
                }

                break;

            /* Regular states: */
            case PHCS_BFLI3P4_STATE_RS:

                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_RS);

                /* No Nad allowed if previous run and current one belong together. */
                if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_PREVIOUS_MORE) != 0)
                {
                    NAD = NULL;
                    p_td->m_Flags &= PHCS_BFLI3P4_FLAG_PREVIOUS_MORE_I;

                }

                /* Send I-Block */
                current_tx_buffer_size = BuildI(p_td,
                                                (((p_td->m_Flags)
                                                    & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0) ?
                                                        &(p_td->m_CID) : NULL,
                                                (((p_td->m_Flags)
                                                    & PHCS_BFLI3P4_FLAG_NAD_SUP_MASK) != 0) ?
                                                        NAD : NULL,
                                                exchange_param->snd_buf,
                                                (uint16_t)exchange_param->snd_buf_len,
                                                (uint8_t)(exchange_param->flags & PHCS_BFLI3P4_CONT_CHAINING));

                /* Advance pointers in user buffer */
                last_StartofBlock = p_td->mp_StartOfBlock;
                p_td->mp_StartOfBlock = p_td->mp_CopyBytes;

                pcb = p_td->mp_TRxBuffer[I3P4_PCB_POS];

                /* Check PCD chaining */
                if ((pcb & I3P4_CHAINING_BIT_MASK) != 0)
                {
                    if ((exchange_param->flags & PHCS_BFLI3P4_CONT_CHAINING) != 0)
                    {

                        if (p_td->mp_CopyBytes == (exchange_param->snd_buf + exchange_param->snd_buf_len))
                        {
                            /*
                             * We're at the end of the User Buffer, so we temporarily quit after
                             * the subsequent ACK:
                             */
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EA);
                            p_td->mp_StartOfBlock = exchange_param->snd_buf;
                            p_td->mp_CopyBytes = p_td->mp_StartOfBlock;

                            /* Apply info about previous run: */
                            p_td->m_Flags &= PHCS_BFLI3P4_FLAG_PREVIOUS_MORE_I;
                            p_td->m_Flags |=
                                ((exchange_param->flags & PHCS_BFLI3P4_CONT_CHAINING) != 0) ?
                                     PHCS_BFLI3P4_FLAG_PREVIOUS_MORE : 0;


                        } else
                        {
                            /* Continue chaining: */
                            NAD = NULL;
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EA);
                        }

                    } else
                    {
                        /* Continue chaining: */
                            NAD = NULL;
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EA);
                    }

                } else
                {
                    /* Chaining has finished: */

                     SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EI);
                    /* We expect now info from the PICC, start with receive buffer: */
                    p_td->mp_StartOfBlock = exchange_param->rec_buf;
                    p_td->mp_CopyBytes = p_td->mp_StartOfBlock;
                }
                break;

            case PHCS_BFLI3P4_STATE_EA:

                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_EA);
                /* Check Timeoutbit*/
				/* Attention: We have to know the exchange status, so for the Cb we,
					take a new status variable. */
                if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_TIMEOUT) != 0)
                {
                    /* WTX reset: */
                    istatus = ReqWtxCb((phcsBflI3P4_t*) exchange_param->self, 0, 0,p_td->m_FWI,p_td->mp_UserWTX);
                    if ((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INTERFACE_NOT_ENABLED)
                    {
                        exchange_param->flags |= PHCS_BFLI3P4_FLAG_CB;
                    }
                    p_td->m_Flags &= PHCS_BFLI3P4_FLAG_TIMEOUT_I;
                }


                 /* Check Transmission */
                if (status != PH_ERR_BFL_SUCCESS)
                {
                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_ENAK);

                } else
                {
                    /* Check Received ACK */
                    status = CheckRAck(p_td, current_rx_buffer_size);
                    if (status != PH_ERR_BFL_SUCCESS)
                    {

                        if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_BLOCKNR_NOT_EQUAL)
                        {
                            /* Yes it is a R(ACK) */
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RSR);

                        } else
                        {
                            if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INVALID_PARAMETER)
                            {
                                /* It's  a R-Block with wrong Parameter*/
                                SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);

                            } else
                            {
                                /* Nope, check Received S(WTX): */
                                status = CheckSWtx(p_td, current_rx_buffer_size);
                                if (status == PH_ERR_BFL_SUCCESS)
                                {
                                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_ST);

                                } else
                                {
                                    if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INVALID_PARAMETER)
                                    {
                                        /* It's a S-Block with wrong Parameter */
                                        SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                                    } else
                                    {
                                        /* Format of block does not match: */
                                        SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);

                                    }
                                }
                            }
                        }
                    } else
                    {
                        /* It's a R(ACK): */
                        if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_PREVIOUS_MORE) != 0)
                        {
                            /* Temp. exit for next run: */
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                            /* Toggle the block Nr.: */
                            p_td->m_BlockNumber = (uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);
                        } else
                        {
                             /* Toggle the block Nr.: */
                            p_td->m_BlockNumber = (uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RS);
                        }
                    }
                }
                inhibit_trx = 1;
                break;

            case PHCS_BFLI3P4_STATE_EI:

                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_EI);

				/* Need for correct NAD handling during meta chaining. */
				if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_INCLUDE_NAD_BEFORE) &&
					!(p_td->m_Flags & PHCS_BFLI3P4_FLAG_PICC_CHAINING) &&
					!(p_td->m_Flags & PHCS_BFLI3P4_FLAG_PICC_APDU_CHAIN))
				{
					p_td->m_Flags |= PHCS_BFLI3P4_FLAG_INCLUDE_NAD;
				}

                /* Check Timeoutbit*/
				/* Attention: We have to know the exchange status, so for the Cb we,
					take a new status variable. */
                if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_TIMEOUT) != 0)
                {
                    /* Reset WTX: */
                    istatus = ReqWtxCb((phcsBflI3P4_t*)exchange_param->self, 0, 0, p_td->m_FWI, p_td->mp_UserWTX);
                    if ((istatus & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INTERFACE_NOT_ENABLED)
                    {
                        exchange_param->flags |= PHCS_BFLI3P4_FLAG_CB;
                    }
                    p_td->m_Flags &= PHCS_BFLI3P4_FLAG_TIMEOUT_I;
                }

                /* Check Tranmission: */
                if (status != PH_ERR_BFL_SUCCESS)
                {
                    SetState(p_td,
                             PHCS_BFLI3P4_STATE_CURRENT,
                             (uint8_t)(((p_td->m_Flags &
                                PHCS_BFLI3P4_FLAG_PICC_CHAINING) != 0) ?
                                    PHCS_BFLI3P4_STATE_EACK : PHCS_BFLI3P4_STATE_ENAK));
                } else
                {
                    /* Check Received I-Block */
                    status = CheckI(p_td, current_rx_buffer_size);
                    if (status == PH_ERR_BFL_SUCCESS)
                    {
                        /* YES, its an INF: Copy exchange buffer into user buffer: */
                        status = ReceiveI(p_td,
                                        current_rx_buffer_size,
                                        exchange_param->rec_buf,
                                        exchange_param->rec_buf_len,
                                        &current_user_rx_buffer_size);
						/* Reset No History flag. */
						p_td->m_HistoryFlag &= PHCS_BFLI3P4_FLAG_NO_HISTORY_I;

                        pcb = p_td->mp_TRxBuffer[I3P4_PCB_POS];

                        if ((pcb & I3P4_CHAINING_BIT_MASK) != 0)
                        {
                            p_td->m_Flags |= PHCS_BFLI3P4_FLAG_PICC_APDU_CHAIN;
                        } else
                        {
                            p_td->m_Flags &= PHCS_BFLI3P4_FLAG_PICC_APDU_CHAIN_I;
                        }

                        if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_USERBUFFER_FULL)
                        {
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EI);
                            exchange_param->flags |= PHCS_BFLI3P4_CONT_CHAINING;
                            my_exit = 1;
                            break;

                        } else
                        {
                            /* Toggle the block Nr.: */
                            p_td->m_BlockNumber =(uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);

                            /* Check PICC chaining */
                            if ((pcb & I3P4_CHAINING_BIT_MASK) != 0)
                            {
                                /* Chaining, more to come: Advance Pointers in user buffer. */
                                p_td->mp_StartOfBlock = p_td->mp_CopyBytes;
                                SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_SA);
                                p_td->m_Flags &= PHCS_BFLI3P4_FLAG_INCLUDE_NAD_I;
                                /* PICC chaining, bit to determine type of error handling. */
                                p_td->m_Flags |= PHCS_BFLI3P4_FLAG_PICC_CHAINING;
                            } else
                            {
                                /* We're done with it: no status modification, remember it later: */
                                SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);

                                /* Clear PICC chaining bit: */
                                p_td->m_Flags &= PHCS_BFLI3P4_FLAG_PICC_CHAINING_I;

                                /* Update receive buffer length */
                                exchange_param->rec_buf_len = (uint32_t)(p_td->mp_CopyBytes - exchange_param->rec_buf);
                            }
                        }
                    } else
                    {
                        if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INVALID_PARAMETER)
                        {
                            /* It's an I-Block with wrong Parameter */
                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                        } else
                        {
                            /* ACK? Check Received ACK: */
                            status = CheckRAck(p_td, current_rx_buffer_size);
                            if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_BLOCKNR_NOT_EQUAL)
                            {
                                SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RSR);

                            } else
                            {
                                if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INVALID_PARAMETER)
                                {
                                    /* It's a R-Block with wrong Parameter*/
                                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);

                                } else
                                {
                                    /* WTX? Check Received SWTX: */
                                    status = CheckSWtx(p_td, current_rx_buffer_size);
                                    if (status == PH_ERR_BFL_SUCCESS)
                                    {
                                        SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_ST);
                                    } else
                                    {
                                        if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INVALID_PARAMETER)
                                        {
                                            /* It's a SWTX with wrong Parameter */
                                            SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                                        } else
                                        {
                                            if ((p_td->m_Flags & PHCS_BFLI3P4_FLAG_PICC_APDU_CHAIN) != 0)
                                            {
                                                /* ISO 14443_4 7.5.4.2 rule 5 */
                                                SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EACK);
                                            } else
                                            {
                                                /* We don't know what it is - bail out: */
                                                SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_RE);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                inhibit_trx = 1;
                break;

            case PHCS_BFLI3P4_STATE_SA:

                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_SA);
                SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EI);

                /* Send R(ACK) */
                current_tx_buffer_size = BuildR(p_td,
                                               (((p_td->m_Flags)
                                                & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0) ?
                                                &(p_td->m_CID) : NULL,
                                                I3P4_R_BLOCK_ACK,
                                                p_td->m_BlockNumber);

                break;

            case PHCS_BFLI3P4_STATE_ST:

                previous_state = GetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS);
                SetState(p_td, PHCS_BFLI3P4_STATE_PREVIOUS, PHCS_BFLI3P4_STATE_ST);

                /* Get inf block of PICC SWTX; int_wtxm is the same that
                   the PCD SWTX shall include */
                wtx_value = ReceiveSWtx(p_td);
                int_wtxm = (uint8_t)(wtx_value & I3P4_WTX_MASK);

                exchange_param->flags = (uint8_t)(exchange_param->flags & (~PHCS_BFLI3P4_FLAG_CB));

                /* WTX Callback: The application must now set extended timeout: */
                status = ReqWtxCb((phcsBflI3P4_t*)exchange_param->self, 1,
                                    (uint8_t)((int_wtxm != 0) ? int_wtxm : 1),
                                    p_td->m_FWI, p_td->mp_UserWTX);
                if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_INTERFACE_NOT_ENABLED)
                {
                    exchange_param->flags |= PHCS_BFLI3P4_FLAG_CB;
                    status = PH_ERR_BFL_SUCCESS;
                }


                /* Set Timeout bit, we remember it when we receive an ACK or INF: */
                if (status == PH_ERR_BFL_SUCCESS)
                {
                    p_td->m_Flags |= PHCS_BFLI3P4_FLAG_TIMEOUT;
                }

                if (previous_state == PHCS_BFLI3P4_STATE_EA)
                {
                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EA);

                } else
                {
                    SetState(p_td, PHCS_BFLI3P4_STATE_CURRENT, PHCS_BFLI3P4_STATE_EI);

                }
                /* Send SWTX: This starts the extended waiting time: */
                current_tx_buffer_size = BuildS(p_td,
                                               (((p_td->m_Flags) & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0) ?
                                                &(p_td->m_CID) : NULL,
                                                I3P4_S_BLOCK_WTX, int_wtxm);
                break;

            case PHCS_BFLI3P4_STATE_RE:
                my_exit = 1;
                break;

            default:
                break;
        }

        /* Transceive, if not about to exit: */
        if ((!my_exit) && (!inhibit_trx))
        {
               transceive_param.self           = ((phcsBflI3P4_t*)(exchange_param->self))->mp_Lower;
               transceive_param.tx_buffer      = p_td->mp_TRxBuffer;
               transceive_param.tx_buffer_size = current_tx_buffer_size;
               transceive_param.rx_buffer      = p_td->mp_TRxBuffer;
               transceive_param.rx_buffer_size = p_td->m_MaxRxBufferSize;
               status                          = ((phcsBflIo_t*)(transceive_param.self))->
                                                       Transceive(&transceive_param);
               current_rx_buffer_size          = transceive_param.rx_buffer_size;
        }
    } while (!my_exit);

    exchange_param->nad_receive = p_td->m_NAD;

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)


    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P4_PresCheck(phcsBflI3P4_PresCheckParam_t *presence_check_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_CommParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_CommParam_t*)(((phcsBflI3P4_t*)(presence_check_param->self))->mp_Members);

    phcsBfl_Status_t               PHCS_BFL_MEMLOC_REM     status = PH_ERR_BFL_SUCCESS;
    uint8_t           PHCS_BFL_MEMLOC_REM     r_block_len;
    phcsBflIo_TransceiveParam_t   PHCS_BFL_MEMLOC_REM     transceive_param;
    uint8_t           PHCS_BFL_MEMLOC_REM     block_type;


    /* Call Callback for Bitrate and Frame waiting time */
    status = ReqReaderParameterCb((phcsBflI3P4_t*)presence_check_param->self, p_td->m_DRI, p_td->m_DSI, p_td->m_FWI, p_td->mp_UserReaderParam);
    /* Added check of Callback here to ensure no transmission of data if anythig wrong in CB */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        return status;
    }


	if ((p_td->m_HistoryFlag) != 0)
	{
		/* Nothing to do */
	} else
	{
		/* Toggle the block Nr.: */
        p_td->m_BlockNumber =(uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);
	}

	/*
     * Compose a R(NAK) in order to check for PICC presence. This mechanism exploits
     * scenarios 6 and 8 by forcing the PICC to reply with an R(ACK) or the last
     * I-block respectively.
     *
     * Build an R-block with a CID (and its presence) depending on the internal operation
     * control byte's CID supported indicator bit:
     */
     r_block_len = BuildR(p_td,
                         (((p_td->m_Flags) & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0) ?
                            &(p_td->m_CID) : NULL,
                         I3P4_R_BLOCK_NAK,
                         p_td->m_BlockNumber);

    transceive_param.tx_buffer = p_td->mp_TRxBuffer;
    transceive_param.tx_buffer_size = r_block_len;
    transceive_param.rx_buffer = p_td->mp_TRxBuffer;
    transceive_param.rx_buffer_size = p_td->m_MaxRxBufferSize;
    transceive_param.self = ((phcsBflI3P4_t*)presence_check_param->self)->mp_Lower;

    status = ((phcsBflI3P4_t*)presence_check_param->self)->mp_Lower->Transceive(&transceive_param);

    if (status ==  PH_ERR_BFL_SUCCESS)
    {
        if (transceive_param.rx_buffer_size > 0)
        {
            block_type = (uint8_t)(transceive_param.rx_buffer[I3P4_PCB_POS] &
                                        I3P4_BLOCK_TYPE_MASK);

            if ((block_type == I3P4_BLOCK_TYPE_R) || (block_type == I3P4_BLOCK_TYPE_I))
            {
                if (block_type == I3P4_BLOCK_TYPE_R)
				{
					if ((p_td->m_HistoryFlag) != 0)
					{
						/* Success: The PICC responded with some block we expected: Do nothing*/
					} else
					{
						/* Success: The PICC responded with some block we expected. */
						p_td->m_BlockNumber =(uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);
					}
				} else
				{
					/* Success: The PICC responded with some block we expected. */
					p_td->m_BlockNumber =(uint8_t)((p_td->m_BlockNumber != 0) ? 0 : 1);
				}
            } else
            {
                /* We reveived a PCB we did not expect. */
                status = PH_ERR_BFL_INVALID_FORMAT;
            }
        }else
        {
            /* We received not even a single byte. */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* Merely return status: */
    }
    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflI3P4_Deselect(phcsBflI3P4_DeselectParam_t *deselect_param)
{
    /* Recover the pointer to the communication parameter structure: */
    phcsBflI3P4_CommParam_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflI3P4_CommParam_t*)(((phcsBflI3P4_t*)(deselect_param->self))->mp_Members);

    phcsBfl_Status_t                PHCS_BFL_MEMLOC_REM     status = PH_ERR_BFL_SUCCESS;
    uint8_t                         PHCS_BFL_MEMLOC_REM     s_block_len;
    phcsBflIo_TransceiveParam_t     PHCS_BFL_MEMLOC_REM     transceive_param;
    uint8_t                         PHCS_BFL_MEMLOC_REM     block_type;

    /* Call Callback for Bitrate and frame waiting time.
       Attention: FWI is fixed to 4 and not the same as for communication! */
    status = ReqReaderParameterCb((phcsBflI3P4_t*)deselect_param->self, p_td->m_DRI, p_td->m_DSI, 4, p_td->mp_UserReaderParam);
    /* Added check of Callback here to ensure no transmission of data if anythig wrong in CB */
    if(status != PH_ERR_BFL_SUCCESS)
    {
        return status;
    }

    /* Build an S-block with a CID (and its presence) depending on the internal operation
     * control byte's CID supported indicator bit:
     */
    s_block_len = BuildS(p_td,
                         (((p_td->m_Flags) & PHCS_BFLI3P4_FLAG_CID_SUP_MASK) != 0) ?
                             &(p_td->m_CID) : NULL,
                         I3P4_S_BLOCK_DESELECT,0);

    transceive_param.tx_buffer = p_td->mp_TRxBuffer;
    transceive_param.tx_buffer_size = s_block_len;
    transceive_param.rx_buffer = p_td->mp_TRxBuffer;
    transceive_param.rx_buffer_size = p_td->m_MaxRxBufferSize;
    transceive_param.self = ((phcsBflI3P4_t*)deselect_param->self)->mp_Lower;


    status = ((phcsBflI3P4_t*)deselect_param->self)->mp_Lower->Transceive(&transceive_param);

    if (status == PH_ERR_BFL_SUCCESS)
    {
        block_type = (uint8_t)(transceive_param.rx_buffer[I3P4_PCB_POS] &
                                    I3P4_BLOCK_TYPE_MASK);

        if (block_type == I3P4_BLOCK_TYPE_S)
        {
            /* Correct: We do nothing. */
        } else
        {
            /* We received something else. */
            status = PH_ERR_BFL_INVALID_FORMAT;
        }
    } else
    {
        /* Error in underlying function - merely return status. */
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_I3P4)
    return status;
}
/* EOF */
