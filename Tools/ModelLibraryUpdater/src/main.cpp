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
#include "OTCore/Logging/Logger.h"
#include "OTCore/Logging/LogNotifierStdCout.h"
#include "OTCore/Logging/LogNotifierFileWriter.h"

// Service header
#include "Application.h"
#include "StartArgumentParser.h"

// Qt header
#include <QtCore/qcoreapplication.h>

void initializeLogging(void) 
{
	std::string _filePath = ".\\ModelLibraryUpdater.log";;
#ifdef _DEBUG
	ot::LogDispatcher::initialize("ModelLibraryUpdater", true);
#else
	ot::LogDispatcher::initialize("ModelLibraryUpdater", false);
#endif // DEBUG

	ot::LogDispatcher& dispatcher = ot::LogDispatcher::instance();
	dispatcher.addReceiver(new ot::LogNotifierStdCout());
}


int main(int _argc, char* _argv[]) {

	initializeLogging();

	QCoreApplication app(_argc, _argv);

	ot::StartArgumentParser argumentParser;
	argumentParser.parse();

	Application::getInstance()->start(argumentParser);

	return 0;
}