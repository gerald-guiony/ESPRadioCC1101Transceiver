
//************************************************************************************************************************
// cc1101X2dTransceiver.h
// Version 1.0 May, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include "cc1101Transceiver.h"


namespace cc1101 {

/**
 * Class: CC1101X2dTransceiver
 *
 * Description: cc1101 X2D packet transceiver
 * CC1101X2dTransceiver interface
 */
class CC1101X2dTransceiver : public CC1101Transceiver
{
public:

	CC1101X2dTransceiver (uint8_t irqPin, uint8_t address = 0x5d)
		: CC1101Transceiver (irqPin, address, 0)
	{
		initRegisters ();
	}

protected:

	virtual void initRegisters	(void) override
	{
		/***************************************************************
		 *  SmartRF Studio(tm) Export
		 *
		 *  Radio register settings specifed with C-code
		 *  compatible #define statements.
		 *
		 *  RF device: CC1101
		 *
		 *  Base Frequency		: 433.445 Mhz
		 *  Xtal Freq			: 26.0000 Mhz
		 *  Modulation			: ASK/OOK
		 *  Whitening 			: Off
		 *  Data Rate			: 1.199 kBaud
		 *  Channel Spacing		: 25.39 KHz
		 *  RX Filter BW		: 101.56 kHz
		 *  TX Power 			: 10 dBm
		 *  Manchester Enable	: On
		 *  Pa Ramping			: Off
		 *
		 ***************************************************************/

		#ifndef SMARTRF_CC1101_H
		#define SMARTRF_CC1101_H

		#define SMARTRF_SETTING_IOCFG0	 	0x06
		#define SMARTRF_SETTING_FIFOTHR		0x47
		#define SMARTRF_SETTING_PKTCTRL0	0x05
		#define SMARTRF_SETTING_FSCTRL1		0x06
		#define SMARTRF_SETTING_FREQ2	  	0x10
		#define SMARTRF_SETTING_FREQ1	  	0xAB
		#define SMARTRF_SETTING_FREQ0	  	0xC4
		#define SMARTRF_SETTING_MDMCFG4		0xC5
		#define SMARTRF_SETTING_MDMCFG3		0x83
		#define SMARTRF_SETTING_MDMCFG2		0x3B
		#define SMARTRF_SETTING_MDMCFG1		0x20
		#define SMARTRF_SETTING_MDMCFG0		0x00
		#define SMARTRF_SETTING_DEVIATN		0x15
		#define SMARTRF_SETTING_MCSM0	  	0x18
		#define SMARTRF_SETTING_FOCCFG	 	0x16
		#define SMARTRF_SETTING_WORCTRL		0xFB
		#define SMARTRF_SETTING_FREND0	 	0x11
		#define SMARTRF_SETTING_FSCAL3	 	0xE9
		#define SMARTRF_SETTING_FSCAL2	 	0x2A
		#define SMARTRF_SETTING_FSCAL1	 	0x00
		#define SMARTRF_SETTING_FSCAL0	 	0x1F
		#define SMARTRF_SETTING_TEST2	  	0x81
		#define SMARTRF_SETTING_TEST1	  	0x35
		#define SMARTRF_SETTING_TEST0		0x09

		// #define SMARTRF_PA_TABLE {0x00,0x12,0x0e,0x34,0x60,0xc5,0xc1,0xc0}

		#endif


		// Set to 433.445 => Frequency must be ajusted for each CC1101.. use SDRcsharp or CubicSDR tool to check frequency value
		setCarrierFreq		(CFREQ_433);
		writeReg 			(CC1101_FREQ1,		SMARTRF_SETTING_FREQ1);
		writeReg			(CC1101_FREQ0,		/*SMARTRF_SETTING_FREQ0*/ 0x9B); 	// specific value for esp ip 12

		writeReg			(CC1101_FSCAL3,		SMARTRF_SETTING_FSCAL3);
		writeReg			(CC1101_FSCAL2,		SMARTRF_SETTING_FSCAL2);
		writeReg			(CC1101_FSCAL1,		SMARTRF_SETTING_FSCAL1);
		writeReg			(CC1101_FSCAL0,		SMARTRF_SETTING_FSCAL0);

		setDataRate			(KBPS_4);
		// Data rate is 1200 baud/s !!
		writeReg 			(CC1101_FSCTRL1,	SMARTRF_SETTING_FSCTRL1);
		writeReg 			(CC1101_MDMCFG4,	SMARTRF_SETTING_MDMCFG4); 	// Modem Configuration
		writeReg 			(CC1101_MDMCFG3,	SMARTRF_SETTING_MDMCFG3); 	// Modem Configuration
		writeReg 			(CC1101_DEVIATN,	SMARTRF_SETTING_DEVIATN); 	// Modem Deviation Setting
		writeReg 			(CC1101_FOCCFG,		SMARTRF_SETTING_FOCCFG); 	// Frequency Offset Compensation Configuration


		setChannel			(0x00);
		setSyncWord			(0x55, 0x7F);
		setDevAddress		(_address);							// Optional broadcast addresses are 0 (0x00) and 255 (0xFF).

		writeReg			(CC1101_PKTLEN,		0xFF);			// The PKTLEN register is used to set the maximum packet length allowed in RX (useless here..)

		writeReg			(CC1101_MCSM0,		0x18);			// 00011000	Main Radio Control State Machine configuration : Auto calibrate When going from IDLE to RX or TX (or FSTXON), PO timeout Approx. 146µs - 171µs
		writeReg			(CC1101_MCSM1,		0x00);			// Always Clear channel indication, Next state after finishing packet reception: IDLE, Next state after finishing packet transmission: IDLE

		writeReg			(CC1101_FIFOTHR,	0x47);			// used to program threshold points in the FIFOs. Bytes in TX FIFO 33, Bytes in RX FIFO 32. A signal will assert when the number of bytes in the FIFO is equal to or higher than the programmed threshold

		writeReg			(CC1101_MDMCFG2,	0x38);			// Modem Configuration: ASK/OOK modulation, Manchester encoding/decoding enabled, no sync word
		writeReg			(CC1101_MDMCFG1,	0x00);			// Minimum of 2 preamble bytes to be transmitted

		writeReg			(CC1101_PKTCTRL0,	0x00);			// Packet Automation Control : Whitening off, CRC disabled, Fixed packet length mode
	 	writeReg			(CC1101_PKTCTRL1,	0x00);			// No status RSSI/LQI/CRC, No address check

		writeReg			(CC1101_WORCTRL,	SMARTRF_SETTING_WORCTRL);
		writeReg			(CC1101_TEST2,		SMARTRF_SETTING_TEST2);
		writeReg			(CC1101_TEST1,		SMARTRF_SETTING_TEST1);
		writeReg			(CC1101_TEST0,		SMARTRF_SETTING_TEST0);


		writeReg			(CC1101_IOCFG0,		0x06);			// Asserts when sync word has been sent / received, and de-asserts at the end of the packet. In RX, the pin will also de-assert when a packet is discarded due to address or maximum length filtering or when the radio enters RXFIFO_OVERFLOW state
		writeReg			(CC1101_IOCFG1,		0x2E);			// High impedance (3-state)
		writeReg			(CC1101_IOCFG2,		0x01);			// Associated to the RX FIFO: Asserts when RX FIFO is filled at or above the RX FIFO threshold or the end of packet is reached. De-asserts when the RX FIFO is empty.

		writeReg			(CC1101_FREND0,		0x11 /*0x17*/);	// Front End TX Configuration : in OOK/ASK mode, this selects the PATABLE index to use

		const byte paTable [8] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};	// Low power
//		const byte paTable [8] = {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0};	// Long distance
//		const byte paTable [8] = SMARTRF_PA_TABLE;									// Pa Ramping
		writeBurstReg	(CC1101_PATABLE, (byte*)paTable, 8);
	}
};

}
