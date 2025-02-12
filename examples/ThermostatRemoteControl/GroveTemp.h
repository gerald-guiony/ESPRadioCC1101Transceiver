//************************************************************************************************************************
// GroveTemp.h
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once


// http://wiki.seeedstudio.com/Grove-Temperature_Sensor_V1.2/


/**
 * Class: GroveTempV12
 *
 * Description:
 * The Grove - Temperature Sensor uses a Thermistor to detect the ambient temperature. The resistance of a thermistor will
 * increase when the ambient temperature decreases. It's this characteristic that we use to calculate the ambient temperature.
 * The detectable range of this sensor is -40 - 125ºC, and the accuracy is ±1.5ºC
 */
class GroveTempV12
{
private:
	const uint8_t _analogPin;					// Grove - Temperature Sensor connect to pin (A0)
	float _ambientTemperature;

private:
	void readADCValue			();
	void readAmbientTemperature ();

public:
	GroveTempV12 (const uint8_t analogPin);

	float getAmbientTemperature () const;

	void setup					();
	void loop					();
};