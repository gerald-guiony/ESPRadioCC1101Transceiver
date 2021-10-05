//************************************************************************************************************************
// cc1101X2dEmitter.h
// Version 1.0 May, 2021
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "cc1101X2dTransceiver.h"


/**
 * Class: CC1101X2dEmitter
 *
 * Description: cc1101 X2D packet emitter
 * CC1101X2dEmitter interface
 */
class CC1101X2dEmitter : public CC1101X2dTransceiver
{
public:

	CC1101X2dEmitter (uint8_t address = 0x5d)
		: CC1101X2dTransceiver (-1, address)
	{
	}

private:

	// Receive disabled
	// using to change the visibility of the method to private
	using  CC1101X2dTransceiver::startReceivePacket;
	using  CC1101X2dTransceiver::stopReceivePacket;
};
