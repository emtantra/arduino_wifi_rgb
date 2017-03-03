/*
 * conf_ap.cpp
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */
#include "conf_ap.h"

Conf_AP::Conf_AP(ESP8266 &wifi)
{
	m_wifi = wifi;
	m_ap_ip = "";
	m_wifi->restart();
	delay(2000);
}

Conf_AP::~Conf_AP()
{
	/* Reset module */
	m_wifi->restart();
	/* delete object */
	delete m_wifi;
}

Conf_AP::en_ret_code Conf_AP::begin(void)
{
	en_ret_code resp = RET_OK;

	/* Create AP */
	if (!m_wifi->autoSetBaud()) {
		resp = -RET_ERR_AUTOBAUD;
		goto __ret;
	}
	/* Setting operation mode to SoftAP */
	if (!m_wifi->setOprToSoftAP()) {
		resp = -RET_ERR_SET_AP;
		goto __ret;
	}

	if (!wifi.disableMUX()) {
		resp =-RET_ERR_DISABLE_MUX;
		goto __ret;
	}
	/* retrieve local IP */
	m_ap_ip = wifi.getLocalIP();

	/* Create a local UDP socket */
	if (m_wifi->registerUDP(m_ap_ip, UDP_PORT)) {
		resp = -RET_ERR_REG_UDP;
		goto __ret;
	}

	/* Start listening for data */
	bool finished = false;
	size_t rcv_buff_size = 100;
	uint8_t rcv_buff[rcv_buff_size];
	uint32_t rcv_len = 0;

	while (!finished) {
		rcv_len = m_wifi->recv(rcv_buff, rcv_buff_size, 1000);
		if (rcv_len) {
			if (parser(rcv_buff, rcv_len)) {
				resp = RET_OK;
				goto __ret;
			}
		}
	}

__ret:
	return(resp);
}


bool Conf_AP::parser(uint8_t * buffer, size_t len)
{
	bool exit_AP = false;
	int i = 0;

	// remove \r\n
	for (i=0; i<len; i++) {
		if ((buffer[i] == '\r') || (buffer[i] == '\n')) buffer[i] = 0;
	}
	/* Debug print the data */
	Serial.print("CMD: "); Serial.println(buffer);

	if (!(strncmp(buffer, "SSID_NAME=", 10))) {
		/* Save ssid name to EEPROM */

	}
	else if (!(strncmp(buffer, "SSID_PASS=", 10))) {
		/* Save ssid passwd to EEPROM */

	}
	else if (!(strncmp(buffer, "UDP_PORT=", 9))) {
		/* Save ssid passwd to EEPROM */

	}
	else if (!(strncmp(buffer, "RESET", 10))) {
		/* Exit */
		exit_AP = true;
	}
	return(exit_AP);
}

