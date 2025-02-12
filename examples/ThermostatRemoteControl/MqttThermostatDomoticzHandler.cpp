//************************************************************************************************************************
// MqttThermostatDomoticzHandler.cpp
// Version 1.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#include "Settings.h"

#include "MqttThermostatDomoticzHandler.h"



// Todo :
// - Ajouter les paramètres du serveur mqtt (Ip, Port..) a la page du wifimanager
// https://github.com/tzapu/WiFiManager/blob/master/examples/AutoConnectWithFSParameters/AutoConnectWithFSParameters.ino


namespace wifix {


SINGLETON_IMPL (MqttThermostatDomoticzPublisher)

//========================================================================================================================
//
//========================================================================================================================
size_t MqttThermostatDomoticzPublisher :: publishAmbientTemperature	(const uint8_t temperature) {

	return publishValue (MQTT_DOMOTICZ_THERMOSTAT_TEMPERATURE_IDX, temperature);
}

/************************************************************************************************************************/


SINGLETON_IMPL (MqttThermostatDomoticzSubscriber)

//========================================================================================================================
//
//========================================================================================================================
void MqttThermostatDomoticzSubscriber :: setup (AsyncMqttClient * asyncMqttClient) {

	_idx = MQTT_DOMOTICZ_THERMOSTAT_SELECTOR_IDX;
	MqttDomoticzSubscriberIdx :: setup (asyncMqttClient);
}

//========================================================================================================================
//
//========================================================================================================================
bool MqttThermostatDomoticzSubscriber :: onTopicIdxReceived	(const JsonObject& jsonArg) {

	if (jsonArg [PAYLOAD_NVALUE].success()) {

		// Nothing to do
		return true;
	}

	return false;
}

}