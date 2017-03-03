/*
 * udp_client.cpp
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */
#include "udp_client.h"


UDP_Client::UDP_Client(ESP8266 &wifi, String &ssid, String &passwd, int udp_port)
{
	m_wifi = wifi;
	m_ssid = ssid;
	m_passwd = passwd;
	m_udp_port = udp_port;
}

virtual UDP_Client::~UDP_Client()
{

}

UDP_Client::en_ret_code UDP_Client::begin(void)
{
	en_ret_code resp = RET_OK;

	if (!m_wifi->autoSetBaud()) {
		resp = -RET_ERR_AUTOBAUD;
		goto __ret;
	}
	/* Setting operation mode to SoftAP */
	if (!m_wifi->setOprToStation()) {
		resp = -RET_ERR_SET_STATION;
		goto __ret;
	}

	if (!wifi.disableMUX()) {
		resp =-RET_ERR_DISABLE_MUX;
		goto __ret;
	}
	/* retrieve local IP */
	String ip = wifi.getLocalIP();

	/* Create a local UDP socket */
	if (m_wifi->registerUDP(ip, m_udp_port)) {
		resp = -RET_ERR_REG_UDP;
		goto __ret;
	}

	return(resp);
}


uint32_t UDP_Client::check_data(void)
{
	return(m_wifi->recv(m_rcv_buff, RECV_BUFF_LEN, 1));
}


