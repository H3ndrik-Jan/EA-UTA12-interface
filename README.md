# EA-UTA12-interface
Make your own interface for Elektro Automatik power supplies using the UTA 12 interface port, and log data to a MySQL server.

## Overview
This project is about an interface to the UTA 12 interface, found om Elektro Automatik power supplies.
A PCB featuring an ESP32 reads the data from the UTA 12 interface and sends it to the SQL-server, using the MySQL_Connector_Arduino library by ChuckBell (https://github.com/ChuckBell/MySQL_Connector_Arduino). An MCP3204 ADC is used to read the analog voltages from the interface, and for communication with the ADC the mcp320x library by labfruits is used (https://github.com/labfruits/mcp320x). The u8g2-library by olikraus is used to print the accurate results to the OLED display (https://github.com/olikraus/u8g2).

The data can be accessed through a web-page run on apache2. A line graph made using chart.js shows the current and voltage. The amount of results can be set to the wished amount. Also, the power supply can be set in standby or external mode using the web interface.
I made this because I am a poor student who can't afford the original interface, sold by Elektro Automatik (https://elektroautomatik.com/shop/de/produkte/digitale-schnittstellen/usb-to-analog-uta12/412/uta-interface).

![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/on.jpg)

## Details
There is an 15 pin analog interface available on numerous Elektro Automatik devices. The interface can be used to read all the things which can also be read on the frontpanel of the power supply. Additionally though, you can get a more precise voltage and current reading, as this allows for extra decimal numbers for the readings.
Even more functionality of the interface can be found in that it can be used to set both voltage and current-limit, and can also put the power supply in standby mode, which will turn off the output of the power supply. This feature can't be controlled using the front panel.

Reading and setting the voltage and current(-limit) of the power supply is done by reading or supplying a voltage varying from 0 to 10 volts. This voltage can be mapped to the output current and voltage of the power supply.


The database and server hosting the web-interface for this project is installed on an Ubuntu virtual machine, running in Hyper-V. The virtual machine is running on my desktop pc. This is a good solution for me as I don't need the interface 24-7, and this enables me to quickly turn on and off the whole system. When I start the virtual machine, the web-interface and MySQL start automatically as well, and everything is up and running within seconds.

Off course, MySQL is installed on the system. To make managing and setting up the database easy, I used phpMyAdmin.
I made a database named 'labpsu', in which are 2 tables. The first is the 'log', table in which all logged data from the device is stored. You can see a screenshot of the log table and its settings from phpMyAdmin under here:
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/logtable.PNG)

The second table is 'settings'. This table has only one row, and is used to store the settings for the device in. Again, a picture of the settings under here:
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/settingstable.PNG)

## Photos of the device
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/on.jpg)
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/back.jpg)

![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/PCB.jpg)
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/Housing.jpg)

## Screenshot of the web-interface
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/server/Screenshot.PNG)
