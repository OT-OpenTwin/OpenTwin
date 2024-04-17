//! @file AppBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "FAR.h"
#include "AppBase.h"
#include "Terminal.h"
#include "TabManager.h"
#include "Randomizer.h"
#include "ToolManager.h"
#include "DockManager.h"
#include "MenuManager.h"
#include "ImageEditor.h"
#include "StatusManager.h"
#include "ColorStyleEditor.h"
#include "LogVisualization.h"
#include "ToolBarManager.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCommunication/actionTypes.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtCore/qdatetime.h>
#include <QtCore/qsettings.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qtabwidget.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qmessagebox.h>

#define APPBASE_LOG(___msg) OTOOLKIT_LOG("OToolkit", ___msg)
#define APPBASE_LOGW(___msg) OTOOLKIT_LOGW("OToolkit", ___msg)
#define APPBASE_LOGE(___msg) OTOOLKIT_LOGE("OToolkit", ___msg)

enum InternLogType {
	InternInfo,
	InternWarning,
	InternError
};

// ###########################################################################################################################################################################################################################################################################################################################

// Static functions

AppBase * AppBase::instance(QApplication* _app) {
	static AppBase* g_instance{ nullptr };
	if (g_instance == nullptr) g_instance = new AppBase();
	if (_app) g_instance->setApplicationInstance(_app);
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

void AppBase::log(const ot::LogMessage& _message) {
	otoolkit::APIInterface::InterfaceLogType typ = otoolkit::APIInterface::Information;
	switch (_message.flags().data())
	{
	case ot::WARNING_LOG:
		typ = otoolkit::APIInterface::Warning;
		break;
	case ot::ERROR_LOG:
		typ = otoolkit::APIInterface::Error;
		break;
	default:
		break;
	}
	this->log(QString::fromStdString(_message.functionName()), typ, QString::fromStdString(_message.text()));
}

void AppBase::log(const QString& _sender, otoolkit::APIInterface::InterfaceLogType _type, const QString& _message) {
	if (QThread::currentThreadId() == m_mainThread) {
		// Same thread -> direct call
		
		switch (_type)
		{
		case otoolkit::APIInterface::Information:
			this->slotLogMessage(_sender, _message);
			break;
		case otoolkit::APIInterface::Warning:
			this->slotLogWarning(_sender, _message);
			break;
		case otoolkit::APIInterface::Error:
			this->slotLogError(_sender, _message);
			break;
		default:
			OTAssert(0, "Unknown log type");
			this->slotLogError("OToolkit", "Unknown log type for message { \"Sender\": \"" + _sender + "\", \"Message\": \"" + _message + "\" }");
			break;
		}
	}
	else {
		// Different thread -> queue
		switch (_type)
		{
		case otoolkit::APIInterface::Information:
			QMetaObject::invokeMethod(this, "slotLogMessage", Qt::QueuedConnection, Q_ARG(QString, _sender), Q_ARG(QString, _message));
			break;
		case otoolkit::APIInterface::Warning:
			QMetaObject::invokeMethod(this, "slotLogWarning", Qt::QueuedConnection, Q_ARG(QString, _sender), Q_ARG(QString, _message));
			break;
		case otoolkit::APIInterface::Error:
			QMetaObject::invokeMethod(this, "slotLogError", Qt::QueuedConnection, Q_ARG(QString, _sender), Q_ARG(QString, _message));
			break;
		default:
			OTAssert(0, "Unknown log type");
			QMetaObject::invokeMethod(this, "slotLogError", Qt::QueuedConnection, Q_ARG(QString, _sender), Q_ARG(QString, QString("Unknown log type for message { \"Sender\": \"" + _sender + "\", \"Message\": \"" + _message + "\" }")));
			break;
		}
	}
}

bool AppBase::addTool(otoolkit::Tool* _tool) {
	return m_toolManager->addTool(_tool);
}

void AppBase::updateStatusString(const QString& _statusText) {
	if (QThread::currentThreadId() == m_mainThread) {
		this->slotSetStatus(_statusText);
	}
	else {
		QMetaObject::invokeMethod(this, "slotSetStatus", Qt::QueuedConnection, Q_ARG(QString, _statusText));
	}
}

void AppBase::updateStatusStringAsError(const QString& _statusText) {
	if (QThread::currentThreadId() == m_mainThread) {
		this->slotSetErrorStatus(_statusText);
	}
	else {
		QMetaObject::invokeMethod(this, "slotSetErrorStatus", Qt::QueuedConnection, Q_ARG(QString, _statusText));
	}
}

void AppBase::registerToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) {

}

void AppBase::removeToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) {

}

otoolkit::SettingsRef AppBase::createSettingsInstance(void) {
	return std::shared_ptr<QSettings>(new QSettings("OpenTwin", "OToolkit"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Qt base functions

void AppBase::closeEvent(QCloseEvent * _event) {
	// Save window state
	otoolkit::SettingsRef settings = this->createSettingsInstance();
	settings->setValue("IsMaximized", isMaximized());
	settings->setValue("SizeX", size().width());
	settings->setValue("SizeY", size().height());
	settings->setValue("PosX", pos().x());
	settings->setValue("PosY", pos().y());
	settings->setValue("WindowState", saveState());

	// Clear tools
	m_toolManager->stopAll();
	m_toolManager->clear();

	// Qt
	QMainWindow::closeEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void AppBase::setUrl(const QString& _url) {
	m_url = _url;
	APPBASE_LOG("OToolkit url set to: " + m_url);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void AppBase::slotProcessMessage(const QString& _json) {
	try {
		std::string msgStd = _json.toStdString();
		ot::JsonDocument inboundAction;
		inboundAction.fromJson(msgStd);
		if (inboundAction.IsObject()) {

			std::string action = ot::json::getString(inboundAction, OT_ACTION_MEMBER);

			if (action == OT_ACTION_CMD_Log) {
				ot::ConstJsonObject logObj = ot::json::getObject(inboundAction, OT_ACTION_PARAM_LOG);

				ot::LogMessage msg;
				msg.setFromJsonObject(logObj);
				msg.setCurrentTimeAsGlobalSystemTime();

				if (m_logger) m_logger->appendLogMessage(msg);
			}
		}
		else {
			this->updateStatusStringAsError("The received message is not a JSON object");
		}
	}
	catch (const std::exception& _e) {
		this->updateStatusStringAsError(_e.what());
	}
	catch (...) {
		this->updateStatusStringAsError("Unknown error occured while processing message");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void AppBase::slotLogMessage(const QString& _sender, const QString& _message) {
	m_logMutex.lock();
	
	QTextCursor cursor = m_output->textCursor();
	cursor.movePosition(QTextCursor::End);

	QTextCharFormat format = cursor.charFormat();
	QTextCharFormat formatTime = format;
	QTextCharFormat formatSender = format;
	formatTime.setForeground(QBrush(QColor(192, 128, 255)));
	formatSender.setForeground(QBrush(QColor(128, 192, 255)));

	cursor.insertText("[");
	cursor.setCharFormat(formatTime);
	cursor.insertText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
	cursor.setCharFormat(format);
	cursor.insertText("] ");

	if (!_sender.isEmpty()) {
		cursor.insertText("[");
		cursor.setCharFormat(formatSender);
		cursor.insertText(_sender);
		cursor.setCharFormat(format);
		cursor.insertText("] ");
	}

	cursor.insertText(_message);
	m_output->setTextCursor(cursor);
	m_output->append("");
	m_logMutex.unlock();
}

void AppBase::slotLogWarning(const QString& _sender, const QString& _message) {
	m_logMutex.lock();
	
	QTextCursor cursor = m_output->textCursor();
	cursor.movePosition(QTextCursor::End);

	QTextCharFormat format = cursor.charFormat();
	QTextCharFormat formatTime = format;
	QTextCharFormat formatSender = format;
	QTextCharFormat formatWarn = format;
	formatTime.setForeground(QBrush(QColor(192, 128, 255)));
	formatSender.setForeground(QBrush(QColor(128, 192, 255)));
	formatWarn.setForeground(QBrush(QColor(255, 242, 0)));

	cursor.insertText("[");
	cursor.setCharFormat(formatTime);
	cursor.insertText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
	cursor.setCharFormat(format);
	cursor.insertText("] ");

	if (!_sender.isEmpty()) {
		cursor.insertText("[");
		cursor.setCharFormat(formatSender);
		cursor.insertText(_sender);
		cursor.setCharFormat(format);
		cursor.insertText("] ");
	}

	cursor.insertText("[");
	cursor.setCharFormat(formatWarn);
	cursor.insertText("Warning");
	cursor.setCharFormat(format);
	cursor.insertText("] ");

	cursor.insertText(_message);
	m_output->setTextCursor(cursor);
	m_output->append("");
	m_logMutex.unlock();
}

void AppBase::slotLogError(const QString& _sender, const QString& _message) {
	m_logMutex.lock();
	
	QTextCursor cursor = m_output->textCursor();
	cursor.movePosition(QTextCursor::End);

	QTextCharFormat format = cursor.charFormat();
	QTextCharFormat formatTime = format;
	QTextCharFormat formatSender = format;
	QTextCharFormat formatError = format;
	formatTime.setForeground(QBrush(QColor(192, 128, 255)));
	formatSender.setForeground(QBrush(QColor(128, 192, 255)));
	formatError.setForeground(QBrush(QColor(255, 0, 0)));

	cursor.insertText("[");
	cursor.setCharFormat(formatTime);
	cursor.insertText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
	cursor.setCharFormat(format);
	cursor.insertText("] ");

	if (!_sender.isEmpty()) {
		cursor.insertText("[");
		cursor.setCharFormat(formatSender);
		cursor.insertText(_sender);
		cursor.setCharFormat(format);
		cursor.insertText("] ");
	}

	cursor.insertText("[");
	cursor.setCharFormat(formatError);
	cursor.insertText("Error");
	cursor.setCharFormat(format);
	cursor.insertText("] ");

	cursor.insertText(_message);
	m_output->setTextCursor(cursor);
	m_output->append("");
	m_logMutex.unlock();
}

void AppBase::slotSetStatus(const QString& _text) {
	m_toolManager->statusManager()->setInfo(_text);
}

void AppBase::slotSetErrorStatus(const QString& _text) {
	m_toolManager->statusManager()->setErrorInfo(_text);
}

void AppBase::slotInitialize(void) {
	QMetaObject::invokeMethod(this, &AppBase::slotInitializeTools, Qt::QueuedConnection);
}

void AppBase::slotInitializeTools(void) {
	
	// Create tools
	m_logger = new LogVisualization;
	
	m_toolManager->addTool(m_logger);
	m_toolManager->addTool(new Terminal);
	m_toolManager->addTool(new FAR);
	m_toolManager->addTool(new Randomizer);
	m_toolManager->addTool(new ColorStyleEditor);
	m_toolManager->addTool(new ImageEditor);

	//m_tabWidget->addTab(m_logger->widget(), "Log Visualization");
	//m_tabWidget->addTab(m_terminal->widget(), "OTerminal");
	//m_tabWidget->addTab(m_textFinder->widget(), "Text Finder");

	// Setup menu
	//m_logger->createMenuBarEntries(m_menuBar);
	//m_terminal->createMenuBarEntries(m_menuBar);

	APPBASE_LOG("Welcome to OToolkit (Build: " __DATE__ " " __TIME__ ")");
}

void AppBase::slotRecenter(void) {
	this->move(0, 0);
	this->resize(800, 600);
}

void AppBase::slotColorStyleChanged(const ot::ColorStyle& _style) {
	//this->hide();
	//QMetaObject::invokeMethod(this, &AppBase::show, Qt::QueuedConnection);
}

AppBase::AppBase(QApplication* _app) : m_mainThread(QThread::currentThreadId()), m_app(_app), m_logger(nullptr) {
	this->deleteLogNotifierLater(true);

	// Initialize Toolkit API
	otoolkit::api::initialize(this);

	// Create tool manager
	m_toolManager = new ToolManager(this);
	this->setMenuBar(m_toolManager->menuManager());
	this->setStatusBar(m_toolManager->statusManager());
	
	// Create output
	m_output = new QTextEdit;
	QDockWidget* outputDock = new QDockWidget("Output");
	
	outputDock->setObjectName("OToolkit_Dock_Output");
	m_output->setObjectName("OToolkit_Output");
	m_output->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	QFont f = m_output->font();
	f.setFamily("Consolas");
	m_output->setFont(f);
	m_output->setReadOnly(true);

	this->setCentralWidget(m_toolManager->tabManager());
	this->setWindowTitle("OToolkit");
	this->setWindowIcon(QIcon(":/images/OToolkit.png"));

	outputDock->setWidget(m_output);
	m_toolManager->dockManager()->add("", outputDock, Qt::BottomDockWidgetArea);

	// Setup global shortcuts
	m_recenterShortcut = new QShortcut(QKeySequence("F11"), this, nullptr, nullptr, Qt::WindowShortcut);

	// Restore settings
	this->setObjectName("OToolkit_MainWindow");

	otoolkit::SettingsRef settings = this->createSettingsInstance();
	bool isMax = settings->value("IsMaximized", false).toBool();
	int sizeX = settings->value("SizeX", 800).toInt();
	int sizeY = settings->value("SizeY", 600).toInt();
	int posX = settings->value("PosX", 0).toInt();
	int posY = settings->value("PosY", 0).toInt();

	this->move(posX, posY);

	if (isMax) {
		this->showMaximized();
	}
	else {
		this->resize(sizeX, sizeY);
		this->showNormal();
	}

	this->restoreState(settings->value("WindowState", "").toByteArray());

	//connect(menuManager, &MenuManager::settingsRequested, this, &AppBase::slotSettings);
	this->connect(m_toolManager->menuManager(), &MenuManager::exitRequested , this, &AppBase::close);
	this->connect(m_recenterShortcut, &QShortcut::activated, this, &AppBase::slotRecenter);
	this->connect(&ot::GlobalColorStyle::instance(), &ot::GlobalColorStyle::currentStyleChanged, this, &AppBase::slotColorStyleChanged);

	QMetaObject::invokeMethod(this, &AppBase::slotInitialize, Qt::QueuedConnection);
}
