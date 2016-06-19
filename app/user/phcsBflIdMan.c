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

/*! \file phcsBflIdMan.c
 *
 * Project: ID Manager.
 *
 * Workfile: phcsBflIdMan.c
 * $Author: mha $
 * $Revision: 1.4 $
 * $Date: Fri Jun 23 07:20:50 2006 $
 * $KeysEnd$
 *
 * Comment:
 *  None.
 *
 * History:
 *  BS:  Generated 24. Mar 2004
 *  MHa: Migrated to MoReUse September 2005
 *
 */

#include <phcsBflIdMan.h>
#include "phcsBflIdMan_Int.h"


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIdMan_Init(phcsBflIdMan_t             *cif,
                               phcsBflIdMan_InitParams_t  *p_td)
{
    uint8_t i;
    cif->mp_Members = p_td;
    cif->GetFreeID = phcsBflIdMan_GetFreeID;
    cif->IsFreeID = phcsBflIdMan_IsFreeID;
    cif->AssignID = phcsBflIdMan_AssignID;
    cif->FreeIDByNumber = phcsBflIdMan_FreeIDByNumber;
    cif->FreeIDByInstance = phcsBflIdMan_FreeIDByInstance;
    

    for (i = 0; i < PHCS_BFLIDMAN_MAX_ID_COUNT; i++)
    {
        p_td->mp_Instances[i] = NULL;
    }

    return PH_ERR_BFL_SUCCESS;
}

phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIdMan_GetFreeID(phcsBflIdMan_Param_t *idman_param)
{
    /* Recover the pointer to the phcsBflIdMan parameter structure: */
     phcsBflIdMan_InitParams_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflIdMan_InitParams_t*)(((phcsBflIdMan_t*)(idman_param->self))->mp_Members);

    phcsBfl_Status_t    PHCS_BFL_MEMLOC_REM     status;
    uint8_t             PHCS_BFL_MEMLOC_REM     my_exit;
    uint8_t             PHCS_BFL_MEMLOC_COUNT   counter = 0;

    /*
     * Search in the array of void Pointers for a free Instance:
     * The Index of the array is the ID.
     * We search the aray, starting at index 1. The index in incremented until a free
     * array location is found. In case that the array positions 1..14 are full, index
     * zero is examined. This is because the ISO specification mandates ID 0 to be the last
     * one assigned.
     */

    /* ID 0 already used: No further ID available. */
    if (p_td->mp_Instances[PHCSBFLIDMAN_EXCLUSIVE_ID] != NULL)
    {
        return PH_ERR_BFL_NO_ID_AVAILABLE | PH_ERR_BFL_IDMAN;
    }

    do
    {
        counter ++;

        if (p_td->mp_Instances[counter] == NULL)
        {
            idman_param->ID = counter;
            status = PH_ERR_BFL_SUCCESS;
            my_exit = 1;
        } else
        {
            status = PH_ERR_BFL_ID_ALREADY_IN_USE;
            if (counter < PHCSBFLIDMAN_MAX_ID_NUM) 
            {
                my_exit = 0;
            } else
            {
                my_exit = 1;
            }
        }

    } while (!my_exit);

    if ((status & PH_ERR_BFL_ERR_MASK) == PH_ERR_BFL_ID_ALREADY_IN_USE)
    {
        if (p_td->mp_Instances[PHCSBFLIDMAN_EXCLUSIVE_ID] == NULL)
        {
            idman_param->ID = PHCSBFLIDMAN_EXCLUSIVE_ID;
            status = PH_ERR_BFL_SUCCESS;

        } else
        {
            status = PH_ERR_BFL_NO_ID_AVAILABLE;
        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IDMAN)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIdMan_IsFreeID(phcsBflIdMan_Param_t *idman_param)
{
   /* Recover the pointer to the phcsBflIdMan parameter structure: */
    phcsBflIdMan_InitParams_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflIdMan_InitParams_t*)(((phcsBflIdMan_t*)(idman_param->self))->mp_Members);

    phcsBfl_Status_t     PHCS_BFL_MEMLOC_REM  status;

    /* Check whether the specified ID is free: */
    /* ID must not be greater than 14. */

    /* ID 0 already used: No further ID available. */
    if (p_td->mp_Instances[PHCSBFLIDMAN_EXCLUSIVE_ID] != NULL)
    {
        return PH_ERR_BFL_NO_ID_AVAILABLE | PH_ERR_BFL_IDMAN;
    }

    if (idman_param->ID < PHCS_BFLIDMAN_MAX_ID_COUNT)
    {
        if (p_td->mp_Instances[idman_param->ID] == NULL)
        {
            status = PH_ERR_BFL_SUCCESS;

        } else
        {
            status = PH_ERR_BFL_ID_ALREADY_IN_USE;        
        }
    } else
    {
        status = PH_ERR_BFL_INVALID_PARAMETER;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IDMAN)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIdMan_AssignID(phcsBflIdMan_Param_t *idman_param)
{
   /* Recover the pointer to the phcsBflIdMan parameter structure: */
    phcsBflIdMan_InitParams_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflIdMan_InitParams_t*)(((phcsBflIdMan_t*)(idman_param->self))->mp_Members);

    phcsBfl_Status_t     PHCS_BFL_MEMLOC_REM    status;
    phcsBfl_Status_t     PHCS_BFL_MEMLOC_COUNT  counter = 0;
    
    /* Assign the ID if it is free: */ 
    /* Is the address instance valid (non-NULL: assumed to be OK) ? */
    /* Check whether the specified instance is already in use: */
    status = phcsBflIdMan_IsFreeID(idman_param);
    if (status == PH_ERR_BFL_SUCCESS)
    {
        do
        {
            if (idman_param->instance != NULL)
            {
                if (p_td->mp_Instances[counter] == idman_param->instance)
                {
                    status = PH_ERR_BFL_INSTANCE_ALREADY_IN_USE;
                }
            } 
            counter ++;
        } while (counter < PHCS_BFLIDMAN_MAX_ID_COUNT);

        if (status == PH_ERR_BFL_SUCCESS)
        {
            p_td->mp_Instances[idman_param->ID] = idman_param->instance;

        }
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IDMAN)
    return status;
}



phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIdMan_FreeIDByNumber(phcsBflIdMan_Param_t *idman_param)
{
    /* Recover the pointer to the phcsBflIdMan parameter structure: */
    phcsBflIdMan_InitParams_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflIdMan_InitParams_t*)(((phcsBflIdMan_t*)(idman_param->self))->mp_Members);

    phcsBfl_Status_t     PHCS_BFL_MEMLOC_REM  status;

    /* Clear the instance by specifying the ID (number/index): */
    /* Check the ID number and if the instance is valid. */
    if (idman_param->ID < PHCS_BFLIDMAN_MAX_ID_COUNT)
    {
        if (p_td->mp_Instances[idman_param->ID] != NULL)
        {
            p_td->mp_Instances[idman_param->ID] = NULL;
            status = PH_ERR_BFL_SUCCESS;
        } else
        {
            status = PH_ERR_BFL_ID_NOT_IN_USE;
        }
    } else
    {
        status = PH_ERR_BFL_INVALID_PARAMETER;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IDMAN)
    return status;
}


phcsBfl_Status_t ICACHE_FLASH_ATTR
phcsBflIdMan_FreeIDByInstance(phcsBflIdMan_Param_t *idman_param)
{
    /* Recover the pointer to the phcsBflIdMan parameter structure: */
    phcsBflIdMan_InitParams_t PHCS_BFL_MEMLOC_REM *p_td =
        (phcsBflIdMan_InitParams_t*)(((phcsBflIdMan_t*)(idman_param->self))->mp_Members);

    phcsBfl_Status_t     PHCS_BFL_MEMLOC_REM    status  = PH_ERR_BFL_ID_NOT_IN_USE;
    phcsBfl_Status_t     PHCS_BFL_MEMLOC_COUNT  counter = 0;

    
    /* Clear the ID by specifying the instance address: */
    /* Check whether the instance is valid, clear all instances in the array
       which are of the same address as the given instance. */
    if (idman_param->instance != NULL)
    {
        do
        {
            if (p_td->mp_Instances[counter] == idman_param->instance)
            {
                p_td->mp_Instances[counter] = NULL;
                status = PH_ERR_BFL_SUCCESS;
            }
            counter ++;
        } while (counter < PHCS_BFLIDMAN_MAX_ID_COUNT);
    } else
    {
        status = PH_ERR_BFL_INVALID_PARAMETER;
    }

    PHCS_BFL_ADD_COMPCODE(status, PH_ERR_BFL_IDMAN)
    return status;
}

/* E.O.F. */
