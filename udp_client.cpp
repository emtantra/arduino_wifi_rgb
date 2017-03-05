/*
 * udp_client.cpp
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */
#include "udp_client.h"


UDP_Client::UDP_Client(ESP8266 *wifi, String &ssid, String &passwd, uint16_t udp_port)
{
	m_wifi = wifi;
	m_ssid = ssid;
	m_passwd = passwd;
	m_udp_port = udp_port;
}

UDP_Client::~UDP_Client()
{

}

UDP_Client::en_ret_code UDP_Client::begin(void)
{
	en_ret_code resp = RET_OK;
	String ip;

	if (m_wifi->getJoinedDeviceIP())

	if (!m_wifi->autoSetBaud()) {
		resp = RET_ERR_AUTOBAUD;
		goto __ret;
	}
	/* Setting operation mode to SoftAP */
	if (!m_wifi->setOprToStation()) {
		resp = RET_ERR_SET_STATION;
		goto __ret;
	}

	if (!m_wifi->disableMUX()) {
		resp = RET_ERR_DISABLE_MUX;
		goto __ret;
	}

	if (!m_wifi->joinAP(m_ssid, m_passwd)) {
			resp = RET_ERR_JOIN_AP;
			goto __ret;
		}

	/* retrieve local IP */
	ip = m_wifi->getLocalIP();

	/* Create a local UDP socket */
	if (!m_wifi->registerUDP(ip, m_udp_port, m_udp_port)) {
		resp = RET_ERR_REG_UDP;
		goto __ret;
	}
	Serial.print(F("[UDP client]: "));
	Serial.print(ip);
	Serial.print(",");
	Serial.print(m_udp_port);

__ret:
	return(resp);
}


uint16_t UDP_Client::check_data(void)
{
	return((uint16_t) m_wifi->recv(m_rcv_buff, RECV_BUFF_LEN, 20000));
}

uint8_t * UDP_Client::get_buffer()
{
	return(m_rcv_buff);
}


