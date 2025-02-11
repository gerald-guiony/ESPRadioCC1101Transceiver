//************************************************************************************************************************
// ccpacket.h
// Creation date: April, 2019
// Author: Gerald Guiony
// Forked repository:  https://github.com/SixK/CC1101-X2D-Heaters
//************************************************************************************************************************

#include <Stream.h>

#include <Common.h>

#include "ccPacket.h"

using namespace corex;


namespace cc1101 {

//========================================================================================================================
//
//========================================================================================================================
bool CCPACKET :: _printTo (Print & p) const {

	bool isValid = (0 < length) && (length <= CCPACKET_DATA_LEN);

	if (isValid) {

		p << F(PRINT_CCPACKET_DATALEN) << length << F(PRINT_CCPACKET_SEPARATOR);
		p << F(PRINT_CCPACKET_ADDRESS) << n2hexstr (address) << F(PRINT_CCPACKET_SEPARATOR);

		p << F(PRINT_CCPACKET_DATA_BEGIN) << n2hexstr (data[0]);
		for (int i = 1; i < length; i++) {
			p << F(PRINT_CCPACKET_SEPARATOR) << n2hexstr (data[i]);
		}
		p << F(PRINT_CCPACKET_DATA_END) << F(PRINT_CCPACKET_SEPARATOR);

		p << (crc_ok ? F(PRINT_CCPACKET_CRC_OK) : F(PRINT_CCPACKET_CRC_NOK)) << F(PRINT_CCPACKET_SEPARATOR);

		p << F(PRINT_CCPACKET_SIGNAL_STRENGTH) << n2hexstr (rssi) << F(PRINT_CCPACKET_SEPARATOR);
		p << F(PRINT_CCPACKET_SIGNAL_QUALITY) << n2hexstr (lqi);
	}

	return isValid;
}

//========================================================================================================================
//
//========================================================================================================================
size_t CCPACKET :: _printSize () const {

	class PrintCounter : public Print {
	public:
		size_t _counter = 0;
		virtual size_t write (uint8_t) override {
			_counter++;
			return _counter;
		}
	};

	PrintCounter printCounter;
	if (_printTo (printCounter)) {
		return printCounter._counter;
	}
	return 0;
}

//========================================================================================================================
//
//========================================================================================================================
size_t CCPACKET :: printTo (Print & p) const {

	if (!_printTo (p)) {
		Logln(F("WARNING !!! => No signal or Packet buffer is too small!!"));
	}

	return _printSize ();
}

//========================================================================================================================
//
//========================================================================================================================
bool CCPACKET :: parse (Stream & stream, Print & out) {

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_DATALEN))
	{
		out << F("Malformatted data length begin! ABORTED!!") << LN;
		return false;
	}
	length = (uint8_t) stream.parseInt();

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SEPARATOR))
	{
		out << F("Malformatted marks spaces! ABORTED!!") << LN;
		goto ERROR;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_ADDRESS))
	{
		out << F("Malformatted address begin! ABORTED!!") << LN;
		return false;
	}
	address = StreamParser::hexstr2Int (stream);

	if ((length < 1) || (CCPACKET_DATA_LEN < length))
	{
		out << F("Buffer length error! ABORTED!!") << LN;
		goto ERROR;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SEPARATOR))
	{
		out << F("Malformatted marks spaces! ABORTED!!") << LN;
		goto ERROR;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_DATA_BEGIN))
	{
		out << F("Malformatted data begin! ABORTED!!") << LN;
		return false;
	}

	data[0] = StreamParser::hexstr2Int (stream);
	for (int i = 1; i < length ; i++) {

		if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SEPARATOR))
		{
			out << F("Malformatted separator! ABORTED!!") << LN;
			goto ERROR;
		}

		data[i] = StreamParser::hexstr2Int (stream);
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_DATA_END))
	{
		out << F("Malformatted data end! ABORTED!!") << LN;
		return false;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SEPARATOR))
	{
		out << F("Malformatted separator! ABORTED!!") << LN;
		goto ERROR;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_CRC_OK) && !StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_CRC_NOK))
	{
		out << F("Malformatted CRC! ABORTED!!") << LN;
		return false;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SEPARATOR))
	{
		out << F("Malformatted separator! ABORTED!!") << LN;
		goto ERROR;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SIGNAL_STRENGTH))
	{
		out << F("Malformatted signal strength! ABORTED!!") << LN;
		goto ERROR;
	}

	rssi = StreamParser::hexstr2Int (stream);

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SEPARATOR))
	{
		out << F("Malformatted separator! ABORTED!!") << LN;
		goto ERROR;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_CCPACKET_SIGNAL_QUALITY))
	{
		out << F("Malformatted signal quality! ABORTED!!") << LN;
		goto ERROR;
	}

	lqi = StreamParser::hexstr2Int (stream);

	return true;

ERROR:
	length = 0;
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
CCPACKET & CCPACKET :: operator=(const CCPACKET & other) // copy assignment
{
	if (this != &other) { // self-assignment check expected

		address = other.address;
		length 	= other.length;
		crc_ok 	= other.crc_ok;
		rssi	= other.rssi;
		lqi		= other.lqi;

		std::copy (other.data, other.data + other.length, data);
	}
	return *this;
}

//========================================================================================================================
//
//========================================================================================================================
void CCPACKET :: reset ()
{
	address = 0;
	length 	= 0;
	crc_ok 	= false;
	rssi	= 0;
	lqi		= 0;

	memset (data, 0, CCPACKET_DATA_LEN * sizeof(uint8_t));
}

//========================================================================================================================
//
//========================================================================================================================
CCPACKET CCPACKET :: getTestPacket (uint8_t address, uint8_t length)
{
	CCPACKET packet;

	packet.address	= address;		// ou 0x00 Broadcast address
	packet.length	= length;

	for (int i=0; i<packet.length; i++)
		packet.data[i] = i;

	return packet;
}

}