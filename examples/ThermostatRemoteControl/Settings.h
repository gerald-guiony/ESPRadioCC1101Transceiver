//************************************************************************************************************************
// Settings.h
// Version 1.0 October, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

//------------------------------------------------------------------------------
//

// Add Wifi request handlers
#define USING_WIFI


#ifdef USING_WIFI

// Add JSON sms message sender
#	define USING_JSON_SMS_MESSAGE_SENDER

#else

// Add sms thermostat request handler
#	define USING_SMS_THERMOSTAT_REQUEST

#endif

//------------------------------------------------------------------------------
// Pins
//

#include <EspBoardDefs.h>


#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E

	// GSM
#	define GSM_TX_PIN			D10					// Software serial can be implemented on any GPIO pin of ESP8266
#	define GSM_RX_PIN			D9
#	define GSM_PWR_KEY			D3					// GPIO pin to connect A6 POWER KEY (SELECT/WAKEUP DEVICE)

	// Radio
//#	define  CC1101_IRQ_PIN 		D2					// Warning : D2 is I2C SDA !!
#	define  CC1101_IRQ_PIN 		D0					// This pin is not used in this project (no RF reception)

	// Temperature
#	define TEMP_SENSOR_PIN 		A0					// Analog pin

	// Push button
#	define PUSH_BUTTON_PIN		D4

#endif


//------------------------------------------------------------------------------
// Domoticz MQTT
//

#define MQTT_DOMOTICZ_ENDPOINT						"192.168.1.138"		// IP Broker MQTT
#define MQTT_DOMOTICZ_PORT							1883				// Port Broker MQTT

// Thermostat components in Domoticz
#define MQTT_DOMOTICZ_THERMOSTAT_SELECTOR_IDX		20
#define MQTT_DOMOTICZ_THERMOSTAT_TEMPERATURE_IDX	21
