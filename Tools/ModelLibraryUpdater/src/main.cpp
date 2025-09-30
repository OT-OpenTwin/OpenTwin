// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/LogNotifierFileWriter.h"

// Service header
#include "Application.h"


int main(int _argc, char* _argv[]) {

	std::string _filePath = ".\\ModelLibraryUpdater.log";;
#ifdef _DEBUG
	ot::LogDispatcher::initialize("ModelLibraryUpdater", true);
#else
	ot::LogDispatcher::initialize("ModelLibraryUpdater", false);
#endif // DEBUG

	ot::LogDispatcher& dispatcher = ot::LogDispatcher::instance();
	dispatcher.addReceiver(new ot::LogNotifierFileWriter(_filePath));

	if (_argc == 1) {
		Application::getInstance()->start("");
	}
	else if(_argc == 2){
		Application::getInstance()->start(_argv[1]);
	}
	else {
		OT_LOG_E("Error: Only a maximum of one argument is allowed!");
		return 0;
	}
	
	

	return 0;
}