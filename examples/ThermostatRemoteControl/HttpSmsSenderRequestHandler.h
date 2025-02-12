//************************************************************************************************************************
// HttpSmsSenderRequestHandler.h
// Version 1.0 October, 2021
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <queue>

#include <ESPAsyncWebServer.h>

#include <Common.h>
#include <HttpRequestHandler.h>
#include <A6SmsTransceiver.h>


#define GSM_ACTIVE_DURATION_ms				180000									// 3m en ms


namespace a6gsm {

class SmsSenderRequestHandler : public Module <A6SmsTransceiver *>
{
	SINGLETON_CLASS (SmsSenderRequestHandler)

private:
	A6SmsTransceiver *  		_smsTransceiver	= nullptr;
	std::queue <SMSmessage>		_smsToSend;

public:

	void send					(const SMSmessage & sms);

	void setup 					(A6SmsTransceiver * smsTransceiver);
	void loop 					() override;
};

}

namespace wifix {

//------------------------------------------------------------------------------
//
class HttpSmsSenderRequestHandler : public HttpRequestHandler
{
	SINGLETON_CLASS (HttpSmsSenderRequestHandler)

private:
	void handleHelp				(AsyncWebServerRequest * request);
	void handleSendSms			(AsyncWebServerRequest * request);

public:
	virtual void setup 			(AsyncWebServer & asyncWebServer) override;
};

}