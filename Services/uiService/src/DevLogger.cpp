// @otlicense
// File: DevLogger.cpp
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

#include "DevLogger.h"

#if OT_UISERVICE_DEV_LOGGER_ACTIVE==1

#include <OTCommunication/ServiceLogNotifier.h>

#include <QtWidgets/qmainwindow.h>

ot::SimpleLogViewer::SimpleLogViewer() : m_creatorThread(QThread::currentThreadId()) {
	QFont f = font();
	f.setFamily("Consolas");
	setFont(f);
	setReadOnly(true);
}

ot::SimpleLogViewer::~SimpleLogViewer() {

}

void ot::SimpleLogViewer::appendLog(const ot::LogMessage& _message) {
	if (QThread::currentThreadId() == m_creatorThread) {
		slotAppendLog(_message);
	}
	else {
		QMetaObject::invokeMethod(this, &SimpleLogViewer::slotAppendLog, Qt::QueuedConnection, const ot::LogMessage&, _message);
	}
}

void ot::SimpleLogViewer::slotAppendLog(const ot::LogMessage& _message) {
	QTextCursor cursor = this->textCursor();
	QTextCharFormat format = cursor.charFormat();
	QTextCharFormat formatTime = format;
	QTextCharFormat formatWarning = format;
	QTextCharFormat formatError = format;
	formatTime.setForeground(QBrush(QColor(40, 40, 255)));
	formatWarning.setForeground(QBrush(QColor(255, 242, 0)));
	formatError.setForeground(QBrush(QColor(255, 0, 0)));

	cursor.insertText("[");
	cursor.setCharFormat(formatTime);
	cursor.insertText(QString::fromStdString(_message.localSystemTime()));
	cursor.setCharFormat(format);
	cursor.insertText("] [");

	if (_message.flags() & ot::INFORMATION_LOG) {
		cursor.insertText("Info]     ");
	}
	else if (_message.flags() & ot::DETAILED_LOG) {
		cursor.insertText("Detailed] ");
	}
	else if (_message.flags() & ot::WARNING_LOG) {
		cursor.setCharFormat(formatWarning);
		cursor.insertText("Warning]  ");
		cursor.setCharFormat(format);
	}
	else if (_message.flags() & ot::ERROR_LOG) {
		cursor.setCharFormat(formatError);
		cursor.insertText("Error]    ");
		cursor.setCharFormat(format);
	}
	else {
		cursor.setCharFormat(formatError);
		cursor.insertText("Unknown]  ");
		cursor.setCharFormat(format);
	}

	cursor.insertText(QString::fromStdString("[" + _message.functionName() + "] "));

	cursor.insertText(QString::fromStdString(_message.text()));
	this->setTextCursor(cursor);
	this->appendPlainText("");
}

// #################################################################################################################################################################################################################

// #################################################################################################################################################################################################################

// #################################################################################################################################################################################################################

ot::intern::UiDevLogger& ot::intern::UiDevLogger::instance(void) {
	static ot::intern::UiDevLogger g_instance;
	return g_instance;
}

void ot::intern::UiDevLogger::initialize(void) {
	ot::intern::UiDevLogger& ref = instance();
	ref.deleteLogNotifierLater();

	ot::LogDispatcher::instance().addReceiver(&ref);

	ot::LogMessage welcomeMessage("", __FUNCTION__, "Frontend DevLogger attached");
	welcomeMessage.setCurrentTimeAsLocalSystemTime();
	ref.log(welcomeMessage);
}

void ot::intern::UiDevLogger::log(const ot::LogMessage& _message) {
	m_viewer->appendLog(_message);
}

ot::intern::UiDevLogger::UiDevLogger() {
	m_window = new QMainWindow;
	m_viewer = new ot::SimpleLogViewer;
	m_window->setCentralWidget(m_viewer);
	m_window->resize(800, 600);
	m_window->setWindowTitle("DevLogger - OpenTwin (DO NOT SHIP THIS BUILD)");
	m_window->showNormal();
}

ot::intern::UiDevLogger::~UiDevLogger() {
	delete m_viewer;
}

#endif