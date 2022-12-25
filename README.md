# Moisture Alert

Set off an alarm when moisture is detected. 

### Hardware

* [ESP32](https://www.espressif.com/en/products/devkits/esp32-devkitc)

* [Qwiic Relay](https://www.sparkfun.com/products/15093)

* [Moisture Sensor](https://www.seeedstudio.com/Grove-Capacitive-Moisture-Sensor-Corrosion-Resistant.html?queryID=925b48bf8674952cc1ddd2dc074bb28c&objectID=2850&indexName=bazaar_retailer_products)

### Wiring

| Pin   | Component     | Dest  |
|-------|---------------|-------|
| 3.3V  | [Qwiic Relay](https://www.sparkfun.com/products/15093)    | Vcc   |
| GND   | [Qwiic Relay](https://www.sparkfun.com/products/15093)     | GND   |
| 21    | [Qwiic Relay](https://www.sparkfun.com/products/15093)     | SDA   |
| 22    | [Qwiic Relay](https://www.sparkfun.com/products/15093)     | SCL   |
| 3.3V  | [Sensor](https://www.seeedstudio.com/Grove-Capacitive-Moisture-Sensor-Corrosion-Resistant.html?queryID=925b48bf8674952cc1ddd2dc074bb28c&objectID=2850&indexName=bazaar_retailer_products)        | Vcc   |
| GND   | [Sensor](https://www.seeedstudio.com/Grove-Capacitive-Moisture-Sensor-Corrosion-Resistant.html?queryID=925b48bf8674952cc1ddd2dc074bb28c&objectID=2850&indexName=bazaar_retailer_products)         | GND   |
| 32    | [Sensor](https://www.seeedstudio.com/Grove-Capacitive-Moisture-Sensor-Corrosion-Resistant.html?queryID=925b48bf8674952cc1ddd2dc074bb28c&objectID=2850&indexName=bazaar_retailer_products)         | ADC   |
