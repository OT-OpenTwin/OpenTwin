//! @file AppBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "ToolManager.h"
#include "StatusBar.h"
#include "LogVisualization.h"
#include "Terminal.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCommunication/actionTypes.h"

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

AppBase * AppBase::instance(void) {
	static AppBase* g_instance{ nullptr };
	if (g_instance == nullptr) g_instance = new AppBase;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

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
			otAssert(0, "Unknown log type");
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
			otAssert(0, "Unknown log type");
			QMetaObject::invokeMethod(this, "slotLogError", Qt::QueuedConnection, Q_ARG(QString, _sender), Q_ARG(QString, QString("Unknown log type for message { \"Sender\": \"" + _sender + "\", \"Message\": \"" + _message + "\" }")));
			break;
		}
	}
}

bool AppBase::addTool(otoolkit::Tool* _tool) {
	return false;
}

void AppBase::updateStatusString(const QString& _statusText) {

}

void AppBase::updateStatusStringAsError(const QString& _statusText) {

}

void AppBase::registerToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) {

}

void AppBase::removeToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Qt base functions

void AppBase::closeEvent(QCloseEvent * _event) {
	// Save window state
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	s.setValue("IsMaximized", isMaximized());
	s.setValue("SizeX", size().width());
	s.setValue("SizeY", size().height());
	s.setValue("PosX", pos().x());
	s.setValue("PosY", pos().y());
	s.setValue("WindowState", saveState());

	// Clear tools
	ToolManager::instance().clear();

	// Clear tabs
	m_tabWidget->clear();

	// Qt
	QMainWindow::closeEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void AppBase::slotLogMessage(const QString& _sender, const QString& _message) {
	QTextCursor cursor = m_output->textCursor();
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
}

void AppBase::slotLogWarning(const QString& _sender, const QString& _message) {
	QTextCursor cursor = m_output->textCursor();
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
}

void AppBase::slotLogError(const QString& _sender, const QString& _message) {
	QTextCursor cursor = m_output->textCursor();
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
}

void AppBase::slotInitialize(void) {
	OTOOLKIT_LOG("OToolkit", "Starting initialize...");
	/*
	// Create tools
	m_logger = new LogVisualization;
	m_terminal = new Terminal;

	m_tabWidget->addTab(m_logger->widget(), "Log Visualization");
	m_tabWidget->addTab(m_terminal->widget(), "OTerminal");
	m_tabWidget->addTab(m_textFinder->widget(), "Text Finder");

	// Setup menu
	m_logger->createMenuBarEntries(m_menuBar);
	m_terminal->createMenuBarEntries(m_menuBar);

	// Setup information
	m_statusBar->setCurrentTool(m_logger);
	*/
}

void AppBase::slotRecenter(void) {
	this->move(0, 0);
	this->resize(800, 600);
}

AppBase::AppBase() : m_mainThread(QThread::currentThreadId()), m_app(nullptr) {
	setObjectName("OToolkit_MainWindow");

	// Create controls
	m_tabWidget = new QTabWidget;
	m_tabWidget->setObjectName("OToolkit_MainTabWidget");

	m_outputDock = new QDockWidget("Output");
	m_output = new QTextEdit;

	m_outputDock->setObjectName("OToolkit_Dock_Output");
	m_output->setObjectName("OToolkit_Output");

	QFont f = m_output->font();
	f.setFamily("Consolas");
	m_output->setFont(f);
	m_output->setReadOnly(true);

	// Setup window
	this->setCentralWidget(m_tabWidget);
	this->setWindowTitle(APP_BASE_APP_NAME);
	this->setWindowIcon(QIcon(":/images/OToolkit.png"));

	m_outputDock->setWidget(m_output);
	this->addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);

	// Setup status
	m_statusBar = new StatusBar;
	this->setStatusBar(m_statusBar);

	// Setup menu
	m_menuBar = new QMenuBar;
	this->setMenuBar(m_menuBar);

	QMenu * fileMenu = m_menuBar->addMenu("File");
	m_settingsAction = fileMenu->addAction(QIcon(":/images/Settings.png"), "Settings");
	fileMenu->addSeparator();
	QAction * exitAction = fileMenu->addAction(QIcon(":/images/Exit.png"), "Exit");

	// Setup global shortcuts
	m_recenterShortcut = new QShortcut(QKeySequence("F11"), this, nullptr, nullptr, Qt::WindowShortcut);

	// Restore settings
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	bool isMax = s.value("IsMaximized", false).toBool();
	int sizeX = s.value("SizeX", 800).toInt();
	int sizeY = s.value("SizeY", 600).toInt();
	int posX = s.value("PosX", 0).toInt();
	int posY = s.value("PosY", 0).toInt();

	this->move(posX, posY);

	if (isMax) {
		this->showMaximized();
	}
	else {
		this->resize(sizeX, sizeY);
		this->showNormal();
	}

	this->restoreState(s.value("WindowState", "").toByteArray());

	//connect(m_settingsAction, &QAction::triggered, this, &AppBase::slotSettings);
	connect(exitAction, &QAction::triggered, this, &AppBase::close);
	connect(m_recenterShortcut, &QShortcut::activated, this, &AppBase::slotRecenter);

	APPBASE_LOG("Welcome to " APP_BASE_APP_NAME " (Build: " __DATE__ " " __TIME__ ")");

	QMetaObject::invokeMethod(this, &AppBase::slotInitialize, Qt::QueuedConnection);
}
