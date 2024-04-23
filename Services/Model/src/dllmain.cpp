// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h" 

// Model header
#include "Application.h"

// OpenTwin header
#include "OTServiceFoundation/Foundation.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#include "OTCommunication/ActionDispatcher.h"
#include "MicroserviceAPI.h"
#include "OTCore/Logger.h"
#include <chrono>
#include <thread>
extern "C" {

	_declspec(dllexport) const char* performAction(const char* _json, const char* _senderIP)
	{
		//return ot::foundation::performAction(_json, _senderIP);


		ot::JsonDocument actionDoc;
		actionDoc.fromJson(_json);

		// Get the requested action (if the member is missing a exception will be thrown)
		std::string action = ot::json::getString(actionDoc, OT_ACTION_MEMBER);

		bool hasHandler{ false };
		std::string result = ot::ActionDispatcher::instance().dispatch(action, actionDoc, hasHandler, ot::EXECUTE);

		if (!hasHandler) {
			result = MicroserviceAPI::dispatchAction(actionDoc, _senderIP);
		}

		// Create a copy of result... -> const char * (new to allocate the memory)
		char* retVal = new char[result.size() + 1];
		strcpy_s(retVal, result.size() + 1, result.c_str());

		return retVal;
	};

	_declspec(dllexport) const char* queueAction(const char* _json, const char* _senderIP)
	{
		//ToDo: If you want to actually queue the command implement the queueing logic here
		// At this point we are simply forwarding the message to the perform action function to execute the action immediatly
		//return ot::foundation::performAction(_json, _senderIP);

		OT_LOG("Forwarding message to execute endpoint", ot::QUEUED_INBOUND_MESSAGE_LOG);
		return performAction(_json, _senderIP);
	};

	_declspec(dllexport) const char* getServiceURL(void)
	{
		//return ot::foundation::getServiceURL();

		return MicroserviceAPI::getServiceURLOld();
	}

	_declspec(dllexport) void deallocateData(const char* _data)
	{
		// *****************
		// This code will deallocate the memory of the return values from the perform- and queueAction calls
		if (_data != nullptr)
		{
			delete[] _data;
		}
		// *****************
	};

	// This function is called once upon startup of this service
	_declspec(dllexport) int init(const char* _localDirectoryServiceURL, const char* _ownIP, const char* _sessionServiceIP, const char* _sessionID)
	{
		//return ot::foundation::init(_localDirectoryServiceURL, _ownIP, _sessionServiceIP, _sessionID, Application::instance());

		return MicroserviceAPI::initOld(_localDirectoryServiceURL, _ownIP, _sessionServiceIP, _sessionID);
	};
}