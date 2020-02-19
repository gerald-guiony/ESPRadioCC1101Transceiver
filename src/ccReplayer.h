//************************************************************************************************************************
// ccReplayer.h
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>

#include "ccPacket.h"
#include "ccPacketStorage.h"

#include "cc1101Transceiver.h"


//------------------------------------------------------------------------------
//
class ccReplayer
{
	SINGLETON_CLASS (ccReplayer)

private:
	CCPACKET 					_currentRadioSignal;	// Memorized Radio Signal

private:
	bool recordSignal			(CC1101Transceiver * transceiver, Print & out, CCPACKET & radio);
	bool emmitSignal			(CC1101Transceiver * transceiver, Print & out, CCPACKET & radio);

public:
	CCPACKET & currentSignal	()												{ return _currentRadioSignal;									}

	bool recordSignal			(CC1101Transceiver * transceiver, Print & out)	{ return recordSignal (transceiver, out, _currentRadioSignal);	}
	bool emmitSignal			(CC1101Transceiver * transceiver, Print & out)	{ return emmitSignal  (transceiver, out, _currentRadioSignal);	}

	bool saveSignal 			(uint8_t id)									{ return I(ccPacketStorage).write	(id, _currentRadioSignal);	}
	bool loadSignal 			(uint8_t id)									{ return I(ccPacketStorage).read 	(id, _currentRadioSignal);	}
};




