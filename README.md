# EA-UTA12-interface
Make your own interface for Elektro Automatik power supplies using the UTA 12 interface port, and log data to a MySQL server.

## Overview
This project is about an interface to the UTA 12 interface, found om Elektro Automatik power supplies.
A PCB featuring an ESP32 reads the data from the UTA 12 interface and sends it to the SQL-server, using the MySQL_Connector_Arduino library by ChuckBell (https://github.com/ChuckBell/MySQL_Connector_Arduino).

The data can be accessed through a web-page run on apache2. A line graph made using chart.js shows the current and voltage. The amount of results can be set to the wished amount. Also, the power supply can be set in standby or external mode using the web interface.
I made this because I am a poor student who can't afford the original interface, sold by Elektro Automatik (https://elektroautomatik.com/shop/de/produkte/digitale-schnittstellen/usb-to-analog-uta12/412/uta-interface). 

## Screenshot of interface
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/Screenshot.PNG)
