//************************************************************************************************************************
// CC1101Transceiver.cpp
// Version 1.0 May, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <Ticker.h>

#include <Common.h>

#include "cc1101Transceiver.h"

using namespace corex;


namespace cc1101 {

Ticker receiveTicker;
std::function<void()> callBakFunction;

//========================================================================================================================
//
//========================================================================================================================
CC1101Transceiver :: CC1101Transceiver (uint8_t irqPin, uint8_t address, uint8_t length)
	: CC1101 (irqPin), _address (address), _len (length)
{
}

//========================================================================================================================
//
//========================================================================================================================
void CC1101Transceiver :: startSendPacket ()
{
	Logln(F("--------- CC1101 Starting to send packet --------- "));

//	setDevAddress		(0x56);								// Filter address (réception des messages qui commencent uniquement par cette adresse) Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).

	printCurrentSettings();
}

//========================================================================================================================
//
//========================================================================================================================
bool CC1101Transceiver :: sendPacket (CCPACKET & packet)
{
	stopReceivePacket ();
	startSendPacket ();

	bool result = sendCCPacket (packet);

	// Return back in Rx state after 100ms
	startReceivePacket ();

	return result;
}

//========================================================================================================================
//
//========================================================================================================================
bool CC1101Transceiver :: sendPackets (CCPACKET * packets, uint8_t nbPackets)
{
	stopReceivePacket ();
	startSendPacket ();

	bool result = true;
	for (int i=0; i<nbPackets; i++)
	{
		result &= sendCCPacket (packets[i]);
	}

	// Return back in Rx state after 100ms
	startReceivePacket ();

	return result;
}

//========================================================================================================================
// Interrupt Service Routines (ISR) handler has to be marked with ICACHE_RAM_ATTR
//========================================================================================================================
#if defined (ESP8266) || defined (ESP32)
void IRAM_ATTR _ISR_cc1101_irq_pin ()
#else
void _ISR_cc1101_irq_pin ()
#endif
{
	Logln(F(":O"));
	receiveTicker.once_ms (50, callBakFunction);
}

//========================================================================================================================
//
//========================================================================================================================
void CC1101Transceiver :: startReceivePacket (uint8_t delayMs /*= 100 */)
{
	if (delayMs > 0) {
		receiveTicker.once_ms (delayMs, std::bind (&CC1101Transceiver::startReceivePacket, this, 0));
	}
	else {

		Logln(F("--------- CC1101 Starting to receive packet --------- "));

		stopReceivePacket	();

	//	setDevAddress		(0x55);								// Filter address (réception des messages qui commencent uniquement par cette adresse)
	//	writeReg			(CC1101_PKTLEN,	_recvPacketLength);	// The PKTLEN register is used to set the maximum packet length allowed in RX. Fixed bytes message length

		// Continue settings..
		continueReceivePacket ();

		printCurrentSettings ();
	}
}

//========================================================================================================================
//
//========================================================================================================================
void CC1101Transceiver :: continueReceivePacket ()
{
	uint8_t marcState;

	setIdleState 		();
	setRxState			(); 								// Switch to RX state

	// Check that the RX state has been entered
	while (((marcState = readStatusReg (CC1101_MARCSTATE)) & CC1101_BITS_MARCSTATE) != CC_MARCSTATE_RX)	{
		Logln (F("MarcState not in RX State !"));
		if (marcState == CC_MARCSTATE_RXFIFO_OVERFLOW) {	// RX_OVERFLOW
			Logln 	(F("=> Flushing RX FIFO"));
			flushRxFifo 	(); 							// Flush RX buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states.
			printFIFOState 	();
		}
		EspBoard::asyncDelayMillis(10);
		setRxState					();						// Switch to RX state
	}


	Logln (F("Attaching Interrupt"));
	callBakFunction = std::bind (&CC1101Transceiver::checkNewPacketReceived, this);
	attachInterrupt (_irqPin, _ISR_cc1101_irq_pin, RISING);
}

//========================================================================================================================
//
//========================================================================================================================
void CC1101Transceiver :: stopReceivePacket ()
{
	Logln (F("Detaching Interrupt"));
	detachInterrupt (_irqPin);
}

//========================================================================================================================
//
//========================================================================================================================
uint8_t CC1101Transceiver :: receivePacket	(CCPACKET & packet)
{
	stopReceivePacket ();

	uint8_t nbBytesReceived = receiveCCPacket (packet);

	continueReceivePacket ();

	return nbBytesReceived;
}

//========================================================================================================================
//
//========================================================================================================================
bool CC1101Transceiver :: checkNewPacketReceived () {
	return (receivePacket (_lastPacketReceived) > 0);
}

}