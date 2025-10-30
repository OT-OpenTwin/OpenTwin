// @otlicense
// File: LogVisualizationItemViewDialog.h
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Dialog.h"

class QWidget;
class QLineEdit;
class QCheckBox;
class QShortcut;
class QPushButton;
class QPlainTextEdit;

class LogVisualizationItemViewDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(LogVisualizationItemViewDialog)
	OT_DECL_NODEFAULT(LogVisualizationItemViewDialog)
public:
	LogVisualizationItemViewDialog(const ot::LogMessage& _msg, size_t _index, QWidget* _parent);
	virtual ~LogVisualizationItemViewDialog();

protected:
	virtual void closeEvent(QCloseEvent* _event) override;

	virtual void mousePressEvent(QMouseEvent* _event) override;

	virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

	virtual bool event(QEvent* _event) override;

private Q_SLOTS:
	void slotRecenter(void);
	void slotDisplayMessageText(int _state);
	void slotUpdateTimestamps();

private:
	QString findJsonSyntax(const QString& _inputString);

	ot::LogMessage m_msg;

	QShortcut* m_closeShortcut;
	QShortcut* m_recenterShortcut;

	QLineEdit* m_time;
	QCheckBox* m_timeUTC;
	QLineEdit* m_timeLocal;
	QCheckBox* m_timeLocalUTC;
	QCheckBox* m_findMessageSyntax;

	QPlainTextEdit* m_message;

	QPushButton* m_okButton;
};
