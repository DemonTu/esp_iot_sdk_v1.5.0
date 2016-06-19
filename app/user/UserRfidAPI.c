/******************************************************************************
 * Copyright 2016-2017 TUT Systems (TQY)
 *
 * FileName: UserRfidAPI.c
 *
 * Description: RFID demo's function realization
 *
 * Modification history:
 *     2016/1/25, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
//#include "mem.h"
//#include "user_interface.h"

#include "GPIO.h"
#include "UserRfidAPI.h"
#include "rfid_polling.h"
#include "PN512_M3_SPI.h"
#include "PN512_reg_test.h"

#define SPI_MOSI_SET_HIGH()		GPIO_OUTPUT_SET(RFID_SPI_MOSI_IO_NUM,1)
#define SPI_MOSI_SET_LOW()		GPIO_OUTPUT_SET(RFID_SPI_MOSI_IO_NUM,0)

LOCAL os_timer_t Rfid_timer;

void ICACHE_FLASH_ATTR 
SendReg(void)
{
    unsigned char i,j,k=1;
    os_printf("\r\nReg   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");
	for(i=0;i<4;i++){
    //SendStr("Page");SendChar('0'+i);SendChar(' ');
		os_printf("Page%d ", i);
      	for(j=0;j<16;j++){
        	k=PN512_reg_read(i*16+j);
			os_printf("%02x ", k);
        	//SendChar((k>>4)+(((k>>4)>9)? '7': '0'));
        	//SendChar((k&0x0f)+(((k&0x0f)>9)? '7': '0'));
        	//SendChar(' ');
       	}
    	os_printf("\r\n");
    }
}


void ICACHE_FLASH_ATTR
UserRfidRequest_A(uint8_t flag)
{
	LOCAL uint8_t flashFlag =0;
	LOCAL uint8_t cnt = 0;
	//uint8_t k=0;
	if (flashFlag){
		//PN512_reg_write(0x02, 0x7F);
		//k=PN512_reg_read(0x02);
		//os_printf("k=%02x\r\n");
		GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_LED1_IO_NUM),1);
		flashFlag = 0;
	}else{
		//SendReg();
		GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_LED1_IO_NUM),0);
		flashFlag = 1;
	}
	if (++cnt >= 2){
		cnt = 0;
		SendReg();
	}
	//TestReadRfidNum();
}

/******************************************************************************
 * FunctionName : userRfid_Init
 * Description  : init RFID SPI GPIO
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
userRfid_Init(void)
{
	// GPIO init
	PIN_PULLUP_DIS(RFID_LED1_IO_MUX);
	PIN_FUNC_SELECT(RFID_LED1_IO_MUX,RFID_LED1_IO_FUNC);
	PN512_SPI_init();
	//os_printf("test resd\r\n");
/*∂® ±…®√Ë rfid*/
	os_timer_disarm(&Rfid_timer);
	os_timer_setfn(&Rfid_timer, (os_timer_func_t *)UserRfidRequest_A, 1);
	os_timer_arm(&Rfid_timer, 2000, 1);
}

