//************************************************************************************************************************
// ccpacket.h
// Creation date: April, 2019
// Author: Gerald Guiony
// Forked repository:  https://github.com/SixK/CC1101-X2D-Heaters
//************************************************************************************************************************

#pragma once

#include <stdio.h>
#if defined (ESP8266) || defined (ESP32)
#	include <Arduino.h>
#endif

/**
 * Buffer and data lengths
 */
#define CCPACKET_RXTXFIFO_LEN			65							// Hardware buffer len
#define CCPACKET_RXTXFIFO_DATA_LEN		CCPACKET_RXTXFIFO_LEN - 4	// Len + address + 2 CRC

#define CCPACKET_DATA_LEN				100


/**
 * Packet printable
 */
#define PRINT_CCPACKET_SEPARATOR		" "
#define PRINT_CCPACKET_ADDRESS			"A-"
#define PRINT_CCPACKET_DATALEN			"L-"
#define PRINT_CCPACKET_DATA_BEGIN		"["
#define PRINT_CCPACKET_DATA_END			"]"
#define PRINT_CCPACKET_CRC_OK			"OK"
#define PRINT_CCPACKET_CRC_NOK			"NOK"
#define PRINT_CCPACKET_SIGNAL_STRENGTH	"S-"
#define PRINT_CCPACKET_SIGNAL_QUALITY	"Q-"



/**
 * Class: CCPACKET
 *
 * Description:
 * CC1101 data packet class
 */
class CCPACKET : public Printable
{
 public:
 	uint8_t length							= 0;			// Data length
	uint8_t address							= 0;			// CC recipient device ID (receiving CC1101 will filter, unless it's 0x00 or 0xFF which is a broadcast message)
	uint8_t data [CCPACKET_DATA_LEN]		= {0};			// Data buffer
	bool	crc_ok							= false;		// CRC OK flag
	uint8_t rssi							= 0;			// Received Strength Signal Indication
	uint8_t lqi								= 0;			// Link Quality Index

private:
	bool _printTo 			(Print & p) const;
	size_t _printSize		() const;

 public:

	virtual size_t printTo	(Print & p) const override;
	bool parse 				(Stream & stream, Print & out);

	CCPACKET & operator=	(const CCPACKET & other);

	template <size_t N>
	CCPACKET & operator=	(const uint8_t (&otherData) [N])
	{
		if (N > 0) {
			length 	= N;
			std::copy (otherData, otherData + length, data);
		}
		return *this;
	}

	void reset				();

	static CCPACKET getTestPacket (uint8_t address, uint8_t length);
};

