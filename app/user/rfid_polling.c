#include "ets_sys.h"
#include "osapi.h"

#include "reader_Mifare.h"
#include "rfid_polling.h"

//#include "includes.h"

enum
{
    Rfid_IDLE = 0,            // 空闲
    Rfid_WaitCard,            // 等卡
    Rfid_WaitMoveCard,        // 移卡


    Rfid_ReadMax
};

uint8_t cardstate = Rfid_WaitCard;

/* 读写 一块数据  先写 后 读 空卡数据 为 0*/
static uint8_t ICACHE_FLASH_ATTR
TestWriteReadBlockData(uint8_t *uid)
{
   uint8_t datbuf[16];
   uint8_t block = 5;
   uint8_t rbuf[16];


   os_memcpy(datbuf,"12345678aabbccdd",16);
   os_memset(rbuf,0,sizeof(rbuf));

   if (MifareReader_block_auth(block,uid, NULL,NULL) != 0)
   {
       os_printf("auth\r\n");
       return 0;
   }
   if (MifareReader_block_write(block, datbuf) != 0)
   {
       os_printf("write\r\n");
       return 0;

   }
   if (MifareReader_block_read(block,rbuf) != 0)
   {
       os_printf("read\r\n");
       return 0;
   }
   os_printf ("wr ok\r\n");
   //USART1_Putbuf(rbuf,16);
   return 1;
}

/* 刷卡 读卡号 */
void ICACHE_FLASH_ATTR
TestReadRfidNum(void)
{
    uint8_t uid[4];
    switch(cardstate)
    {
         case Rfid_WaitCard:
             if (PN512RDSerialnum(uid) == 0)
             {
                 os_printf("read uid ok\r\n");
                 uart0_tx_buffer(uid,4);
                 if (TestWriteReadBlockData(uid))
                 {

                 }
                 cardstate = Rfid_WaitMoveCard;
             }
             else
             {
                 //os_printf("wait\r\n");
             }
             break;
         case Rfid_WaitMoveCard:
             //os_printf("move\r\n");
             if (PN512RDSerialnum(uid) != 0)
             {
                 if(PN512RDSerialnum(uid) == 0)break;
                 if(PN512RDSerialnum(uid) == 0)break;
                 PN512Close();
                 cardstate =  Rfid_WaitCard;
             }
             break;

         default:
             if(cardstate >= Rfid_ReadMax) cardstate = Rfid_WaitCard;
             break;
     }
}

/* ===================  NFC Initiator ( 设备 ) ======================== */
/*
  一 、通信模式
   NFC设备支持两种通信模式。
   1、主动模式
   在这种模式，目标设备和发起通信设备都有动力，互相之间可以轮流传输信号。

   2、被动模式
      发起设备差生无线电信号，目标设备由这个信号的电磁场提供动力。目标设备通过调制电磁场回应发起设备。

  二、操作模式
   NFC可以运行在ISO/IEC 18092, NFC IP-1, 和ISO/IEC 14443三种无线智能卡标准下（contactless smart card standard）

   1、读/写
   在这种模式，开启NFC功能的手机可以读写任何支持的标签， 读取其中的NFC数据格式标准的数据。

   2、点对点
   在这种模式下，两个NFC设备可以交换数据。 例如，你可以分享启动蓝牙或Wi-Fi连接的参数来启动 蓝牙或Wi-Fi连接。
   你可以交换如虚拟名片或数字相片等数据。点对点模式符合ISO/IEC 18092标准。

   3、模拟卡片
   支持NFC的手机在与标签交互时扮演读取器的角色。在这种模式手机也可做为标签或被读取的无线卡片。

  三、NFC论坛标签种类
   在NFC论坛的技术规范网站http://www.nfc-forum.org/specs/spec_list/查看NFC论坛标签种类。
   标签的规范定义了实现 阅读器/擦写器的技术信息和相应的与之互动的NFC设备的控制功能

	1、标签类型1 (NFC Forum Type 1)

	类型1标签比较便宜适合于多种NFC应用。
	基于ISO-14443A标准
	可读可重写，可配置成只读
	96 byte内存，可扩展到2KB
	传输速率 106kbits/s
	没有数据冲突保护
	市场上有兼容的产品 ———— Innovision Topaz, Broadcom BCM20203
	NFC论坛设备如何操作类型1标签，如何发现，读取和写入NDEF数据（参看章节'NDEF'）
	参看类型1标签操作规范：http://www.nfc-forum.org/specs/spec_list/

    2、标签类型2 (NFC Forum Type 2)

    类型2与类型1类似，也是由NXP/Philips MIFARE Ultralight标签衍生而来的。
    基于ISO-14443A标准
    可读可重写，可配置成只读
    传输速率 106kbits/s
    支持数据冲突保护
    市场上有兼容的产品 ：NXP MIFARE Ultralight

    3、标签类型3

    类型3由索尼FeliCa标签的非保密部分衍生而来。比类型1,2的标签昂贵。
    基于日本工业标准(JIS) X 6319-4
    在生产时定义可读，可重写或只读的属性。
    可变内存，每个服务最多1MB空间
    支持两种传输速率：212或424kbits/s
    支持数据冲突保护
    市场上有兼容的产品 ：Sony FeliCa

    4、标签类型4

    类型4与类型1类似，是由NXP DESFire标签衍生而来的。
    基于ISO-14443A标准
    在生产时定义可读，可重写或只读的属性。
    可变内存，每个服务最大32kB
    支持三种传输速率： 106，212或424kbits/s
    支持数据冲突保护
    市场上有兼容的产品 ：NXP DESFire, SmartMX-JCOP

    5、NXP规范标签类型(由NXP半导体定义的私有标签类型)

    MIFARE类型经典标签
    基于ISO-14443A标准
    可读可重写，可配置成只读
    可变内存192/768/3584 bytes
    传输速率 106kbits/s
    支持数据冲突保护
    市场上有兼容的产品：NXP MIFARE Classic 1K, MIFARE Classic 4K, and Classic Mini
    参见NXP网站：http://www.nxp.com/
  四、相关规范
    1、ISO 14443
    ISO 14443是著名的国际标准，原来是为非接触芯片卡片在13.56MHz无线电通信设计的。
    ISO 14443 从无线层到命令协议定义了一个协议栈。无线层ISO14443-2有两个版本，具有不同的调制和bit编码方法。
    称为A，B版。类似的，ISO 14443指定了两个版本的包框架和底层协议部分（ISO 14443-3）。
    ISO协议栈的最高层定义了传输信息的命令接口（ISO 14443-4）。

    2、NFCIP-1
     两台NFC设备间的点到点通信是由近场通信--接口和协议规范，NFCIP-1定义的机制实现的。
     这个NFC关键规范也被称为ISO 18092和ECMA-340。
     NFCIP-1的协议栈基于ISO 14443. 主要的不同时一个新的命令协议，替换了之前协议栈的最高层。
     NFCIP-1 包括两个通信模块使得NFC设备之间能工作在点对点的模式，也支持与基于NFCIP-1的NFC标签通信。
    3、MIFARE
     MIFARE 指由NXP半导体开发的NFC标签类型。MIFARE标签被广泛用于运输工具应用的内存卡。
     ISO 14443定义了从无线层到命令协议的协议栈。
    4、FeliCa
	FeliCa是Sony公司开发的的专利NFC标签技术，被广泛用于专买支付和亚洲的运输工具应用。
	FeliCa标签也被集成在移动FeliCa系统的手机模型中。Felica标签属于日本的工业标准。
	标签基于被动模式的ISO 18902，带有额外的认证和加密功能。



*/
extern void  NfcInitiatorPolling(void)
{
    /*   comHandle:  串口调试口
         isPassive:  0 主动  1 被动
		 opSpeed:    速率
		             0   106kBit/s
		             1   212kBit/s
		             2   424kBit/s
    */
   // ActivateNfcInitiator(0,0,0);
}
/* ===================  NFC  Target( 标签 ) ======================== */
extern void  NfcTargetPolling(void)
{

}

