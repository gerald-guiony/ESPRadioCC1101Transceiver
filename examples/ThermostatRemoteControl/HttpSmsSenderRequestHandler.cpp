//************************************************************************************************************************
// HttpSmsSenderRequestHandler.cpp
// Version 1.0 October, 2021
// Author Gerald Guiony
//************************************************************************************************************************

#include <ArduinoJson.h>

#include <HttpServer.h>

#include "HttpSmsSenderRequestHandler.h"


// Important things to remember
// ----------------------------
// * This is fully asynchronous server and as such does not run on the loop thread.
// * You can not use yield or delay or any function that uses them inside the callbacks
// * The server is smart enough to know when to close the connection and free resources
// * You can not send more than one response to a single request

// HTTP Request Methods
// https://www.w3schools.com/tags/ref_httpmethods.asp


// Use PROGMEM and server.send_P to send data from PROGMEM — in this case it doesn't need to be copied to RAM twice, you avoid allocating Strings and all the associated issues.
static const char PROGMEM PRINT_HELP[] = R"rawliteral(
===========================================================================================================
 *** AVAILABLE SMS JSON COMMANDS ON THIS DEVICE ***
===========================================================================================================

-----------------------------------------------------------------------------------------------------------
 Json command format :                         /sms/$cmd?json={"$name":"$value", ..}
-----------------------------------------------------------------------------------------------------------

 Supported commands list ($cmd?json={"$name":"$value", ..}) :

 help ......................................... Print this help
 sendsms : {"phone": $1, "message" : $2} ...... Send a sms with the message $2 to the phone number $1

===========================================================================================================
)rawliteral";



namespace a6gsm {

SINGLETON_IMPL (SmsSenderRequestHandler)

//========================================================================================================================
//
//========================================================================================================================
void SmsSenderRequestHandler :: send (const SMSmessage & sms)
{
	_smsToSend.push (sms);
}

//========================================================================================================================
//
//========================================================================================================================
void SmsSenderRequestHandler :: setup (A6SmsTransceiver * smsTransceiver)
{
	_smsTransceiver = smsTransceiver;
}

//========================================================================================================================
//
//========================================================================================================================
void SmsSenderRequestHandler :: loop ()
{
	static unsigned long lastGsmStartTimestamp = 0;

	if (!_smsToSend.empty()) {

		if (!_smsTransceiver->isStarted ()) {

			// Mandatory on some chips .. ??
			EspBoard::disableHardwareWatchdog ();

			if (_smsTransceiver->start ()) {
				lastGsmStartTimestamp = millis ();
			}
		}
		else {

			_smsTransceiver->updateSignalStrength ();

			const SMSmessage & sms = _smsToSend.front();
			_smsTransceiver->sendSMS (sms);

			_smsToSend.pop();
		}
	}
	else {

		if (_smsTransceiver->isStarted ()) {
			// Time ellapsed since the gsm device was turned on
			if (millis () - lastGsmStartTimestamp > GSM_ACTIVE_DURATION_ms) {
				_smsTransceiver->stop ();
			}
		}
	}
}

}

/************************************************************************************************************************/


namespace wifix {

SINGLETON_IMPL (HttpSmsSenderRequestHandler)

//========================================================================================================================
//
//========================================================================================================================
void HttpSmsSenderRequestHandler :: handleHelp (AsyncWebServerRequest * request)
{
	Logln(F("=> help"));
	request->send_P(200, F("text/plain"), PRINT_HELP);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpSmsSenderRequestHandler :: handleSendSms (AsyncWebServerRequest * request)
{
	Logln(F("=> send sms"));

	if (!request->hasArg("json")) {
		request->send(400, F("text/plain"), F("400: json argument not found"));				// The request is invalid, so send HTTP status 400
		return;
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonArg = jsonBuffer.parse(request->arg("json"));

	// Test if parsing succeeds.
	if (!jsonArg.success()) {
		request->send(400, F("text/plain"), F("400: Invalid json argument"));
		return;
	}

	if (!jsonArg ["phone"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	if (!jsonArg ["message"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	String number = jsonArg ["phone"];
	String message = jsonArg ["message"];

	I(a6gsm::SmsSenderRequestHandler).send (a6gsm::SMSmessage {number, "", message});

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject & jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "sendsms";
	jsonRsp["status"] = true;

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpSmsSenderRequestHandler :: setup (AsyncWebServer & asyncWebServer)
{
	asyncWebServer.on("/sms/help",		std::bind(&HttpSmsSenderRequestHandler::handleHelp,	this, _1));
	asyncWebServer.on("/sms/sendsms",	std::bind(&HttpSmsSenderRequestHandler::handleSendSms,	this, _1));
}

}