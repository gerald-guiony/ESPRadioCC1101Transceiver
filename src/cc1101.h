//************************************************************************************************************************
// cc1101.h
// Creation date: April, 2019
// Author: Gerald Guiony
// Forked repository: https://github.com/SixK/CC1101-X2D-Heaters
//************************************************************************************************************************

#pragma once

/*
  CC11xx pins    ESP pins Arduino pins  Description
  1 - VCC        VCC      VCC           3v3
  2 - GND        GND      GND           Ground
  3 - MOSI       13=D7    Pin 11        Data input to CC11xx
  4 - SCK        14=D5    Pin 13        Clock pin
  5 - MISO/GDO1  12=D6    Pin 12        Data output from CC11xx / serial clock from CC11xx
  6 - GDO2       04=D2    Pin 2?        Serial data to CC11xx
  7 - GDO0       ?        Pin  ?        output as a symbol of receiving or sending data
  8 - CSN        15=D8    Pin 10        Chip select / (SPI_SS)
*/


#include <SPI.h>							// On Arduino, SPI pins are predefined

#include "ccPacket.h"

// The PATABLE is an 8-byte table that defines the PA control settings to use for each of the eight PA power values 
// (selected by the 3-bit value FREND0.PA_POWER). The table is written and read from the lowest setting (0) to the 
// highest (7), one byte at a time.
const uint8_t PA_TABLE10	[8] {0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,};
const uint8_t PA_TABLE7		[8] {0x00,0xC8,0x00,0x00,0x00,0x00,0x00,0x00,};
const uint8_t PA_TABLE5		[8] {0x00,0x84,0x00,0x00,0x00,0x00,0x00,0x00,};
const uint8_t PA_TABLE0		[8] {0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,};
const uint8_t PA_TABLE_10	[8] {0x00,0x34,0x00,0x00,0x00,0x00,0x00,0x00,};
const uint8_t PA_TABLE_15	[8] {0x00,0x1D,0x00,0x00,0x00,0x00,0x00,0x00,};
const uint8_t PA_TABLE_20	[8] {0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,};
const uint8_t PA_TABLE_30	[8] {0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,};


// -----------------------------------------------------------------
// Don't change this - http://www.ti.com/lit/an/swra112b/swra112b.pdf
#define NUM_CONFIG_REGISTERS      0x2F  // 47 registers
// -----------------------------------------------------------------


/**
 * Miscellaneous
 */
#define CRYSTAL_FREQUENCY         26000000
#define FIFOBUFFER                0x42  //size of Fifo Buffer
#define RSSI_OFFSET_868MHZ        0x4E  //dec = 74
#define BROADCAST_ADDRESS         0x00  //broadcast address
#define CC1101_TEMP_ADC_MV        3.225 //3.3V/1023 . mV pro digit
#define CC1101_TEMP_CELS_CO       2.47  //Temperature coefficient 2.47mV per Grad Celsius

#define CHECK_BIT(var,pos)        ((var) & (1<<(pos)))
#define MIN(x, y)                 (((x) < (y)) ? (x) : (y))


/**
 * Frequency channels
 */
#define NUMBER_OF_FCHANNELS      10

/**
 * Type of transfers
 */
/*
 * From cc1101.pdf:
 * 
 * All transactions on the SPI interface start with
 * a header byte containing a R/W¯ bit, a burst
 * access bit (B), and a 6-bit address (A5 – A0).
 * The CSn pin must be kept low during transfers
 * on the SPI bus. If CSn goes high during the
 * transfer of a header byte or during read/write
 * from/to a register, the transfer will be
 * cancelled. The timing for the address and data
 * transfer on the SPI interface is shown in Figure
 * 15 with reference to Table 22.
 */

/*---------------------------[CC1101 - R/W offsets]---------------------------*/
#define WRITE_SINGLE_BYTE   0x00 // 0x00000000
#define WRITE_BURST         0x40 // 0x01000000
#define READ_SINGLE_BYTE    0x80 // 0x10000000
#define READ_BURST          0xC0 // 0x11000000
/*---------------------------[END R/W offsets]--------------------------------*/

/*
/*------------------------[CC1101 - FIFO commands]----------------------------*/
#define TXFIFO_BURST        0x7F    //write burst only      0b01111111
#define TXFIFO_SINGLE_BYTE  0x3F    //write single only     0b00111111
#define RXFIFO_BURST        0xFF    //read burst only       0b11111111
#define RXFIFO_SINGLE_BYTE  0xBF    //read single only      0b10111111
/*---------------------------[END FIFO commands]------------------------------*/

/**
 * Type of register
 */
#define CC1101_CONFIG_REGISTER   READ_SINGLE_BYTE
#define CC1101_STATUS_REGISTER   READ_BURST

/**
 * PATABLE & FIFO's
 * The PATABLE is an 8-byte table that defines
 * the PA control settings to use for each of the
 * eight PA power values (selected by the 3-bit
 * value FREND0.PA_POWER). The table is
 * written and read from the lowest setting (0) to
 * the highest (7), one byte at a time.
 */
#define CC1101_PATABLE           0x3E        // PATABLE address
#define CC1101_TXFIFO            0x3F        // TX FIFO address
#define CC1101_RXFIFO            0x3F        // RX FIFO address
#define CC1101_PA_LowPower		 0x60
#define CC1101_PA_LongDistance	 0xC0

/**
 * Command strobes
 */
#define CC1101_SRES              0x30        // Reset CC1101 chip
#define CC1101_SFSTXON           0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA):
                                             // Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define CC1101_SXOFF             0x32        // Turn off crystal oscillator
#define CC1101_SCAL              0x33        // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without
                                             // setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define CC1101_SRX               0x34        // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CC1101_STX               0x35        // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
                                             // If in RX state and CCA is enabled: Only go to TX if channel is clear
#define CC1101_SIDLE             0x36        // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#define CC1101_SWOR              0x38        // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if
                                             // WORCTRL.RC_PD=0
#define CC1101_SPWD              0x39        // Enter power down mode when CSn goes high
#define CC1101_SFRX              0x3A        // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC1101_SFTX              0x3B        // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC1101_SWORRST           0x3C        // Reset real time clock to Event1 value
#define CC1101_SNOP              0x3D        // No operation. May be used to get access to the chip status byte

/**
 * CC1101 configuration registers
 */
// Register values set regardless of frequency and bitrate
#define CC1101_IOCFG2            0x00        // GDO2 Output Pin Configuration
#define CC1101_IOCFG1            0x01        // GDO1 Output Pin Configuration
#define CC1101_IOCFG0            0x02        // GDO0 Output Pin Configuration
#define CC1101_FIFOTHR           0x03        // RX FIFO and TX FIFO Thresholds
#define CC1101_SYNC1             0x04        // Sync Word, High Byte
#define CC1101_SYNC0             0x05        // Sync Word, Low Byte
#define CC1101_PKTLEN            0x06        // Packet Length
#define CC1101_PKTCTRL1          0x07        // Packet Automation Control
#define CC1101_PKTCTRL0          0x08        // Packet Automation Control
#define CC1101_ADDR              0x09        // Device Address
#define CC1101_CHANNR            0x0A        // Channel Number

// Register values which change depending on frequency and bitrate
#define CC1101_FSCTRL1           0x0B        // Frequency Synthesizer Control
#define CC1101_FSCTRL0           0x0C        // Frequency Synthesizer Control
#define CC1101_FREQ2             0x0D        // Frequency Control Word, High Byte
#define CC1101_FREQ1             0x0E        // Frequency Control Word, Middle Byte
#define CC1101_FREQ0             0x0F        // Frequency Control Word, Low Byte
#define CC1101_MDMCFG4           0x10        // Modem Configuration
#define CC1101_MDMCFG3           0x11        // Modem Configuration
#define CC1101_MDMCFG2           0x12        // Modem Configuration
#define CC1101_MDMCFG1           0x13        // Modem Configuration
#define CC1101_MDMCFG0           0x14        // Modem Configuration
#define CC1101_DEVIATN           0x15        // Modem Deviation Setting
#define CC1101_MCSM2             0x16        // Main Radio Control State Machine Configuration
#define CC1101_MCSM1             0x17        // Main Radio Control State Machine Configuration
#define CC1101_MCSM0             0x18        // Main Radio Control State Machine Configuration
#define CC1101_FOCCFG            0x19        // Frequency Offset Compensation Configuration
#define CC1101_BSCFG             0x1A        // Bit Synchronization Configuration
#define CC1101_AGCCTRL2          0x1B        // AGC Control
#define CC1101_AGCCTRL1          0x1C        // AGC Control
#define CC1101_AGCCTRL0          0x1D        // AGC Control
#define CC1101_WOREVT1           0x1E        // High Byte Event0 Timeout
#define CC1101_WOREVT0           0x1F        // Low Byte Event0 Timeout
#define CC1101_WORCTRL           0x20        // Wake On Radio Control
#define CC1101_FREND1            0x21        // Front End RX Configuration
#define CC1101_FREND0            0x22        // Front End TX Configuration
#define CC1101_FSCAL3            0x23        // Frequency Synthesizer Calibration
#define CC1101_FSCAL2            0x24        // Frequency Synthesizer Calibration
#define CC1101_FSCAL1            0x25        // Frequency Synthesizer Calibration
#define CC1101_FSCAL0            0x26        // Frequency Synthesizer Calibration

// Others
#define CC1101_RCCTRL1           0x27        // RC Oscillator Configuration
#define CC1101_RCCTRL0           0x28        // RC Oscillator Configuration
#define CC1101_FSTEST            0x29        // Frequency Synthesizer Calibration Control
#define CC1101_PTEST             0x2A        // Production Test
#define CC1101_AGCTEST           0x2B        // AGC Test
#define CC1101_TEST2             0x2C        // Various Test Settings
#define CC1101_TEST1             0x2D        // Various Test Settings
#define CC1101_TEST0             0x2E        // Various Test Settings

/**
 * Status registers
 */
#define CC1101_PARTNUM           0x30        // Chip ID
#define CC1101_VERSION           0x31        // Chip ID
#define CC1101_FREQEST           0x32        // Frequency Offset Estimate from Demodulator
#define CC1101_LQI               0x33        // Demodulator Estimate for Link Quality
#define CC1101_RSSI              0x34        // Received Signal Strength Indication
#define CC1101_MARCSTATE         0x35        // Main Radio Control State Machine State
#define CC1101_WORTIME1          0x36        // High Byte of WOR Time
#define CC1101_WORTIME0          0x37        // Low Byte of WOR Time
#define CC1101_PKTSTATUS         0x38        // Current GDOx Status and Packet Status
#define CC1101_VCO_VC_DAC        0x39        // Current Setting from PLL Calibration Module
#define CC1101_TXBYTES           0x3A        // Underflow and Number of Bytes
#define CC1101_RXBYTES           0x3B        // Overflow and Number of Bytes
#define CC1101_RCCTRL1_STATUS    0x3C        // Last RC Oscillator Calibration Result
#define CC1101_RCCTRL0_STATUS    0x3D        // Last RC Oscillator Calibration Result 


/* Bit fields in the chip status byte */
#define CC1101_STATUS_CHIP_RDYn_BM              0x80
#define CC1101_STATUS_STATE_BM                  0x70
#define CC1101_STATUS_FIFO_BYTES_AVAILABLE_BM   0x0F

/**
 * RX/TXBYTES Status Byte Masks
 */
#define CC1101_BYTES_IN_FIFO  					0x7F 		  // bitmask to get number in FIFO (first 7 bits)
#define CC1101_RX_FIFO_OVERFLOW 				0x80		  // byte number in RXfifo
#define CC1101_TX_FIFO_UNDERFLOW				0x80
#define CC1101_BITS_MARCSTATE					0x1F

/**
 * Marcstates from 0x35 (0xF5): MARCSTATE – Main Radio Control State Machine State
 */
/* Marc states */
enum CC_MARCSTATE
{
	CC_MARCSTATE_UNKNOWN 						= 0xFF,
	CC_MARCSTATE_SLEEP							= 0x00,
	CC_MARCSTATE_IDLE							= 0x01,
	CC_MARCSTATE_XOFF							= 0x02,
	CC_MARCSTATE_VCOON_MC						= 0x03,
	CC_MARCSTATE_REGON_MC						= 0x04,
	CC_MARCSTATE_MANCAL							= 0x05,
	CC_MARCSTATE_VCOON							= 0x06,
	CC_MARCSTATE_REGON							= 0x07,
	CC_MARCSTATE_STARTCAL						= 0x08,
	CC_MARCSTATE_BWBOOST						= 0x09,
	CC_MARCSTATE_FS_LOCK						= 0x0A,
	CC_MARCSTATE_IFADCON						= 0x0B,
	CC_MARCSTATE_ENDCAL							= 0x0C,
	CC_MARCSTATE_RX								= 0x0D,
	CC_MARCSTATE_RX_END							= 0x0E,
	CC_MARCSTATE_RX_RST							= 0x0F,
	CC_MARCSTATE_TXRX_SWITCH					= 0x10,
	CC_MARCSTATE_RXFIFO_OVERFLOW				= 0x11,
	CC_MARCSTATE_FSTXON							= 0x12,
	CC_MARCSTATE_TX								= 0x13,
	CC_MARCSTATE_TX_END							= 0x14,
	CC_MARCSTATE_RXTX_SWITCH					= 0x15,
	CC_MARCSTATE_TXFIFO_UNDERFLOW				= 0x16
};

/**
 * Carrier frequencies
 * Use the right one for your location:
 * https://www.analog.com/media/en/analog-dialogue/volume-40/number-1/articles/wireless-short-range-devices.pdf
 * https://www.acma.gov.au/Industry/Spectrum/Radiocomms-licensing/Class-licences/shortrange-spreadspectrum-devices-fact-sheet
 */
enum CFREQ
{
	CFREQ_868 = 0,
	CFREQ_433,
	CFREQ_915,
	CFREQ_918
};

/** 
 *  Data Rate
 */
enum DATA_RATE
{
	KBPS_250,
	KBPS_38,
	KBPS_4
};

/* Chip states */
enum CC_STATE
{
	CC_STATE_UNKNOWN      						= 0xFF,
	CC_STATE_IDLE         						= 0x00,
	CC_STATE_RX           						= 0x10,
	CC_STATE_TX           						= 0x20,
	CC_STATE_FSTXON       						= 0x30,
	CC_STATE_CALIBRATE    						= 0x40,
	CC_STATE_SETTLING     						= 0x50,
	CC_STATE_RX_OVERFLOW  						= 0x60,
	CC_STATE_TX_UNDERFLOW 						= 0x70	
};


/**
 * CC1101 configuration register names labels for Serial printing
 */
static const char CC1101_CONFIG_REGISTER_NAME[NUM_CONFIG_REGISTERS][16] PROGMEM = 
	{ "** IOCFG2","IOCFG1","IOCFG0","** FIFOTHR","SYNC1","SYNC0","** PKTLEN","** PKTCTRL1","** PKTCTRL0","** ADDR","CHANNR","FSCTRL1","FSCTRL0","FREQ2","FREQ1","FREQ0","MDMCFG4",
	"MDMCFG3","** MDMCFG2","MDMCFG1","MDMCFG0","DEVIATN","MCSM2","** MCSM1","MCSM0","FOCCFG","BSCFG","AGCCTRL2","AGCCTRL1","AGCCTRL0","WOREVT1","WOREVT0","WORCTRL",
	"FREND1","FREND0","FSCAL3","FSCAL2","FSCAL1","FSCAL0","RCCTRL1","RCCTRL0","FSTEST","PTEST","AGCTEST","TEST2","TEST1","TEST0" };

/**
 * CC1101 configuration registers - Default values extracted from SmartRF Studio
 *
 * Configuration:
 *
 * Deviation = 20.629883 
 * Base frequency = 867.999939 
 * Carrier frequency = 867.999939 
 * Channel number = 0 
 * Carrier frequency = 867.999939 
 * Modulated = true 
 * Modulation format = GFSK 
 * Manchester enable = false
 * Data whitening = off
 * Sync word qualifier mode = 30/32 sync word bits detected 
 * Preamble count = 4 
 * Channel spacing = 199.951172 
 * Carrier frequency = 867.999939 
 * Data rate = 38.3835 Kbps
 * RX filter BW = 101.562500 
 * Data format = Normal mode 
 * Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 * CRC enable = true 
 * Packet length = 255 
 * Device address = 1 
 * Address config = Enable address check
 * Append status = Append two status bytes to the payload of the packet. The status bytes contain RSSI and
 * LQI values, as well as CRC OK
 * CRC autoflush = false 
 * PA ramping = false 
 * TX power = 12
 * GDO0 mode = Asserts when sync word has been sent / received, and de-asserts at the end of the packet.
 * In RX, the pin will also de-assert when a packet is discarded due to address or maximum length filtering
 * or when the radio enters RXFIFO_OVERFLOW state. In TX the pin will de-assert if the TX FIFO underflows
 * Settings optimized for low current consumption
 */
/*
//#define CC1101_DEFVAL_IOCFG2     0x29        // GDO2 Output Pin Configuration
//#define CC1101_DEFVAL_IOCFG2     0x2E        // GDO2 Output Pin Configuration
#define CC1101_DEFVAL_IOCFG2     0x00        // GDO2 Output Pin Configuration
#define CC1101_DEFVAL_IOCFG1     0x2E        // GDO1 Output Pin Configuration
#define CC1101_DEFVAL_IOCFG0     0x06        // GDO0 Output Pin Configuration
#define CC1101_DEFVAL_FIFOTHR    0x47        // RX FIFO and TX FIFO Thresholds
#define CC1101_DEFVAL_SYNC1      0xD3        // Synchronization word, high byte
#define CC1101_DEFVAL_SYNC0      0x91        // Synchronization word, low byte
#define CC1101_DEFVAL_PKTLEN     0x4A        // Packet Length
#define CC1101_DEFVAL_PKTCTRL1   0x04        // Packet Automation Control
#define CC1101_DEFVAL_PKTCTRL0   0x00        // Packet Automation Control
#define CC1101_DEFVAL_ADDR       0x11        // Device Address
#define CC1101_DEFVAL_CHANNR     0x00        // Channel Number
#define CC1101_DEFVAL_FSCTRL1    0x06        // Frequency Synthesizer Control
#define CC1101_DEFVAL_FSCTRL0    0x01        // Frequency Synthesizer Control
// Carrier frequency = 868 MHz
#define CC1101_DEFVAL_FREQ2_868  0x21        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_868  0x65        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_868  0xC4        // Frequency Control Word, Low Byte
// Carrier frequency = 902 MHz
#define CC1101_DEFVAL_FREQ2_915  0x22        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_915  0xB1        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_915  0x3B        // Frequency Control Word, Low Byte
// Carrier frequency = 918 MHz
#define CC1101_DEFVAL_FREQ2_918  0x23        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_918  0x4E        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_918  0xC4        // Frequency Control Word, Low Byte

// Carrier frequency = 433 MHz
#define CC1101_DEFVAL_FREQ2_433  0x10        // Frequency Control Word, High Byte
#define CC1101_DEFVAL_FREQ1_433  0xA7        // Frequency Control Word, Middle Byte
#define CC1101_DEFVAL_FREQ0_433  0x62        // Frequency Control Word, Low Byte

#define CC1101_DEFVAL_MDMCFG4    0xC7        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG3    0x83        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG2    0x38        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG1    0x22        // Modem Configuration
#define CC1101_DEFVAL_MDMCFG0    0xF8        // Modem Configuration
#define CC1101_DEFVAL_DEVIATN    0x15        // Modem Deviation Setting
#define CC1101_DEFVAL_MCSM2      0x07        // Main Radio Control State Machine Configuration
//#define CC1101_DEFVAL_MCSM1      0x30        // Main Radio Control State Machine Configuration
#define CC1101_DEFVAL_MCSM1      0x20        // Main Radio Control State Machine Configuration
#define CC1101_DEFVAL_MCSM0      0x18        // Main Radio Control State Machine Configuration
#define CC1101_DEFVAL_FOCCFG     0x14        // Frequency Offset Compensation Configuration
#define CC1101_DEFVAL_BSCFG      0x6C        // Bit Synchronization Configuration
#define CC1101_DEFVAL_AGCCTRL2   0x03        // AGC Control
#define CC1101_DEFVAL_AGCCTRL1   0x00        // AGC Control
#define CC1101_DEFVAL_AGCCTRL0   0x91        // AGC Control
#define CC1101_DEFVAL_WOREVT1    0x87        // High Byte Event0 Timeout
#define CC1101_DEFVAL_WOREVT0    0x6B        // Low Byte Event0 Timeout
#define CC1101_DEFVAL_WORCTRL    0xFB        // Wake On Radio Control
#define CC1101_DEFVAL_FREND1     0x56        // Front End RX Configuration
#define CC1101_DEFVAL_FREND0     0x11        // Front End TX Configuration
#define CC1101_DEFVAL_FSCAL3     0xE9        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_FSCAL2     0x2A        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_FSCAL1     0x00        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_FSCAL0     0x1F        // Frequency Synthesizer Calibration
#define CC1101_DEFVAL_RCCTRL1    0x41        // RC Oscillator Configuration
#define CC1101_DEFVAL_RCCTRL0    0x00        // RC Oscillator Configuration
#define CC1101_DEFVAL_FSTEST     0x59        // Frequency Synthesizer Calibration Control
#define CC1101_DEFVAL_PTEST      0x7F        // Production Test
#define CC1101_DEFVAL_AGCTEST    0x3F        // AGC Test
#define CC1101_DEFVAL_TEST2      0x88        // Various Test Settings
#define CC1101_DEFVAL_TEST1      0x31        // Various Test Settings
#define CC1101_DEFVAL_TEST0      0x0B        // Various Test Settings
*/

/**
 * Macros
 */
// Select CC1101
#define select()                  digitalWrite(SS, LOW);
// Deselect CC1101
#define deselect()                digitalWrite(SS, HIGH);
// Read CC1101 Config register
#define readConfigReg(regAddr)    readReg(regAddr, CC1101_CONFIG_REGISTER)
// Read CC1101 Status register
#define readStatusReg(regAddr)    readReg(regAddr, CC1101_STATUS_REGISTER)
// Enter Rx state
#define setRxState()              cmdStrobe(CC1101_SRX)
// Enter Tx state
#define setTxState()              cmdStrobe(CC1101_STX)
// Enter IDLE state
#define setIdleState()            cmdStrobe(CC1101_SIDLE)
// Flush Rx FIFO
#define flushRxFifo()             cmdStrobe(CC1101_SFRX)
// Flush Tx FIFO
#define flushTxFifo()             cmdStrobe(CC1101_SFTX)
// Disable address check
#define disableAddressCheck()     writeReg(CC1101_PKTCTRL1, 0x04)
// Enable address check
#define enableAddressCheck()      writeReg(CC1101_PKTCTRL1, 0x06)
// Disable CCA
#define disableCCA()              writeReg(CC1101_MCSM1, 0)
// Enable CCA
#define enableCCA()               writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1)
// Radio calibration in manual mode
#define calibrate()				  cmdStrobe(CC1101_SCAL)
// Set PATABLE single byte
#define setTxPowerAmp(setting)    paTableByte = setting



/**
 * Class: CC1101
 * 
 * Description:
 * CC1101 interface
 */
class CC1101
{
protected:

	uint8_t _irqPin;

    CFREQ     _carrierFreq				= CFREQ_868;        	// The frequency chosen
	DATA_RATE _dataRate					= KBPS_38;
	uint8_t   _devAddress				= 0x00;

    CC_STATE  _currentState				= CC_STATE_UNKNOWN;    	// What the state of the CC1101 is according to our last check
	CC_STATE  _lastState				= CC_STATE_UNKNOWN;

	CC_MARCSTATE _currentMarcState		= CC_MARCSTATE_UNKNOWN;
	CC_MARCSTATE _lastMarcState			= CC_MARCSTATE_UNKNOWN;

	CCPACKET  _lastPacketReceived;								// Last radio signal received by CC1101

protected:

	// SPI helper functions
	void wait_Miso						(void) const;

	void cmdStrobe						(uint8_t cmd);
 	void writeReg						(uint8_t regAddr, uint8_t value);

 	uint8_t readReg						(uint8_t regAddr) const;
 	uint8_t readRegWithSyncProblem		(uint8_t address, uint8_t registerType) const;
    uint8_t readReg						(uint8_t regAddr, uint8_t regType) const;

	void writeBurstReg					(uint8_t regAddr, const uint8_t* buffer, const uint8_t len);
	void readBurstReg					(uint8_t * buffer, uint8_t regAddr, uint8_t len);

	bool isFixedPacketLength			() const;
	bool isAddressCheck					() const;
	uint8_t getFixedPacketLength		() const;
	bool isRssiLqiCrc					() const;

	virtual bool sendCCPacket 			(CCPACKET & packet);
 	virtual uint8_t receiveCCPacket		(CCPACKET & packet);

	void printState						(uint8_t status); 

	void printCurrentSettings			(void);
	void printRegisterConfiguration		(void);
	void printFIFOState					(void);
	void printMarcstate					(void);
	void printLQI_RSSI					(void);
	void printGD0xStatus				(void);	

public:

	CC1101 								(uint8_t irqPin);
	virtual ~CC1101						();

    void hardReset						(void);
    void softReset						(void);

	void wakeUp							(void);
	void setPowerDownState				(void);

	void setSyncWord					(uint8_t sync1, uint8_t sync0);
	void setDevAddress					(uint8_t addr);
	void setCarrierFreq					(CFREQ freq);
	void setDataRate 					(DATA_RATE dataRate);
	void setChannel						(uint8_t chnl);

	virtual bool sendPacket 			(CCPACKET & packet) = 0;
 	
	virtual void startReceivePacket		(uint8_t delayMs) 	= 0;
	virtual void stopReceivePacket		(void) 				= 0;

	CCPACKET & getLastPacketReceived	(void) { return _lastPacketReceived; }
 };
