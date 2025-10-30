// @otlicense
// File: CommitMessageDialog.h
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

#pragma once

// Qt header
#include <qobject.h>
#include <qicon.h>
#include <qstring.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <qdialog.h>
#include <qcheckbox.h>

class QLayout;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QWidget;
class QMouseEvent;
class QVBoxLayout;
class QHBoxLayout;

class CommitMessageDialog : public QDialog {
	Q_OBJECT
public:
	CommitMessageDialog(QString type, QString message);
	virtual ~CommitMessageDialog();

	QString changeMessage(void);
	bool includeResults(void);

	bool wasConfirmed(void) const { return my_confirmed; }

	void setNotConfirmed(void) { my_confirmed = false; }

	void cancelClose(void) { my_cancelClose = true; }

	void reset(const QString& _projectToCopy);

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonConfirmPressed();
	void slotButtonCancelPressed();
	void slotReturnPressed();

private:

	void Close(void);

	bool			my_confirmed;
	bool			my_cancelClose;

	QString			my_projectToCopy;

	QPushButton* my_buttonConfirm;
	QPushButton* my_buttonCancel;
	QLineEdit* my_input;
	QLabel* my_label;
	QVBoxLayout* my_layout;

	QHBoxLayout* my_layoutButtons;
	QWidget* my_widgetButtons;

	QHBoxLayout* my_layoutInput;
	QWidget* my_widgetInput;

	QCheckBox* my_includeResults;

	CommitMessageDialog(const CommitMessageDialog&) = delete;
	CommitMessageDialog& operator = (CommitMessageDialog&) = delete;
};
