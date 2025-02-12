//************************************************************************************************************************
// HttpRadioCommandRequestHandler.cpp
// Version 1.0 May, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <ArduinoJson.h>

#include <HttpServer.h>

#include <cc1101VarLenTransceiver.h>
#include <cc1101FixedLenTransceiver.h>
#include <cc1101X2dTransceiver.h>
#include <cc1101X2dEmitter.h>
#include <ccReplayer.h>

#include "Settings.h"

#include "HttpRadioCommandRequestHandler.h"


using namespace cc1101;


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
 *** AVAILABLE RADIO JSON COMMANDS ON THIS DEVICE ***
===========================================================================================================

-----------------------------------------------------------------------------------------------------------
 Json command format :            /radio/$cmd?json={"$name":"$value", ..}
-----------------------------------------------------------------------------------------------------------
 Radio signal format :            ($len) [$val1 $val2 ..] (N)OK S$val Q$val
-----------------------------------------------------------------------------------------------------------

 Supported commands list ($cmd?json={"$name":"$value", ..}) :

 help ............................ Print this help
 record .......................... Record in memory the radio signal emitted by your remote control
 emmit ........................... Emmit the memorized radio signal
 print ........................... Print the memorized radio signal in the response body (POST)
 parse [radio signal in POST] .... Parse radio signal in the request body (POST: No restriction on data len)
 store : {"id": $1} .............. Store the memorized radio signal in a file with an Id between 1 to 255
 load : {"id": $1} ............... Load in memory the stored radio signal from the corresponding file
 delete : {"id": $1} ............. Delete the corresponding file (containing a radio signal)
 idlist .......................... List of all files containing stored radio signals

===========================================================================================================
)rawliteral";


namespace wifix {


//inline Print & operator <<(Print & printer, JsonObject & jsonObj) { jsonObj.printTo(printer); return printer; }

//CC1101VarLenTransceiver	cc1101Trancseiver (CC1101_IRQ_PIN);
//CC1101FixedLenTransceiver	cc1101Transceiver (CC1101_IRQ_PIN);
CC1101X2dTransceiver		cc1101Transceiver (CC1101_IRQ_PIN);
//CC1101X2dEmitter			cc1101Transceiver;					// Emitter only


SINGLETON_IMPL (HttpRadioCommandRequestHandler)


//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handleHelp (AsyncWebServerRequest * request)
{
	Logln(F("=> help"));
	request->send_P(200, F("text/plain"), PRINT_HELP);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handleRecord (AsyncWebServerRequest * request)
{
	Logln(F("=> record radio signal"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));

	// Memory pool for JSON object tree.
	// Inside the brackets, 200 is the size of the pool in bytes.
	// Don't forget to change this value to match your JSON document.
	// Use arduinojson.org/assistant to compute the capacity.
	//StaticJsonBuffer<200> jsonBuffer;

	// StaticJsonBuffer allocates memory on the stack, it can be
	// replaced by DynamicJsonBuffer which allocates in the heap.
	DynamicJsonBuffer jsonBuffer;

	// Create the root of the object tree.
	// It's a reference to the JsonObject, the actual bytes are inside the
	// JsonBuffer with all the other nodes of the object tree.
	// Memory is freed when jsonBuffer goes out of scope.
	JsonObject& jsonRsp = jsonBuffer.createObject();

	StreamString sstr;
	jsonRsp["command"] = "record";
	jsonRsp["status"] = I(ccReplayer).recordSignal (&cc1101Transceiver, sstr);
	jsonRsp["message"] = sstr.c_str();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handleEmmit (AsyncWebServerRequest * request)
{
	Logln(F("=> emmit radio signal"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonRsp = jsonBuffer.createObject();

	StreamString sstr;
	jsonRsp["command"] = "emmit";
	jsonRsp["status"] = I(ccReplayer).emmitSignal (&cc1101Transceiver, sstr);
	jsonRsp["message"] = sstr.c_str();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handlePrint (AsyncWebServerRequest * request)
{
	Logln(F("=> print radio signal"));
/*
	MemStream memStream;
	memStream << IrSignal;
	AsyncWebServerResponse * response = request->beginResponse (memStream, F("text/plain"), memStream.available ());
	request->send (response);
*/

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("text/plain"));
	*response << I(ccReplayer).currentSignal();
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handleParseBody (AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
	Logln(F("=> parse radio signal"));

	// A revoir dans le cas de plusieurs connections en même temps..
	static MemStream * memStream = NULL;

	if (index == 0) {																		// Body start
		memStream = new MemStream();
	}

	if (memStream != NULL) {

		memStream->write (data, len);

		if (index + len == total) {															// Body end

			// This way of sending Json is great for when the result is below 4KB
			AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
			DynamicJsonBuffer jsonBuffer;
			JsonObject& jsonRsp = jsonBuffer.createObject();

			StreamString sstr;
			jsonRsp["command"] = "parse";
			jsonRsp["status"] = I(ccReplayer).currentSignal().parse (*memStream, sstr);
			jsonRsp["message"] = sstr.c_str();

			jsonRsp.prettyPrintTo(*response);
			request->send(response);

			delete memStream;
			memStream = NULL;
		}
	}
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handleStore (AsyncWebServerRequest * request)
{
	Logln(F("=> store radio signal"));

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

	if (!jsonArg ["id"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	int fileId = jsonArg ["id"];
	if ((fileId <= 0) || (255 < fileId)) {
		request->send(400, F("text/plain"), F("400: Invalid id argument"));
		return;
	}

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject & jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "store";
	jsonRsp["status"] = I(ccReplayer).saveSignal (fileId);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handleLoad (AsyncWebServerRequest * request)
{
	Logln(F("=> load radio signal"));

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

	if (!jsonArg ["id"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	int fileId = jsonArg ["id"];
	if ((fileId <= 0) || (255 < fileId)) {
		request->send(400, F("text/plain"), F("400: Invalid id argument"));
		return;
	}

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "load";
	// deserialize ir signal
	jsonRsp["status"] = I(ccReplayer).loadSignal (fileId);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handleDelete (AsyncWebServerRequest * request)
{
	Logln(F("=> delete radio signal"));

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

	if (!jsonArg ["id"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	int fileId = jsonArg ["id"];
	if ((fileId <= 0) || (255 < fileId)) {
		request->send(400, F("text/plain"), F("400: Invalid id argument"));
		return;
	}

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "delete";
	jsonRsp["status"] = I(ccPacketStorage).remove (fileId);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: handlePrintIdList (AsyncWebServerRequest * request)
{
	Logln(F("=> idlist"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "idlist";
	jsonRsp["result"] = I(ccPacketStorage).getList();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpRadioCommandRequestHandler :: setup (AsyncWebServer & asyncWebServer)
{
	asyncWebServer.on("/radio/help",		std::bind(&HttpRadioCommandRequestHandler::handleHelp,			this, _1));
	asyncWebServer.on("/radio/record",		std::bind(&HttpRadioCommandRequestHandler::handleRecord,		this, _1));
	asyncWebServer.on("/radio/emmit",		std::bind(&HttpRadioCommandRequestHandler::handleEmmit,			this, _1));
	asyncWebServer.on("/radio/print",		std::bind(&HttpRadioCommandRequestHandler::handlePrint,			this, _1));
	asyncWebServer.on("/radio/parse",		HTTP_ANY, [](AsyncWebServerRequest *request) {/* nothing and dont remove it */}, NULL, std::bind(&HttpRadioCommandRequestHandler::handleParseBody, this, _1, _2, _3, _4, _5));
	asyncWebServer.on("/radio/store",		std::bind(&HttpRadioCommandRequestHandler::handleStore,			this, _1));
	asyncWebServer.on("/radio/load",		std::bind(&HttpRadioCommandRequestHandler::handleLoad,			this, _1));
	asyncWebServer.on("/radio/delete",		std::bind(&HttpRadioCommandRequestHandler::handleDelete,		this, _1));
	asyncWebServer.on("/radio/idlist",		std::bind(&HttpRadioCommandRequestHandler::handlePrintIdList,	this, _1));
}


}