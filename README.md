# Arduino Weather Station

This project is an embedded weather station built with Arduino. It collects environmental data such as temperature, humidity, pressure and light intensity, and logs them into files on an SD card with timestamps using a real-time clock.

I worked on this project to get hands-on experience with embedded systems, especially sensor integration, data acquisition and basic system design on a microcontroller.

The hardware setup includes a BME280 sensor for temperature, humidity and pressure measurements, a DS3231 RTC module for timekeeping, and an SD card module for data logging. A simple analog light sensor is used for luminosity readings. Two push buttons are used to control the system modes, and a chainable RGB LED provides visual feedback about the system state and possible errors.

The software is written in C++ using the Arduino framework. The program handles communication with multiple components (I2C and SPI), reads sensor data at regular intervals, formats it, and stores it into log files. It also includes basic error handling, for example when a sensor or the SD card is not detected.

Several operating modes are implemented. In standard mode, the system logs data at a fixed interval. An eco mode reduces the frequency of measurements to save power. A maintenance mode allows real-time monitoring without writing to the SD card. A configuration mode is available at startup and allows parameters to be changed via the serial monitor (such as logging interval or date/time settings).

Data is stored in a simple text format, with each line containing a timestamp followed by the measured values. Log files are created daily, which makes the data easy to analyze or reuse later.

The project is currently implemented in a single file, but it could be improved by splitting the code into multiple modules (sensor management, logging, configuration), which would make it easier to maintain and extend. Other possible improvements include adding a display (LCD or OLED), integrating wireless communication (WiFi or Bluetooth), or optimizing power consumption further.

This project is not meant to be a production-ready system, but rather a practical learning project to explore embedded programming and system integration.
