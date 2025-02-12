//************************************************************************************************************************
// HttpThermostatRequestHandler.cpp
// Version 1.0 May, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <ArduinoJson.h>

#include <HttpServer.h>

#include "DeltaDoreThermostat.h"

#include "HttpThermostatRequestHandler.h"


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
 *** AVAILABLE HEATING JSON COMMANDS ON THIS DEVICE ***
===========================================================================================================

-----------------------------------------------------------------------------------------------------------
 Json command format :                         /heating/$cmd?json={"$name":"$value", ..}
-----------------------------------------------------------------------------------------------------------

 Supported commands list ($cmd?json={"$name":"$value", ..}) :

 help ......................................... Print this help
 settings ..................................... Print heating settings
 hgtemp : {"value": $1} ....................... Set the "hors gel" temperature value ("HG")
 ecotemp : {"value": $1} ...................... Set the economic temperature value ("ECO")
 conftemp : {"value": $1} ..................... Set the confort temperature value ("CONF")
 regulmode : {"mode": "$1"} ................... Set the temperature regulation mode : "HG", "ECO" or "CONF"
 ambienttemp .................................. Get the current ambient temperature
 forceon ...................................... Force heating ON
 forceoff ..................................... Force heating OFF

===========================================================================================================
)rawliteral";


namespace wifix {


SINGLETON_IMPL (HttpThermostatRequestHandler)


//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleHelp (AsyncWebServerRequest * request)
{
	Logln(F("=> help"));
	request->send_P(200, F("text/plain"), PRINT_HELP);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleSettings (AsyncWebServerRequest * request)
{
	Logln(F("=> settings"));

		// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"]		= "settings";
	jsonRsp["hgtemp"]		= I(DeltaDoreThermostat).getTempRegulationValue (TRegulationMode::HG);
	jsonRsp["ecotemp"]		= I(DeltaDoreThermostat).getTempRegulationValue (TRegulationMode::ECO);
	jsonRsp["conftemp"]		= I(DeltaDoreThermostat).getTempRegulationValue (TRegulationMode::CONF);
	jsonRsp["regulmode"]	= I(DeltaDoreThermostat).getTempRegulationModeStr ();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleSetHgTemp (AsyncWebServerRequest * request)
{
	Logln(F("=> set HG temp"));

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

	if (!jsonArg ["value"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	uint8_t temp = jsonArg ["value"];

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject & jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "hgtemp";
	jsonRsp["status"] = I(DeltaDoreThermostat).setTempRegulationValue (TRegulationMode::HG, temp);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleSetEcoTemp (AsyncWebServerRequest * request)
{
	Logln(F("=> set ECO temp"));

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

	if (!jsonArg ["value"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	uint8_t temp = jsonArg ["value"];

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject & jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "ecotemp";
	jsonRsp["status"] = I(DeltaDoreThermostat).setTempRegulationValue (TRegulationMode::ECO, temp);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleSetConfTemp (AsyncWebServerRequest * request)
{
	Logln(F("=> set CONF temp"));

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

	if (!jsonArg ["value"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	uint8_t temp = jsonArg ["value"];

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject & jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "conftemp";
	jsonRsp["status"] = I(DeltaDoreThermostat).setTempRegulationValue (TRegulationMode::CONF, temp);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleSetRegulMode (AsyncWebServerRequest * request)
{
	Logln(F("=> set regulation mode"));

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

	if (!jsonArg ["mode"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	String mode = jsonArg ["mode"];

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject & jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "regulmode";
	jsonRsp["status"] = I(DeltaDoreThermostat).setTempRegulationModeStr (mode);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleGetAmbientTemp (AsyncWebServerRequest * request)
{
	Logln(F("=> ambient temp"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "ambienttemp";
	jsonRsp["result"] = I(DeltaDoreThermostat).getAmbientTemperature ();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleForceHeatingOn (AsyncWebServerRequest * request)
{
	Logln(F("=> heating on"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "forceon";
	jsonRsp["status"] = I(DeltaDoreThermostat).emmitHeatingCommand (true);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: handleForceHeatingOff (AsyncWebServerRequest * request)
{
	Logln(F("=> heating off"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "forceoff";
	jsonRsp["status"] = I(DeltaDoreThermostat).emmitHeatingCommand (false);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpThermostatRequestHandler :: setup (AsyncWebServer & asyncWebServer)
{
	asyncWebServer.on("/heating/help",			std::bind(&HttpThermostatRequestHandler::handleHelp,				this, _1));
	asyncWebServer.on("/heating/settings",		std::bind(&HttpThermostatRequestHandler::handleSettings,			this, _1));
	asyncWebServer.on("/heating/hgtemp",		std::bind(&HttpThermostatRequestHandler::handleSetHgTemp,			this, _1));
	asyncWebServer.on("/heating/ecotemp",		std::bind(&HttpThermostatRequestHandler::handleSetEcoTemp,			this, _1));
	asyncWebServer.on("/heating/conftemp",		std::bind(&HttpThermostatRequestHandler::handleSetConfTemp,			this, _1));
	asyncWebServer.on("/heating/regulmode",		std::bind(&HttpThermostatRequestHandler::handleSetRegulMode,		this, _1));
	asyncWebServer.on("/heating/ambienttemp",	std::bind(&HttpThermostatRequestHandler::handleGetAmbientTemp,		this, _1));
	asyncWebServer.on("/heating/forceon",		std::bind(&HttpThermostatRequestHandler::handleForceHeatingOn,		this, _1));
	asyncWebServer.on("/heating/forceoff",		std::bind(&HttpThermostatRequestHandler::handleForceHeatingOff,		this, _1));
}

}