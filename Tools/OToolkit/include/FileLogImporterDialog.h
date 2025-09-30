//! @file FileLogImporterDialog.h
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

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

	const std::list<ot::LogMessage>& getLogMessages() const { return m_logMessages; };

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