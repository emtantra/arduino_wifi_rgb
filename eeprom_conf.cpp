/*
 * eeprom_conf.cpp
 *
 *  Created on: 4 Feb 2017
 *      Author: dimtass
 */
#include "Arduino.h"
#include "EEPROM.h"
#include "eeprom_conf.h"


EERPOM_conf::EERPOM_conf()
{
	load();
	if (m_conf.preamble != EEPROM_PREAMBLE) {
		/* create default conf */
		m_conf.preamble = EEPROM_PREAMBLE;
		memset(m_conf.ssid, 0, sizeof(m_conf.ssid));
		memset(m_conf.passwd, 0, sizeof(m_conf.passwd));
		m_conf.udp_port = 15500;
	}
}

EERPOM_conf::~EERPOM_conf()
{

}

EERPOM_conf::tp_eeprom_conf& EERPOM_conf::get(void)
{
	return m_conf;
}

void EERPOM_conf::set(tp_eeprom_conf & conf)
{
	m_conf = conf;
	save();
}

/**
 * Save data to EEPROM
 */
void EERPOM_conf::save(void)
{
	size_t i = 0;
	uint8_t * p = (uint8_t*) &m_conf;
	for(i=0; i<sizeof(tp_eeprom_conf); i++) {
		EEPROM.write(i, p[i]);
	}
}

/**
 * Read config data from EEPROM
 */
void EERPOM_conf::load(void)
{
	size_t i = 0;
	uint8_t * p = (uint8_t*) &m_conf;
	for(i=0; i<sizeof(tp_eeprom_conf); i++) {
		p[i] = EEPROM.read(i);
	}
}

