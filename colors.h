/*
 * colors.h
 *
 *  Created on: 5 Mar 2017
 *      Author: dimtass
 */

#ifndef COLORS_H_
#define COLORS_H_

#include "Arduino.h"
#include "ChainableLED.h"
#include "glb_common.h"


class RGB_Colors
{
public:

	typedef enum {
		DIM_COLOR_DEF = 10
	} en_dim_color_def_value;

	typedef enum {
		DIM_BRIGHT_DEF = 5
	} en_dim_bright_def_value;

	typedef enum {
		R = 0,
		G,
		B
	} en_color;

	typedef enum {
		DIM_DN = 0,
		DIM_UP
	} en_dim;

	RGB_Colors();
	virtual ~RGB_Colors();
	void reset(void);
	void set_R(uint8_t value);
	void set_G(uint8_t value);
	void set_B(uint8_t value);
	byte get_R(void);
	byte get_G(void);
	byte get_B(void);
	void print_rgb(void);
	void update(void);
	void set_brightness(RGB_Colors::en_dim up_dn, float value);
	void dim(en_color color, en_dim up_dn, uint8_t value);
	void fluctuate(byte value);

private:
	byte dim_up(byte value, byte up_value);
	byte dim_dn(byte value, byte dn_value);

	uint8_t		m_colors[3];
	bool		m_inc[3];
	float		m_brightness;
	ChainableLED *m_driver;
};



#endif /* COLORS_H_ */
