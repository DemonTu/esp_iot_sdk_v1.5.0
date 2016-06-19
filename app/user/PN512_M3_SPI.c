#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
/* BSP library */
//#include "includes.h"
#define get_system_clk_MHZ_()           (80)
/**************************************************************************************************/
/**************************************************************************************************/
/**************************************************************************************************/
#if 1
#define RFID_SPI_MOSI_IO_MUX      PERIPHS_IO_MUX_MTDI_U
#define RFID_SPI_MOSI_IO_NUM      12
#define RFID_SPI_MOSI_IO_FUNC     FUNC_GPIO12

//#define RFID_SPI_MISO_IO_MUX      PERIPHS_IO_MUX_MTCK_U
//#define RFID_SPI_MISO_IO_NUM      13
//#define RFID_SPI_MISO_IO_FUNC     FUNC_GPIO13

#define RFID_SPI_MISO_IO_MUX      PERIPHS_IO_MUX_GPIO5_U
#define RFID_SPI_MISO_IO_NUM      5
#define RFID_SPI_MISO_IO_FUNC     FUNC_GPIO5

#define RFID_SPI_CLK_IO_MUX       PERIPHS_IO_MUX_GPIO4_U
#define RFID_SPI_CLK_IO_NUM       4
#define RFID_SPI_CLK_IO_FUNC      FUNC_GPIO4
#if 0
#define RFID_SPI_CS_IO_MUX        PERIPHS_IO_MUX_U0TXD_U	// GPIO1
#define RFID_SPI_CS_IO_NUM        1
#define RFID_SPI_CS_IO_FUNC       FUNC_GPIO1
#else
#define RFID_SPI_CS_IO_MUX        PERIPHS_IO_MUX_GPIO2_U	// GPIO2
#define RFID_SPI_CS_IO_NUM        2
#define RFID_SPI_CS_IO_FUNC       FUNC_GPIO2
#endif
#define RFID_SPI_RESET_IO_MUX     PERIPHS_IO_MUX_MTMS_U
#define RFID_SPI_RESET_IO_NUM     14
#define RFID_SPI_RESET_IO_FUNC    FUNC_GPIO14
#else
#define RFID_SPI_MOSI_IO_MUX      PERIPHS_IO_MUX_MTCK_U
#define RFID_SPI_MOSI_IO_NUM      13
#define RFID_SPI_MOSI_IO_FUNC     FUNC_GPIO13

#define RFID_SPI_MISO_IO_MUX      PERIPHS_IO_MUX_MTDI_U
#define RFID_SPI_MISO_IO_NUM      12
#define RFID_SPI_MISO_IO_FUNC     FUNC_GPIO12

#define RFID_SPI_CS_IO_MUX        PERIPHS_IO_MUX_MTDO_U	// GPIO1
#define RFID_SPI_CS_IO_NUM        15
#define RFID_SPI_CS_IO_FUNC       FUNC_GPIO15

#define RFID_SPI_CLK_IO_MUX       PERIPHS_IO_MUX_MTMS_U
#define RFID_SPI_CLK_IO_NUM       14
#define RFID_SPI_CLK_IO_FUNC      FUNC_GPIO14

#define RFID_SPI_RESET_IO_MUX      PERIPHS_IO_MUX_GPIO4_U
#define RFID_SPI_RESET_IO_NUM      4
#define RFID_SPI_RESET_IO_FUNC     FUNC_GPIO4

#endif

static void ICACHE_FLASH_ATTR  
PN512_RESET_HIGH(void)
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_RESET_IO_NUM),1);
}
static void ICACHE_FLASH_ATTR
PN512_RESET_LOW(void)
{
    GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_RESET_IO_NUM),0);
}
static void ICACHE_FLASH_ATTR
PN512_MOSI_HIGH(void)
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_MOSI_IO_NUM),1);
}
static void ICACHE_FLASH_ATTR
PN512_MOSI_LOW(void)
{
    GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_MOSI_IO_NUM),0);
}
static void ICACHE_FLASH_ATTR
PN512_CLK_HIGH(void)
{
    GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_CLK_IO_NUM),1);
}
static void ICACHE_FLASH_ATTR
PN512_CLK_LOW(void)
{
    GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_CLK_IO_NUM),0);
}
static void ICACHE_FLASH_ATTR
PN512_CS_HIGH(void)
{
    GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_CS_IO_NUM),1);
}
static void ICACHE_FLASH_ATTR
PN512_CS_LOW(void)
{
    GPIO_OUTPUT_SET(GPIO_ID_PIN(RFID_SPI_CS_IO_NUM),0);
}

uint8_t ICACHE_FLASH_ATTR
PN512_MISO_STATE(void)
{
    return (GPIO_INPUT_GET(GPIO_ID_PIN(RFID_SPI_MISO_IO_NUM)) ? 0x01 : 0x00);
}

/**************************************************************************************************/

/**************************************************************************************************/
void ICACHE_FLASH_ATTR
PN512_SPI_init(void)
{
	uint16_t delay;
	
    PIN_PULLUP_DIS(RFID_SPI_CS_IO_MUX);
	PIN_FUNC_SELECT(RFID_SPI_CS_IO_MUX,RFID_SPI_CS_IO_FUNC);

    PIN_PULLUP_DIS(RFID_SPI_MOSI_IO_MUX);
	PIN_FUNC_SELECT(RFID_SPI_MOSI_IO_MUX,RFID_SPI_MOSI_IO_FUNC);

	PIN_PULLUP_EN(RFID_SPI_MISO_IO_MUX);
	PIN_FUNC_SELECT(RFID_SPI_MISO_IO_MUX,RFID_SPI_MISO_IO_FUNC);
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(RFID_SPI_MISO_IO_NUM));

	PIN_PULLUP_DIS(RFID_SPI_CLK_IO_MUX);
	PIN_FUNC_SELECT(RFID_SPI_CLK_IO_MUX,RFID_SPI_CLK_IO_FUNC);

	PIN_PULLUP_DIS(RFID_SPI_RESET_IO_MUX);
	PIN_FUNC_SELECT(RFID_SPI_RESET_IO_MUX,RFID_SPI_RESET_IO_FUNC);

    //os_printf("PN512 IO init\r\n");

	/* input define */
    PN512_RESET_LOW();
    PN512_CLK_HIGH();
    PN512_MOSI_HIGH();

    delay = 1000*get_system_clk_MHZ_()>>1;
    while (delay--);
    PN512_RESET_HIGH();
}
/**************************************************************************************************/
void ICACHE_FLASH_ATTR
PN512_CS_set(void)
{
    PN512_CS_HIGH();
}
/**************************************************************************************************/
void ICACHE_FLASH_ATTR
PN512_CS_reset(void)
{
    PN512_CS_LOW();
}
/**************************************************************************************************/
extern uint_fast8_t TxSeri1ByteData(uint_fast8_t ch)
{
    uint_fast8_t i;
    for (i=8; i; i--)
    {
        if (ch & 0x80)       //Max first    高位先
        {
            PN512_MOSI_HIGH();
        }
        else
        {
            PN512_MOSI_LOW();
        }
        PN512_CLK_LOW();
        ch <<= 1;
        if (PN512_MISO_STATE())       //Max first    高位先
        {
            ch |= 0x01;
        }
        PN512_CLK_HIGH();
    }
    return (ch&0xFF);
}


/**************************************************************************************************/
/**************************************************************************************************/

