/*
 * eeprom_conf.h
 *
 *  Created on: 4 Feb 2017
 *      Author: dimtass
 */

#ifndef EEPROM_CONF_H_
#define EEPROM_CONF_H_

#include "Arduino.h"

class EERPOM_conf {
public:
	enum { EEPROM_PREAMBLE = 0x1EE1 };

	typedef struct {
		uint16_t	preamble;
		char 		ssid[20];
		char		passwd[20];
		uint16_t	udp_port;
	} tp_eeprom_conf;

	EERPOM_conf();
	virtual ~EERPOM_conf();
	tp_eeprom_conf& get(void);
	void set(tp_eeprom_conf & conf);

private:
	void load();
	void save();
	tp_eeprom_conf m_conf;

};



#endif /* EEPROM_CONF_H_ */
