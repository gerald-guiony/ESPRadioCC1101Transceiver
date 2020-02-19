//************************************************************************************************************************
// cc1101Transceiver.h
// Version 1.0 May, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "cc1101.h"

/**
 * Class: CC1101Transceiver
 * 
 * Description:
 * CC1101Transceiver interface
 */
class CC1101Transceiver : public CC1101
{
protected:

	uint8_t _address;
	uint8_t	_len;
	
protected:

	virtual void initRegisters			() = 0;

	virtual void startSendPacket		();
	
	uint8_t receivePacket				(CCPACKET & packet);
	virtual void continueReceivePacket	();
	
	bool checkNewPacketReceived			();

public:

	CC1101Transceiver 					(uint8_t irqPin, uint8_t address, uint8_t length);

	virtual uint8_t getAddress			() const { return _address; }
	virtual uint8_t getLength			() const { return _len - (isAddressCheck () ? 1 : 0); }	

	virtual bool sendPacket 			(CCPACKET & packet) override;
	virtual bool sendPackets			(CCPACKET * packets, uint8_t nbPackets);

	virtual void startReceivePacket		(uint8_t delayMs = 100) override;
	virtual void stopReceivePacket		() override;
};