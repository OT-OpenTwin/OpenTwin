/*
 * dllmain.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

// Relay Service header
#include "SocketServer.h"

// Open Twin header
#include "OTCore/JSON.h"					// rapidjson wrapper
#include "OTCore/Logger.h"				// Logger
#include "OTCore/ServiceBase.h"			// Logger initialization
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"	// action member and types definition
#include "OTCommunication/ServiceLogNotifier.h"

// Qt header
#include <QtCore/qcoreapplication.h>
#include <QtWebSockets/QtWebSockets>

// std header
#include <mutex>
#include <thread>
#include <fstream>
#include <Windows.h>
#include <exception>

bool g_appStartFailed{ false };
bool g_appStartCompleted{ false };
std::mutex g_appStartMutex;

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

	OT_LOG_D("Starting LSS health check at \"" + _sessionServiceURL + "\"");

	bool alive = true;
	while (alive) {
		// Wait for 20s
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20s);

		// Try to send message and check the response
		std::string ret;
		try {
			if (!ot::msg::send("", _sessionServiceURL, ot::EXECUTE, ping, ret)) { alive = false; }
			else OT_ACTION_IF_RESPONSE_ERROR(ret) { alive = false; }
			else OT_ACTION_IF_RESPONSE_WARNING(ret) { alive = false; }
			else if (ret != OT_ACTION_CMD_Ping) { alive = false; }
		}
		catch (...) { alive = false; }
	}

	OT_LOG_E("Session service \"" + _sessionServiceURL + "\" has died unexpectedly. Shutting down...");
	exit(0);
}

void mainApplicationThread(std::string _websocketIp, unsigned int _websocketPort, std::string _relayUrl)
{
	try {
		// Launch the Websocket Server
		int argc = 0;
		QCoreApplication application(argc, nullptr);

		SocketServer::instance().setRelayUrl(_relayUrl);
		SocketServer::instance().setWebsocketIp(_websocketIp);
		SocketServer::instance().setWebsocketPort(_websocketPort);

		OT_LOG_D("Relay url set to: " + SocketServer::instance().getRelayUrl());
		OT_LOG_D("Websocket set to: " + SocketServer::instance().getWebsocketIp() + ":" + std::to_string(SocketServer::instance().getWebsocketPort()));

		if (SocketServer::instance().startServer()) {
			g_appStartMutex.lock();
			g_appStartCompleted = true;
			g_appStartMutex.unlock();

			OT_LOG_D("Starting main event loop");
			application.exec();
		}
		else {
			g_appStartMutex.lock();
			g_appStartFailed = true;
			g_appStartMutex.unlock();

			OT_LOG_E("Server start failed");
		}
		
		OT_LOG_D("Closing realay service");
	}
	catch (const std::exception & e) {
		OT_LOG_EAS(std::string(e.what()));
	}
	catch (...) {
		OT_LOG_EA("Unknown error");
	}
}

extern "C"
{
	_declspec(dllexport) int init(const char * _localDirectoryServiceURL, const char * _ownIP, const char * _websocketIP, const char * _sessionServiceIP)
	{
		try {
			std::string logUrl = qgetenv("OPEN_TWIN_LOGGING_URL").toStdString();

#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
			ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_RelayService, logUrl, true);
#else
			ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_RelayService, logUrl, false);
#endif // _DEBUG

			std::string relayUrl;
			std::string websocketIp;
			unsigned int websocketPort = 0;
			std::string lssUlr = _sessionServiceIP;
			
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
				relayUrl = ot::json::getString(params, OT_ACTION_PARAM_SERVICE_URL);
				std::string websocketUrl = ot::json::getString(params, OT_ACTION_PARAM_WebsocketURL);
				websocketPort = std::stoi(websocketUrl.substr(websocketUrl.rfind(":") + 1));
				websocketIp = websocketUrl.substr(0, websocketUrl.rfind(":"));
				lssUlr = ot::json::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
				//globalDirectoryServiceIP = ot::json::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);
			}
#else
			std::string websocketUrl(_websocketIP);
			websocketPort = std::stoi(websocketUrl.substr(websocketUrl.rfind(":") + 1));
			websocketIp = websocketUrl.substr(0, websocketUrl.rfind(":"));
			relayUrl = _ownIP;
#endif

			// Start session service health check
			std::thread mainThread(mainApplicationThread, websocketIp, websocketPort, relayUrl);
			mainThread.detach();

			// The following while loop ensures that the main application thread started and the QApplication was created and the websocket started.
			bool needWait = true;
			while (needWait) {
				g_appStartMutex.lock();
				needWait = !g_appStartCompleted;
				if (g_appStartFailed) {
					exit(0);
				}
				g_appStartMutex.unlock();
			}

			std::thread t(sessionServiceHealthChecker, lssUlr);
			t.detach();

		}
		catch (const std::exception & e) {
			OT_LOG_EAS(e.what());
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
		}
		return 0;
	}

	_declspec(dllexport) const char *performAction(const char *json, const char *senderIP)
	{
		QString retVal;
		try {

			//std::cout << "performAction: " << json << std::endl;
			QMetaObject::invokeMethod(&SocketServer::instance(), "performAction", /*Qt::BlockingQueuedConnection*/
				Qt::DirectConnection, Q_RETURN_ARG(QString, retVal), Q_ARG(const char*, json), Q_ARG(const char*, senderIP));

			// std::cout << "performAction completed: " << retval << std::endl;
		}
		catch (const std::exception & e) {
			OT_LOG_EAS(e.what());
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
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

			QMetaObject::invokeMethod(&SocketServer::instance(), "queueAction", Qt::QueuedConnection, Q_ARG(const char*, dataCopy), Q_ARG(const char*, senderIPCopy));
		}
		catch (const std::exception & e) {
			OT_LOG_EAS(e.what());
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
		}
		return retval;
	}

	_declspec(dllexport) const char *getServiceURL(void)
	{
		char *retVal = new char[SocketServer::instance().getRelayUrl().size() + 1];
		strcpy(retVal, SocketServer::instance().getRelayUrl().c_str());

		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char *data)
	{
		try {
			// std::cout << "deallocateData: ";
			if (data != nullptr)
			{
				QMetaObject::invokeMethod(&SocketServer::instance(), "deallocateData", Qt::QueuedConnection, Q_ARG(const char*, data));
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
