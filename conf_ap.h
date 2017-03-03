/*
 * conf_ap.h
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */

#ifndef CONF_AP_H_
#define CONF_AP_H_

#include "Arduino.h"
#include "ESP8266.h"

class Conf_AP {
public:
	typedef enum {
		RET_OK,
		RET_ERR_AUTOBAUD,
		RET_ERR_SET_AP,
		RET_ERR_DISABLE_MUX,
		RET_ERR_REG_UDP,
	} en_ret_code;

	Conf_AP(ESP8266 & wifi);
	virtual ~Conf_AP();
	bool begin(void);

private:
	enum {UDP_PORT = 7700};

	String m_ap_ip;
	ESP8266 * m_wifi;

	bool parser(uint8_t * buffer, size_t len);
};


#endif /* CONF_AP_H_ */
