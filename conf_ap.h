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

#include "glb_common.h"
#include "config.h"

class Conf_AP {
public:
	typedef enum {
		RET_OK,
		RET_ERR_AUTOBAUD = -1,
		RET_ERR_SET_AP = -2,
		RET_ERR_DISABLE_MUX = - 3,
		RET_ERR_REG_UDP = -4,
	} en_ret_code;

	Conf_AP(ESP8266 * wifi, Config * cfg);
	virtual ~Conf_AP();
	en_ret_code begin(void);

private:
	enum {UDP_PORT = 7700};

	String m_ap_ip;
	ESP8266 * m_wifi;
	Config *m_cfg;
	bool parser(uint8_t * buffer, size_t len);
};


#endif /* CONF_AP_H_ */
