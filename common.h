/*
 * config.h
 *
 *  Created on: 4 Feb 2017
 *      Author: dimtass
 */

#ifndef COMMON_H_
#define COMMON_H_

#define ESP8266_USE_SOFTWARE_SERIAL

#include "ChainableLED.h"

#define EEPROM_CONF_HEADER	0xABCD
#define EEPROM_CONF_VERSION	1
#define EEPROM_CONF_POS		0

#define WIFI_SSID_LEN	25
#define WIFI_PASSWD_LEN	25

typedef enum {
	LED_PATTERN_HEARTBEAT = 0b11110000,
	LED_PATTERN_CFG_ERR = 0b00110011,
	LED_PATTERN_
};

typedef struct {
	byte	r;
	byte	g;
	byte	b;
} tp_colors;

// EEPROM config structure
//
#pragma pack(1)
typedef struct {
	char		wifi_ssid[WIFI_SSID_LEN];
	char		wifi_passwd[WIFI_PASSWD_LEN];
	uint16_t	udp_port;
} tp_conf;
tp_conf conf;

typedef struct {
	/* timers */
	uint16_t	tmr_1ms;
	uint8_t		tmr_100ms;
	uint16_t	tmr_1000ms;
	/* rgb */
	ChainableLED *rgb;
	tp_colors	colors;
	/* config mode */
	uint16_t	led_pattern;
	uint16_t 	led_pattern_index;
} tp_glb;

extern tp_glb glb;
extern tp_conf glb_conf;


#endif /* COMMON_H_ */
