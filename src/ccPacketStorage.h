//************************************************************************************************************************
// ccPacketStorage.h
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>

#include "ccPacket.h"

#define RF_HEADER_NAMEFILE					"/Rf"
#define RF_EXT_NAMEFILE						".dat"

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class ccPacketStorage
{
	SINGLETON_CLASS (ccPacketStorage)

public:
	bool 	read		(uint8_t fileId, CCPACKET & ccPacket);
	bool	write		(uint8_t fileId, CCPACKET & ccPacket);
	bool	remove		(uint8_t fileId);
	String	getList		();
};