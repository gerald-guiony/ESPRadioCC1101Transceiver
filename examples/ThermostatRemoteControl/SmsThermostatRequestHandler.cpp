//************************************************************************************************************************
// SmsThermostatRequestHandler.cpp
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include "SmsThermostatRequestHandler.h"

#include "Settings.h"
#include "DeltaDoreThermostat.h"


#define LASTSMSNUMBER_NAMEFILE			"/LastSmsNumber.dat"


namespace a6gsm {


SINGLETON_IMPL (SmsThermostatRequestHandler)

//========================================================================================================================
//
//========================================================================================================================
void SmsThermostatRequestHandler :: setup (A6SmsTransceiver * smsTransceiver)
{
	_smsTransceiver = smsTransceiver;
	_smsTransceiver->notifySmsReceived += std::bind (&SmsThermostatRequestHandler::onSmsRequestReceived, this, std::placeholders::_1);
}

//========================================================================================================================
//
//========================================================================================================================
void SmsThermostatRequestHandler :: sendDebugSMS (const String & message)
{
#ifdef DEBUG
	String smsNumber;
	if (FileStorage::readTextFile (F(LASTSMSNUMBER_NAMEFILE), smsNumber)) {

		SMSmessage sms { smsNumber, "", message };
		_smsTransceiver->sendSMS (sms);
	}
#endif
}

//========================================================================================================================
//
//========================================================================================================================
static bool strToLong (const String & str, long & value) {

	if (str.length () <= 0) return false;
	if ((str[0] != '-') && !isDigit (str[0])) return false;
	if ((str[0] == '-') && (str.length ()<=1)) return false;

	for (int i=1; i<str.length (); i++) {
		if (!isdigit (str [i])) return false;
	}

	value = str.toInt();
	return true;
}

//========================================================================================================================
//
//========================================================================================================================
void SmsThermostatRequestHandler :: onSmsRequestReceived (const SMSmessage & sms)
{

#ifdef DEBUG
	if (!FileStorage::isFileExists (F(LASTSMSNUMBER_NAMEFILE))) {
		FileStorage::writeTextFile (F(LASTSMSNUMBER_NAMEFILE), sms.number);
	}
#endif

	bool result = false;
	SMSmessage smsResp;

	smsResp.number = sms.number;
	String command = sms.message;

	command.trim ();
	command.toUpperCase ();

	// Search "=" in the string
	int indexEq = command.indexOf('=');

	// "=" not found
	if (indexEq <= 0) {
		// Set Regulation mode : HG, ECO, or CONF
		if (I(DeltaDoreThermostat).setTempRegulationModeStr (command)) {
			result = true;
		}
		// Settings
		else if (command == F("CONFIG")) {
			StreamString sstr;
			sstr << &I(DeltaDoreThermostat);
			smsResp.message = sstr;
			result = true;
		}
		// Get Ambient temperature
		else if (command == F("TEMP")) {
			smsResp.message = F("Temperature ambiante = ");
			smsResp.message += I(DeltaDoreThermostat).getAmbientTemperature ();
			result = true;
		}
		else if (command == F("REBOOT")) {
			result = _isRebootRequested = true;
		}
	}
	// "=" found
	else {
		// Set temperature regulation
		String regulMode = command.substring(0, indexEq);
		regulMode.trim();
		if (regulMode.length() > 0) {

			TRegulationMode::Type regulationMode;

			// Check if regulation mode is valid
			if (TRegulationMode::FromStr (regulMode, regulationMode)) {

				String strValue = command.substring(indexEq + 1);
				strValue.trim();

				long value = 0;
				// Check if integer value is valid
				if (strToLong (strValue, value)) {
					if (I(DeltaDoreThermostat).setTempRegulationValue (regulationMode, (uint8_t)value)) {
						result = true;
					}
				}
			}
		}
	}

	if (smsResp.message.length () <= 0) {

		StreamString sstr;
		sstr << command << (result ? F(" => OK") : F(" => Commande inconnue")) << LN << LN;
		if (!result) {
			sstr << F("-- Liste des commandes --") << LN;
			sstr << F("* HG (=temperature)") << LN;
			sstr << F("* ECO (=temperature)") << LN;
			sstr << F("* CONF (=temperature)") << LN;
			sstr << F("* CONFIG") << LN;
			sstr << F("* TEMP") << LN;
			sstr << F("* REBOOT") << LN;
		}
		smsResp.message = sstr;
	}

	_smsTransceiver->sendSMS (smsResp);
}

//========================================================================================================================
//
//========================================================================================================================
void SmsThermostatRequestHandler :: startGsm ()
{
	_isStartGsmRequested = true;
}

//========================================================================================================================
//
//========================================================================================================================
void SmsThermostatRequestHandler :: loop ()
{
	static bool isFirstGsmStart						= true;
	static unsigned long gsmOffTimestamp 			= 0;
	static unsigned long gsmOnTimestamp 			= 0;
	static unsigned long gsmSignalStrengthTimestamp	= 0;

	if (!_smsTransceiver->isStarted()) {

		if (_isStartGsmRequested || (millis () - gsmOffTimestamp > (isFirstGsmStart ? GSM_OFF_FIRST_DURATION_ms : GSM_OFF_DURATION_ms))) {

			_isStartGsmRequested = false;

			// Needed on some chips .. ??
			EspBoard::disableHardwareWatchdog ();

			if (_smsTransceiver->start ()) {
				gsmOnTimestamp = gsmSignalStrengthTimestamp = millis ();
			}
		}
	}
	else {

		if (isFirstGsmStart) {

			sendDebugSMS (F("Le thermostat a redemarre"));
			isFirstGsmStart = false;
		}
		else if (_smsTransceiver->getNbSms() > 0) {

			_smsTransceiver->handleSms (0);
		}
		else if ((millis () - gsmOnTimestamp > GSM_ON_DURATION_ms) || _isRebootRequested) {

			// Clear sms storage
			_smsTransceiver->deleteAllSms ();

			// Stop sms
			_smsTransceiver->stop ();

			gsmOffTimestamp = millis ();

			if (_isRebootRequested) {
				I(ModuleSequencer).requestReboot ();
			}

			// Re-enable hardware watchdog
//			EspBoard::enableHardwareWatchdog ();
		}
		else if (millis () - gsmSignalStrengthTimestamp > GSM_SIGNAL_STRENGTH_PERIOD_ms) {

			gsmSignalStrengthTimestamp = millis ();
			_smsTransceiver->updateSignalStrength ();
		}

	}
}


}