//************************************************************************************************************************
// HttpThermostatRequestHandler.h
// Version 1.0 July, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <ESPAsyncWebServer.h>

#include <Common.h>
#include <HttpRequestHandler.h>

namespace wifix {

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class HttpThermostatRequestHandler : public HttpRequestHandler
{
	SINGLETON_CLASS (HttpThermostatRequestHandler)

private:

	void handleHelp									(AsyncWebServerRequest * request);
	void handleSettings								(AsyncWebServerRequest * request);
	void handleSetHgTemp							(AsyncWebServerRequest * request);
	void handleSetEcoTemp							(AsyncWebServerRequest * request);
	void handleSetConfTemp							(AsyncWebServerRequest * request);
	void handleSetRegulMode							(AsyncWebServerRequest * request);
	void handleGetAmbientTemp						(AsyncWebServerRequest * request);
	void handleForceHeatingOn						(AsyncWebServerRequest * request);
	void handleForceHeatingOff						(AsyncWebServerRequest * request);

public:

	virtual void setup 								(AsyncWebServer & asyncWebServer) override;
};

}









