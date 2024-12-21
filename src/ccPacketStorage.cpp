//************************************************************************************************************************
// ccPacketStorage.cpp
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include "ccPacketStorage.h"


SINGLETON_IMPL (ccPacketStorage)


//========================================================================================================================
//
//========================================================================================================================
bool ccPacketStorage :: read (uint8_t fileId, CCPACKET & ccPacket)
{
//	spiffsInfos ();

	String filename = F(RF_HEADER_NAMEFILE);
	filename += fileId;
	filename += F(RF_EXT_NAMEFILE);

	File f = LittleFS.open(filename, "r");
	if (!f) {
		Logln(F("Warning : Can't open the file : ") << filename);
		return false;
	}

	Logln(F("Open ") << filename);

	bool isRead = false;

	// returns the number of characters placed in the buffer (0 means no valid data found)
	// size_t readBytes( uint8_t *buffer, size_t length)
	if (f.readBytes ((char *) &ccPacket.length, 1) == 1) {
		if (f.readBytes ((char *) &ccPacket.address, 1) == 1) {
			if (f.readBytes ((char *) &(ccPacket.data[0]), ccPacket.length) == ccPacket.length) {
				isRead = true;
			}
		}
	}
	f.close ();

	if (isRead)
		Logln(ccPacket.length << F(" bytes Read"));
	else {
		Logln(F("ERROR : file ") << filename << F(" is invalid"));
		ccPacket.length = 0;
	}

	return isRead;
}

//========================================================================================================================
//
//========================================================================================================================
bool ccPacketStorage :: write (uint8_t fileId, CCPACKET & ccPacket)
{
	if (!FileStorage::spiffsCheckRemainingBytes ()) return false;
	if (ccPacket.length < 1) {
		Logln(F("No Rf Signal!"));
		return false;
	}

	String filename = F(RF_HEADER_NAMEFILE);
	filename += fileId;
	filename += F(RF_EXT_NAMEFILE);

	File f = LittleFS.open(filename, "w");
	if (!f) {
		Logln(F("ERROR : Can't open the file : ") << filename);
		return false;
	}

	bool isWrote = false;

	// size_t write(const uint8_t *buffer, size_t size);
	if (f.write ((const uint8_t *) &ccPacket.length, 1) == 1) {
		if (f.write ((const uint8_t *) &ccPacket.address, 1) == 1) {
			if (f.write ((const uint8_t *) &(ccPacket.data[0]), ccPacket.length) == ccPacket.length) {
				isWrote = true;
			}
		}
	}
	f.close ();

	if (isWrote)
		Logln(ccPacket.length << F(" bytes wrote"));
	else
		Logln(F("ERROR : Can't write in the file : ") << filename);

	FileStorage::spiffsInfos ();

	return isWrote;
}

//========================================================================================================================
//
//========================================================================================================================
bool ccPacketStorage :: remove (uint8_t fileId)
{
	String filename = F(RF_HEADER_NAMEFILE);
	filename += fileId;
	filename += F(RF_EXT_NAMEFILE);

	return LittleFS.remove (filename);
}

//========================================================================================================================
//
//========================================================================================================================
String ccPacketStorage :: getList ()
{
	FileStorage::spiffsListFiles ();

	String result = "";
	Dir dir = LittleFS.openDir("/");
	while (dir.next()) {
		String filename = dir.fileName();
		if (filename.indexOf (F(RF_HEADER_NAMEFILE)) == 0) {
			int lasti = filename.indexOf (F(RF_EXT_NAMEFILE));
			if (lasti > 0) {
				if (result.length()>0) {
					result += MSG_SEPARATOR_PARAM + filename.substring (strlen(RF_HEADER_NAMEFILE), lasti);
				}
				else {
					result = filename.substring (strlen(RF_HEADER_NAMEFILE), lasti);
				}
			}
		}
	}
	return result;
}

