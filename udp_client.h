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
		RET_ERR_AUTOBAUD = -1,
		RET_ERR_SET_STATION = -2,
		RET_ERR_DISABLE_MUX = -3,
		RET_ERR_REG_UDP = -4,
		RET_ERR_JOIN_AP = -5,
	} en_ret_code;

	enum {RECV_BUFF_LEN = 50};

	UDP_Client(ESP8266 *wifi, String &ssid, String &passwd, uint16_t udp_port);
	virtual 	~UDP_Client();
	en_ret_code begin(void);
	uint16_t 	check_data(void);
	bool		check_status(void);
	uint8_t * 	get_buffer();

private:
	ESP8266	*m_wifi;
	String 	m_ssid;
	String 	m_passwd;
	int 	m_udp_port;
	String 	m_router_ip;
	uint8_t m_rcv_buff[RECV_BUFF_LEN];
};


#endif /* UDP_CLIENT_H_ */
