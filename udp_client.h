/*
 * udp_client.h
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */

#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

#include "Arduino.h"
#include "ESP8266.h"

class UDP_Client
{
public:
	typedef enum {
		RET_OK,
		RET_ERR_AUTOBAUD,
		RET_ERR_SET_STATION,
		RET_ERR_DISABLE_MUX,
		RET_ERR_REG_UDP,
	} en_ret_code;

	enum {RECV_BUFF_LEN = 50};

	UDP_Client(ESP8266 &wifi, String &ssid, String &passwd, int udp_port);
	virtual ~UDP_Client();
	en_ret_code begin(void);
	uint32_t check_data(void);

private:
	ESP8266 * m_wifi;
	String m_ssid;
	String m_passwd;
	int m_udp_port;

	uint8_t m_rcv_buff[RECV_BUFF_LEN];
};


#endif /* UDP_CLIENT_H_ */
