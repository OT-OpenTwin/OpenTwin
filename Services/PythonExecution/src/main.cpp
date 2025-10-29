// @otlicense

// Service header
#include "Application.h"

// OpenTwin header
#include "OTCommunication/ServiceLogNotifier.h"

// Qt header
#include <QtWidgets/qapplication.h>

#undef slots
#include "PythonWrapper.h"

namespace ot {
	namespace intern {
		void initializeLogger(void) {
#ifdef _RELEASEDEBUG
			ot::ServiceLogNotifier::initialize("PythonSubprocess", "", true);
#else
			ot::ServiceLogNotifier::initialize("PythonSubprocess", "", false);
#endif
		}

		bool initializeConnection(int _argc, char* _argv[]) {
#ifdef _RELEASEDEBUG
			Application::instance().getCommunicationHandler().setServerName("TestServerPython");
			PythonWrapper::setRedirectOutput(true);

#else
			if (_argc < 2) {
				OT_LOG_E("Missing arguments");
				return false;
			}
			OT_LOG_D("arg 1: " + std::string(_argv[1]));
			Application::instance().getCommunicationHandler().setServerName(_argv[1]);


#endif
			if (_argc > 2)
			{
				const std::string customSitePackagePath = std::string(_argv[2]);
				OT_LOG_D("arg 2: " + customSitePackagePath);
				if (!customSitePackagePath.empty())
				{
					PythonWrapper::setSitePackage(_argv[2]);
				}
			}

			if (_argc > 3)
			{
				OT_LOG_D("arg 3: " + std::string(_argv[3]));
				if (std::string(_argv[3]) == "1")
				{
					PythonWrapper::setRedirectOutput(true);
				}
			}

			return true;
		}
	}
}

int main(int _argc, char* _argv[], char* _envp[]) {
	// Create qt app
	QApplication app(_argc, _argv);

	// Initialize
	ot::intern::initializeLogger();
	if (!ot::intern::initializeConnection(_argc, _argv)) {
		return 1;
	}

	// Connect to server
	if (!Application::instance().getCommunicationHandler().ensureConnectionToServer()) {
		return 1;
	}

	// Run event loop
	int result = app.exec();

	return result;
}