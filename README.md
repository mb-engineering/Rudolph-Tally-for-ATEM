# Rudolph
Tally for ATEM switcher based on ESP8266
 
The project is based on the work of

https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering <br>
https://github.com/tzapu/WiFiManager <br>
https://oneguyoneblog.com/2020/06/13/tally-light-esp32-for-blackmagic-atem-switcher/

Thanks for the fundamentals!

To make it work with Constellation 8K you have to make a change in one of the SKAARHOJ libraries.



version 4
----------------------------
In version 4 the sketch reads the parameters from a microSD card and deliveres the tally signal of an ATEM switcher. You will need the libraries from SKAARHOJ Open Engineering and 'Rudolph_SD' and 'Rudolph_Error'. Add a LED to PIN6 of the ESP8266.

version 5
----------------------------
Version 5 brings in the wifi manager and enables to change and store the configuration using your web browser. If the device does not find an already known network, it opens up its own wifi. Before using the first time, run the prep.ino once.<br>
Configuration interface by Kathleen Preikschas.

You will need the libraries from SKAARHOJ Open Engineering and the WiFiManager.
