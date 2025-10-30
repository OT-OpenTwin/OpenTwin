// @otlicense
// File: FileLogImporterDialog.h
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

// std header
#include <list>

namespace std {
	class thread;
}

namespace ot {
	class Label;
	class PushButton;
	class PlainTextEdit;
}

class FileLogImporterDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(FileLogImporterDialog)
public:
	FileLogImporterDialog(QWidget* _parent = (QWidget*)nullptr);
	virtual ~FileLogImporterDialog();

	//! @brief Move the buffered log messages to the provided list.
	//! @param _messages List to which the messages will be moved.
	void grabLogMessages(std::list<ot::LogMessage>& _messages) { _messages = std::move(m_logMessages); m_logMessages.clear(); };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotClear();
	void slotImport();

	void slotWorkerCompleted();

	void slotAppendOutputMessage(QString _msg);
	void slotAppendFilePathInfo(QString _path);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	void setControlsEnabled(bool _enable);
	void appendOutputMessage(const QString& _msg);
	void appendFilePathInfo(const QString& _path);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Worker

	void importWorker();
	void importFileLoggerFile(const QString& _filePath, const std::list<std::string>& _lines, std::list<ot::LogMessage>& _logMessages);
	void importJsonFile(const QString& _filePath, const std::list<std::string>& _lines, std::list<ot::LogMessage>& _logMessages);
	void importLogBufferFile(const QString& _filePath, const std::list<std::string>& _lines, std::list<ot::LogMessage>& _logMessages);

	std::list<ot::LogMessage> m_logMessages;

	std::atomic_bool m_isRunning;
	std::thread* m_workerThread;
	std::list<std::pair<QString, std::list<std::string>>> m_filesToImport;

	ot::PlainTextEdit* m_importedFiles;
	ot::Label* m_countLabel;

	ot::PlainTextEdit* m_output;
	ot::PushButton* m_confirmButton;
	ot::PushButton* m_importButton;
	ot::PushButton* m_clearButton;
};