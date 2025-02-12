//************************************************************************************************************************
// MqttThermostatDomoticzHandler.h
// Version 1.0 February, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>
#include <MqttDomoticzHandler.h>


namespace wifix {

//------------------------------------------------------------------------------
// Singleton
class MqttThermostatDomoticzPublisher : public MqttDomoticzPublisher
{
	SINGLETON_CLASS(MqttThermostatDomoticzPublisher)

public:
	size_t publishAmbientTemperature	(const uint8_t temperature);
};


//------------------------------------------------------------------------------
// Singleton
class MqttThermostatDomoticzSubscriber : public MqttDomoticzSubscriberIdx
{
	SINGLETON_CLASS(MqttThermostatDomoticzSubscriber)

public:

	virtual void setup 				(AsyncMqttClient * asyncMqttClient) override;
	virtual bool onTopicIdxReceived	(const JsonObject& jsonObj) override;

};

}








