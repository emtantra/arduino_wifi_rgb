# ard_wifi_rgb
WIFI controlled RGB strip with arduino mini

### Description:
----
This project is for controlling an RGB controller with an Arduino with ESP8266. The ESP8266 needs to be compatible with 9600 baud rate. The RGB controller can be any P9813 compatible controller.

### Details:
----

##### Configuration mode
When PIN_ENTER_CFG is shorted then the board gets in configuration mode. In the configuration mode the ESP8266 is an AP. You need to connect to the AP and use a UDP client to send some commands to configure the SSID and the password for your router.

When you're connected in the ESP8266
AP then you need to send the commands to the following address and port:
Address: 192.168.4.255
Port: 7700

The supported commands in the configuration mode are the following:

```SSID_NAME=<your router's SSID>```
- The SSID of the router that ESP8266 will connect

```SSID_PASS=<your_router's password>```
- Sets the password needed for the ESP8266 to connect to your router

```RESET```
- resets the ESP8266 and the board

##### Controller mode
When PIN_ENTER_CFG is unconnected then the board reads the EEPROM for a valid configuration and then tries to connect to the router using the SSID and password from the EEPROM. When it's connected then the LED toggles every 1 sec. Every 5 mins the connection is tested and if the ESP8266 is not connected then it tries to re-connect.

The ESP8266 binds to the UDP port 7700 and waits for valid commands. The commands supported are the following:

```RGB=<Red>,<Green>,<Blue>```
- sets the RGB value in the controller
- example: ```RGB=100,20,255```

```CMD=<command>```
- Sends a command

```
where <command>:
 	R+ : increases the red value (example: "CMD=R+")
 	R- : decreases the red value
 	G+ : increases the green value
 	G- : decreases the green value
 	B+ : increases the blue value
 	B- : decreases the blue value
 	DIM+ : dim up the brightness (example: "CMD=DIM+")
 	DIM- : dim down the brightness
 	PLAY : starts the color play pattern begining from a random value (example: "CMD=PLAY")
 	QUICK : when in play mode fastens the change rate
 	SLOW : when in play mode slowers the change rate
```

Web server files
----
The www/rgb folder containg a demo javascript/php web service for the application. You need to copy these files in a web server to your local network (e.g. a raspberry pi) and then edit the functions.php file and edit the ```$remote_ip``` to point to the correct IP address of the ESP8266. It's better to set your router to assign always the same IP on the ESP8266 MAC address. After you copy the rgb folder from www to your web server you can access the page and controll the device. If you need to add/remove button or change the default colors, then you need to edit the rgb_buttons.json file. The html will parse this file to create the buttons. On the color change button you may need to trim the colour values for your led strip. The "color" field is the color that is displayed on the html and the "data" field is the color that is sent to the board. These color usually are not the same.

If you need to change the buttons or font size to fit your mobile/tablet screen better, then change the ```font-size: 42px;```, ```height: 200px;``` and ```width: 200px;``` in the index.html file.