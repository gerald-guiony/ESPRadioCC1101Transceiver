//************************************************************************************************************************
// ccReplayer.cpp
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include "ccReplayer.h"


SINGLETON_IMPL (ccReplayer)


//========================================================================================================================
//
//========================================================================================================================
bool ccReplayer :: recordSignal (CC1101Transceiver * transceiver, Print & out, CCPACKET & radio) {

	if (transceiver->getLastPacketReceived ().length < 1) {
		out << F("No captured radio signal memorized, please try again later") << LN;
		return false;
	}

	radio = transceiver->getLastPacketReceived ();
	out << F("SUCCESS! Memorized captured radio signal found (") << radio.length << " bytes)" << LN;

	blinkStatus (radio.length / 10);

	// Reset packet 
	transceiver->getLastPacketReceived ().reset ();

	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool ccReplayer :: emmitSignal (CC1101Transceiver * transceiver, Print & out, CCPACKET & radio) {

	bool result = false;
	
	// **************************************************
	// TEST PACKET (radio signal was not recorded before)
	// **************************************************
	bool test = false;
	// randomSeed (analogRead(0));

	if (radio.length <= 0) {
		test = true;
		uint8_t address	= transceiver->getAddress();
		uint8_t length	= transceiver->getLength();
		if (length == 0) {
			length = random (40, CCPACKET_RXTXFIFO_DATA_LEN);	// Generate a random number between 40 and 61
		}
		radio = CCPACKET::getTestPacket (address, length);
	}
	// **************************************************

	
	if (radio.length > 0) {
		
		result = transceiver->sendPacket (radio);
		
		if (result) {
			// Visual indicator that signal sent
			blinkStatus (radio.length / 10);

			out << F("Radio signal sent (") << radio.length << " bytes)" << LN;
		}
		else {
			out << F("Fail to send radio signal !") << LN;
		}
	}
	else
	{
		out << F("No radio signal found, record one first") << LN;
	}

	// **************************************************
	if (test) radio.length = 0;
	// **************************************************

	return result;
}