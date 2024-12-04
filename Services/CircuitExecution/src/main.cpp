#include <QtWidgets/qapplication.h>
#include "OTCommunication/ServiceLogNotifier.h"
#include <QtCore/qeventloop.h>
#include "Application.h"
#include <QtCore/qobject.h>



int main(int argc, char* argv[], char* envp[]) {

	QCoreApplication a(argc, argv);
#ifdef _DEBUG
	const std::string serverName = "TestServer";
	ot::ServiceLogNotifier::initialize("CircuitSubprocess", "", true);

#else
	const std::string serverName = argv[1];
	ot::ServiceLogNotifier::initialize("CircuitSubprocess", "", false);
#endif // _DEBUG

	Application::getInstance()->start(serverName);
	

	return a.exec();


	
}