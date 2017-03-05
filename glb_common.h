/*
 * config.h
 *
 *  Created on: 4 Feb 2017
 *      Author: dimtass
 */

#ifndef GLB_COMMON_H_
#define GLB_COMMON_H_

#define ESP8266_USE_SOFTWARE_SERIAL

#include "ChainableLED.h"

/**
 * buffers len
 */
#define WIFI_SSID_LEN	25
#define WIFI_PASSWD_LEN	25

/**
 *  Pinout
 */
#define PIN_LED	13
#define PIN_RGB_CLK 5
#define PIN_RGB_DAT	6
#define PIN_ENTER_CFG 4

#define TMR_INT_MS	50

/**
 * LED patterns
 */
typedef enum {
	LED_PATTERN_HEARTBEAT = 0b11110000,
	LED_PATTERN_CFG_ERR = 0b00110011,
	LED_PATTERN_
} en_led_pattern;


/**
 * RGB decoder struct
 */
typedef struct {
	byte	r;
	byte	g;
	byte	b;
} tp_colors;


/**
 * EEPROM configuration
 */
#define EEPROM_CONF_HEADER	0xABCD
#define EEPROM_CONF_VERSION	1
#define EEPROM_CONF_POS		0
#pragma pack(1)
typedef struct {
	char		wifi_ssid[WIFI_SSID_LEN];
	char		wifi_passwd[WIFI_PASSWD_LEN];
	uint16_t	udp_port;
} tp_conf;

/**
 * Global data
 */
typedef struct {
	/* timers */
	uint16_t	tmr_1ms;
	uint8_t		tmr_250ms;
	uint16_t	tmr_1000ms;
	uint16_t	tmr_check_wifi;
	/* rgb */
	uint16_t	tmr_play_speed;
	bool		play;
	uint8_t		play_speed;

	/* config mode */
	uint16_t	led_pattern;
	uint16_t 	led_pattern_index;
} tp_glb;

extern tp_glb glb;
extern tp_conf glb_conf;


#endif /* GLB_COMMON_H_ */
