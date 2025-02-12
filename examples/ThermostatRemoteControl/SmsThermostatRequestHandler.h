//************************************************************************************************************************
// SmsThermostatRequestHandler.h
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>

#include <A6SmsTransceiver.h>

using namespace corex;


#define GSM_OFF_FIRST_DURATION_ms		180000									// 3m en ms
#define GSM_ON_DURATION_ms				3600000									// 1h en ms
#define GSM_OFF_DURATION_ms				60000									// 1m en ms
#define GSM_SIGNAL_STRENGTH_PERIOD_ms	60000


namespace a6gsm {

//------------------------------------------------------------------------------
//
class SmsThermostatRequestHandler: public Module <A6SmsTransceiver *>
{
	SINGLETON_CLASS (SmsThermostatRequestHandler)

private:

	bool _isStartGsmRequested					= false;
	bool _isRebootRequested 					= false;

	A6SmsTransceiver * _smsTransceiver			= nullptr;

private:

	void sendDebugSMS							(const String & message);
	void onSmsRequestReceived 					(const SMSmessage & sms);

public:

	void startGsm								();

	void setup 									(A6SmsTransceiver * smsTransceiver) override;
	void loop 									() override;
};

}