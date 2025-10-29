// @otlicense

//! @file dllmain.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "StatusManager.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// C++ header
#include <Windows.h>			// windows types
#include <thread>				// thread

#include <exception>

#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>

// Open Twin header
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/ApplicationPropertiesManager.h"
#include "OTCommunication/actionTypes.h"

#define DLLMAIN_LOG(___msg) OTOOLKIT_LOG("main", ___msg)
#define DLLMAIN_LOGW(___msg) OTOOLKIT_LOGW("main", ___msg)
#define DLLMAIN_LOGE(___msg) OTOOLKIT_LOGE("main", ___msg)

namespace otoolkit {
	namespace intern {
		static std::string g_serviceURL{ };
		static std::string g_otoolitArgs{ };
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
		// Initialize seed
		srand(time(0));

		// First create Qt Application instance, we do not pass any arguments
		int argc = 0;
		QApplication application(argc, nullptr);

		// Initialize OpenTwin API
		#ifdef OT_RELEASE_DEBUG
			ot::LogDispatcher::initialize("Toolkit", true);
		#else
			ot::LogDispatcher::initialize("Toolkit", false);
		#endif 
		ot::LogDispatcher::instance().setLogFlags(ot::ALL_GENERAL_LOG_FLAGS | ot::TEST_LOG);

		// Set global text size
		ot::GlobalColorStyle::instance().setApplication(&application);

		// Setup icon manager
		int iconPathCounter{ 0 };
		int stylePathCounter{ 0 };
#ifdef _DEBUG
		if (ot::IconManager::addSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/Icons/"))) {
			iconPathCounter++;
		}
		if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/ColorStyles/"))) {
			stylePathCounter++;
		}
#endif // _DEBUG
		if (ot::IconManager::addSearchPath(QDir::currentPath() + "/icons/")) {
			iconPathCounter++;
		}
		if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QDir::currentPath() + "/ColorStyles/")) {
			stylePathCounter++;
		}

		// Check if at least one icon directory was found
		if (iconPathCounter == 0) {
			OT_LOG_EA("No icon path found");
			QMessageBox msg(QMessageBox::Critical, "Error", "No icon path was found. Try to reinstall the application", QMessageBox::Ok);
			msg.exec();
			exit(-1);
		}

		// Check if at least one style directory was found
		if (stylePathCounter == 0) {
			OT_LOG_EA("No color style path found");
			QMessageBox msg(QMessageBox::Critical, "Error", "No color style path was found. Try to reinstall the application", QMessageBox::Ok);
			msg.exec();
			exit(-2);
		}

		// Initialize application icon
		ot::IconManager::setApplicationIcon(QIcon(":/images/OToolkit.png"));

		// Initialize color style
		ot::GlobalColorStyle::instance().scanForStyleFiles();
		ot::GlobalColorStyle::instance().setCurrentStyle(ot::toString(ot::ColorStyleName::BrightStyle));

		// Initialize settings manager
		ot::ApplicationPropertiesManager::instance().setReplaceExistingPropertiesOnMerge(true);

		// Initialize OToolkit
		AppBase::instance()->setApplicationInstance(&application);
		AppBase::instance()->setUrl(QString::fromStdString(otoolkit::intern::g_serviceURL));

		AppBase::instance()->setUpdateTransparentColorStyleValueEnabled(true);
		
		AppBase::instance()->parseStartArgs(otoolkit::intern::g_otoolitArgs);

		otoolkit::intern::g_starting = false;

		// Run Qt EventLoop
		exit(application.exec());
	}
	catch (const std::exception& _e) {
		OTAssert(0, "Exception caught");
		QMessageBox msg(QMessageBox::Critical, "Fatal Error", _e.what(), QMessageBox::Ok);
		msg.exec();
		exit(2);
	}
	catch (...) {
		OTAssert(0, "Unknown error");
		QMessageBox msg(QMessageBox::Critical, "Fatal Error", "Unknown error occured", QMessageBox::Ok);
		msg.exec();
		exit(1);
	}
}

extern "C"
{
	_declspec(dllexport) int init(const char * _unused1, const char * _ownUrl, const char * _unused2, const char * _OToolkitArgs)
	{
		try {
			otoolkit::intern::g_serviceURL = _ownUrl;
			otoolkit::intern::g_otoolitArgs = _OToolkitArgs;

			// Run the main application thread detached
			std::thread appt(mainApplicationThread);
			appt.detach();
			return 0;
		}
		catch (const std::exception& _e) {
			OTAssert(0, "Exception caught");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", _e.what(), QMessageBox::Ok);
			msg.exec();
			return 2;
		}
		catch (...) {
			OTAssert(0, "Unknown error");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", "Unknown error occured", QMessageBox::Ok);
			msg.exec();
			return 1;
		}
	}

	_declspec(dllexport) const char *performAction(const char* _json, const char* _senderUrl) {
		// Ensure the initialization process is done
		while (otoolkit::intern::g_starting) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}

		try {
			// Check if the received message was a ping message
			ot::JsonDocument inboundAction;
			inboundAction.fromJson(_json);
			if (inboundAction.IsObject()) {
				std::string action = ot::json::getString(inboundAction, OT_ACTION_MEMBER);
				if (action == OT_ACTION_CMD_Ping) {
					char * r = new char[strlen(OT_ACTION_CMD_Ping) + 1];
					strcpy(r, OT_ACTION_CMD_Ping);
					return r;
				}
				else {
					QMetaObject::invokeMethod(AppBase::instance(), &AppBase::slotProcessMessage, Qt::QueuedConnection, QString(_json));
					std::string ret = ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					char * r = new char[ret.length() + 1];
					strcpy(r, ret.c_str());
					return r;
				}
			}
		}
		catch (const std::exception & _e) {
			OTAssert(0, "Exception caught");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", _e.what(), QMessageBox::Ok);
			msg.exec();
			
		}
		catch (...) {
			OTAssert(0, "Unknown error");
			QMessageBox msg(QMessageBox::Critical, "Fatal Error", "Unknown error occured", QMessageBox::Ok);
			msg.exec();
		}

		// Return empty string
		char *retval = new char[1];
		retval[0] = 0;
		return retval;
	}

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderUrl) {
		return performAction(_json, _senderUrl);
	}

	_declspec(dllexport) const char* performActionOneWayTLS(const char* _json, const char* _senderUrl) {
		return performAction(_json, _senderUrl);
	}

	_declspec(dllexport) const char *getServiceURL() {
		char *retVal = new char[otoolkit::intern::g_serviceURL.size() + 1];
		strcpy(retVal, otoolkit::intern::g_serviceURL.c_str());

		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char * _data) {
		if (_data != nullptr) {
			delete[] _data;
		}
	}
}
