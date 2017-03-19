/*
 * conf_ap.cpp
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */
#include "conf_ap.h"

Conf_AP::Conf_AP(ESP8266 *wifi, Config * cfg)
{
	m_wifi = wifi;
	m_ap_ip = "";
	m_cfg = cfg;
}

Conf_AP::~Conf_AP()
{
	/* Reset module */
	m_wifi->restart();
	delay(2000);
}

Conf_AP::en_ret_code Conf_AP::begin(void)
{
	en_ret_code resp = RET_OK;

	bool finished = false;
	size_t rcv_buff_size = 100;
	uint8_t rcv_buff[rcv_buff_size];
	uint32_t rcv_len = 0;

	m_wifi->restart();
	delay(4000);

	/* Create AP */
	if (!m_wifi->autoSetBaud()) {
		resp = RET_ERR_AUTOBAUD;
		goto __ret;
	}
	delay(100);
	/* Setting operation mode to SoftAP */
	if (!m_wifi->setOprToSoftAP()) {
		resp = RET_ERR_SET_AP;
		goto __ret;
	}
	delay(100);

	if (!m_wifi->disableMUX()) {
		resp = RET_ERR_DISABLE_MUX;
		goto __ret;
	}
	delay(100);
	/* retrieve local IP */
	m_ap_ip = m_wifi->getLocalIP();

	/* Create a local UDP socket */
	if (!m_wifi->registerUDP("192.168.4.255", UDP_PORT, UDP_PORT)) {
		resp = RET_ERR_REG_UDP;
		goto __ret;
	}
	delay(100);

	/* Start listening for data */
	Serial.print(F("UDP listen: "));
	Serial.print(m_ap_ip);
	Serial.print(F(":"));
	Serial.println(UDP_PORT);

	digitalWrite(PIN_LED, HIGH);

	while (!finished) {
		rcv_len = m_wifi->recv(rcv_buff, rcv_buff_size, 10000);
		if (rcv_len) {
			if (parser(rcv_buff, rcv_len)) {
				resp = RET_OK;
				goto __ret;
			}
		}
	}
	digitalWrite(PIN_LED, LOW);

__ret:
	return(resp);
}


bool Conf_AP::parser(uint8_t * buffer, size_t len)
{
	bool exit_AP = false;
	size_t i = 0;

	// remove \r\n
	for (i=0; i<len; i++) {
		if ((buffer[i] == '\r') || (buffer[i] == '\n')) buffer[i] = 0;
	}
	buffer[len] = 0;

	/* Debug print the data */
	Serial.print(F("CMD: ")); Serial.println((char*)buffer);

	if (!(strncmp((char*)buffer, "SSID_NAME=", 10))) {
		tp_conf tmp_conf;

		digitalWrite(PIN_LED, LOW);
		/* Get defaults */
		m_cfg->GetConf((uint8_t*) &tmp_conf, sizeof(tp_conf));
		strcpy(tmp_conf.wifi_ssid, (char*) &buffer[10]);

		/* Set new defaults */
		m_cfg->SetConf((uint8_t*) &tmp_conf, sizeof(tp_conf));
		m_cfg->Save();
		digitalWrite(PIN_LED, HIGH);
	}
	else if (!(strncmp((char*)buffer, "SSID_PASS=", 10))) {
		/* Save ssid passwd to EEPROM */
		tp_conf tmp_conf;

		digitalWrite(PIN_LED, LOW);
		/* Get defaults */
		m_cfg->GetConf((uint8_t*) &tmp_conf, sizeof(tp_conf));
		strcpy(tmp_conf.wifi_passwd, (char*) &buffer[10]);

		/* Set new defaults */
		m_cfg->SetConf((uint8_t*) &tmp_conf, sizeof(tp_conf));
		m_cfg->Save();
		digitalWrite(PIN_LED, HIGH);
	}
	else if (!(strncmp((char*)buffer, "UDP_PORT=", 9))) {
		/* Save ssid passwd to EEPROM */

	}
	else if (!(strncmp((char*)buffer, "RESET", 5))) {
		/* Exit */
		Serial.println("EXIT");
		m_wifi->restart();
		delay(2000);
		exit_AP = true;
	}
	return(exit_AP);
}

