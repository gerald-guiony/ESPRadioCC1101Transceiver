//************************************************************************************************************************
// X2dRadioTyboxCommands.h
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once


	/*
		================================================================
		Radio Tybox X2D DELTA DORE
		================================================================

		CubicSDR => 433.446 Mhz

		rtl_433 : https://github.com/merbanan/rtl_433

		----------------------------
		rtl_433 -A -f 433446000
		----------------------------

		rtl_433 says :
			Detected OOK package
			Guessing modulation: Manchester coding
			Use a flex decoder with -X 'n=name,m=OOK_MC_ZEROBIT,s=844,l=0,r=1656'

		---------------------------------------------------------------------------
		rtl_433 -A -f 433446000 -g 10 -X 'n=name,m=OOK_MC_ZEROBIT,s=844,l=0,r=1656'
		---------------------------------------------------------------------------


		*****************************************************************************************************
		Température courante > température programmée (mode confort ou mode eco selectionné)
		*****************************************************************************************************

		------------------------------------------------------------------------------------------------------
		Package 1 :
		-----------

		[00] {122} 55 55 7f 7e a2 55 54 aa aa aa db bf 1b c0 15 40

		Guessing modulation: Manchester coding
		Attempting demodulation... short_width: 852, long_width: 0, reset_limit: 1656, sync_width: 0
		Use a flex decoder with -X 'n=name,m=OOK_MC_ZEROBIT,s=852,l=0,r=1656'
		pulse_demod_manchester_zerobit(): Analyzer Device

		------------------------------------------------------------------------------------------------------
		Package 2 :
		-----------

		codes {122} 55 55 7f 7e a2 55 54 aa 95 55 24 40 f4 3f ea 8

		Guessing modulation: Pulse Width Modulation with sync/delimiter
		Attempting demodulation... short_width: 848, long_width: 1684, reset_limit: 1656, sync_width: 232
		Use a flex decoder with -X 'n=name,m=OOK_PWM,s=848,l=1684,r=1656,g=0,t=0,y=232'
		pulse_demod_pwm(): Analyzer Device

		[00] {80} 00 7e f9 a0 20 80 ad f7 77 f8
		[01] { 0}

		------------------------------------------------------------------------------------------------------
		Package 3 :
		-----------

		[00] {122} 55 55 7f 7e a2 55 54 aa b5 55 5b bf 14 00 15 40

		Guessing modulation: Manchester coding
		Attempting demodulation... short_width: 848, long_width: 0, reset_limit: 1652, sync_width: 0
		Use a flex decoder with -X 'n=name,m=OOK_MC_ZEROBIT,s=848,l=0,r=1652'
		pulse_demod_manchester_zerobit(): Analyzer Device


		******************************************************************************************************
		Température courante < température programmée (mode confort ou mode eco selectionné)
		******************************************************************************************************

		------------------------------------------------------------------------------------------------------
		Package 1 :
		-----------

		codes {123} 55 55 7f 7e a2 55 54 aa aa 8a db bf 03 1f f5 4

		Attempting demodulation... short_width: 848, long_width: 1680, reset_limit: 1652, sync_width: 220
		Use a flex decoder with -X 'n=name,m=OOK_PWM,s=848,l=1680,r=1652,g=0,t=0,y=220'
		pulse_demod_pwm(): Analyzer Device

		[00] {82} 00 7e f9 a0 20 0c 56 ff dd fe 00
		[01] { 0}

		------------------------------------------------------------------------------------------------------
		Package 2 :
		-----------

		[00]  {122} 55 55 7f 7e a2 55 54 aa 95 75 24 40 e1 c0 15 40

		Guessing modulation: Manchester coding
		Attempting demodulation... short_width: 848, long_width: 0, reset_limit: 1656, sync_width: 0
		Use a flex decoder with -X 'n=name,m=OOK_MC_ZEROBIT,s=848,l=0,r=1656'
		pulse_demod_manchester_zerobit(): Analyzer Device

		------------------------------------------------------------------------------------------------------
		Package 3 :
		-----------

		[00]  {122} 55 55 7f 7e a2 55 54 aa b5 75 5b bf 0e 00 15 40

		Guessing modulation: Manchester coding
		Attempting demodulation... short_width: 848, long_width: 0, reset_limit: 1652, sync_width: 0
		Use a flex decoder with -X 'n=name,m=OOK_MC_ZEROBIT,s=848,l=0,r=1652'
		pulse_demod_manchester_zerobit(): Analyzer Device


		*** Répétition des 3 frames 10s plus tard puis toutes les 15 min en fonction de la température ***

	*/



	const uint8_t HEATING_ON_CMD [][16]		= { {0x55, 0x55, 0x7f, 0x36, 0x39, 0xaa, 0xa9, 0x55, 0x55, 0x15, 0xb7, 0x3f, 0x04, 0xff, 0xd5, 0x00},
												{0x55, 0x55, 0x7f, 0x36, 0x39, 0xaa, 0xa9, 0x55, 0x2a, 0xea, 0x48, 0xc0, 0xc4, 0xff, 0xd5, 0x00},
												{0x55, 0x55, 0x7f, 0x36, 0x39, 0xaa, 0xa9, 0x55, 0x6a, 0xea, 0xb7, 0x7e, 0x1b, 0x7f, 0xd5, 0x00} };


	const uint8_t HEATING_OFF_CMD [][16]	= { {0x55, 0x55, 0x7f, 0x36, 0x39, 0xaa, 0xa9, 0x55, 0x55, 0x55, 0xb7, 0x3f, 0x33, 0x00, 0x2a, 0x80},
												{0x55, 0x55, 0x7f, 0x36, 0x39, 0xaa, 0xa9, 0x55, 0x2a, 0xaa, 0x48, 0xc0, 0xec, 0xff, 0xd5, 0x00},
												{0x55, 0x55, 0x7f, 0x36, 0x39, 0xaa, 0xa9, 0x55, 0x6a, 0xaa, 0xb7, 0x7e, 0x2c, 0x80, 0x2a, 0x80} };



