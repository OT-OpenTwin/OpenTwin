/*
 * dllmain.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

// C++ header
#include <Windows.h>			// windows types
#include <thread>				// thread

// uiService header
#include <exception>
#include <fstream>

#include <QtCore/QCoreApplication>
#include <QtWebSockets/QtWebSockets>

// Open Twin header
#include "OTCore/JSON.h"					// rapidjson wrapper
#include "OTCore/Logger.h"				// Logger
#include "OTCore/ServiceBase.h"			// Logger initialization
#include "OTCommunication/ActionTypes.h"	// action member and types definition
#include "OTCommunication/ServiceLogNotifier.h"

#include "SocketServer.h"

SocketServer *globalSocketServer = nullptr;
unsigned int globalWebsocketPort = 0;
std::string globalWebsocketIP;
std::string globalServiceIP;
std::string globalSessionServiceIP;
std::string globalDirectoryServiceIP;

BOOL APIENTRY DllMain(HMODULE hModule,
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

void sessionServiceHealthChecker(std::string _sessionServiceURL) {
	// Create ping request
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string ping = pingDoc.toJson();

	bool alive = true;
	while (alive) {
		// Wait for 20s
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20s);

		// Try to send message and check the response
		std::string ret;
		try {
			if (!SocketServer::sendHttpRequest("execute", _sessionServiceURL, ping, ret)) { alive = false; }
			else OT_ACTION_IF_RESPONSE_ERROR(ret) { alive = false; }
			else OT_ACTION_IF_RESPONSE_WARNING(ret) { alive = false; }
			else if (ret != OT_ACTION_CMD_Ping) { alive = false; }
		}
		catch (...) { alive = false; }
	}

	OT_LOG_E("Session service has died unexpectedly. Shutting donw...");
	exit(0);
}

void mainApplicationThread()
{
	try {
		OT_LOG_I("Main application thread started");

		// Launch the Websocket Server
		int argc = 0;
		QCoreApplication application(argc, nullptr);

		globalSocketServer = new SocketServer(globalWebsocketIP, globalWebsocketPort);
		QObject::connect(globalSocketServer, &SocketServer::closed, &application, &QCoreApplication::quit);

		OT_LOG_D("Setup completed");

		application.exec();

		OT_LOG_D("Closing realay service");

		delete globalSocketServer;
		globalSocketServer = nullptr;
	}
	catch (const std::exception & e) {
		OT_LOG_E(std::string(e.what()));
		assert(0); // Something went wrong
	}
	catch (...) {
		OT_LOG_E("Unknown error");
		assert(0);	// Something went horribly wrong
	}
}

extern "C"
{
	_declspec(dllexport) int init(const char * _localDirectoryServiceURL, const char * _ownIP, const char * _websocketIP, const char * _sessionServiceIP)
	{
		try {
			std::string logUrl = qgetenv("OPEN_TWIN_LOGGING_URL").toStdString();

#ifdef _DEBUG
			ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_RelayService, logUrl, true);
#else
			ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_RelayService, logUrl, false);
#endif // _DEBUG

			OT_LOG_I("Initializing application");

			globalSessionServiceIP = _sessionServiceIP;

#ifdef _DEBUG
			{
				// Get file path
				char * buffer = new char[100];
				size_t bufferSize = 100;
				if (_dupenv_s(&buffer, &bufferSize, "OPENTWIN_DEV_ROOT") != 0) {
					OT_LOG_E("[ERROR] Please specify the environment variable \"OPENTWIN_DEV_ROOT\"");
					return -20;
				}
				std::string path = buffer;
				delete[] buffer;
				path.append("\\Deployment\\" OT_INFO_SERVICE_TYPE_RelayService ".cfg");

				// Read file
				std::ifstream stream(path);
				char inBuffer[512];
				stream.getline(inBuffer, 512);
				std::string info(inBuffer);
				assert(info.length() > 0);	// No file created

				// Parse doc
				ot::JsonDocument params;
				params.fromJson(info);

				// Apply data
				globalServiceIP = ot::json::getString(params, OT_ACTION_PARAM_SERVICE_URL);
				std::string websocketUrl = ot::json::getString(params, OT_ACTION_PARAM_WebsocketURL);
				globalWebsocketPort = std::stoi(websocketUrl.substr(websocketUrl.rfind(":") + 1));
				globalWebsocketIP = websocketUrl.substr(0, websocketUrl.rfind(":"));
				globalSessionServiceIP = ot::json::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
				globalDirectoryServiceIP = ot::json::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);

				OT_LOG_I("Application parameters were overwritten by configuration file: " + path);
			}
#else
			std::string websocketUrl(_websocketIP);
			globalWebsocketPort = std::stoi(websocketUrl.substr(websocketUrl.rfind(":") + 1));
			globalWebsocketIP = websocketUrl.substr(0, websocketUrl.rfind(":"));
			globalServiceIP = _ownIP;
#endif

			

			OT_LOG_I("Initializing relay service");
			OT_LOG_I("Session service HTTP address: " + globalSessionServiceIP);
			OT_LOG_I("Relay service HTTP address: " + globalServiceIP);
			OT_LOG_I("Relay service Websocket address: " + globalWebsocketIP + ":" + std::to_string(globalWebsocketPort));

			// Start session service health check
			std::thread t{ sessionServiceHealthChecker, globalSessionServiceIP.c_str() };
			t.detach();

			std::thread *app = new std::thread(mainApplicationThread);
		}
		catch (const std::exception & e) {
			assert(0); // Something went wrong
		}
		catch (...) {
			assert(0);	// Something went horribly wrong
		}
		return 0;
	}

	_declspec(dllexport) const char *performAction(const char *json, const char *senderIP)
	{
		QString retVal;
		try {

			//std::cout << "performAction: " << json << std::endl;
			QMetaObject::invokeMethod(globalSocketServer, "performAction", /*Qt::BlockingQueuedConnection*/
				Qt::DirectConnection, Q_RETURN_ARG(QString, retVal), Q_ARG(const char*, json), Q_ARG(const char*, senderIP));

			// std::cout << "performAction completed: " << retval << std::endl;
		}
		catch (const std::exception & e) {
			int x = 1;
			assert(0); // Error
		}
		catch (...) {
			int x = 1;
			assert(0); // Error
		}

		char *retval = new char[retVal.length() + 1];
		strcpy(retval, retVal.toStdString().c_str());

		return retval;
	}

	_declspec(dllexport) const char *queueAction(const char *json, const char *senderIP)
	{
		char *retval = nullptr;

		try {

			char *dataCopy = new char[strlen(json) + 1];
			strcpy(dataCopy, json);

			char *senderIPCopy = new char[strlen(senderIP) + 1];
			strcpy(senderIPCopy, senderIP);

			// std::cout << "queueAction: " << dataCopy << std::endl;

			QMetaObject::invokeMethod(globalSocketServer, "queueAction", Qt::QueuedConnection, Q_ARG(const char*, dataCopy), Q_ARG(const char*, senderIPCopy));
		}
		catch (const std::exception & e) {
			int x = 1;
			assert(0); // Error
		}
		catch (...) {
			int x = 1;
			assert(0); // Error
		}
		return retval;
	}

	_declspec(dllexport) const char *getServiceURL(void)
	{
		char *retVal = new char[globalServiceIP.size() + 1];
		strcpy(retVal, globalServiceIP.c_str());

		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char *data)
	{
		try {
			// std::cout << "deallocateData: ";
			if (data != nullptr)
			{
				QMetaObject::invokeMethod(globalSocketServer, "deallocateData", Qt::QueuedConnection, Q_ARG(const char*, data));
			}
		}
		catch (const std::exception & e) {
			assert(0); // Error
		}
		catch (...) {
			assert(0); // Error
		}
	}
}
