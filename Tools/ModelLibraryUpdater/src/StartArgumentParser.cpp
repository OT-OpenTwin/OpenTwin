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

	QCommandLineOption dataPathOption(QStringList() << "d" << "data", "Path to the data directory", "datapath");
	parser.addOption(dataPathOption);

	QCommandLineOption collectionNameOption(QStringList() << "c" << "collection", "Name of the collection to update", "collection");
	parser.addOption(collectionNameOption);

	QCommandLineOption lmsUrlOption(QStringList() << "l" << "lmsurl", "URL of the LMS to connect to", "lmsurl");
	parser.addOption(lmsUrlOption);

	QCommandLineOption databasePswOption(QStringList() << "p" << "dbpwd", "Password for the database connection", "dbpwd");
	parser.addOption(databasePswOption);

	QStringList arguments = QCoreApplication::arguments();
	parser.process(QCoreApplication::arguments());

	if(parser.isSet(dataPathOption)) {
		m_dataPath = parser.value(dataPathOption);
	}

	if (parser.isSet(collectionNameOption)) {
		m_collectionName = parser.value(collectionNameOption);
	}

	if (parser.isSet(lmsUrlOption)) {
		m_lmsUrl = parser.value(lmsUrlOption);
	}

	if(parser.isSet(databasePswOption)) {
		m_databasePsw = parser.value(databasePswOption);
	}

	OT_LOG_IS("Starting Model Library Updater with { \"DataPath\": \"" << m_dataPath.toStdString() << "\", \"CollectionName\": \"" << m_collectionName.toStdString() << "\", \"LMSUrl\": \"" << m_lmsUrl.toStdString() << "\", \"DatabasePassword\": \"" << m_databasePsw.toStdString() << "\" }");

	return true;
}