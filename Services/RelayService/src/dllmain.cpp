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

#include <Logger.h>

#include <QtCore/QCoreApplication>
#include <QtWebSockets/QtWebSockets>

// Open Twin header
#include "OpenTwinCore/rJSON.h"					// rapidjson wrapper
#include "OpenTwinCore/Logger.h"				// Logger
#include "OpenTwinCore/ServiceBase.h"			// Logger initialization
#include "OpenTwinCommunication/ActionTypes.h"	// action member and types definition

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
	OT_rJSON_createDOC(pingDoc);
	ot::rJSON::add(pingDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
	std::string ping = ot::rJSON::toJSON(pingDoc);

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
			// Initialize the logging service. This will enable logging to the file path base specified in OPEN_TWIN_LOGFILE
			ot::LogDispatcher::instance().setServiceName(OT_INFO_SERVICE_TYPE_RelayService);

			OT_LOG_I("Initializing application");

			globalSessionServiceIP = _sessionServiceIP;

#ifdef _DEBUG
			{
				// Get file path
				char * buffer = new char[100];
				size_t bufferSize = 100;
				if (_dupenv_s(&buffer, &bufferSize, "SIM_PLAT_ROOT") != 0) {
					OT_LOG_E("[ERROR] Please specify the environment variable \"SIM_PLAT_ROOT\"");
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
				OT_rJSON_parseDOC(params, info.c_str());
				OT_rJSON_docCheck(params);

				// Apply data
				globalServiceIP = ot::rJSON::getString(params, OT_ACTION_PARAM_SERVICE_URL);
				std::string websocketUrl = ot::rJSON::getString(params, OT_ACTION_PARAM_WebsocketURL);
				globalWebsocketPort = std::stoi(websocketUrl.substr(websocketUrl.rfind(":") + 1));
				globalWebsocketIP = websocketUrl.substr(0, websocketUrl.rfind(":"));
				globalSessionServiceIP = ot::rJSON::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
				globalDirectoryServiceIP = ot::rJSON::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);

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
