//************************************************************************************************************************
// HttpRadioCommandRequestHandler.h
// Version 1.0 May, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <ESPAsyncWebServer.h>

#include <Common.h>
#include <HttpRequestHandler.h>


namespace wifix {

//------------------------------------------------------------------------------
//
class HttpRadioCommandRequestHandler : public HttpRequestHandler
{
	SINGLETON_CLASS (HttpRadioCommandRequestHandler)

private:

	void handleHelp									(AsyncWebServerRequest * request);
	void handleRecord								(AsyncWebServerRequest * request);
	void handleEmmit								(AsyncWebServerRequest * request);
	void handlePrint								(AsyncWebServerRequest * request);
	void handleParseBody							(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
	void handleStore								(AsyncWebServerRequest * request);
	void handleLoad									(AsyncWebServerRequest * request);
	void handleDelete								(AsyncWebServerRequest * request);
	void handlePrintIdList							(AsyncWebServerRequest * request);

public:

	virtual void setup 								(AsyncWebServer & asyncWebServer) override;
};


}









