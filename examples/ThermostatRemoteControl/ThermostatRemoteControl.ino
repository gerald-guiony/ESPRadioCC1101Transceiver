//************************************************************************************************************************
// ThermostatRemoteControl.ino
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include <Common.h>
#include <Switches/AsyncPushButton.h>

#include <HttpServer.h>
#include <HttpAdminCommandRequestHandler.h>
#include <MqttDomoticzClient.h>

#include "Settings.h"

#include "OledDisplayThermostat.h"
#include "DeltaDoreThermostat.h"
#include "SmsThermostatRequestHandler.h"

#include "HttpRadioCommandRequestHandler.h"
#include "HttpThermostatRequestHandler.h"
#include "HttpSmsSenderRequestHandler.h"
#include "MqttThermostatDomoticzHandler.h"
#include "WiFiConnectionManager.h"


#ifdef USING_WIFI
#	warning -- USING_WIFI defined --
#endif
#ifdef USING_JSON_SMS_MESSAGE_SENDER
#	warning -- USING_JSON_SMS_MESSAGE_SENDER defined --
#endif
#ifdef USING_SMS_THERMOSTAT_REQUEST
#	warning -- USING_SMS_THERMOSTAT_REQUEST defined --
#endif


using namespace corex;
using namespace wifix;
using namespace a6gsm;


// => WARNING: First instruction mandatory !!
// Do "digitalWrite (GSM_PWR_KEY, LOW)" as soon as possible to not wake up A6 gsm module during reboot
#if defined(USING_SMS_THERMOSTAT_REQUEST) || defined(USING_JSON_SMS_MESSAGE_SENDER)
A6SmsTransceiver smsTransceiver (GSM_RX_PIN, GSM_TX_PIN, GSM_PWR_KEY);
#endif

AsyncPushButton pushButtonWakeUp (PUSH_BUTTON_PIN);


//========================================================================================================================
//
//========================================================================================================================
void setup() {

	// ------------ Global Init

	EspBoard::init ();

	// ------------- Connect signals

	pushButtonWakeUp.notifyPressedState					+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));

	I(DeltaDoreThermostat).notifySettingsUpdated		+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
	I(DeltaDoreThermostat).notifyHeatingStateChanged	+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
	I(DeltaDoreThermostat).notifySettingsUpdated		+= std::bind (&OledDisplayThermostat::onThermostatSettingsUpdated, &I(OledDisplayThermostat), _1);
	I(DeltaDoreThermostat).notifyHeatingStateChanged	+= std::bind (&OledDisplayThermostat::onHeatingStateChanged, &I(OledDisplayThermostat), _1);

	I(ModuleSequencer).notifyAwake						+= std::bind (&OledDisplayThermostat::showDisplay, &I(OledDisplayThermostat), _1);

#ifdef USING_WIFI
	I(HttpServer).notifyRequestReceived					+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
	I(WiFiConnectionManager).notifyArduinoOTAStart 		+= std::bind (&OledDisplayThermostat::onArduinoOTAStart, &I(OledDisplayThermostat));
	I(WiFiConnectionManager).notifyArduinoOTAProgress 	+= std::bind (&OledDisplayThermostat::onArduinoOTAProgress, &I(OledDisplayThermostat), _1, _2);
	I(WiFiConnectionManager).notifyArduinoOTAEnd		+= std::bind (&OledDisplayThermostat::onArduinoOTAEnd, &I(OledDisplayThermostat));
	I(DeltaDoreThermostat).notifySettingsUpdated 		+= [] (const ThermostatSettings & thermostatSettings) {
		I(MqttThermostatDomoticzPublisher).publishAmbientTemperature (thermostatSettings.ambientTemp);
	};
#endif

#if defined(USING_SMS_THERMOSTAT_REQUEST) || defined(USING_JSON_SMS_MESSAGE_SENDER)

	smsTransceiver.notifySmsReceived					+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
	smsTransceiver.notifySmsSent						+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
	smsTransceiver.notifyGsmStateChanged				+= std::bind (&OledDisplayThermostat::onGsmStateChanged, &I(OledDisplayThermostat), _1, _2);
	smsTransceiver.notifySmsReceived					+= std::bind (&OledDisplayThermostat::onSmsReceived, &I(OledDisplayThermostat));

#	ifdef USING_SMS_THERMOSTAT_REQUEST
	pushButtonWakeUp.notifyPressedState					+= std::bind (&SmsThermostatRequestHandler::startGsm, &I(SmsThermostatRequestHandler));
#	endif
#endif

	// ------------- Setup

	I(OledDisplayThermostat).setup ();

#ifdef USING_JSON_SMS_MESSAGE_SENDER
	I(OledDisplayThermostat).drawMessage (F("Init GSM"));
	I(SmsSenderRequestHandler).setup (&smsTransceiver);
#elif defined(USING_SMS_THERMOSTAT_REQUEST)
	I(OledDisplayThermostat).drawMessage (F("Init GSM"));
	I(SmsThermostatRequestHandler).setup (&smsTransceiver);
#endif

#ifdef USING_WIFI
	I(OledDisplayThermostat).drawMessage (F("Starting WiFi"));

	I(WiFiConnectionManager).setWifiManagerEnabled (true, false /* No access point */);
	I(WiFiConnectionManager).notifySetupWifiConnections += []() {

		I(HttpServer).setup	( { &I(HttpAdminCommandRequestHandler),
								&I(HttpRadioCommandRequestHandler),
								&I(HttpThermostatRequestHandler),
#	ifdef USING_JSON_SMS_MESSAGE_SENDER
								&I(HttpSmsSenderRequestHandler)
#	endif
						  	} );
		I(MqttDomoticzClient).setup (MQTT_DOMOTICZ_ENDPOINT, MQTT_DOMOTICZ_PORT, {	&I(MqttDomoticzLogPublisher),
																					&I(MqttThermostatDomoticzPublisher),
																					&I(MqttThermostatDomoticzSubscriber) });
	};
	I(WiFiConnectionManager).setup ({	&I(HttpServer),
								 		&I(MqttDomoticzClient) });
#endif

	I(OledDisplayThermostat).drawMessage (F("Init thermostat"));
	I(DeltaDoreThermostat).setup (TEMP_SENSOR_PIN);

	I(OledDisplayThermostat).drawMessage (F("Init sequencer"));
	I(ModuleSequencer).setup ({
		&I(DeltaDoreThermostat),
		&I(OledDisplayThermostat),
#ifdef USING_WIFI
		&I(WiFiConnectionManager),
#	ifdef USING_JSON_SMS_MESSAGE_SENDER
		&I(SmsSenderRequestHandler),
#	endif
#elif defined(USING_SMS_THERMOSTAT_REQUEST)
		&I(SmsThermostatRequestHandler),
#endif
	}, false);// Do not use blinker module because D4 (LEDBUILTIN) pin is connected to push button
}

//========================================================================================================================
//
//========================================================================================================================
void loop() {
	I(ModuleSequencer).loop ();
}
