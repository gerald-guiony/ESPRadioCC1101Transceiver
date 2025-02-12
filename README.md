# ESPRadioCC1101Transceiver

ESPRadioCC1101Transceiver is a C++ library for ESP8266 Wi-Fi chip that enables the transmission and reception of RF signals using the CC1101 module.

This library is a compilation of various CC1101 Arduino libraries especially this one: [X2D Heaters drivers for Arduino and CC1101 RF chip](https://github.com/SixK/CC1101-X2D-Heaters)

## Getting started

To get started with ESPRadioCC1101Transceiver, you will need:

* An ESP8266 board
* A CC1101 based device
* An USB DVB Key (compatible with SDR) 
* [RTL_433 tool](https://github.com/merbanan/rtl_433) probably only work on Linux, you will probably have to compile it manually
* The Arduino IDE for ESP8266 (version 1.8.8 minimum)
* Basic knowledge of the Arduino environment (upload a sketch, import libraries, ...)

## Installing ESPRadioCC1101Transceiver

1. Download the latest master source code [.zip](https://github.com/gerald-guiony/ESPRadioCC1101Transceiver/archive/master.zip) file
2. Import the `.zip` file by navigating to **Sketch → Include Library → Add .ZIP Library**

## Dependencies

To use this library, you may need to have the latest version of my [ESPCoreExtension](https://github.com/gerald-guiony/ESPCoreExtension) library

## CC1101 Wiring

![CC1101 Wiring](https://github.com/gerald-guiony/ESPRadioCC1101Transceiver/blob/master/docs/CC1101%20ESP8266.png)

## Example

[Smart thermostat](https://github.com/gerald-guiony/ESPRadioCC1101Transceiver/blob/master/examples/ThermostatRemoteControl)
