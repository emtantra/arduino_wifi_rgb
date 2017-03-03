/**
 * Libarires used:
 *
 * ESP8266_SoftwareSerial - https://github.com/Circuito-io/ESP8266_SoftwareSerial
 */

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "ESP8266.h"
#include "ChainableLED.h"
#include "TimerOne.h"

#include "common.h"
#include "eeprom_conf.h"

#define PIN_LED	13
#define PIN_RGB_CLK 5
#define PIN_RGB_DAT	6
#define PIN_ENTER_CFG 4

/* Create a soft serial port */
SoftwareSerial wifi_uart(10, 11);
/* Create ESP8266 object */
ESP8266 wifi(wifi_uart);
/* Create EEPROM */
EERPOM_conf eeprom;
/* Configuration/EEPROM obj */
Conf *cfg_conf;
/* UDP client obj */
UDP_Client * udp;


tp_glb glb;
tp_conf glb_conf;

/* 1 sec interrupt */
void timer1ms_IRQ()
{
	glb.tmr_1ms++;
	glb.tmr_100ms++;
	glb.tmr_1000ms++;
}


//The setup function is called once at startup of the sketch
void setup() {
	// Add your initialization code here
	glb.led_pattern_index = 0;
	glb.led_pattern = LED_PATTERN_HEARTBEAT;
	glb.tmr_1000ms = 0;
	glb.tmr_100ms = 0;
	glb.tmr_1ms = 0;

	/* Start serial port */
	Serial.begin(9600);
	while (!Serial);

	/* Init timer */
	Timer1.initialize(1000);
	Timer1.attachInterrupt(timer1ms_IRQ);

	/* Init LED */
	pinMode(PIN_LED, OUTPUT);
	glb.ledstate = LOW;

	/* Create conf */
	cfg_conf = new Conf(EEPROM_CONF_HEADER, EEPROM_CONF_VERSION, EEPROM_CONF_POS, (uint8_t*)&glb_conf, sizeof(tp_conf));
	/* Load default EEPROM values */
	tp_conf tmp_conf;
	memset(tmp_conf.wifi_ssid, 0, WIFI_SSID_LEN);
	memset(tmp_conf.wifi_passwd, 0, WIFI_PASSWD_LEN);
	tmp_conf.udp_port = 7700;

	/* Set default values */
	cfg_conf->SetDefaultValues((uint8_t*) &tmp_conf, sizeof(tp_conf));
	/* Load configuration */
	cfg_conf->Load();

	/* Init RGB led driver */
	glb.rgb = new ChainableLED(PIN_RGB_CLK, PIN_RGB_DAT, 1);
	glb.rgb->init();
	glb.colors.r = 0;
	glb.colors.g = 0;
	glb.colors.b = 0;
	update_colors();

	Serial.print("RGB Wifi controller v.0.1\n");

	/* Check which mode to load */
	pinMode(PIN_ENTER_CFG, INPUT_PULLUP);
	if (digitalRead(PIN_ENTER_CFG) == LOW) {
		Conf_AP cfg_ap(wifi);
		Conf_AP::en_ret_code ret = cfg_ap.begin();
		if (ret != RET_OK) {
			Serial.print("Conf_AP failed with code: ");
			Serial.writeln(ret);
			glb.led_pattern = LED_PATTERN_CFG_ERR;
			/* Lock in playing the LED pattern */
			while(1) {
				update_led();
			}
		}
	}

	/* Create a new UDP client */
	udp = new UDP_Client(wifi, glb_conf.wifi_ssid,
			glb_conf.wifi_passwd, glb_conf.udp_port);

}

/**
 * The loop function is called in an endless loop
 */
void loop() {
	//Add your repeated code here
	update_led();
	update_timers();
}

/**
 * Update the status LED with the current pattern
 */
void update_led(void)
{
	if (glb.tmr_250ms >= 250) {
		glb.tmr_250ms = 0;
		if (glb.led_pattern & (1 << glb.led_pattern_index) )
			digitalWrite(PIN_LED, HIGH);
		else
			digitalWrite(PIN_LED, LOW);
		if ((++glb.led_pattern_index) == 8)
			glb.led_pattern_index = 0;
	}
}

/**
 * Do timers updates
 */
void update_timers(void)
{
	if (glb.tmr_1ms) {
		glb.tmr_1ms = 0;
		glb.colors.r++;
		glb.colors.g++;
		glb.colors.b++;
		update_colors();
	}
	if (glb.tmr_1000ms >= 1000) {
		glb.tmr_1000ms = 0;
	}
}


/**
 * Update the LED strip colors
 */
void update_colors(void)
{
	glb.rgb->setColorRGB(0, glb.colors.r, glb.colors.g, glb.colors.b);
}


