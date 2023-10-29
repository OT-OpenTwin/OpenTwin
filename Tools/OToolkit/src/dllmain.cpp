//! @file dllmain.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "StatusBar.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// C++ header
#include <Windows.h>			// windows types
#include <thread>				// thread

#include <exception>

#include <QtCore/qfile.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmessagebox.h>

// Open Twin header
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCommunication/actionTypes.h"

namespace otoolkit {
	namespace intern {
		static std::string g_serviceURL{ };
		static bool g_starting{ true };
	}
}

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
		// First create Qt Application instance, we do not pass any arguments
		int argc = 0;
		QApplication application(argc, nullptr);

		// Set global text size
		application.setStyleSheet("* { font-size: 9pt; }");

		// Apply global stylesheet
		QFile styleFile(":/OToolkit.qss");
		if (styleFile.exists()) {
			if (styleFile.open(QIODevice::ReadOnly)) {
				application.setStyleSheet(styleFile.readAll());
				styleFile.close();
			}
		}

		// Initialize API
		otoolkit::api::initialize(AppBase::instance());

		// Initialize OpenTwin API
		ot::LogDispatcher::instance().setLogFlags(ot::NO_LOG);

		// Initialize OToolkit
		AppBase::instance()->setApplicationInstance(&application);
		AppBase::instance()->setUrl(QString::fromStdString(otoolkit::intern::g_serviceURL));

		otoolkit::intern::g_starting = false;
		AppBase::instance()->sb()->setInfo("OToolkit running at: " + QString::fromStdString(otoolkit::intern::g_serviceURL));

		// Run Qt EventLoop
		exit(application.exec());
	}
	catch (const std::exception& _e) {
		otAssert(0, "Exception caught");
		QMessageBox msg(QMessageBox::Critical, "Fatal Error", _e.what(), QMessageBox::Ok);
		msg.exec();
		exit(2);
	}
	catch (...) {
		otAssert(0, "Unknown error");
		QMessageBox msg(QMessageBox::Critical, "Fatal Error", "Unknown error occured", QMessageBox::Ok);
		msg.exec();
		exit(1);
	}
}

extern "C"
{
	_declspec(dllexport) int init(const char * _unused1, const char * _ownUrl, const char * _unused2, const char * _unused3)
	{
		try {
			otoolkit::intern::g_serviceURL = _ownUrl;

			// Run the main application thread detached
			std::thread appt(mainApplicationThread);
			appt.detach();
			return 0;
		}
		catch (const std::exception& _e) {
			otAssert(0, "Exception caught");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", _e.what(), QMessageBox::Ok);
			msg.exec();
			return 2;
		}
		catch (...) {
			otAssert(0, "Unknown error");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", "Unknown error occured", QMessageBox::Ok);
			msg.exec();
			return 1;
		}
	}

	_declspec(dllexport) const char *performAction(const char *json, const char *senderIP)
	{
		// Ensure the initialization process is done
		while (otoolkit::intern::g_starting) {
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
		catch (const std::exception & _e) {
			otAssert(0, "Exception caught");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", _e.what(), QMessageBox::Ok);
			msg.exec();
			
		}
		catch (...) {
			otAssert(0, "Unknown error");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", "Unknown error occured", QMessageBox::Ok);
			msg.exec();
		}

		// Return empty string
		char *retval = new char[1];
		retval[0] = 0;
		return retval;
	}

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderUrl)
	{
		return performAction(_json, _senderUrl);
	}

	_declspec(dllexport) const char *getServiceURL(void)
	{
		char *retVal = new char[otoolkit::intern::g_serviceURL.size() + 1];
		strcpy(retVal, otoolkit::intern::g_serviceURL.c_str());

		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char * _data)
	{
		if (_data != nullptr)
		{
			delete[] _data;
		}
	}
}
