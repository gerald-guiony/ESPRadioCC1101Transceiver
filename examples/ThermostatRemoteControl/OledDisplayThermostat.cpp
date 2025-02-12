//************************************************************************************************************************
// OledDisplayThermostat.cpp
// Version 1.0 October, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include "OledDisplayThermostat.h"

// https://github.com/ThingPulse/esp8266-oled-ssd1306

// Include the correct display library

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>			   // Only needed for Arduino 1.6.5 and earlier
#include <SSD1306Wire.h>		// legacy: #include "SSD1306.h


// Initialize the OLED display using Arduino Wire:
SSD1306Wire display (0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h



SINGLETON_IMPL (OledDisplayThermostat)


//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: drawMessage (const String & message)
{
	display.clear();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display.drawString(display.getWidth()/2, display.getHeight()/2, message);
	display.display();
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: drawTimeElapsedSinceLastBoot ()
{
	display.setFont (ArialMT_Plain_10);
	display.setTextAlignment (TEXT_ALIGN_RIGHT);
	display.drawString (display.getWidth(), display.getHeight()-10, EspBoard::getTimeElapsedSinceBoot());
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: drawThermostatSettings ()
{
	char buffer [20];

	display.setFont (ArialMT_Plain_10);
	display.setTextAlignment (TEXT_ALIGN_RIGHT);

	for (int i=0; i<TRegulationMode::All.size(); i++) {
		String regulModeIt = TRegulationMode::ToStr.at (TRegulationMode::All[i]);
		while (regulModeIt.length() < 4) regulModeIt += "_";
		bool isCurrentRegulMode = (_thermostatSettings.tempRegulationMode == TRegulationMode::All[i]);
		sprintf (buffer, isCurrentRegulMode ? "> %s [%02d°C]":"%s [%02d°C]", regulModeIt.c_str(), _thermostatSettings.tempRegulationValue [TRegulationMode::All[i]]);
		display.drawString (display.getWidth(), 17 + 10*i, buffer);
	}

	display.setFont (ArialMT_Plain_16);
	display.setTextAlignment (TEXT_ALIGN_LEFT);
//	sprintf (buffer, _isThermostatHeatingOn ? "%d°C ¶" : "%d°C", _thermostatSettings.ambientTemp);
	sprintf (buffer, _isThermostatHeatingOn ? "%d°C ^" : "%d°C", _thermostatSettings.ambientTemp);
	display.drawString (0, 24, buffer);
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: drawHeader ()
{
	display.drawHorizontalLine(0, 12, display.getWidth());

	display.setFont(ArialMT_Plain_10);

	display.setTextAlignment (TEXT_ALIGN_LEFT);
	display.drawString (0, 0, WiFiHelper::isWifiAvailable() ? (WiFiHelper :: getIpAddress ().toString()) : EspBoard::getDeviceName());

	if (_isGsmActive) {
		char buffer [20];
		display.setTextAlignment (TEXT_ALIGN_RIGHT);
		//display.drawString (display.getWidth(), 0, _isSmsReceived ? "GSM*":"GSM");
		sprintf (buffer, _isSmsReceived ? "GSM: %d*" : "GSM: %d", _gsmSignalStrength);
		display.drawString (display.getWidth(), 0, buffer);
		_isSmsReceived = false;
	}
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: drawFooter ()
{
	display.drawHorizontalLine(0, display.getHeight()-10, display.getWidth());

	drawTimeElapsedSinceLastBoot ();
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: onArduinoOTAStart ()
{
	showDisplay (true);

	display.clear();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display.drawString(display.getWidth()/2, display.getHeight()/2 - 10, "OTA Update");
	display.display();
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: onArduinoOTAProgress (unsigned int progress, unsigned int total)
{
	display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
	display.display();
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: onArduinoOTAEnd ()
{
	display.clear();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display.drawString(display.getWidth()/2, display.getHeight()/2, "Restart");
	display.display();
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: onThermostatSettingsUpdated (const ThermostatSettings & param)
{
	for (int i=0; i<TRegulationMode::All.size(); i++) {
		_thermostatSettings.tempRegulationValue [TRegulationMode::All[i]] = param.tempRegulationValue.at(TRegulationMode::All[i]);
	}
	_thermostatSettings.tempRegulationMode = param.tempRegulationMode;
	_thermostatSettings.ambientTemp = param.ambientTemp;
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: onHeatingStateChanged (bool isOn)
{
	_isThermostatHeatingOn = isOn;
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: onGsmStateChanged (bool isOn, int signalStrength)
{
	_isGsmActive = isOn;
	_gsmSignalStrength = signalStrength;
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: onSmsReceived ()
{
	_isSmsReceived = true;
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: showDisplay (bool displayOn)
{
	if (_isDiplayOn != displayOn) {

		_isDiplayOn = displayOn;

		display.clear ();

		if (displayOn)
			display.displayOn ();
		else
			display.displayOff ();
	}
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: setup ()
{
	// Initialising the UI will init the display too.
	display.init ();
	display.flipScreenVertically ();
	display.setFont (ArialMT_Plain_10);
}

//========================================================================================================================
//
//========================================================================================================================
void OledDisplayThermostat :: loop ()
{
	if (_isDiplayOn) {
		// clear the display
		display.clear ();

		drawHeader ();
		drawThermostatSettings ();
		drawFooter ();

		// write the buffer to the display
		display.display ();
	}
}