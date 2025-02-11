//************************************************************************************************************************
// cc1101.cpp
// Creation date: April, 2019
// Author: Gerald Guiony
// Forked repository:  https://github.com/SixK/CC1101-X2D-Heaters
//************************************************************************************************************************

#include <Common.h>

#include "cc1101.h"

using namespace corex;


namespace cc1101 {

//========================================================================================================================
// default constructor
//========================================================================================================================
CC1101::CC1101 (uint8_t irqPin /*= -1*/) :
	_irqPin (irqPin)
{
	// Initialize the SPI pins
	pinMode (SCK,	OUTPUT);
	pinMode (MOSI,	OUTPUT);
	pinMode (MISO,	INPUT);
	pinMode (SS,	OUTPUT);

	SPI.begin ();							// Initialize SPI interface

	if (_irqPin != -1) {
		pinMode (_irqPin, INPUT);			// Config GDO2 as input
	}

	hardReset ();							// Reset CC1101
}

//========================================================================================================================
// default destructor
//========================================================================================================================
CC1101::~CC1101 ()
{
	// Disable SPI
	SPI.endTransaction ();
	SPI.end ();
}

//========================================================================================================================
//
//========================================================================================================================
void CC1101::wait_Miso () const
{
	while (digitalRead(MISO) == HIGH) {EspBoard::asyncDelayMillis(10);}
}

//========================================================================================================================
//	hard reset after power on
//	Ref: http://e2e.ti.com/support/wireless-connectivity/other-wireless/f/667/t/396609
//	Reset CC1101
//========================================================================================================================
void CC1101::hardReset (void)
{
	Logln (F("CC1101 Hardreset"));

	deselect			();					// Deselect CC1101
	delayMicroseconds	(5);
	select				();					// Select CC1101
	delayMicroseconds	(10);
	deselect			();					// Deselect CC1101
	delayMicroseconds	(45);
	select				();					// Select CC1101

	softReset			();
}

//========================================================================================================================
// soft reset
//
// Reset CC1101
//========================================================================================================================
void CC1101::softReset (void)
{
	select				();					// Select CC1101
	wait_Miso			();					// Wait until MISO goes low
	SPI.transfer		(CC1101_SRES);		// Send reset command strobe
	wait_Miso			();					// Wait until MISO goes low
	deselect			();					// Deselect CC1101
}

//========================================================================================================================
// wakeUp
//
// Wake up CC1101 from Power Down state
//========================================================================================================================
void CC1101::wakeUp (void)
{
	select				();					// Select CC1101
	wait_Miso			();					// Wait until MISO goes low
	deselect			();					// Deselect CC1101
}

//========================================================================================================================
// setPowerDownState
//
// Put CC1101 into power-down state
//========================================================================================================================
void CC1101::setPowerDownState (void)
{
	cmdStrobe			(CC1101_SIDLE);		// Comming from RX state, we need to enter the IDLE state first
	cmdStrobe			(CC1101_SPWD);		// Enter Power-down state
}


//========================================================================================================================
// cmdStrobe => writeCommand
//
// Send command strobe to the CC1101 IC via SPI
//
// 'cmd'	Command strobe
//========================================================================================================================
void CC1101::cmdStrobe (uint8_t cmd)
{
	uint8_t sta;
	Logln(F("Sending strobe: "));
	switch (cmd)
	{
		case CC1101_SRES	: Logln (F("CC1101_SRES		")); break;
		case CC1101_SFSTXON	: Logln (F("CC1101_SFSTXON	")); break;
		case CC1101_SXOFF	: Logln (F("CC1101_SXOFF		")); break;
		case CC1101_SCAL	: Logln (F("CC1101_SCAL		")); break;
		case CC1101_SRX		: Logln (F("CC1101_SRX		")); break;
		case CC1101_STX		: Logln (F("CC1101_STX		")); break;
		case CC1101_SIDLE	: Logln (F("CC1101_SIDLE		")); break;
		case CC1101_SWOR	: Logln (F("CC1101_SWOR		")); break;
		case CC1101_SPWD	: Logln (F("CC1101_SPWD		")); break;
		case CC1101_SFRX	: Logln (F("CC1101_SFRX		")); break;
		case CC1101_SFTX	: Logln (F("CC1101_SFTX		")); break;
		case CC1101_SWORRST : Logln (F("CC1101_SWORRST	")); break;
		case CC1101_SNOP	: Logln (F("CC1101_SNOP		")); break;
	}

	select				();					// Select CC1101
	wait_Miso			();					// Wait until MISO goes low
	sta = SPI.transfer	(cmd);				// Send strobe command
	deselect			();					// Deselect CC1101

	printState 			(sta);
}

//========================================================================================================================
// writeReg
//
// Write single register into the CC1101 IC via SPI
//
// 'regAddr'	Register address
// 'value'		Value to be writen
//========================================================================================================================
void CC1101::writeReg (uint8_t regAddr, uint8_t value)
{
	uint8_t sta;

	// Print extra info when we're writing to CC register
	if (regAddr < NUM_CONFIG_REGISTERS)
	{
		Logln(	F("Writing to CC1101 reg ") << FPSTR(CC1101_CONFIG_REGISTER_NAME[regAddr]) <<
										F(" [") << String (regAddr, HEX) << F("] value (HEX):") << String (value, HEX));
	}

	select						();					// Select CC1101
	wait_Miso					();					// Wait until MISO goes low
	SPI.transfer				(regAddr);			// Send register address
	EspBoard::asyncDelayMillis	(1);				// HACK
	sta = SPI.transfer			(value);			// Send value
	deselect					();					// Deselect CC1101

	printState					(sta);
}

//========================================================================================================================
// readReg
//
// Read CC1101 register via SPI
//
// 'regAddr'	Register address
//
// Return:
// 	Data uint8_t returned by the CC1101 IC
//========================================================================================================================
uint8_t CC1101::readReg (uint8_t addr) const
{
	uint8_t val;

	select						();					// Select CC1101
	wait_Miso					();					// Wait until MISO goes low
	SPI.transfer				(addr);				// Send register address
	EspBoard::asyncDelayMillis	(1);				// HACK
	val = SPI.transfer			(0x00);				// Read result
	deselect					();					// Deselect CC1101

	return val;
}

//========================================================================================================================
//	readRegWithSyncProblem(uint8_t address, uint8_t registerType)
//	CC1101 bug with SPI and return values of Status Registers
//	https://e2e.ti.com/support/wireless-connectivity/sub-1-ghz/f/156/t/570498?CC1101-stuck-waiting-for-CC1101-to-bring-GDO0-low-with-IOCFG0-0x06-why-#
//	as per: http://e2e.ti.com/support/wireless-connectivity/other-wireless/f/667/t/334528?CC1101-Random-RX-FIFO-Overflow
//
// Known SPI/26MHz synchronization bug (see CC1101 errata)
// This issue affects the following registers: SPI status byte (fields STATE and FIFO_BYTES_AVAILABLE),
// FREQEST or RSSI while the receiver is active, MARCSTATE at any time other than an IDLE radio state,
// RXBYTES when receiving or TXBYTES when transmitting, and WORTIME1/WORTIME0 at any time.*/
//========================================================================================================================
uint8_t CC1101::readRegWithSyncProblem (uint8_t address, uint8_t registerType) const
{
	uint8_t value1, value2;

	value1 = readReg (address | registerType);

	// If two consecutive reads gives us the same result then we know we are ok
	do
	{
		value2 = value1;
		EspBoard::asyncDelayMillis (10);
		value1 = readReg (address | registerType);
	}
	while (value1 != value2);

	return value1;
}

//========================================================================================================================
// readReg
//
// Read CC1101 register via SPI
//
// 'regAddr'	Register address
// 'regType'	Type of register: CC1101_CONFIG_REGISTER or CC1101_STATUS_REGISTER
//
// Return:
// 	Data uint8_t returned by the CC1101 IC
//========================================================================================================================
uint8_t CC1101::readReg (uint8_t address, uint8_t registerType) const
{
	switch (address)
	{
		case CC1101_FREQEST:
		case CC1101_MARCSTATE:
		case CC1101_RXBYTES:
		case CC1101_TXBYTES:
		case CC1101_WORTIME1:
		case CC1101_WORTIME0:
			return readRegWithSyncProblem (address, registerType);

		default:
			return readReg (address | registerType);
	}
}

//========================================================================================================================
// writeBurstReg
//
// Write multiple registers into the CC1101 IC via SPI
//
// 'regAddr'	Register address
// 'buffer'	Data to be writen
// 'len'	Data length
//========================================================================================================================
void CC1101::writeBurstReg (uint8_t regAddr, const uint8_t* buffer, const uint8_t len)
/*
{
	uint8_t addr, i;

	addr = regAddr | WRITE_BURST;			// Enable burst transfer
	select			();						// Select CC1101
	wait_Miso		();						// Wait until MISO goes low
	printState	(SPI.transfer(addr));	// Send register address

	for (i=0; i<len; i++)
		SPI.transfer(buffer[i]);			// Send value

	deselect		();						// Deselect CC1101
}
*/
{
	// ISSUE: writeBurstReg doesn't work properly on ESP8266, or perhaps
	//	 other microcontrollers... perhaps they are too fast for the CC1101
	//	 given it's a 10+ year old chip.
	//	 Sending each byte individually works without issue however.
	//	 https://e2e.ti.com/support/wireless-connectivity/sub-1-ghz/f/156/t/554535
	//	 https://e2e.ti.com/support/microcontrollers/other/f/908/t/217117

	// Send the contents of th RX/TX buffer to the CC1101, one byte at a time
	// the receiving CC1101 will append two bytes for the LQI and RSSI
	for (int i = 0; i< len; i++)
		writeReg (regAddr,	buffer[i]);
}


//========================================================================================================================
// readBurstReg
//
// Read burst data from CC1101 via SPI
//
// 'buffer'	Buffer where to copy the result to
// 'regAddr'	Register address
// 'len'	Data length
//========================================================================================================================
void CC1101::readBurstReg (uint8_t * buffer, uint8_t regAddr, uint8_t len)
/*
{
	uint8_t addr, i;

	addr = regAddr | READ_BURST;
	select			();						// Select CC1101
	wait_Miso		();						// Wait until MISO goes low
	SPI.transfer	(addr);					// Send register address
	for (i=0 ; i<len ; i++)
		buffer[i] = SPI.transfer (0x00);	// Read result uint8_t by uint8_t
	deselect();								// Deselect CC1101
}
*/
{
	for (int i = 0; i< len; i++)
		buffer[i] = readConfigReg (regAddr);
}

//========================================================================================================================
// setSyncWord (overriding method)
//
// Set synchronization word
//
// 'syncH'	Synchronization word - pointer to 2-uint8_t array
//========================================================================================================================
void CC1101::setSyncWord (uint8_t sync1, uint8_t sync0)
{
	writeReg (CC1101_SYNC1, sync1);
	writeReg (CC1101_SYNC0, sync0);
}

//========================================================================================================================
// setDevAddress
//
// Set device address
//
// @param addr	Device address
//========================================================================================================================
void CC1101::setDevAddress (uint8_t addr)
{
	writeReg (CC1101_ADDR, addr);
}

//========================================================================================================================
// setChannel
//
// Set frequency channel
//
// 'chnl'	Frequency channel
//========================================================================================================================
void CC1101::setChannel (uint8_t chnl)
{
	writeReg (CC1101_CHANNR,	chnl);
}

//========================================================================================================================
// setDataRate
//
// Data Rate - details extracted from SmartRF Studio
//
// 'chnl'	Frequency channel
//========================================================================================================================
void CC1101::setDataRate (DATA_RATE dataRate)
{
	// Data Rate - details extracted from SmartRF Studio
	switch (dataRate)
	{
		case KBPS_250:

			Logln (F("250kbps data rate"));

			writeReg (CC1101_FSCTRL1,	0x0C); // Frequency Synthesizer Control (optimised for sensitivity)
			writeReg (CC1101_MDMCFG4,	0x2D); // Modem Configuration
			writeReg (CC1101_MDMCFG3,	0x3B); // Modem Configuration
			writeReg (CC1101_DEVIATN,	0x62); // Modem Deviation Setting
			writeReg (CC1101_FOCCFG,	0x1D); // Frequency Offset Compensation Configuration
			writeReg (CC1101_BSCFG,		0x1C); // Bit Synchronization Configuration
			writeReg (CC1101_AGCCTRL2,	0xC7); // AGC Control
			writeReg (CC1101_AGCCTRL1,	0x00); // AGC Control
			writeReg (CC1101_AGCCTRL0,	0xB0); // AGC Control
			writeReg (CC1101_FREND1,	0xB6); // Front End RX Configuration
			break;

		case KBPS_38:

			Logln (F("38kbps data rate"));

			writeReg (CC1101_FSCTRL1,	0x06); // Frequency Synthesizer Control
			writeReg (CC1101_MDMCFG4,	0xCA); // Modem Configuration
			writeReg (CC1101_MDMCFG3,	0x83); // Modem Configuration
			writeReg (CC1101_DEVIATN,	0x35); // Modem Deviation Setting
			writeReg (CC1101_FOCCFG,	0x16); // Frequency Offset Compensation Configuration
			writeReg (CC1101_AGCCTRL2,	0x43); // AGC Control
			break;

		case KBPS_4:

			Logln (F("4kbps data rate"));

			writeReg (CC1101_FSCTRL1,	0x06); // Frequency Synthesizer Control
			writeReg (CC1101_MDMCFG4,	0xC7); // Modem Configuration
			writeReg (CC1101_MDMCFG3,	0x83); // Modem Configuration
			writeReg (CC1101_DEVIATN,	0x40); // Modem Deviation Setting
			writeReg (CC1101_FOCCFG,	0x16); // Frequency Offset Compensation Configuration
			writeReg (CC1101_AGCCTRL2,	0x43); // AGC Control
			break;
	}
}

//========================================================================================================================
// setCarrierFreq
//
// Set carrier frequency
//
// 'freq'	New carrier frequency
//========================================================================================================================
void CC1101::setCarrierFreq (CFREQ freq)
{
	switch(freq)
	{
		case CFREQ_433:

			Logln (F("Carrier frequency 433 MHz"));

			writeReg (CC1101_FREQ2,	0x10);
			writeReg (CC1101_FREQ1,	0xA7);
			writeReg (CC1101_FREQ0,	0x62);
			break;

		case CFREQ_915: // 902 Mhz ??

			Logln (F("Carrier frequency 915 MHz"));

			writeReg (CC1101_FREQ2,	0x22);
			writeReg (CC1101_FREQ1,	0xB1);
			writeReg (CC1101_FREQ0,	0x3B);
			break;

		case CFREQ_918:

			Logln (F("Carrier frequency 918 MHz"));

			writeReg (CC1101_FREQ2,	0x23);
			writeReg (CC1101_FREQ1,	0x4E);
			writeReg (CC1101_FREQ0,	0xC4);
			break;

		default:

			Logln (F("Carrier frequency 868 MHz"));

			writeReg (CC1101_FREQ2,	0x21);
			writeReg (CC1101_FREQ1,	0x65);
			//writeReg (CC1101_FREQ0,	0xC4); X2D ??
			writeReg (CC1101_FREQ0,	0x6A);
			break;
	}
}

//===================================================================================================================
// Configure the packet length
//===================================================================================================================
bool CC1101::isFixedPacketLength () const
{
	return ((readConfigReg (CC1101_PKTCTRL0) & 0x03) == 0x00);
}

//===================================================================================================================
// Controls address check configuration of received packages.
//===================================================================================================================
bool CC1101::isAddressCheck () const
{
	return ((readConfigReg (CC1101_PKTCTRL1) & 0x03) != 0x00);
}

//===================================================================================================================
// Indicates the packet length when fixed packet length mode is enabled. If variable packet length mode is used,
// this value indicates the maximum packet length allowed. This value must be different from 0.
//===================================================================================================================
uint8_t CC1101::getFixedPacketLength () const
{
	return readConfigReg (CC1101_PKTLEN);
}

//===================================================================================================================
// When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and
// LQI values, as well as CRC OK.
//===================================================================================================================
bool CC1101::isRssiLqiCrc () const
{
	return ((readConfigReg (CC1101_PKTCTRL1) & 0x04) != 0x00);
}

//===================================================================================================================
//	sendPacket
//
//	Send data packet via RF
//
//	'packet' Packet to be transmitted. First byte is the destination address
//
//	Return:
//			True if the transmission succeeds
//			False otherwise
//===================================================================================================================
bool CC1101::sendCCPacket (CCPACKET & packet)
{
	uint8_t marcState;

	Logln (F("--------- CC1101 send packet --------- "));

	if (packet.length == 0) return false;

	// ===================================================================================================
	// Check to see if stuff is already in the TX FIFO. If so. Flush it.

	// Vide TX FIFO au cas où...
	setTxState		();

	// Any left over bytes in the TX FIFO ?
	uint8_t txStatus = readStatusReg (CC1101_TXBYTES);

	if (txStatus & CC1101_TX_FIFO_UNDERFLOW) { 		// Clear TX fifo if needed

		Logln (F("TX FIFO is in overflow or contains garbage. Flushing. "));
		flushTxFifo		();							// Flush Tx FIFO. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states.
	}

	setIdleState	();								// Enter IDLE state

	// ===================================================================================================
	// Send the radio packet.

	/*
	15.4 Packet Handling in Transmit Mode
	The payload that is to be transmitted must be written into the TX FIFO. The first byte written must be
	the length byte when variable packet length is enabled. The length byte has a value equal to the payload
	of the packet (including the optional address byte). If address recognition is enabled on the receiver,
	the second byte written to the TX FIFO must be the address byte.
	If fixed packet length is enabled, the first byte written to the TX FIFO should be the address (assuming
	the receiver uses address recognition).
	*/
	// (Preamble bits) (Sync word) (Optional length byte (+1 if address)) (Optional address byte) [Payload] (optional RSSI) (optional LQI+CRCbit)

	bool isFixedLength	= isFixedPacketLength ();
	bool isAddrCheck	= isAddressCheck ();

	Logln (F("*** Writing packet: (") << packet << F(") to TX FIFO ***"));

	if (!isFixedLength) {
		// Packet length configured by the first byte after sync word
		writeReg (CC1101_TXFIFO, packet.length + (isAddrCheck ? 1 : 0));
	}

	if (isAddrCheck) {
		// Set address
		writeReg (CC1101_TXFIFO, packet.address);
	}

	uint8_t len = (packet.length > CCPACKET_RXTXFIFO_DATA_LEN) ? CCPACKET_RXTXFIFO_DATA_LEN : packet.length;
	uint8_t index = len;

	// Write data into the TX FIFO
	writeBurstReg	(CC1101_TXFIFO, packet.data, len);

	printFIFOState	();

	setTxState		();								// Start sending packet

	// If Packet length > TX FIFO => write pending bytes in TX FIFO during transmit
	while (packet.length > index) {

		uint8_t txBytes = readStatusReg (CC1101_TXBYTES) & CC1101_BYTES_IN_FIFO;
		uint8_t freeBytes = CCPACKET_RXTXFIFO_DATA_LEN - txBytes;						// Nb free bytes in TX FIFO
		len = (packet.length - index < freeBytes) ? packet.length - index : freeBytes;

		// Write pending data into the TX FIFO
		writeBurstReg (CC1101_TXFIFO, &(packet.data [index]), len);

		index += len;
	}

	// Wait until transmission is finished (TXOFF_MODE is expected to be set to 0/IDLE or TXFIFO_UNDERFLOW)
	do
	{
		marcState = (readStatusReg (CC1101_MARCSTATE) & CC1101_BITS_MARCSTATE);
	}
	while ((marcState != CC_MARCSTATE_IDLE) && (marcState != CC_MARCSTATE_TXFIFO_UNDERFLOW));

	printFIFOState	();

	// Check that the TX FIFO is empty
 	if ((readStatusReg (CC1101_TXBYTES) & CC1101_BYTES_IN_FIFO) == 0)
 		return true;

	return false;
}

//===================================================================================================================
//	receivePacket
//
//	Read data packet from RX FIFO
//
// 'packet' Container for the packet received
//
//	Return:
//	 Amount of bytes received
//===================================================================================================================
uint8_t CC1101::receiveCCPacket (CCPACKET & packet)	// if data available
{
	Logln (F("--------- CC1101 receive packet --------- "));

	uint8_t rxBytes = readStatusReg (CC1101_RXBYTES);
	if (rxBytes & CC1101_RX_FIFO_OVERFLOW) Logln (F("* RX FIFO OVERFLOW !!"));

	uint8_t rxBytesPending = rxBytes & CC1101_BYTES_IN_FIFO;
	Logln (F("* RX FIFO bytes pending read: ") << rxBytesPending);

	if (rxBytesPending)
	{
		// Check for rx fifo overflow
		bool isOverFlow = ((readStatusReg (CC1101_MARCSTATE) & CC1101_BITS_MARCSTATE) == CC_MARCSTATE_RXFIFO_OVERFLOW);

		// (Preamble bits) (Sync word) (Optional length byte (+1 if address)) (Optional address byte) [Payload] (optional RSSI) (optional LQI+CRCbit)

		if (isOverFlow) {
			packet.length = CCPACKET_RXTXFIFO_DATA_LEN;
			// Processed but not removed in RX
			uint8_t len = readConfigReg (CC1101_RXFIFO);
			Logln (F("Receiving overflow length: ") << len);
		}
		else if (isFixedPacketLength()) {
			packet.length = getFixedPacketLength ();
		}
		else {
			// Processed but not removed in RX
			packet.length = readConfigReg (CC1101_RXFIFO);
		}

		if (isAddressCheck()) {
			// Processed but not removed in RX
			packet.address = readConfigReg (CC1101_RXFIFO);
			packet.length -= 1;
		}

		if (packet.length > rxBytesPending) {
			Logln (F("/!\\ Receiving packet length doesn't match: Packet length (") << packet.length << F(") > RX Fifo bytes (") << rxBytesPending << F(")"));
			packet.length = rxBytesPending;
		}

		// Read data packet
		readBurstReg (packet.data, CC1101_RXFIFO, packet.length);

		if (isRssiLqiCrc ()) {

			// Read RSSI
			packet.rssi = readConfigReg (CC1101_RXFIFO);
			//packet.rssi = readStatusReg (CC1101_RSSI);

			// Read LQI and CRC_OK
			uint8_t val = readConfigReg (CC1101_RXFIFO);
			//uint8_t val = readStatusReg (CC1101_LQI)
			packet.lqi = val & 0x7F;
			packet.crc_ok = bitRead (val, 7);
		}

		Logln (F("*** Receiving packet: (") << packet << F(") from RX FIFO ***"));

		printLQI_RSSI ();
	}
	else
		rxBytesPending = 0;

	// Continue RX
	setIdleState	();					// Idle
	flushRxFifo		();					// Flush RX buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states.
	setRxState		();					// Switch to RX state

	return rxBytesPending;
}

//===================================================================================================================
// explainStatus
//===================================================================================================================
void CC1101::printState (uint8_t status)
{
	/*
	10.1 Chip Status Byte
	When the header byte, data byte, or command strobe is sent on the SPI interface, the chip status byte is sent by
	the CC1101 on the SO pin. The status byte contains key status signals, useful for the MCU. The first bit, s7, is the
	CHIP_RDYn signal and this signal must go low before the first positive edge of SCLK. The CHIP_RDYn signal indicates
	that the crystal is running.
	*/

	// Data is MSB, so get rid of the fifo bytes, extract the three we care about.
	// https://stackoverflow.com/questions/141525/what-are-bitwise-shift-bit-shift-operators-and-how-do-they-work
	_currentState = static_cast<CC_STATE> (status & 0b01110000);

	// TODO: Do something when we hit OVERFLOW / UNDERFLOW STATE.

	// Refer to page 31 of cc1101.pdf
	// Bit 7	= CHIP_RDY
	// Bit 6:4	= STATE[2:0]
	// Bit 3:0	= FIFO_BYTES_AVAILABLE[3:0]
	if ((0b10000000 & status) != 0x00) // is bit 7 0 (low)
	{
		Logln (F("SPI Result: FAIL: CHIP_RDY is LOW! The CC1101 isn't happy. Has a over/underflow occured?"));
	}

	if (_lastState != _currentState)
	{
		switch (_currentState)
		{
			case (CC_STATE_IDLE):			Logln (F("STATE_IDLE			"));	break;
			case (CC_STATE_RX):				Logln (F("STATE_RX				"));	break;
			case (CC_STATE_TX):				Logln (F("STATE_TX				"));	break;
			case (CC_STATE_FSTXON):			Logln (F("STATE_FSTXON			"));	break;
			case (CC_STATE_CALIBRATE):		Logln (F("STATE_CALIBRATE		"));	break;
			case (CC_STATE_SETTLING):		Logln (F("STATE_SETTLING		"));	break;
			case (CC_STATE_RX_OVERFLOW):	Logln (F("STATE_RXFIFO_OVERFLOW	"));	break;
			case (CC_STATE_TX_UNDERFLOW):	Logln (F("STATE_TXFIFO_UNDERFLOW"));	break;
			default:						Logln (F("UNKNOWN STATE			"));
		}
	}

	printMarcstate ();

	uint8_t bytesFree = 0b00001111 & status;
	if (bytesFree < 15) Logln (F("SPI Result: Bytes free in FIFO: ") << String (bytesFree, DEC));

	_lastState = _currentState;
}

//===================================================================================================================
//
//===================================================================================================================
void CC1101::printCurrentSettings (void)
{
	Logln (F("========== CC1101 Current settings ========== "));

	Logln (F("PARTNUM ")	<< readStatusReg (CC1101_PARTNUM));
	Logln (F("VERSION ")	<< readStatusReg (CC1101_VERSION));

	printRegisterConfiguration ();

	printMarcstate ();

	printFIFOState ();
	printGD0xStatus ();
}

//===================================================================================================================
//
//===================================================================================================================
void CC1101::printRegisterConfiguration (void)
{
	Logln (F("--------- Register Configuration Dump --------- "));

	byte reg_value		= 0;

	for (uint8_t i = 0; i < NUM_CONFIG_REGISTERS; i++)
	{
		reg_value = readReg(i, CC1101_CONFIG_REGISTER);
		Logln (F("Reg ") << FPSTR(CC1101_CONFIG_REGISTER_NAME[i]) << F(" ( ") << String (i, HEX) << F(" ) = ") << String (reg_value, HEX));

		EspBoard::asyncDelayMillis (10);
	}
}

//===================================================================================================================
// Marcstate (Main Radio Control State Machine State)
//===================================================================================================================
void CC1101::printMarcstate (void)
{
	_currentMarcState = static_cast<CC_MARCSTATE> (readStatusReg (CC1101_MARCSTATE) & CC1101_BITS_MARCSTATE);

	if (_lastMarcState != _currentMarcState)
	{
		Logln (F("Marcstate: "));
		switch (_currentMarcState)
		{
			case 0x00: Logln (F("SLEEP SLEEP					")); break;
			case 0x01: Logln (F("IDLE IDLE						")); break;
			case 0x02: Logln (F("XOFF XOFF						")); break;
			case 0x03: Logln (F("VCOON_MC MANCAL				")); break;
			case 0x04: Logln (F("REGON_MC MANCAL				")); break;
			case 0x05: Logln (F("MANCAL MANCAL					")); break;
			case 0x06: Logln (F("VCOON FS_WAKEUP				")); break;
			case 0x07: Logln (F("REGON FS_WAKEUP				")); break;
			case 0x08: Logln (F("STARTCAL CALIBRATE				")); break;
			case 0x09: Logln (F("BWBOOST SETTLING				")); break;
			case 0x0A: Logln (F("FS_LOCK SETTLING				")); break;
			case 0x0B: Logln (F("IFADCON SETTLING				")); break;
			case 0x0C: Logln (F("ENDCAL CALIBRATE				")); break;
			case 0x0D: Logln (F("RX RX							")); break;
			case 0x0E: Logln (F("RX_END RX						")); break;
			case 0x0F: Logln (F("RX_RST RX						")); break;
			case 0x10: Logln (F("TXRX_SWITCH TXRX_SETTLING		")); break;
			case 0x11: Logln (F("RXFIFO_OVERFLOW RXFIFO_OVERFLOW")); break;
			case 0x12: Logln (F("FSTXON FSTXON					")); break;
			case 0x13: Logln (F("TX TX							")); break;
			case 0x14: Logln (F("TX_END TX						")); break;
			case 0x15: Logln (F("RXTX_SWITCH RXTX_SETTLING		")); break;
			case 0x16: Logln (F("TXFIFO_UNDERFLOW TXFIFO_UNDERFLOW")); break;
		}
	}

	_lastMarcState = _currentMarcState;
}

//===================================================================================================================
//
//===================================================================================================================
void CC1101::printFIFOState (void)
{
	uint8_t rxBytes = readStatusReg (CC1101_RXBYTES) & CC1101_BYTES_IN_FIFO;
	uint8_t txBytes = readStatusReg (CC1101_TXBYTES) & CC1101_BYTES_IN_FIFO;

	Logln(F("Nb bytes in RX FIFO: ") << rxBytes << F(" in TX FIFO: ") << txBytes);
}

//===================================================================================================================
// p92
//===================================================================================================================
void CC1101::printLQI_RSSI (void)
{
	uint8_t quality = readStatusReg (CC1101_LQI);
	uint8_t lqi = quality & 0x7F;
	bool crc = bitRead (quality, 7);

	uint8_t sinalStrength = readStatusReg (CC1101_RSSI);

	Logln (F("CRC: ") << crc << F(" RSSI: ") << sinalStrength << F(" LQI: ") << lqi);
}

//===================================================================================================================
// p94
//===================================================================================================================
void CC1101::printGD0xStatus (void)
{
	uint8_t pktStatus = readStatusReg (CC1101_PKTSTATUS);
	bool gdo2 = pktStatus & 0b00000100;
	bool gdo0 = pktStatus & 0b00000001;

	Logln (F("GDO2: ") << gdo2 << F(" GDO0: ") << gdo0);
}

}