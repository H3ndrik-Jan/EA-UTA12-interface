# EA-UTA12-interface
Make your own interface for Elektro Automatik power supplies using the UTA 12 interface port.

## Overview
This project is about an interface to the UTA 12 interface, found om Elektro Automatik power supplies.
A PCB featuring an ESP32 reads the data from the UTA 12 interface and logs it to an SQL-server. 
The data can be accessed through a web-page run on apache2. Also, settings can be set using the html-page, and are sent back to the ESP32. 
I made this because I am a poor student who can't afford the original interface, sold by Elektro Automatik (https://elektroautomatik.com/shop/de/produkte/digitale-schnittstellen/usb-to-analog-uta12/412/uta-interface).

## How does it work
