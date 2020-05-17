# EA-UTA12-interface
An DIY interface for Elektro Automatik power supplies using the UTA 12 interface port; Control/log your PSU with MySql!

## Overview
This project is about an interface to the UTA 12 interface, found om Elektro Automatik power supplies.
A PCB featuring an ESP32 reads the data from the UTA 12 interface and sends it to the SQL-server, using the MySQL_Connector_Arduino library by ChuckBell (https://github.com/ChuckBell/MySQL_Connector_Arduino). An MCP3204 ADC is used to read the analog voltages from the interface, and for communication with the ADC the mcp320x library by labfruits is used (https://github.com/labfruits/mcp320x). The u8g2-library by olikraus is used to print the accurate results to the OLED display (https://github.com/olikraus/u8g2).

The data can be accessed through a web-page run on apache2. A line graph made using chart.js shows the current and voltage. The amount of results can be set to the wished amount. Also, the power supply can be set in standby or external mode using the web interface.
I made this because I am a poor student who can't afford the original interface, sold by Elektro Automatik (https://elektroautomatik.com/shop/de/produkte/digitale-schnittstellen/usb-to-analog-uta12/412/uta-interface).

![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/on.jpg)
Look underneath for more pictures.

## Details
There is an 15 pin analog interface available on numerous Elektro Automatik devices. The interface can be used to read all the things which can also be read on the frontpanel of the power supply. Additionally though, you can get a more precise voltage and current reading, as this allows for extra decimal numbers for the readings.
Even more functionality of the interface can be found in that it can be used to set both voltage and current-limit, and can also put the power supply in standby mode, which will turn off the output of the power supply. This feature can't be controlled using the front panel.
A schematic diagram of the internal connections, taken from the datasheet of the power supply, can be seen here:
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/Internal.PNG)

Reading and setting the voltage and current(-limit) of the power supply is done by reading or supplying a voltage varying from 0 to 10 volts. This voltage can be mapped to the output current and voltage of the power supply. The other outputs are open collector outputs, which are left 'floating' when not active, and are tied to ground when active. They can be pulled up by a high value resistor to easily read the state of the pin. The inputs can be set by tying the pins to GND, and are disabled when left floating.

I made a board based on the ESP-Wroom-32 to enable communicating with the power supply from other devices. The logging goes over wifi, so no connection with a computer is necessary. Power over USB is necessary. On the board is a USB-to-serial converter to communicate with the ESP32. There is a 12-bit ADC to read the analog inputs from the power supply. An opamp is used to amplify the output of the ESP32 internal 8 bit DAC to a 0-10 Volts. A -5V supply is available so the DAC can operate to 0 volts. The measurements are printed on a 0.96inch OLED display. Also, there is an LED on the board so the user can see if there is a connection with the SQL-database. A push button is used to turn the WiFi on and off. Network settings for the device, such as network SSID's and passwords, can be given over the serial port. The schematics can be found in this repository. The board basically consists of an ESP32 as the mcu, with an CH340G USB to UART bridge. The ADC is an MCP3204, which has good linearity which is obviously important in this case. The ADC is spoken to over SPI via an TXS0104 voltage level translator, as the ADC is powered with 5 volts. An TP2272 dual opamp is used to amplify the ESP32's internal DAC outputs to a maximum of 10 volts. The opamp is powered by the VCC bus provided by the UTA12, and -5 volts made with the USB 5 volt bus using an isolated DC-DC converter.

The database and server hosting the web-interface for this project is installed on an Ubuntu virtual machine, running in Hyper-V. The virtual machine is running on my desktop pc. This is a good solution for me as I don't need the interface 24-7, and this enables me to quickly turn on and off the whole system. When I start the virtual machine, the web-interface and MySQL start automatically as well, and everything is up and running within seconds.

Off course, MySQL is installed on the system. To make managing and setting up the database easy, I used phpMyAdmin.
I made a database named 'labpsu', in which are 2 tables. The first is the 'log', table in which all logged data from the device is stored. You can see a screenshot of the log table and its settings from phpMyAdmin under here:
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/logtable.PNG)

The second table is 'settings'. This table has only one row, and is used to store the settings for the device in. Again, a picture of the settings under here:
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/settingstable.PNG)

The web interface is basically a simple php script with a simple HTML lay-out. The power supplies settings can be set, and the update frequency and number of results on the chart can be set. Using Chart.js a line graph with the measured voltage and current is drawn. The webserver runs on apache2.

## Screenshot of the web-interface
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/Screenshot.PNG)
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/menu.PNG)

## More photos of the device
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/on.jpg)
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/back.jpg)

![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/PCB.jpg)
![alt text](https://github.com/H3ndrik-Jan/EA-UTA12-interface/blob/master/pictures/Housing.jpg)

Anyways while doing this project I learned a lot of things, because I never used MySql before, and also never really made a web interface with php, javascript and apache2 before. On the electronics side, it was very educational 'hacking' an interface of a professional device.
