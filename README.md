# Agriculture Monitoring System

An IoT-based smart agriculture system using Arduino UNO and ESP32-S3 to monitor temperature, humidity, light intensity, and soil moisture.

## What the Project Does

The system collects environmental and soil data using sensors connected to an Arduino UNO. The measured values are displayed on a 16×2 I2C LCD and used to determine the overall plant condition.

The Arduino UNO sends the collected data to an ESP32-S3 through UART communication. The ESP32-S3 connects to Wi-Fi and provides an IP address for further IoT integration.

## Features

- Temperature and humidity monitoring using DHT22
- Soil moisture monitoring
- Light intensity monitoring using LDR
- Plant condition classification
- 16×2 LCD display
- Temperature alert using LED
- UART communication between Arduino UNO and ESP32-S3
- Wi-Fi connectivity through ESP32-S3

## Technologies Used

- Arduino UNO
- ESP32-S3
- Embedded C/C++
- UART Communication
- Wi-Fi
- I2C
- DHT22
- LDR
- Soil Moisture Sensor

## System Architecture

Sensors → Arduino UNO → UART → ESP32-S3 → Wi-Fi

## Project Report

The complete project report is available in the `Report` folder.

## Author

**MVN Sashank Naidu**
