
//************************************************************************************************************************
// cc1101FixedLenTransceiver.h
// Version 1.0 May, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "cc1101Transceiver.h"

namespace cc1101 {

/**
 * Class: CC1101FixedLenTransceiver
 *
 * Description: cc1101 fixed packet length transceiver
 * CC1101FixedLenTransceiver interface
 */
class CC1101FixedLenTransceiver : public CC1101Transceiver
{
public:

	CC1101FixedLenTransceiver (uint8_t irqPin, uint8_t address = 0x56, uint8_t length = 60)
		: CC1101Transceiver (irqPin, address, length)
	{
		initRegisters 		();
		startReceivePacket	();
	}

protected:

	virtual void initRegisters	(void) override
	{
		/**
		 * Configuration:
		 *
		 * Channel number = 0
		 * Modulation format = GFSK
		 * Manchester enable = false
		 * Data whitening = on
		 * Sync word qualifier mode = 30/32 sync word bits detected
		 * Preamble count = 4
		 * Carrier frequency = 433
		 * Data rate = 38 Kbps
		 * Data format = Normal mode
		 * Length config = Fixed packet length mode
		 * CRC enable = true
		 * Packet length = 60
		 * Device address = 0x56
		 * Address config = Enable address check
		 * Append status = Append two status bytes to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK
		 * CRC autoflush = false
		 */

		setCarrierFreq		(CFREQ_433);						// 433.0198 pour ip 14 et 433.0184 pour ip 12
		setDataRate			(KBPS_38);
		setChannel			(0x00);
		setSyncWord			(0x47, 0xB5);
		setDevAddress		(_address);							// Optional broadcast addresses are 0 (0x00) and 255 (0xFF).

		writeReg			(CC1101_PKTLEN,		_len);			// The PKTLEN register is used to set the maximum packet length allowed in RX : 59 + 1 (address)

		writeReg			(CC1101_MCSM0,		0x18);			// 00011000	Main Radio Control State Machine configuration : Auto calibrate When going from IDLE to RX or TX (or FSTXON), PO timeout Approx. 146µs - 171µs
		writeReg			(CC1101_MCSM1,		0x00);			// Always Clear channel indication, Next state after finishing packet reception: IDLE, Next state after finishing packet transmission: IDLE

		writeReg			(CC1101_FIFOTHR,	0x07);			// used to program threshold points in the FIFOs. Bytes in TX FIFO 33, Bytes in RX FIFO 32. A signal will assert when the number of bytes in the FIFO is equal to or higher than the programmed threshold

		writeReg			(CC1101_MDMCFG2,	0x93);			// Modem Configuration: Enable digital DC blocking filter before demodulator, GFSK + 30/32 sync word bits detected
		writeReg			(CC1101_MDMCFG1,	0x22);			// 00100010 minimum of 4 preamble bytes to be transmitted + 2 bit exponent of channel spacing

		writeReg			(CC1101_PKTCTRL0,	0x04);			// whitening off + fixed length packet + crc appended
//	 	disableAddressCheck	();									// Append two bytes with status RSSI/LQI/CRC OK, No address check
		enableAddressCheck	();									// Address check and 0 (0x00) broadcast + append two bytes on reciept for CRC info and RSSI/LQI

		writeReg			(CC1101_IOCFG0,		0x06);			// Asserts when sync word has been sent / received, and de-asserts at the end of the packet. In RX, the pin will also de-assert when a packet is discarded due to address or maximum length filtering or when the radio enters RXFIFO_OVERFLOW state
		writeReg			(CC1101_IOCFG1,		0x2E);			// High impedance (3-state)
		writeReg			(CC1101_IOCFG2,		0x01);			// Associated to the RX FIFO: Asserts when RX FIFO is filled at or above the RX FIFO threshold or the end of packet is reached. De-asserts when the RX FIFO is empty.

		writeReg			(CC1101_FREND0,		0x11 /*0x17*/);	// Front End TX Configuration : in OOK/ASK mode, this selects the PATABLE index to use

		const byte paTable [8] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};	// Low power
//		const byte paTable [8] = {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0};	// Long distance
		writeBurstReg	(CC1101_PATABLE, (byte*)paTable, 8);
	}
};

}