/**
 * Description:
 * This project is for controlling an RGB controller with an Arduino with ESP8266.
 * The ESP8266 needs to be compatible with 9600 baud rate.
 * The RGB controller can be any P9813 compatible controller.
 *
 * Details:
 *
 * Configuration mode
 * ----
 * When PIN_ENTER_CFG is shorted then the board gets in configuration mode.
 * In the configuration mode the ESP8266 is an AP. You need to connect to
 * the AP and use a UDP client to send some commands to configure the SSID
 * and the password for your router.
 *
 * When you're connected in the ESP8266
 * AP then you need to send the commands to the following address and port:
 * Address: 192.168.4.255
 * Port: 7700
 *
 * The supported commands in the configuration mode are the following:
 *
 * SSID_NAME=<your router's SSID>
 *  - The SSID of the router that ESP8266 will connect
 *
 * SSID_PASS=<your_router's password>
 *  - sets the password needed for the ESP8266 to connect to your router
 *
 * RESET
 * 	- resets the ESP8266 and the board
 *
 * Controller mode
 * ----
 * When PIN_ENTER_CFG is unconnected then the board reads the EEPROM for a
 * valid configuration and then tries to connect to the router using the
 * SSID and password from the EEPROM. When it's connected then the LED
 * toggles every 1 sec. Every 5 mins the connection is tested and if the
 * ESP8266 is not connected then it tries to re-connect.
 *
 * The ESP8266 binds to the UDP port 7700 and waits for valid commands. The
 * commands supported are the following:
 *
 * RGB=<Red>,<Green>,<Blue>
 *  - sets the RGB value in the controller
 *  - example: RGB=100,20,255
 *
 * CMD=<command>
 *  - Send a command
 *  <command>:
 *  	R+ : increases the red value (example: "CMD=R+")
 *  	R- : decreases the red value
 *  	G+ : increases the green value
 *  	G- : decreases the green value
 *  	B+ : increases the blue value
 *  	B- : decreases the blue value
 *  	DIM+ : dim up the brightness (example: "CMD=DIM+")
 *  	DIM- : dim down the brightness
 *  	PLAY : starts the color play pattern begining from a random value (example: "CMD=PLAY")
 *  	QUICK : when in play mode fastens the change rate
 *  	SLOW : when in play mode slowers the change rate
 *
 *
 * ESP8266_SoftwareSerial - https://github.com/Circuito-io/ESP8266_SoftwareSerial
 */

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "ESP8266.h"
#include "TimerOne.h"
#include "conf_ap.h"
#include "config.h"
#include "udp_client.h"
#include "colors.h"

#include "glb_common.h"
#include "eeprom_conf.h"


/* Create a soft serial port */
SoftwareSerial wifi_uart(10, 11);
/* Create ESP8266 object */
ESP8266 wifi(wifi_uart);
/* Create EEPROM */
EERPOM_conf eeprom;
/* Configuration/EEPROM obj */
Config *cfg_conf = NULL;
/* UDP client obj */
UDP_Client * udp = NULL;

RGB_Colors glb_rgb;

tp_glb glb;
tp_conf glb_conf;

UDP_Client::en_ret_code udp_resp;


/* 1 sec interrupt */
void timer1ms_IRQ()
{
	if ((glb.tmr_250ms++) >= (250/TMR_INT_MS)) {
		glb.tmr_250ms = 0;
		update_led();
	}
	if ((glb.tmr_1000ms++) >= (1000/TMR_INT_MS)) {
		glb.tmr_1000ms = 0;
		glb.tmr_check_wifi++;
	}

	/* Play random colors */
	if (glb.play) {
		if ((glb.tmr_play_speed++) >= (50/TMR_INT_MS)) {
			glb.tmr_play_speed = 0;
			glb_rgb.fluctuate(glb.play_speed);
//			glb_rgb.print_rgb();
		}
	}
}


//The setup function is called once at startup of the sketch
void setup() {
	// Add your initialization code here
	glb.led_pattern_index = 0;
	glb.led_pattern = LED_PATTERN_HEARTBEAT;
	glb.tmr_1000ms = 0;
	glb.tmr_250ms = 0;
	glb.tmr_1ms = 0;
	glb.tmr_check_wifi = 0;
	glb.play = false;
	glb.play_speed = 1;
	glb.tmr_play_speed = 0;

	/* Create random seed */
	randomSeed(analogRead(0));

	/* Start serial port */
	Serial.begin(9600);
	while (!Serial);
	Serial.print(F("RGB Wifi controller v.0.1\n"));

	/* Create conf */
	cfg_conf = new Config(EEPROM_CONF_HEADER, EEPROM_CONF_VERSION,
			EEPROM_CONF_POS, (uint8_t*) &glb_conf, sizeof(tp_conf));

	/* Load default EEPROM values */
	tp_conf tmp_conf;
	memset(tmp_conf.wifi_ssid, 0, WIFI_SSID_LEN);
	memset(tmp_conf.wifi_passwd, 0, WIFI_PASSWD_LEN);
	tmp_conf.udp_port = 7700;

	/* Set default values */
	cfg_conf->SetDefaultValues((uint8_t*) &tmp_conf, sizeof(tp_conf));
	/* Load configuration */
	cfg_conf->Load();

	/* Init timer */
	Timer1.initialize(50000); //TMR_INT_MS * 1000
	Timer1.attachInterrupt(timer1ms_IRQ);

	/* Init LED */
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_ENTER_CFG, INPUT_PULLUP);

	/* Check which mode to load */
	if (digitalRead(PIN_ENTER_CFG) == LOW) {
		Serial.println(F("Loading conf AP"));
		Conf_AP cfg_ap(&wifi, cfg_conf);
		Conf_AP::en_ret_code ret = cfg_ap.begin();
		if (ret != Conf_AP::RET_OK) {
			Serial.print(F("Conf_AP failed with code: "));
			Serial.println(ret);
			glb.led_pattern = LED_PATTERN_CFG_ERR;
			/* Lock in playing the LED pattern */
			while(1) {
				update_led();
			}
		}
	}

	/* Connect UDP client */
	connect_udp_client();
}

/**
 * The loop function is called in an endless loop
 */
void loop() {
	//Add your repeated code here
	uint16_t data_len = udp->check_data();
	if (data_len) {
		udp_parser(data_len);
	}

	/* Every 5 mins check status */
	if (glb.tmr_check_wifi >= 300) {
		glb.tmr_check_wifi = 0;
		if (!check_udp_status()) {
			/* Try to reconnect to UDP socket */
			Serial.println(F("reconnect"));
			connect_udp_client();
		}
	}
}

bool check_udp_status(void)
{
	bool status = wifi.ping("192.168.0.1");
	bool status2 = false;

	String cip_status = wifi.getIPStatus();

	if (cip_status.indexOf("CIPSTATUS:0,\"UDP\",\"192.168")) {
		status2 = true;
	}
	else {
		Serial.println(F("Status disconnected."));
		status2 = false;
	}

	return(status & status2);
}

bool connect_udp_client(void)
{
	bool connected = true;
	String s_ssid = String(glb_conf.wifi_ssid);
	String s_passwd = String(glb_conf.wifi_passwd);
	Serial.print(F("Connecting to: "));
	Serial.print(s_ssid);
	Serial.print(F(" with "));
	Serial.println(s_passwd);

	/* Create a new UDP client */
	if (udp == NULL) {
		udp = new UDP_Client(&wifi, s_ssid,
				s_passwd, glb_conf.udp_port);
	}
	udp_resp = udp->begin();
	if (udp_resp != UDP_Client::RET_OK) {
		Serial.print(F("UDP_Client failed with code: "));
		Serial.println(udp_resp);
		glb.led_pattern = LED_PATTERN_CFG_ERR;
		connected = false;
	}
	return(connected);
}

void udp_parser(uint16_t data_len)
{
	uint8_t * udp_buff = udp->get_buffer();
	udp_buff[data_len] = 0;

	Serial.print(F("[UDP]: "));
	Serial.println((char*) udp_buff);

	if (!strncmp((char*) udp_buff, "RGB=", 4)) {
		char * p;
		const char token = ',';

		/* Get RGB color values  */
		p = strtok((char*) &udp_buff[4], &token);
		glb_rgb.set_R(atoi(p));
		p = strtok(NULL, &token);
		glb_rgb.set_G(atoi(p));
		p = strtok(NULL, &token);
		glb_rgb.set_B(atoi(p));
		glb_rgb.update();

		glb_rgb.print_rgb();
	}
	else if (!strncmp((char*) udp_buff, "CMD=", 4)) {
		char * p = (char*) &udp_buff[4];
		Serial.print(p);
		if (!strncmp(p, "OFF", 3)) {
			glb_rgb.reset();
		}
		else if (!strncmp(p, "R+", 2)) {
			glb_rgb.dim(RGB_Colors::R, RGB_Colors::DIM_UP, RGB_Colors::DIM_COLOR_DEF);
		}
		else if (!strncmp(p, "R-", 2)) {
			glb_rgb.dim(RGB_Colors::R, RGB_Colors::DIM_DN, RGB_Colors::DIM_COLOR_DEF);
		}
		else if (!strncmp(p, "G+", 2)) {
			glb_rgb.dim(RGB_Colors::G, RGB_Colors::DIM_UP, RGB_Colors::DIM_COLOR_DEF);
		}
		else if (!strncmp(p, "G-", 2)) {
			glb_rgb.dim(RGB_Colors::G, RGB_Colors::DIM_DN, RGB_Colors::DIM_COLOR_DEF);
		}
		else if (!strncmp(p, "B+", 2)) {
			glb_rgb.dim(RGB_Colors::B, RGB_Colors::DIM_UP, RGB_Colors::DIM_COLOR_DEF);
		}
		else if (!strncmp(p, "B-", 2)) {
			glb_rgb.dim(RGB_Colors::B, RGB_Colors::DIM_DN, RGB_Colors::DIM_COLOR_DEF);
		}
		else if (!strncmp(p, "DIM+", 4)) {
			glb_rgb.set_brightness(RGB_Colors::DIM_UP, 10.0);
		}
		else if (!strncmp(p, "DIM-", 4)) {
			glb_rgb.set_brightness(RGB_Colors::DIM_DN, 10.0);
		}
		else if (!strncmp(p, "PLAY", 4)) {
			if (glb.play) {
				Serial.println(F("Disable play"));

				byte random_val = random(255);
				glb_rgb.set_R(random_val);

				random_val = random(255);
				glb_rgb.set_G(random_val);

				random_val = random(255);
				glb_rgb.set_B(random_val);
				glb.play = false;
			}
			else {
				Serial.println(F("Enable play"));
				glb.play = true;
			}
		}
		else if (!strncmp(p, "QUICK", 5)) {
			play_speed(5);
		}
		else if (!strncmp(p, "SLOW", 4)) {
			play_speed(-5);
		}
	}
}

void play_speed(int value)
{
	int new_value = 1;

	new_value = glb.play_speed;
	new_value += value;
	if (new_value > 255)
		glb.play_speed = 255;
	else if (new_value < 1)
		glb.play_speed = 1;
	else
		glb.play_speed = (byte) new_value;
}

/**
 * Update the status LED with the current pattern
 */
void update_led(void)
{
	if (glb.led_pattern & (1 << glb.led_pattern_index) )
		digitalWrite(PIN_LED, HIGH);
	else
		digitalWrite(PIN_LED, LOW);
	if ((++glb.led_pattern_index) == 8)
		glb.led_pattern_index = 0;
}

/**
 * Do timers updates
 */
void update_timers(void)
{
	if (glb.tmr_1ms) {
		glb.tmr_1ms = 0;
	}
	if (glb.tmr_1000ms >= 1000) {
		glb.tmr_1000ms = 0;
	}
}



