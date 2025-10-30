// @otlicense
// File: ProjectInformationDialog.cpp
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

#include "LTSpiceConnector/ProjectInformationDialog.h"
#include "OTWidgets/IconManager.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtablewidget.h>
#include <qtablewidget.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qcombobox.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qheaderview.h>
#include <qevent.h>

ProjectInformationDialog::ProjectInformationDialog(const std::string& localFileName, const std::string& serverVersion, const std::string& localVersion)
{
	// Create controls
	my_buttonClose = new QPushButton{ "Close" };

	my_fileName = new QLineEdit;
	my_serverVersion = new QLineEdit;
	my_localVersion = new QLineEdit;

	my_lFileName = new QLabel("Local File Name:");
	my_lServerVersion = new QLabel("Server Version:");
	my_lLocalVersion = new QLabel("Local Version:");

	my_lFileName->setBuddy(my_fileName);
	my_lServerVersion->setBuddy(my_serverVersion);
	my_lLocalVersion->setBuddy(my_localVersion);

	// Create main layout
	my_layout = new QVBoxLayout{ this };

	// Create input layout
	my_widgetFileName = new QWidget;
	my_layoutFileName = new QHBoxLayout{ my_widgetFileName };
	my_layoutFileName->addWidget(my_lFileName);
	my_layoutFileName->addWidget(my_fileName);

	my_widgetServerVersion = new QWidget;
	my_layoutServerVersion = new QHBoxLayout{ my_widgetServerVersion };
	my_layoutServerVersion->addWidget(my_lServerVersion);
	my_layoutServerVersion->addWidget(my_serverVersion);

	my_widgetLocalVersion = new QWidget;
	my_layoutLocalVersion = new QHBoxLayout{ my_widgetLocalVersion };
	my_layoutLocalVersion->addWidget(my_lLocalVersion);
	my_layoutLocalVersion->addWidget(my_localVersion);

	my_layout->addWidget(my_widgetFileName);
	my_layout->addWidget(my_widgetServerVersion);
	my_layout->addWidget(my_widgetLocalVersion);

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonClose);
	my_layout->addWidget(my_widgetButtons);

	my_fileName->setText(localFileName.c_str());
	my_serverVersion->setText(serverVersion.c_str());
	my_localVersion->setText(localVersion.c_str());

	my_fileName->setReadOnly(true);
	my_serverVersion->setReadOnly(true);
	my_localVersion->setReadOnly(true);

	my_fileName->setMinimumWidth(300);

	setWindowTitle("Project Information");
	setWindowIcon(ot::IconManager::getApplicationIcon());

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(my_buttonClose, SIGNAL(clicked()), this, SLOT(slotButtonClosePressed()));
}

ProjectInformationDialog::~ProjectInformationDialog() {
	delete my_buttonClose;
	delete my_fileName;
	delete my_serverVersion;
	delete my_localVersion;
	delete my_lFileName;
	delete my_lServerVersion;
	delete my_lLocalVersion;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layoutFileName;
	delete my_widgetFileName;
	delete my_layoutLocalVersion;
	delete my_widgetLocalVersion;
	delete my_layoutServerVersion;
	delete my_widgetServerVersion;

	delete my_layout;
}

void ProjectInformationDialog::slotButtonClosePressed() { Close(); }



void ProjectInformationDialog::Close(void) {

	Q_EMIT isClosing();
	close();
}
