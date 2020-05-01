# EA-UTA12-interface
Make your own interface for Elektro Automatik power supplies using the UTA 12 interface port, and log data to a MySQL server.

## Overview
This project is about an interface to the UTA 12 interface, found om Elektro Automatik power supplies.
A PCB featuring an ESP32 reads the data from the UTA 12 interface and sends it to the SQL-server, using the MySQL_Connector_Arduino library by ChuckBell (https://github.com/ChuckBell/MySQL_Connector_Arduino). An MCP3204 ADC is used to read the analog voltages from the interface, and for communication with the ADC the mcp320x library by labfruits is used (https://github.com/labfruits/mcp320x). The u8g2-library by olikraus is used to print the accurate results to the OLED display (https://github.com/olikraus/u8g2).

The data can be accessed through a web-page run on apache2. A line graph made using chart.js shows the current and voltage. The amount of results can be set to the wished amount. Also, the power supply can be set in standby or external mode using the web interface.
I made this because I am a poor student who can't afford the original interface, sold by Elektro Automatik (https://elektroautomatik.com/shop/de/produkte/digitale-schnittstellen/usb-to-analog-uta12/412/uta-interface). 

## Photo of device
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/on.jpg)
## Screenshot of interface
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/software/Screenshot.PNG)
