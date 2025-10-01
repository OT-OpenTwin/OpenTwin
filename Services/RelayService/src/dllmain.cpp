//! @file dllmain.cpp
//! @author Peter Thoma, Alexander Kuester
//! @date April 2020
// ###########################################################################################################################################################################################################################################################################################################################

// Relay Service header
#include "SocketServer.h"

// Open Twin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/JSON.h"					// rapidjson wrapper
#include "OTCore/DebugHelper.h"
#include "OTCore/LogDispatcher.h"				// Logger
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

void mainApplicationThread(std::string _websocketIp, unsigned int _websocketPort, std::string _relayUrl, std::string _lssUrl)
{
	try {
		// Launch the Websocket Server
		int argc = 0;
		QCoreApplication application(argc, nullptr);

		SocketServer& server = SocketServer::instance();
		server.setRelayUrl(_relayUrl);
		server.setWebsocketIp(_websocketIp);
		server.setWebsocketPort(_websocketPort);
		
		ot::DebugHelper::serviceSetupCompleted(server.getServiceDebugInformation());

		// Start session service health check if LSS URL is given
		if (!_lssUrl.empty()) {
			server.startSessionServiceHealthCheck(_lssUrl);
		}

		if (server.startServer()) {
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
	_declspec(dllexport) int init(const char * _ownUrl, const char * _websocketUrl, const char * _unused1, const char * _unused2)
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
			std::string lssUrl;
			
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
				lssUrl = ot::json::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
			}
#else
			std::string websocketUrl(_websocketUrl);
			websocketPort = std::stoi(websocketUrl.substr(websocketUrl.rfind(":") + 1));
			websocketIp = websocketUrl.substr(0, websocketUrl.rfind(":"));
			relayUrl = _ownUrl;
#endif

			// Start session service health check
			std::thread mainThread(mainApplicationThread, websocketIp, websocketPort, relayUrl, lssUrl);
			mainThread.detach();

			// The following while loop ensures that the main application thread started and the QApplication was created and the websocket started.
			bool needWait = true;
			while (needWait) {
				g_appStartMutex.lock();
				needWait = !g_appStartCompleted;
				if (g_appStartFailed) {
					exit(ot::AppExitCode::GeneralError);
				}
				g_appStartMutex.unlock();
			}
		}
		catch (const std::exception & e) {
			OT_LOG_EAS(e.what());
			exit(ot::AppExitCode::GeneralError);
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
			exit(ot::AppExitCode::UnknownError);
		}
		return 0;
	}

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP)
	{
		QString retVal;
		try {
			QMetaObject::invokeMethod(
				&SocketServer::instance(),
				"performAction",
				Qt::DirectConnection, 
				Q_RETURN_ARG(QString, retVal), 
				Q_ARG(const char*, _json),
				Q_ARG(const char*, _senderIP)
			);
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

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderIP)
	{
		char *retval = nullptr;

		try {

			char *dataCopy = new char[strlen(_json) + 1];
			strcpy(dataCopy, _json);

			char *senderIPCopy = new char[strlen(_senderIP) + 1];
			strcpy(senderIPCopy, _senderIP);

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

	_declspec(dllexport) const char *getServiceURL()
	{
		char *retVal = new char[SocketServer::instance().getRelayUrl().size() + 1];
		strcpy(retVal, SocketServer::instance().getRelayUrl().c_str());

		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char * _data)
	{
		try {
			// std::cout << "deallocateData: ";
			if (_data != nullptr)
			{
				QMetaObject::invokeMethod(&SocketServer::instance(), "deallocateData", Qt::QueuedConnection, Q_ARG(const char*, _data));
			}
		}
		catch (const std::exception & e) {
			OT_LOG_EAS(e.what());
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
		}
	}
}
