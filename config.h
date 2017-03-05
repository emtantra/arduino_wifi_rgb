/*
 * conf.h
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "Arduino.h"
#include "EEPROM.h"

class Config {
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

	Config(uint16_t header, uint8_t version, uint16_t conf_pos, uint8_t * ext_conf, uint8_t ext_conf_size);
	~Config();
	void	SetDefaultValues(uint8_t * def_values, uint16_t conf_size);
	uint16_t GetConf(uint8_t *buffer, uint16_t buff_size);
	uint16_t SetConf(uint8_t *buffer, uint16_t buff_size);
	void	Load();
	void	Save();
	void	Defaults();

private:
	tp_conf m_conf;
	uint8_t * m_def_values;
};



#endif /* CONF_H_ */
