/**
 * Libarires used:
 *
 * ESP8266_SoftwareSerial - https://github.com/Circuito-io/ESP8266_SoftwareSerial
 */

#include "Arduino.h"
#include "config.h"
#include "SoftwareSerial.h"
#include "ESP8266.h"
#include "ChainableLED.h"
#include "TimerOne.h"
#include "eeprom_conf.h"

#define PIN_LED	13
#define PIN_RGB_CLK 5
#define PIN_RGB_DAT	6

/* Create a soft serial port */
SoftwareSerial wifi_uart(10, 11);
/* Create ESP8266 object */
ESP8266 wifi(wifi_uart);
/* Create EEPROM */
EERPOM_conf eeprom;

tp_glb glb;

/* 1 sec interrupt */
void timer1ms_IRQ()
{
	glb.tmr_1ms++;
	glb.tmr_100ms++;
	glb.tmr_1000ms++;
}

const char *SSID     = "DTLB";
const char *PASSWORD = "RTFMYFM78";

//The setup function is called once at startup of the sketch
void setup() {
	// Add your initialization code here

	/* Start serial port */
	Serial.begin(9600);
	while (!Serial);

	/* Init timer */
	Timer1.initialize(1000);
	Timer1.attachInterrupt(timer1ms_IRQ);

	/* Init LED */
	pinMode(PIN_LED, OUTPUT);
	glb.ledstate = LOW;

	/* Init RGB led driver */
	glb.rgb = new ChainableLED(PIN_RGB_CLK, PIN_RGB_DAT, 1);
	glb.rgb->init();
	randomSeed(analogRead(0));
	glb.colors.r = random(255);
	glb.colors.g = random(255);
	glb.colors.b = random(255);
	update_colors();

	/* Load EEPROM */
//	EERPOM_conf::tp_eeprom_conf conf = eeprom.get();
//	if (strlen(conf.ssid) > 2 ) {
//		/* Enable WiFi */
//		wifi.registerUDP(addr, port);
//	}

//	bool repeat = true;
//
//	wifi_uart.begin(9600);
//	while(repeat) {
//		wifi_uart.println("AT");
//		delay(500);
//	}


	delay(2000);
	Serial.print("RGB Wifi controller v.0.1\n");

	if (wifi.autoSetBaud(9600))
		Serial.println("Baudrate set success");
	else {
		Serial.println("Baudrate set failed");
	}

	//Setting operation mode to Station + SoftAP
	if (wifi.setOprToStation())
		Serial.println("Station + softAP - OK");
	else
	{
		Serial.println("Station + softAP - Error, Reset Board!");
	}

	if (wifi.joinAP(SSID, PASSWORD))
	{
		Serial.print("Joining AP successful, ");
		Serial.println(wifi.getLocalIP().c_str());
	}
	else
	{
		Serial.println("Join AP failure, Reset Board!");
	}

	if (wifi.disableMUX())
	{
		Serial.println("Single Mode OK");
	}
	else
	{
		Serial.println("Single Mode Error, Reset Board!");
	}

	String myip = wifi.getLocalIP();
	Serial.print("My IP: "); Serial.println(myip);
}

// The loop function is called in an endless loop
void loop() {
	//Add your repeated code here
	update_timers();
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
	if (glb.tmr_100ms >= 100) {
		glb.tmr_100ms = 0;
	}
	if (glb.tmr_1000ms >= 1000) {
		glb.tmr_1000ms = 0;

		if (glb.ledstate)
			glb.ledstate = LOW;
		else
			glb.ledstate = HIGH;
		digitalWrite(PIN_LED, glb.ledstate);
//		Serial.print("1 sec\n");

		//update_colors();
	}
}


/* Update the LED strip colors */
void update_colors(void)
{
	glb.rgb->setColorRGB(0, glb.colors.r, glb.colors.g, glb.colors.b);
}


