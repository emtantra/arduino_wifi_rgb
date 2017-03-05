/*
 * colors.cpp
 *
 *  Created on: 5 Mar 2017
 *      Author: dimtass
 */
#include "colors.h"

RGB_Colors::RGB_Colors()
{
	/* Init RGB led driver */
	m_driver = new ChainableLED(PIN_RGB_CLK, PIN_RGB_DAT, 1);
	m_driver->init();
	m_brightness = 100.0;
	reset();
}

RGB_Colors::~RGB_Colors()
{
	delete m_driver;
}

void RGB_Colors::reset(void)
{
	int i = 0;
	for (i=0; i<=B; i++) {
		m_colors[i] = 0;
		m_inc[i] = true;
	}
	update();
}


void RGB_Colors::set_R(byte value)
{
	m_colors[R] = value;
}

void RGB_Colors::set_G(byte value)
{
	m_colors[G] = value;
}

void RGB_Colors::set_B(byte value)
{
	m_colors[B] = value;
}

byte RGB_Colors::get_R(void)
{
	return(m_colors[R]);
}

byte RGB_Colors::get_G(void)
{
	return(m_colors[G]);
}

byte RGB_Colors::get_B(void)
{
	return(m_colors[B]);
}

void RGB_Colors::print_rgb(void)
{
	Serial.print("r:");Serial.print(m_colors[R]);Serial.print(",");
	Serial.print("g:");Serial.print(m_colors[G]);Serial.print(",");
	Serial.print("b:");Serial.println(m_colors[B]);
}

/**
 * Update the LED strip colors
 */
void RGB_Colors::update(void)
{
	byte r = m_colors[R] * (float)(m_brightness/100.0);
	byte g = m_colors[G] * (float)(m_brightness/100.0);
	byte b = m_colors[B] * (float)(m_brightness/100.0);
	m_driver->setColorRGB(0, r, g, b);
}

void RGB_Colors::set_brightness(en_dim up_dn, float value)
{
	if (up_dn == DIM_UP) {
		m_brightness += value;
		if (m_brightness >= 100.0)
			m_brightness = 100.0;
	}
	else if (up_dn == DIM_DN) {
		m_brightness -= value;
		if (m_brightness <= 0)
			m_brightness = 0;
	}
	update();
}

void RGB_Colors::dim(en_color color, en_dim up_dn, uint8_t value)
{
	if (up_dn == DIM_UP) {
		if (color == R) {
			m_colors[R] = dim_up(m_colors[R], value);
		}
		else if (color == G) {
			m_colors[G] = dim_up(m_colors[G], value);
		}
		else if (color == B) {
			m_colors[B] = dim_up(m_colors[B], value);
		}
	}
	else if (up_dn == DIM_DN) {
		if (color == R) {
			m_colors[R] = dim_dn(m_colors[R], value);
		}
		else if (color == G) {
			m_colors[G] = dim_dn(m_colors[G], value);
		}
		else if (color == B) {
			m_colors[B] = dim_dn(m_colors[B], value);
		}
	}
	update();
}

byte RGB_Colors::dim_up(byte value, byte up_value)
{
	int new_val = 0;

	new_val = value + up_value;
	if (new_val > 0xff)
		new_val = 0xff;
	Serial.print(value);
	Serial.print(" -> ");
	Serial.println(new_val);

	return((byte)new_val);
}

byte RGB_Colors::dim_dn(byte value, byte dn_value)
{
	int new_val = 0;

	new_val = value - dn_value;
	if (new_val < 0)
		new_val = 0;
	Serial.print(value);
	Serial.print(" -> ");
	Serial.println(new_val);

	return((byte)new_val);
}

void RGB_Colors::fluctuate(byte value)
{
	int i = 0;
	int prev = 0;
	int new_val = 0;
	for (i=R; i<=B; i++) {
		prev = m_colors[i];
		if (m_inc[i]) {
			new_val = m_colors[i]+value;
			if (new_val >= 255) {
				m_inc[i] = false;
				new_val = 255;
			}
		}
		else {
			new_val = m_colors[i]-value;
			if (new_val <= 0) {
				m_inc[i] = true;
				new_val = 0;
			}
		}
		m_colors[i] = (byte) new_val;
	}
	update();
}
