/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"

#include "user_devicefind.h"
#include "user_webserver.h"

#include "driver/uart.h"
// user device
#include "UserRfidAPI.h"
#include "UserKeyDeviceAPI.h"
#if ESP_PLATFORM
#include "user_esp_platform.h"
#endif

void user_rf_pre_init(void)
{
}

void ICACHE_FLASH_ATTR
user_set_station_config(void)
{

	struct station_config stationConf;
	os_memset(&stationConf, 0, sizeof(stationConf));
	stationConf.bssid_set = 0; //need not check MAC address of AP
	os_memcpy(&stationConf.ssid, "tuqianyin", 9);
	os_memcpy(&stationConf.password, "tu200906", 8);

	wifi_station_set_config(&stationConf);
}

void wifi_handle_event_cb(System_Event_t *evt)
{
	os_printf("event %x\n", evt->event);
	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
			os_printf("connect to ssid %s, channel %d\n",
			evt->event_info.connected.ssid,
			evt->event_info.connected.channel);
			break;
		case EVENT_STAMODE_DISCONNECTED:
			os_printf("disconnect from ssid %s, reason %d\n",
			evt->event_info.disconnected.ssid,
			evt->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			os_printf("mode: %d -> %d\n",
			evt->event_info.auth_change.old_mode,
			evt->event_info.auth_change.new_mode);
			break;
		case EVENT_STAMODE_GOT_IP:
			os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
			IP2STR(&evt->event_info.got_ip.ip),
			IP2STR(&evt->event_info.got_ip.mask),
			IP2STR(&evt->event_info.got_ip.gw));
			os_printf("\n");
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			os_printf("station: " MACSTR "join, AID = %d\n",
				MAC2STR(evt->event_info.sta_connected.mac),
			evt->event_info.sta_connected.aid);
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			os_printf("station: " MACSTR "leave, AID = %d\n",
			MAC2STR(evt->event_info.sta_disconnected.mac),
			evt->event_info.sta_disconnected.aid);
			break;
		default:
			break;
	}
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	
    os_printf("\r\nSDK version:%s\r\n", system_get_sdk_version());
	os_printf("system Freq=%dMhz\r\n", system_get_cpu_freq());
// user device
	userRfid_Init();
	userKeyDevice_Init();
	
#if ESP_PLATFORM
    /*Initialization of the peripheral drivers*/
    /*For light demo , it is user_light_init();*/
    /* Also check whether assigned ip addr by the router.If so, connect to ESP-server  */
   // user_esp_platform_init();


	user_set_station_config();
	wifi_set_event_handler_cb(wifi_handle_event_cb);

	wifi_set_opmode(STATION_MODE);
    os_printf("sta2=%d\r\n", wifi_get_opmode());
#endif
    /*Establish a udp socket to receive local device detect info.*/
    /*Listen to the port 1025, as well as udp broadcast.
    /*If receive a string of device_find_request, it rely its IP address and MAC.*/
    user_devicefind_init();

    /*Establish a TCP server for http(with JSON) POST or GET command to communicate with the device.*/
    /*You can find the command in "2B-SDK-Espressif IoT Demo.pdf" to see the details.*/
    /*the JSON command for curl is like:*/
    /*3 Channel mode: curl -X POST -H "Content-Type:application/json" -d "{\"period\":1000,\"rgb\":{\"red\":16000,\"green\":16000,\"blue\":16000}}" http://192.168.4.1/config?command=light      */
    /*5 Channel mode: curl -X POST -H "Content-Type:application/json" -d "{\"period\":1000,\"rgb\":{\"red\":16000,\"green\":16000,\"blue\":16000,\"cwhite\":3000,\"wwhite\",3000}}" http://192.168.4.1/config?command=light      */
#ifdef SERVER_SSL_ENABLE
    user_webserver_init(SERVER_SSL_PORT);
#else
    user_webserver_init(SERVER_PORT);
#endif
}

