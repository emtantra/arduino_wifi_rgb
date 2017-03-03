/*
 * config.h
 *
 *  Created on: 4 Feb 2017
 *      Author: dimtass
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define ESP8266_USE_SOFTWARE_SERIAL

#include "ChainableLED.h"

typedef struct {
	byte	r;
	byte	g;
	byte	b;
} tp_colors;

typedef struct {
	/* timers */
	uint16_t	tmr_1ms;
	uint8_t		tmr_100ms;
	uint16_t	tmr_1000ms;
	/* rgb */
	uint8_t		ledstate;
	ChainableLED *rgb;
	tp_colors	colors;
} tp_glb;

extern tp_glb glb;


#endif /* CONFIG_H_ */
