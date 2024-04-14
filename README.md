# Ordnungsplan-mit-GPS-Uhrzeit---ESP32

Das Projekt Zeigt einen Ordnungsplan für eine 3 Personen WG auf einem Display an.
Die Aufgaben routieren wöchentlich.
Weiterhin wird noch die akuelle Uhrzeit sowie das Datum auf dem Display mit angezeigt.

Da es nicht möglich ist den ESP32 mit einem WLAN-Netzwerk zu verbinden, wird auf ein GPS-Modul zurück gegriffen um eine genaue Uhrezeit zu erhalten.

Teile:
- ESP32 Dev Module
- GY-NEO6MV2
- 2,42 Zoll 128x64 oled I2C SSD1309 Display
- 2x 4,7kOhm Wiederstände
