//************************************************************************************************************************
// DeltaDoreThermostat.h
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Ticker.h>

#include <Common.h>
#include <cc1101X2dEmitter.h>

#include "GroveTemp.h"

#include "ThermostatSettings.h"

using namespace corex;
using namespace cc1101;

/**
 * Class: DeltaDoreThermostat
 *
 * Description:
 * DeltaDore X2d radio thermostat
 */
class DeltaDoreThermostat : public Module <uint8_t>, public Printable
{
	SINGLETON_CLASS (DeltaDoreThermostat)

private:

	ThermostatSettings							_settings;

	GroveTempV12 *								_tempSensor	= nullptr;
	CC1101X2dEmitter *							_x2dEmmiter	= nullptr;

	Ticker										_firstTempRegulationTicker;
	Ticker										_secondTempRegulationTicker;
	Ticker										_tempRegulationTicker;

	bool										_heatingOn	= false;

public:

	Signal <const ThermostatSettings &> 		notifySettingsUpdated;
	Signal <bool>								notifyHeatingStateChanged;

private:

	void serializeSettings						();
	void deserializeSettings					();

	void regulateTemperature					();
	void startTemperatureRegulation				();

public:

	uint8_t getTempRegulationValue				(TRegulationMode::Type regulMode)	const			{ return _settings.tempRegulationValue.at(regulMode); 				}
	TRegulationMode::Type getTempRegulationMode	() 									const			{ return _settings.tempRegulationMode; 								}
	String getTempRegulationModeStr 			()									const			{ return TRegulationMode::ToStr.at(_settings.tempRegulationMode); 	}
	int8_t getAmbientTemperature				()									const			{ return _settings.ambientTemp;										}

	bool setTempRegulationValue					(TRegulationMode::Type regulMode, uint8_t newTempValue);
	void setTempRegulationMode					(TRegulationMode::Type regulationMode);
	bool setTempRegulationModeStr				(const String & regulationModeStr);

	void updateAmbientTemperature				();

	bool emmitHeatingCommand					(bool on);

	virtual size_t printTo						(Print & p) const override;
	friend Print & operator <<					(Print & p, DeltaDoreThermostat * ddt) 				{ ddt->printTo (p); return p; }

	void setup									(uint8_t tempPin) override;
	void loop									() override;
};
