// @otlicense
// File: main.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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