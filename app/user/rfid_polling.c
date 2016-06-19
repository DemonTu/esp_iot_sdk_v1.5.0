#include "ets_sys.h"
#include "osapi.h"

#include "reader_Mifare.h"
#include "rfid_polling.h"

//#include "includes.h"

enum
{
    Rfid_IDLE = 0,            // ����
    Rfid_WaitCard,            // �ȿ�
    Rfid_WaitMoveCard,        // �ƿ�


    Rfid_ReadMax
};

uint8_t cardstate = Rfid_WaitCard;

/* ��д һ������  ��д �� �� �տ����� Ϊ 0*/
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

/* ˢ�� ������ */
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

/* ===================  NFC Initiator ( �豸 ) ======================== */
/*
  һ ��ͨ��ģʽ
   NFC�豸֧������ͨ��ģʽ��
   1������ģʽ
   ������ģʽ��Ŀ���豸�ͷ���ͨ���豸���ж���������֮��������������źš�

   2������ģʽ
      �����豸�������ߵ��źţ�Ŀ���豸������źŵĵ�ų��ṩ������Ŀ���豸ͨ�����Ƶ�ų���Ӧ�����豸��

  ��������ģʽ
   NFC����������ISO/IEC 18092, NFC IP-1, ��ISO/IEC 14443�����������ܿ���׼�£�contactless smart card standard��

   1����/д
   ������ģʽ������NFC���ܵ��ֻ����Զ�д�κ�֧�ֵı�ǩ�� ��ȡ���е�NFC���ݸ�ʽ��׼�����ݡ�

   2����Ե�
   ������ģʽ�£�����NFC�豸���Խ������ݡ� ���磬����Է�������������Wi-Fi���ӵĲ��������� ������Wi-Fi���ӡ�
   ����Խ�����������Ƭ��������Ƭ�����ݡ���Ե�ģʽ����ISO/IEC 18092��׼��

   3��ģ�⿨Ƭ
   ֧��NFC���ֻ������ǩ����ʱ���ݶ�ȡ���Ľ�ɫ��������ģʽ�ֻ�Ҳ����Ϊ��ǩ�򱻶�ȡ�����߿�Ƭ��

  ����NFC��̳��ǩ����
   ��NFC��̳�ļ����淶��վhttp://www.nfc-forum.org/specs/spec_list/�鿴NFC��̳��ǩ���ࡣ
   ��ǩ�Ĺ淶������ʵ�� �Ķ���/��д���ļ�����Ϣ����Ӧ����֮������NFC�豸�Ŀ��ƹ���

	1����ǩ����1 (NFC Forum Type 1)

	����1��ǩ�Ƚϱ����ʺ��ڶ���NFCӦ�á�
	����ISO-14443A��׼
	�ɶ�����д�������ó�ֻ��
	96 byte�ڴ棬����չ��2KB
	�������� 106kbits/s
	û�����ݳ�ͻ����
	�г����м��ݵĲ�Ʒ �������� Innovision Topaz, Broadcom BCM20203
	NFC��̳�豸��β�������1��ǩ����η��֣���ȡ��д��NDEF���ݣ��ο��½�'NDEF'��
	�ο�����1��ǩ�����淶��http://www.nfc-forum.org/specs/spec_list/

    2����ǩ����2 (NFC Forum Type 2)

    ����2������1���ƣ�Ҳ����NXP/Philips MIFARE Ultralight��ǩ���������ġ�
    ����ISO-14443A��׼
    �ɶ�����д�������ó�ֻ��
    �������� 106kbits/s
    ֧�����ݳ�ͻ����
    �г����м��ݵĲ�Ʒ ��NXP MIFARE Ultralight

    3����ǩ����3

    ����3������FeliCa��ǩ�ķǱ��ܲ�������������������1,2�ı�ǩ����
    �����ձ���ҵ��׼(JIS) X 6319-4
    ������ʱ����ɶ�������д��ֻ�������ԡ�
    �ɱ��ڴ棬ÿ���������1MB�ռ�
    ֧�����ִ������ʣ�212��424kbits/s
    ֧�����ݳ�ͻ����
    �г����м��ݵĲ�Ʒ ��Sony FeliCa

    4����ǩ����4

    ����4������1���ƣ�����NXP DESFire��ǩ���������ġ�
    ����ISO-14443A��׼
    ������ʱ����ɶ�������д��ֻ�������ԡ�
    �ɱ��ڴ棬ÿ���������32kB
    ֧�����ִ������ʣ� 106��212��424kbits/s
    ֧�����ݳ�ͻ����
    �г����м��ݵĲ�Ʒ ��NXP DESFire, SmartMX-JCOP

    5��NXP�淶��ǩ����(��NXP�뵼�嶨���˽�б�ǩ����)

    MIFARE���;����ǩ
    ����ISO-14443A��׼
    �ɶ�����д�������ó�ֻ��
    �ɱ��ڴ�192/768/3584 bytes
    �������� 106kbits/s
    ֧�����ݳ�ͻ����
    �г����м��ݵĲ�Ʒ��NXP MIFARE Classic 1K, MIFARE Classic 4K, and Classic Mini
    �μ�NXP��վ��http://www.nxp.com/
  �ġ���ع淶
    1��ISO 14443
    ISO 14443�������Ĺ��ʱ�׼��ԭ����Ϊ�ǽӴ�оƬ��Ƭ��13.56MHz���ߵ�ͨ����Ƶġ�
    ISO 14443 �����߲㵽����Э�鶨����һ��Э��ջ�����߲�ISO14443-2�������汾�����в�ͬ�ĵ��ƺ�bit���뷽����
    ��ΪA��B�档���Ƶģ�ISO 14443ָ���������汾�İ���ܺ͵ײ�Э�鲿�֣�ISO 14443-3����
    ISOЭ��ջ����߲㶨���˴�����Ϣ������ӿڣ�ISO 14443-4����

    2��NFCIP-1
     ��̨NFC�豸��ĵ㵽��ͨ�����ɽ���ͨ��--�ӿں�Э��淶��NFCIP-1����Ļ���ʵ�ֵġ�
     ���NFC�ؼ��淶Ҳ����ΪISO 18092��ECMA-340��
     NFCIP-1��Э��ջ����ISO 14443. ��Ҫ�Ĳ�ͬʱһ���µ�����Э�飬�滻��֮ǰЭ��ջ����߲㡣
     NFCIP-1 ��������ͨ��ģ��ʹ��NFC�豸֮���ܹ����ڵ�Ե��ģʽ��Ҳ֧�������NFCIP-1��NFC��ǩͨ�š�
    3��MIFARE
     MIFARE ָ��NXP�뵼�忪����NFC��ǩ���͡�MIFARE��ǩ���㷺�������乤��Ӧ�õ��ڴ濨��
     ISO 14443�����˴����߲㵽����Э���Э��ջ��
    4��FeliCa
	FeliCa��Sony��˾�����ĵ�ר��NFC��ǩ���������㷺����ר��֧�������޵����乤��Ӧ�á�
	FeliCa��ǩҲ���������ƶ�FeliCaϵͳ���ֻ�ģ���С�Felica��ǩ�����ձ��Ĺ�ҵ��׼��
	��ǩ���ڱ���ģʽ��ISO 18902�����ж������֤�ͼ��ܹ��ܡ�



*/
extern void  NfcInitiatorPolling(void)
{
    /*   comHandle:  ���ڵ��Կ�
         isPassive:  0 ����  1 ����
		 opSpeed:    ����
		             0   106kBit/s
		             1   212kBit/s
		             2   424kBit/s
    */
   // ActivateNfcInitiator(0,0,0);
}
/* ===================  NFC  Target( ��ǩ ) ======================== */
extern void  NfcTargetPolling(void)
{

}

