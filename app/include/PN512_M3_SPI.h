//#include "includes.h"

#ifndef __PN512_M3_SPI_H__
#define __PN512_M3_SPI_H__

extern void  PN512_SPI_init(void);
extern void  PN512_CS_set(void);
extern void  PN512_CS_reset(void);

extern uint_fast8_t TxSeri1ByteData(uint_fast8_t ch);

#endif
/**************************************************************************************************/

