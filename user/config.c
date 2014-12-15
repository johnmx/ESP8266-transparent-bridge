#include "espmissingincludes.h"
#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "osapi.h"

#include "config.h"

#ifdef CONFIG_STATIC

void config_execute(void) {
	uint8_t mode;
	struct station_config sta_conf;
	struct softap_config ap_conf;
	uint8_t macaddr[6];

	// make sure the device is in AP and STA combined mode
	mode = wifi_get_mode();
	if (mode != STATIONAP_MODE) {
		wifi_set_mode(STATIONAP_MODE);
		system_restart();
	}

	// connect to our station
	os_strncpy(sta_conf.ssid, STA_SSID, sizeof(sta_conf.ssid));
	os_strncpy(sta_conf.password, STA_PASSWORD, sizeof(sta_conf.password));
	wifi_station_set_config(&sta_conf);		
	wifi_station_disconnect();	// probably not necessary
	wifi_station_connect();

	// setup the soft AP
	wifi_get_macaddr(SOFTAP_IF, macaddr);
	os_strncpy(ap_conf.ssid, AP_SSID, sizeof(ap_conf.ssid));
	os_strncpy(ap_conf.password, AP_PASSWORD, sizeof(ap_conf.password));
	os_snprintf(&ap_conf.password[strlen(AP_PASSWORD)], sizeof(ap_conf.password), "%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
	ap_conf.authmode = AUTH_WPA_PSK;
	wifi_softap_set_config(&ap_conf);
}

#endif

#ifdef CONFIG_DYNAMIC

#define MSG_OK "OK\r\n"
#define MSG_INVALID_CMD "INVALID COMMAND\r\n"

void config_parse(struct espconn *conn, char *buf, int len) {
	char *cmd;

	if (os_strncmp(buf, "+++AT", 5) != 0) {
		return;
	}
	cmd=&buf[5];
	for (; *cmd == ' ' || *cmd == '\t'; ++cmd); // absorb spaces
	if (os_strncmp(cmd, "STA", 3) == 0) {
		espconn_sent(conn, MSG_OK, strlen(MSG_OK));
	} else if (os_strncmp(cmd, "AP", 2) == 0) {
		espconn_sent(conn, MSG_OK, strlen(MSG_OK));
	} else if (os_strncmp(cmd, "MODE", 4) == 0) {
		espconn_sent(conn, MSG_OK, strlen(MSG_OK));
	} else if (*cmd == '\n' || *cmd == '\r') {
		espconn_sent(conn, MSG_OK, strlen(MSG_OK));
	} else {
		espconn_sent(conn, MSG_INVALID_CMD, strlen(MSG_INVALID_CMD));
	}
}

#endif
