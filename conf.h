/*
 * conf.h
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */

#ifndef CONF_H_
#define CONF_H_

#include "Arduino.h"
#include "EEPROM.h"

class Conf {
public:
#pragma pack(1)

	typedef struct {
		uint16_t	preamble;
		uint8_t		version;
		uint16_t	cs;
	} tp_conf_header;

	typedef struct {
		tp_conf_header header;
		uint16_t	start_pos;
		uint8_t		* ext_conf;
		uint16_t	m_ext_conf_size;
	} tp_conf;

	Conf(uint16_t header, uint16_t version, uint16_t conf_pos, uint8_t * ext_conf, uint8_t ext_conf_size);
	~Conf();
	void	SetDefaultValues(uint8_t * def_values, uint16_t conf_size);
	int		Load();
	int		Save();
	int		Defaults();

private:
	tp_conf m_conf;
	uint8_t * m_def_values;
};



#endif /* CONF_H_ */
