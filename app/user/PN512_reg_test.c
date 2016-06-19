//#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"

#include <phcsBflRefDefs.h>
#include <phcsBflHw1Reg.h>
#include <phcsBflRegCtl.h>
#include <phcsBflAux.h>

#include <phcsBflOpCtl.h>
#include <phcsBflOpCtl_Hw1Ordinals.h>
#include "phcsBflOpCtl_Hw1Int.h"

/* Header file which contains all the information about the register configuration to be
   able to operate in all communication modes and data rates. */

//#include <debug_frmwrk.h>
extern void  PN512_SPI_init(void);
//void PN512_parallel_init(void);

extern phcsBfl_Status_t phcsBflRegCtl_SpiHw1GetReg(phcsBflRegCtl_GetRegParam_t *getreg_param);
extern phcsBfl_Status_t phcsBflRegCtl_SpiHw1SetReg(phcsBflRegCtl_SetRegParam_t *setreg_param);

//phcsBfl_Status_t phcsBflRegCtl_ParaHw1GetReg(phcsBflRegCtl_GetRegParam_t *getreg_param);
//phcsBfl_Status_t phcsBflRegCtl_ParaHw1SetReg(phcsBflRegCtl_SetRegParam_t *setreg_param);


void ICACHE_FLASH_ATTR
phcsBflOpCtl_dump_version(void)
{
    phcsBflRegCtl_GetRegParam_t        grp;
    grp.address = PHCS_BFL_JREG_VERSION;
    phcsBflRegCtl_SpiHw1GetReg(&grp);
    //os_printf("hwt=0x%x\r\n", grp.reg_data);
}

void ICACHE_FLASH_ATTR
PN512_reg_write(uint_fast8_t addr,  uint_fast8_t val)
{
    phcsBflRegCtl_SetRegParam_t        grp;
    grp.address = addr&0x7F;
    grp.reg_data= val;
    if (0 == (addr&0x80))
    {
        PN512_SPI_init();
    }
    phcsBflRegCtl_SpiHw1SetReg(&grp);
}

uint_fast8_t ICACHE_FLASH_ATTR
PN512_reg_read(uint_fast8_t addr)
{
    phcsBflRegCtl_GetRegParam_t        grp;
    grp.address = addr&0x7F;
    if (0 == (addr&0x80))
    {
        PN512_SPI_init();
    }
    phcsBflRegCtl_SpiHw1GetReg(&grp);
    return grp.reg_data;
}

