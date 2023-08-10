/*
 * dllmain.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#include "AppBase.h"
#include "StatusBar.h"

// C++ header
#include <Windows.h>			// windows types
#include <thread>				// thread

#include <exception>

#include <QtCore/qfile.h>
#include <QtWidgets/qapplication.h>

// Open Twin header
#include "OpenTwinCore/rJSON.h"				// rapidjson wrapper
#include "OpenTwinCore/Logger.h"		// logger
#include "OpenTwinCommunication/actionTypes.h"		// action member and types definition

static std::string g_serviceURL;
static bool g_starting{ true };

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

void mainApplicationThread()
{
	try {
		int argc = 0;
		QApplication application(argc, nullptr);

		// Set global text size
		application.setStyleSheet("* { font-size: 9pt; }");

		QFile styleFile(":/OToolkit.qss");
		if (styleFile.exists()) {
			if (styleFile.open(QIODevice::ReadOnly)) {
				application.setStyleSheet(styleFile.readAll());
				styleFile.close();
			}
		}

		ot::LogDispatcher::instance().setLogFlags(ot::NO_LOG);
		AppBase::instance()->setApplicationInstance(&application);
		AppBase::instance()->setUrl(QString::fromStdString(g_serviceURL));

		g_starting = false;
		AppBase::instance()->sb()->setInfo("OToolkit running at: " + QString::fromStdString(g_serviceURL));

		application.exec();
	}
	catch (const std::exception & e) {
		assert(0); // Something went wrong
	}
	catch (...) {
		assert(0);
	}
	exit(0);
}

extern "C"
{
	_declspec(dllexport) int init(const char * _unused1, const char * _ownIP, const char * _unused2, const char * _unused3)
	{
		try {
			g_serviceURL = _ownIP;

			// Start session service health check
			std::thread appt(mainApplicationThread);
			appt.detach();
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
		while (g_starting) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}

		try {
			// Check if the received message was a ping message
			OT_rJSON_parseDOC(inboundAction, json);
			if (inboundAction.IsObject()) {
				std::string action = ot::rJSON::getString(inboundAction, OT_ACTION_MEMBER);
				if (action == OT_ACTION_CMD_Ping) {
					char * r = new char[strlen(OT_ACTION_CMD_Ping) + 1];
					strcpy(r, OT_ACTION_CMD_Ping);
					return r;
				}
				else {
					QMetaObject::invokeMethod(AppBase::instance(), "slotProcessMessage", Qt::QueuedConnection, Q_ARG(const QString&, QString(json)));
					char * r = new char[strlen(OT_ACTION_RETURN_VALUE_OK) + 1];
					strcpy(r, OT_ACTION_RETURN_VALUE_OK);
					return r;
				}
			}
		}
		catch (const std::exception & e) {
			assert(0); // Error
		}
		catch (...) {
			assert(0); // Error
		}

		char *retval = new char[1];
		retval[0] = 0;
		return retval;
	}

	_declspec(dllexport) const char *queueAction(const char *json, const char *senderIP)
	{
		return performAction(json, senderIP);
	}

	_declspec(dllexport) const char *getServiceURL(void)
	{
		char *retVal = new char[g_serviceURL.size() + 1];
		strcpy(retVal, g_serviceURL.c_str());

		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char *data)
	{
		if (data != nullptr)
		{
			delete[] data;
		}
	}
}
