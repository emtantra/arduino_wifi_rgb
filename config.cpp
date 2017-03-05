/*
 * conf.cpp
 *
 *  Created on: 3 Mar 2017
 *      Author: dimtass
 */
#include "config.h"

#define HEADER_SIZE	5	//2B: header, 1B: version, 2B: cs
#define EEPROM_SIZE HEADER_SIZE + m_conf.m_ext_conf_size

#define DBG Serial

Config::Config(uint16_t preamble, uint8_t version, uint16_t conf_pos,
	uint8_t * ext_conf, uint8_t ext_conf_size)
{
	//DBG.print(F("[CONF] Create new conf with preamble: 0x")); DBG.println(preamble, HEX);
	m_conf.header.preamble = preamble;
	m_conf.header.version = version;
	m_conf.start_pos = conf_pos;
	m_conf.ext_conf = ext_conf;
	m_conf.m_ext_conf_size = ext_conf_size;
	m_def_values = NULL;
}

Config::~Config()
{
	delete m_def_values;
}

void Config::SetDefaultValues(uint8_t * def_values, uint16_t conf_size)
{
	if (m_def_values == NULL) {
		DBG.println(F("[CONF] Setting new defaults"));
		m_def_values = (uint8_t*) malloc(conf_size);
	}
	else {
		//DBG.println(F("[CONF] Setting new defaults ?????????"));
	}
	memcpy(m_def_values, def_values, conf_size);
}

uint16_t Config::SetConf(uint8_t *buffer, uint16_t buff_size)
{
	memcpy(m_conf.ext_conf, buffer, buff_size);
	m_conf.m_ext_conf_size = buff_size;

	return(m_conf.m_ext_conf_size);
}

uint16_t Config::GetConf(uint8_t *buffer, uint16_t buff_size)
{
	uint16_t ret_size = 0;
	if (buff_size >= m_conf.m_ext_conf_size) {
		memcpy(buffer, m_conf.ext_conf, m_conf.m_ext_conf_size);
		ret_size = m_conf.m_ext_conf_size;
	}
	return(ret_size);
}

void Config::Load()
{
	uint16_t i;
	uint8_t valid_data = false;
	tp_conf_header eeprom_header;
	uint8_t * p = (uint8_t*) &eeprom_header;

	//DBG.print(F("[CONF] HEADER content:"));
	// Read EEPROM header w/o reading the data
	for (i = 0; i<HEADER_SIZE; i++) {
		p[i] = EEPROM.read(m_conf.start_pos + i);
		//DBG.print(p[i], HEX); DBG.print(F(","));
	}
	//DBG.println();

	for (i = 0; i<m_conf.m_ext_conf_size; i++) {
		//DBG.print(EEPROM.read(m_conf.start_pos + HEADER_SIZE + i), HEX); DBG.print(F(","));
	}
	//DBG.println();

	if (eeprom_header.preamble == m_conf.header.preamble) {
		// probably valid data
		p = (uint8_t*) m_conf.ext_conf;
		m_conf.header.cs = 0;
		// Read data and calculate cs
		for (i = 0; i<m_conf.m_ext_conf_size; i++) {
			p[i] = EEPROM.read(m_conf.start_pos + HEADER_SIZE + i);
			m_conf.header.cs += p[i];
//			DBG.print(p[i], HEX); DBG.print(F(","));
		}
		//DBG.print(F("Checksum: ")); DBG.print(m_conf.header.cs);
		//DBG.print(F(" / ")); DBG.println(eeprom_header.cs);
		if (m_conf.header.cs == eeprom_header.cs) {
			//valid data
			valid_data = true;
		}
	}

	// also check version
	if (eeprom_header.version != m_conf.header.version) {
		valid_data = false;
	}
	else {
		// do actions for different versions
	}

	// if not valid data found or false cs the load defaults
	if (!valid_data) {
		DBG.println(F("[CONF] Not valid config found"));
		Defaults();
	}
	else {
		DBG.println(F("[CONF] Loaded OK."));
	}
}


void Config::Save()
{
	uint16_t i;
	uint8_t * p = (uint8_t*) m_conf.ext_conf;

	m_conf.header.cs = 0;	// reset checksum
	// Calculate checksum
	for (i = 0; i<m_conf.m_ext_conf_size; i++) {
		m_conf.header.cs += p[i];
	}
	// Save data to EEPROM
	for (i = 0; i<m_conf.m_ext_conf_size; i++) {
		EEPROM.write(m_conf.start_pos + HEADER_SIZE + i, p[i]);
	}
	p = (uint8_t*)&m_conf.header;
	// Save header to EEPROM
	for (i = 0; i<HEADER_SIZE; i++) {
		EEPROM.write(i + m_conf.start_pos, p[i]);
	}
	DBG.println(F("[CONF] saved."));
}


void Config::Defaults()
{
	DBG.println(F("[CONF] loading defaults..."));
	memcpy(m_conf.ext_conf, m_def_values, m_conf.m_ext_conf_size);
	Save();
}
