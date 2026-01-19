// @otlicense
// File: ProjectInformationDialog.h
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

#pragma once

// Qt header
#include <qobject.h>
#include <qicon.h>
#include <qstring.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <qdialog.h>

class QLayout;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QWidget;
class QMouseEvent;
class QVBoxLayout;
class QHBoxLayout;

class ProjectInformationDialog : public QDialog {
	Q_OBJECT
public:
	ProjectInformationDialog(const std::string &localFileName, const std::string& serverVersion, const std::string& localVersion);
	virtual ~ProjectInformationDialog();

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonClosePressed();

private:

	void Close(void);

	QPushButton* my_buttonClose;
	QLineEdit* my_fileName;
	QLineEdit* my_serverVersion;
	QLineEdit* my_localVersion;
	QLabel* my_lFileName;
	QLabel* my_lServerVersion;
	QLabel* my_lLocalVersion;
	QVBoxLayout* my_layout;

	QHBoxLayout* my_layoutButtons;
	QWidget* my_widgetButtons;

	QHBoxLayout* my_layoutFileName;
	QWidget* my_widgetFileName;
	QHBoxLayout* my_layoutServerVersion;
	QWidget* my_widgetServerVersion;
	QHBoxLayout* my_layoutLocalVersion;
	QWidget* my_widgetLocalVersion;

	ProjectInformationDialog(const ProjectInformationDialog&) = delete;
	ProjectInformationDialog& operator = (ProjectInformationDialog&) = delete;
};
