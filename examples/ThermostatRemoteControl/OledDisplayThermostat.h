//************************************************************************************************************************
// OledDisplayThermostat.h
// Version 1.0 October, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>

#include "ThermostatSettings.h"

using namespace corex;


//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class OledDisplayThermostat: public Module <>
{
	SINGLETON_CLASS (OledDisplayThermostat)

private:

	void drawTimeElapsedSinceLastBoot	();
	void drawThermostatSettings			();
	void drawHeader						();
	void drawFooter						();

public:

	void drawMessage					(const String & message);

	void onArduinoOTAStart				();
	void onArduinoOTAProgress			(unsigned int progress, unsigned int total);
	void onArduinoOTAEnd				();

	void onThermostatSettingsUpdated	(const ThermostatSettings & param);
	void onHeatingStateChanged			(bool isOn);

	void onGsmStateChanged				(bool isOn, int signalStrength);
	void onSmsReceived					();

	void showDisplay					(bool displayOn);

	void setup 							();
	void loop 							() override;

private:

	ThermostatSettings					_thermostatSettings;
	bool								_isThermostatHeatingOn 	= false;
	bool								_isGsmActive			= false;
	int									_gsmSignalStrength		= 0;
	bool								_isSmsReceived			= false;

	bool								_isDiplayOn				= true;
};