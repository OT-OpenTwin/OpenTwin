// @otlicense

// OToolkit header
#include "AppBase.h"
#include "FileLogImporterDialog.h"

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTCore/String.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <QtCore/qthread.h>
#include <QtCore/qdatetime.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qfiledialog.h>

// std header
#include <thread>

FileLogImporterDialog::FileLogImporterDialog(QWidget* _parent) 
	: ot::Dialog(_parent), m_workerThread(nullptr), m_isRunning(false)
{
	// Create layouts
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QVBoxLayout* centralLayout = new QVBoxLayout;
	QHBoxLayout* bottomLayout = new QHBoxLayout;
	QHBoxLayout* outputLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	m_importedFiles = new ot::PlainTextEdit;
	m_importedFiles->setReadOnly(true);

	m_countLabel = new ot::Label("Message count: 0");
	m_importButton = new ot::PushButton("Import");
	m_clearButton = new ot::PushButton("Clear");
	
	m_confirmButton = new ot::PushButton("Confirm");
	ot::PushButton* btnCancel = new ot::PushButton("Cancel");

	m_output = new ot::PlainTextEdit;
	m_output->setReadOnly(true);

	// Setup layouts
	mainLayout->addLayout(centralLayout);
	mainLayout->addLayout(bottomLayout);
	mainLayout->addLayout(outputLayout, 1);
	mainLayout->addLayout(buttonLayout);

	centralLayout->addWidget(m_importedFiles);

	bottomLayout->addWidget(m_countLabel, 1);
	bottomLayout->addWidget(m_importButton);
	bottomLayout->addWidget(m_clearButton);

	outputLayout->addWidget(m_output);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(m_confirmButton);
	buttonLayout->addWidget(btnCancel);

	// Setup window
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->setWindowTitle("File Log Importer");
	this->resize(400, 200);

	// Connect signals
	this->connect(m_importButton, &ot::PushButton::clicked, this, &FileLogImporterDialog::slotImport);
	this->connect(m_clearButton, &ot::PushButton::clicked, this, &FileLogImporterDialog::slotClear);
	this->connect(m_confirmButton, &ot::PushButton::clicked, this, &FileLogImporterDialog::closeOk);
	this->connect(btnCancel, &ot::PushButton::clicked, this, &FileLogImporterDialog::closeCancel);
}

FileLogImporterDialog::~FileLogImporterDialog() {
	m_isRunning = false;
	if (m_workerThread) {
		if (m_workerThread->joinable()) {
			m_workerThread->join();
		}
		delete m_workerThread;
		m_workerThread = nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void FileLogImporterDialog::slotClear() {
	m_logMessages.clear();
	m_importedFiles->clear();
	m_output->clear();

	m_countLabel->setText("Message count: 0");
}

void FileLogImporterDialog::slotImport() {
	if (m_workerThread) {
		m_output->appendPlainText("Import is already running. Please wait until it completes.");
		return;
	}

	auto settings = AppBase::instance()->createSettingsInstance();

	// Select file to import

	QStringList filePaths = QFileDialog::getOpenFileNames(this, "Import Log File", settings->value("Logging.FileImporter.LastDirectory", QString()).toString(), "Plain Log Files(*.otlog);;Log Files(*.otlog.json);;Log Buffer File(*.otlogbuf)");

	if (filePaths.isEmpty()) {
		return;
	}

	settings->setValue("Logging.FileImporter.LastDirectory", QFileInfo(filePaths.front()).absolutePath());
	
	m_filesToImport.clear();

	for (const QString& filePath : filePaths) {
		// Read file content
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			OT_LOG_E("Failed to open file for reading: " + filePath.toStdString());
			return;
		}

		std::list<std::string> lines;
		while (!file.atEnd()) {
			QString line = file.readLine();
			if (!line.isEmpty()) {
				lines.push_back(line.toStdString());
			}
		}
		file.close();

		if (!lines.empty()) {
			m_filesToImport.push_back(std::pair<QString, std::list<std::string>>(filePath, std::move(lines)));
		} else {
			m_output->appendPlainText("File is empty: \"" + filePath + "\"");
		}
	}

	if (m_filesToImport.empty()) {
		m_output->appendPlainText("No valid files to import.");
		return;
	}

	this->setControlsEnabled(false);
	m_isRunning = true;
	m_workerThread = new std::thread(&FileLogImporterDialog::importWorker, this);
}

void FileLogImporterDialog::slotWorkerCompleted() {
	m_isRunning = false;
	if (m_workerThread) {
		if (m_workerThread->joinable()) {
			m_workerThread->join();
		}
		delete m_workerThread;
		m_workerThread = nullptr;
	}

	this->setControlsEnabled(true);
	m_countLabel->setText("Message count: " + QString::number(m_logMessages.size()));
}

void FileLogImporterDialog::slotAppendOutputMessage(QString _msg) {
	m_output->appendPlainText(_msg);
}

void FileLogImporterDialog::slotAppendFilePathInfo(QString _path) {
	m_importedFiles->appendPlainText(_path);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void FileLogImporterDialog::setControlsEnabled(bool _enable) {
	m_clearButton->setEnabled(_enable);
	m_importButton->setEnabled(_enable);
	m_confirmButton->setEnabled(_enable);
}

void FileLogImporterDialog::appendOutputMessage(const QString& _msg) {
	if (QThread::currentThread() == this->thread()) {
		m_output->appendPlainText(_msg);
	} else {
		QMetaObject::invokeMethod(this, &FileLogImporterDialog::slotAppendOutputMessage, Qt::QueuedConnection, _msg);
	}
}

void FileLogImporterDialog::appendFilePathInfo(const QString& _path) {
	if (QThread::currentThread() == this->thread()) {
		m_importedFiles->appendPlainText(_path);
	} else {
		QMetaObject::invokeMethod(this, &FileLogImporterDialog::slotAppendFilePathInfo, Qt::QueuedConnection, _path);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Worker

void FileLogImporterDialog::importWorker() {
	std::list<ot::LogMessage> newLogMessages;

	QDateTime startTime = QDateTime::currentDateTime();

	while (m_isRunning && !m_filesToImport.empty()) {
		std::pair<QString, std::list<std::string>> info = m_filesToImport.front();
		m_filesToImport.pop_front();

		if (info.first.endsWith(".otlog")) {
			this->importFileLoggerFile(info.first, info.second, newLogMessages);
		}
		else if (info.first.endsWith(".otlog.json")) {
			this->importJsonFile(info.first, info.second, newLogMessages);
		}
		else if (info.first.endsWith(".otlogbuf")) {
			this->importLogBufferFile(info.first, info.second, newLogMessages);
		}
		else {
			this->appendOutputMessage("Unsupported file format: " + info.first);
			continue;
		}
	}

	// Sort messages by timestamp
	if (m_isRunning) {
		QDateTime importEndTime = QDateTime::currentDateTime();

		this->appendOutputMessage("Importing " + QString::number(newLogMessages.size()) + " messages completed. Took " + QString::number(startTime.msecsTo(importEndTime)) + " ms");

		m_logMessages.splice(m_logMessages.end(), std::move(newLogMessages));

		m_logMessages.sort([](const ot::LogMessage& _a, const ot::LogMessage& _b) {
			return _a.getGlobalSystemTime() < _b.getGlobalSystemTime();
		});

		QDateTime sortEndTime = QDateTime::currentDateTime();

		this->appendOutputMessage("Sorting messages completed. Took " + QString::number(importEndTime.msecsTo(sortEndTime)) + " ms\nImport completed.");
	}
	else {
		this->appendOutputMessage("Import was cancelled.");
	}

	QMetaObject::invokeMethod(this, &FileLogImporterDialog::slotWorkerCompleted, Qt::QueuedConnection);
}

void FileLogImporterDialog::importFileLoggerFile(const QString& _filePath, const std::list<std::string>& _lines, std::list<ot::LogMessage>& _logMessages) {
	this->appendOutputMessage("Parsing file logger file: \"" + _filePath + "\"");
	
	size_t lineCt = 0;
	size_t validMessages = 0;

	// Parse lines into log messages
	for (const std::string& line : _lines) {
		if (!m_isRunning) {
			break; // Stop processing if the import was cancelled
		}

		lineCt++;

		if (line.empty()) {
			continue;
		}
		if (line.front() != '[') {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Invalid starting character");
		}

		std::string timeString;
		std::string serviceString;
		std::string typeString;
		std::string functionString;
		std::string messageString;

		// Timestamp
		size_t fromIx = line.find(']', 1);
		if (fromIx == std::string::npos) {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Missing closing bracket for timestamp");
			continue;
		}
		timeString = line.substr(1, fromIx - 1);

		// Service name
		fromIx = line.find('[', fromIx + 1);
		if (fromIx == std::string::npos) {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Missing opening bracket for service name");
			continue;
		}
		size_t toIx = line.find(']', fromIx + 1);
		if (toIx == std::string::npos) {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Missing closing bracket for service name");
			continue;
		}
		serviceString = line.substr(fromIx + 1, toIx - fromIx - 1);

		// Type
		fromIx = line.find('[', toIx + 1);
		if (fromIx == std::string::npos) {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Missing opening bracket for type");
			continue;
		}
		toIx = line.find(']', fromIx + 1);
		if (toIx == std::string::npos) {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Missing closing bracket for type");
			continue;
		}
		typeString = line.substr(fromIx + 1, toIx - fromIx - 1);

		// Function name
		fromIx = line.find('[', toIx + 1);
		if (fromIx == std::string::npos) {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Missing opening bracket for function name");
			continue;
		}
		toIx = line.find(']', fromIx + 1);
		if (toIx == std::string::npos) {
			this->appendOutputMessage("Invalid syntax at line " + QString::number(lineCt) + ": Missing closing bracket for function name");
			continue;
		}
		functionString = line.substr(fromIx + 1, toIx - fromIx - 1);

		// Message content
		messageString = line.substr(toIx + 2);

		// Convert timestamp to milliseconds
		int64_t timestampMsec = 0;
		try {
			timestampMsec = ot::DateTime::timestampToMsec(timeString, ot::DateTime::SimpleUTC);
		}
		catch (const std::exception& e) {
			this->appendOutputMessage("Invalid timestamp format at line " + QString::number(lineCt) + ": " + e.what());
			continue;
		}

		ot::LogFlag typeFlag = ot::LogFlag::INFORMATION_LOG;
		if (typeString == ot::LogMessage::logTypeInformation()) {
			typeFlag = ot::LogFlag::INFORMATION_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeDetailed()) {
			typeFlag = ot::LogFlag::DETAILED_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeWarning()) {
			typeFlag = ot::LogFlag::WARNING_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeError()) {
			typeFlag = ot::LogFlag::ERROR_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeTest()) {
			typeFlag = ot::LogFlag::TEST_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeMTLS()) {
			typeFlag = ot::LogFlag::INBOUND_MESSAGE_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeTLS()) {
			typeFlag = ot::LogFlag::ONEWAY_TLS_INBOUND_MESSAGE_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeQueued()) {
			typeFlag = ot::LogFlag::QUEUED_INBOUND_MESSAGE_LOG;
		}
		else if (typeString == ot::LogMessage::logTypeOutgoing()) {
			typeFlag = ot::LogFlag::OUTGOING_MESSAGE_LOG;
		}
		else {
			this->appendOutputMessage("Unknown log type at line " + QString::number(lineCt) + ": " + QString::fromStdString(typeString));
			continue;
		}

		ot::LogMessage msg;
		msg.setGlobalSystemTime(timestampMsec);
		msg.setLocalSystemTime(timestampMsec);

		msg.setServiceName(serviceString);
		msg.setFlags(typeFlag);
		msg.setFunctionName(functionString);
		msg.setText(messageString);

		_logMessages.push_back(std::move(msg));
		validMessages++;
	}

	if (validMessages > 0) {
		this->appendOutputMessage("Parsed " + QString::number(validMessages) + " messages from file: \"" + _filePath + "\"");
		this->appendFilePathInfo(_filePath);
	}
	else {
		this->appendOutputMessage("No valid messages found in file: \"" + _filePath + "\"");
	}
}

void FileLogImporterDialog::importJsonFile(const QString& _filePath, const std::list<std::string>& _lines, std::list<ot::LogMessage>& _logMessages) {
	this->appendOutputMessage("Parsing exported log file: \"" + _filePath + "\"");

	bool hasValidLines = false;
	std::list<ot::LogMessage> messages;
	for (const std::string& line : _lines) {
		if (!line.empty()) {
			if (!ot::importLogMessagesFromString(line, messages)) {
				this->appendOutputMessage("Failed to parse line: " + QString::fromStdString(line));
			}
			else {
				hasValidLines = true;
			}
		}
	}

	if (hasValidLines) {
		this->appendOutputMessage("Parsed " + QString::number(messages.size()) + " messages from file: \"" + _filePath + "\"");
		this->appendFilePathInfo(_filePath);
		_logMessages.splice(_logMessages.end(), std::move(messages));
	}
	else {
		this->appendOutputMessage("No valid messages found in file: \"" + _filePath + "\"");
	}
}

void FileLogImporterDialog::importLogBufferFile(const QString& _filePath, const std::list<std::string>& _lines, std::list<ot::LogMessage>& _logMessages) {
	this->appendOutputMessage("Parsing exported log buffer file: \"" + _filePath + "\"");

	size_t validLines = 0;
	size_t invalidLines = 0;
	size_t ct = 0;
	for (const std::string& line : _lines) {
		std::string trimmed = ot::String::removePrefixSuffix(line, "\n\t ");
		if (trimmed.empty()) {
			continue;
		}
		ct++;
		if (trimmed.find('{') != 0 || trimmed.rfind('}') != trimmed.size() - 1) {
			this->appendOutputMessage("Invalid log buffer line syntax in line " + QString::number(ct) + ": Missing brackets");
			invalidLines++;
			continue;
		}

		ot::JsonDocument lineDoc;
		if (!lineDoc.fromJson(trimmed)) {
			this->appendOutputMessage("Invalid log buffer line syntax in line " + QString::number(ct) + ": Failed to parse JSON document");
			invalidLines++;
			continue;
		}

		ot::LogMessage msg;
		msg.setFromJsonObject(lineDoc.getConstObject());
		validLines++;
		_logMessages.push_back(std::move(msg));
	}

	QString resultInfo("Parsed " + QString::number(validLines) + " messages from file: \"" + _filePath + "\".");
	if (invalidLines > 0) {
		resultInfo.append(QString::number(invalidLines) + " invalid lines detected.");
	}
	this->appendOutputMessage(resultInfo);
}
