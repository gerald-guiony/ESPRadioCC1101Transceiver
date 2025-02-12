//************************************************************************************************************************
// GroveTemp.cpp
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <Arduino.h>
//#include <math.h>
#include <cmath>

#include <Common.h>

#include "GroveTemp.h"



extern "C" {
	#include "user_interface.h"
}
//========================================================================================================================
// More pretty stable result than "analogRead (A0)"
//========================================================================================================================
int getAnalog() {

	unsigned int total = 0;

	wifi_set_opmode(NULL_MODE);
	system_soft_wdt_stop();
	ets_intr_lock();
	noInterrupts();

	total = system_adc_read();

	interrupts();
	ets_intr_unlock();
	system_soft_wdt_restart();

	return total;
}





#define VOLTAGE_3v3									3.3			// 3.3v
#define VOLTAGE_5v									4.7			// 5v

//#define CORRECTIVE_VOLTAGE_DIV					VOLTAGE_3v3 / VOLTAGE_5v
#define CORRECTIVE_VOLTAGE_DIV						1

#define MIN_DELAY_BETWEEN_READ_AMBIENT_TEMP_MS		60*1000		// 1m en ms
#define NB_ADC_VALUES								5

#define B											4275		// B value of the thermistor
#define R0											100000		// R0 = 100k



uint16_t adcValues [NB_ADC_VALUES];
uint8_t adcIdx 			= 0;


//========================================================================================================================
//
//========================================================================================================================
GroveTempV12 :: GroveTempV12 (const uint8_t analogPin) :
	_analogPin (analogPin)
{
}

//========================================================================================================================
//
//========================================================================================================================
void GroveTempV12 :: readADCValue ()
{
	int a = analogRead (_analogPin);
//	More pretty stable result than "analogRead (A0)"
//	int a = getAnalog ();
	Logln (F("=> analogRead (") << _analogPin << F(") : ") << a);

	adcValues [adcIdx] = a;
	adcIdx ++;
	if (adcIdx >= NB_ADC_VALUES) adcIdx = 0;
}

//========================================================================================================================
// http://wiki.seeedstudio.com/Grove-Temperature_Sensor_V1.2/
//========================================================================================================================
void GroveTempV12 :: readAmbientTemperature ()
{
	readADCValue ();

	// Average
	double a;
	for (uint8_t i=0; i<NB_ADC_VALUES;i++) {
		a += adcValues [i];
	}
	a = a / NB_ADC_VALUES;
	a *= CORRECTIVE_VOLTAGE_DIV;

#ifdef ARDUINO_ESP8266_NODEMCU

	// These dev kits have a voltage divider on the A0 pin.
	// Le Nodemcu possède une seule entrée analogique A0 avec une conversion sur 10 bits (1024) pour une valeur d’entrée de
	// 0v à 3.3v grace a un diviseur de tension (et non pas de 0 a 1v comme à l'origine)
	// (220k + 100k ) / 100k = 3.3
	// Connecting 1V to the ADC pin of the ESP12 itself ouputs a value of 1023/1024.

	// Mesure de la tension d'alimentation pour un bon calibrage
	// https://itechnofrance.wordpress.com/2018/01/14/nodemcu-entree-analogique-a0/

	// ???
	// a /= 3.3;

//	a = round (a * 1.031);
//	Logln(F("=> analogRead corrected (") << TEMP_SENSOR_PIN << F(") : ") <<  a);

#endif

	double R = (1023.0 / a) - 1.0;

	// R = R0*R;

	// log = ln, logarithme népérien
	// Tracer la courbe sur https://www.desmos.com/calculator => 1/(ln(1023/x-1)/4275+1/298.15)-273.15
	// Quelques points : (a=536;t=27), (a=512;t=25), (a=500;t=24.1), (a=445;t=20), (a=386;t=15)
	_ambientTemperature = (1.0 / ((std::log(R /*/R0*/)/B) + (1.0/298.15))) - 273.15;		// convert to temperature via datasheet
}

//========================================================================================================================
//
//========================================================================================================================
float GroveTempV12 :: getAmbientTemperature () const
{
	return _ambientTemperature;
}

//========================================================================================================================
//
//========================================================================================================================
void GroveTempV12 :: setup ()
{
	readADCValue ();

	// Initialize adcValues
	for (int i=1; i<NB_ADC_VALUES; i++) {
		adcValues [i] = adcValues [0];
	}

	// Initialize _ambientTemperature
	readAmbientTemperature ();
}

//========================================================================================================================
//
//========================================================================================================================
void GroveTempV12 :: loop ()
{
	static unsigned long previousReadTime = 0;

	if (millis () - previousReadTime > MIN_DELAY_BETWEEN_READ_AMBIENT_TEMP_MS) {

		previousReadTime = millis ();
		readAmbientTemperature ();
	}
}