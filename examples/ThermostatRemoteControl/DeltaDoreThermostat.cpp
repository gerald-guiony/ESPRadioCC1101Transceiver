//************************************************************************************************************************
// DeltaDoreThermostat.cpp
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <Stream.h>
#include <StreamString.h>

#include "DeltaDoreThermostat.h"

#include "X2dRadioTyboxCommands.h"



#define THERMOSTAT_NAMEFILE						"/Thermostat.dat"

#define DELAY_TO_START_TEMP_REGULATION_ms		120000

#define TEMP_REGULATION_FIRST_DELAY_s			5					// 5s
#define TEMP_REGULATION_SECOND_DELAY_s			120					// 2 minutes
#define TEMP_REGULATION_PERIODIC_DELAY_s		15*60				// 15 minutes




SINGLETON_IMPL (DeltaDoreThermostat)


//========================================================================================================================
//
//========================================================================================================================
size_t DeltaDoreThermostat :: printTo (Print & p) const {

	StreamString sstr;

	sstr << F("-- Parametres Thermostat --")	<< LN << LN;
	sstr << F(" * Temp de regulation: ")	<< LN;
	sstr << F("    - HG:   ") 				<< getTempRegulationValue (TRegulationMode::HG) << LN;
	sstr << F("    - ECO:  ")				<< getTempRegulationValue (TRegulationMode::ECO) << LN;
	sstr << F("    - CONF: ")				<< getTempRegulationValue (TRegulationMode::CONF) << LN;
	sstr << F(" * Mode de regulation: ")	<< getTempRegulationModeStr () << LN;
	sstr << F(" * Temp ambiente: ") 		<< getAmbientTemperature () << LN;
	sstr << LN;

	p << sstr;

	return sstr.length ();
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: deserializeSettings ()
{
	String filename = F(THERMOSTAT_NAMEFILE);
	File f = LittleFS.open(filename, "r");
	if (!f) {
		serializeSettings ();
		f = LittleFS.open(filename, "r");
		if (!f) return;
	}

	Logln(F("Open ") << filename);

	const uint8_t nbValues = TRegulationMode::All.size() + 1;
	uint8_t values [nbValues];

	// returns the number of characters placed in the buffer (0 means no valid data found)
	bool readResult = (f.readBytes ((char *) &(values[0]), nbValues) == nbValues);

	f.close ();

	if (readResult)
		Logln(F("Thermostat settings deserialized"));
	else {
		Logln(F("ERROR : file ") << filename << F(" is invalid"));
		return;
	}

	for (int i=0; i<TRegulationMode::All.size(); i++) {
		_settings.tempRegulationValue [TRegulationMode::All[i]] = values [i];
	}

	_settings.tempRegulationMode = (TRegulationMode::Type) values [nbValues-1];

	Logln(this);
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: serializeSettings ()
{
	if (!FileStorage::spiffsCheckRemainingBytes ()) return;

	String filename = F(THERMOSTAT_NAMEFILE);

	File f = LittleFS.open(filename, "w");
	if (!f) {
		Logln(F("ERROR : Can't open the file : ") << filename);
		return;
	}

	const uint8_t nbValues = TRegulationMode::All.size() + 1;
	uint8_t values [nbValues];

	for (int i=0; i<TRegulationMode::All.size(); i++) {
		values [i] = getTempRegulationValue (TRegulationMode::All[i]);
	}

	values [nbValues-1] = (uint8_t) getTempRegulationMode ();

	bool writeResult = (f.write ((const uint8_t *) &(values[0]), nbValues) == nbValues);

	f.close ();

	if (writeResult)
		Logln(F("Thermostat settings serialized"));
	else
		Logln(F("ERROR : Can't write in the file : ") << filename);

	Logln(this);

	FileStorage::spiffsInfos ();
}

//========================================================================================================================
//
//========================================================================================================================
bool DeltaDoreThermostat :: setTempRegulationValue	(TRegulationMode::Type regulMode, uint8_t newTempValue)
{
	if ((newTempValue <= 4) || (25 < newTempValue)) return false;
	if (_settings.tempRegulationValue [regulMode] == newTempValue) return true;

	_settings.tempRegulationValue [regulMode] = newTempValue;

	serializeSettings ();

	notifySettingsUpdated (_settings);

	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool DeltaDoreThermostat :: setTempRegulationModeStr (const String & regulationModeStr)
{
	TRegulationMode::Type regulationMode;
	if (TRegulationMode::FromStr (regulationModeStr, regulationMode)) {
		setTempRegulationMode (regulationMode);
		return true;
	}
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: setTempRegulationMode (TRegulationMode::Type regulationMode)
{
	if (_settings.tempRegulationMode == regulationMode) return;

	_settings.tempRegulationMode = regulationMode;
	serializeSettings ();

	notifySettingsUpdated (_settings);

	// Launch the temperature regulation timer
	startTemperatureRegulation ();
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: updateAmbientTemperature ()
{
	int8_t ambientTemp = round (_tempSensor->getAmbientTemperature ());

	if (_settings.ambientTemp != ambientTemp) {
		_settings.ambientTemp = ambientTemp;
		notifySettingsUpdated (_settings);
	}
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: regulateTemperature	()
{
	uint8_t regulationTemp = getTempRegulationValue (_settings.tempRegulationMode);

	int8_t ambientTemp = getAmbientTemperature ();

	Logln(F("Current ambient temperature: ") << ambientTemp);
	Logln(F("Regulation temperature: ") << regulationTemp);

	emmitHeatingCommand (ambientTemp < regulationTemp);
}

//========================================================================================================================
//
//========================================================================================================================
bool DeltaDoreThermostat :: emmitHeatingCommand (bool on)
{
	const uint8_t nbPackets = 3;
	CCPACKET packetsToSend [nbPackets];

	Logln(F("Emmiting command to switch ") << (on ? F("ON") : F("OFF")) << F(" heating"));

	if (_heatingOn != on) {
		_heatingOn = on;
		notifyHeatingStateChanged (_heatingOn);
	}

	for (int i=0; i<nbPackets; i++) { packetsToSend [i] = (on ? HEATING_ON_CMD [i] : HEATING_OFF_CMD [i]); }

	return _x2dEmmiter->sendPackets (packetsToSend, nbPackets);
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: startTemperatureRegulation ()
{
	_firstTempRegulationTicker.detach ();
	_secondTempRegulationTicker.detach ();
	_tempRegulationTicker.detach ();

	// Initialize temperature regulation
	_firstTempRegulationTicker.once (TEMP_REGULATION_FIRST_DELAY_s, std::bind (&DeltaDoreThermostat::regulateTemperature, this));
	_secondTempRegulationTicker.once (TEMP_REGULATION_SECOND_DELAY_s, std::bind (&DeltaDoreThermostat::regulateTemperature, this));

	// Then launch the temperature regulation every 15 minutes
	_tempRegulationTicker.attach (TEMP_REGULATION_PERIODIC_DELAY_s, std::bind (&DeltaDoreThermostat::regulateTemperature, this));
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: setup (uint8_t tempPin)
{
	Logln (F("Initializing thermostat.."));

	_tempSensor = new GroveTempV12 (tempPin);
	_x2dEmmiter = new CC1101X2dEmitter ();

	_tempSensor->setup ();

	deserializeSettings ();

	notifySettingsUpdated (_settings);
}

//========================================================================================================================
//
//========================================================================================================================
void DeltaDoreThermostat :: loop ()
{
	static bool isTemperatureRegulationStarted = false;
	static unsigned long rebootTimeStamp = millis ();

	if (!isTemperatureRegulationStarted) {
		if (millis () - rebootTimeStamp > DELAY_TO_START_TEMP_REGULATION_ms) {
			startTemperatureRegulation ();
			isTemperatureRegulationStarted = true;
		}
	}

	_tempSensor->loop ();

	updateAmbientTemperature ();
}
