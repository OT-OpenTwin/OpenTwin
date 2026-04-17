// @otlicense
// File: StartArgumentParser.cpp
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

// Service header
#include "StartArgumentParser.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/Logging/Logger.h"

// Qt header
#include <QtCore/qcommandlineparser.h>

bool ot::StartArgumentParser::parse() {

	QCommandLineParser parser;
	parser.setApplicationDescription("Model Library Updater");
	parser.addHelpOption();

	QCommandLineOption collectionNameOption(QStringList() << "c" << "collection", "Name of the collection to update", "collection");
	parser.addOption(collectionNameOption);

	QCommandLineOption lmsUrlOption(QStringList() << "l" << "lmsurl", "URL of the LMS to connect to", "lmsurl");
	parser.addOption(lmsUrlOption);


	// Debug: Alle Argumente loggen
	QStringList arguments = QCoreApplication::arguments();
	OT_LOG_D("Total arguments: " + std::to_string(arguments.size()));
	for (int i = 0; i < arguments.size(); ++i) {
		OT_LOG_D("Arg[" + std::to_string(i) + "]: " + arguments[i].toStdString());
	}

	// Debug: Nach dem Parsing
	OT_LOG_D("Collection option set: " + std::to_string(parser.isSet(collectionNameOption)));
	OT_LOG_D("LMS URL option set: " + std::to_string(parser.isSet(lmsUrlOption)));

	parser.process(QCoreApplication::arguments());

	if (parser.isSet(collectionNameOption)) {
		m_collectionName = parser.value(collectionNameOption);
	}

	if (parser.isSet(lmsUrlOption)) {
		m_lmsUrl = parser.value(lmsUrlOption);
	}

	return true;
}