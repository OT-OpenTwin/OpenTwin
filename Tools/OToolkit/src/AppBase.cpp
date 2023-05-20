#include "AppBase.h"
#include "StatusBar.h"
#include "LogVisualization.h"
#include "Terminal.h"

// OT header
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCommunication/actionTypes.h"

// Qt header
#include <QtCore/qdatetime.h>
#include <QtCore/qsettings.h>
#include <QtWidgets/qtabwidget.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qtextedit.h>

enum InternLogType {
	InternInfo,
	InternWarning,
	InternError
};

static AppBase * g_instance{ nullptr };

AppBase * AppBase::instance(void) {
	if (g_instance == nullptr) g_instance = new AppBase;
	return g_instance;
}

void AppBase::closeEvent(QCloseEvent * _event) {
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	s.setValue("IsMaximized", isMaximized());
	s.setValue("SizeX", size().width());
	s.setValue("SizeY", size().height());
	s.setValue("PosX", pos().x());
	s.setValue("PosY", pos().y());
	s.setValue("WindowState", saveState());

	if (m_logger) {
		delete m_logger;
		m_logger = nullptr;
	}
	if (m_terminal) {
		delete m_terminal;
		m_terminal = nullptr;
	}
	m_tabWidget->clear();

	QMainWindow::closeEvent(_event);
}

void AppBase::log(const QString& _sender, const QString& _message) {
	if (QThread::currentThreadId() == m_mainThread) {
		slotLog(_sender, _message, InternInfo);
	}
	else {
		QMetaObject::invokeMethod(this, "slotLog", Qt::QueuedConnection, Q_ARG(const QString&, _sender), Q_ARG(const QString&, _message), Q_ARG(int, InternInfo));
	}
}

void AppBase::logWarning(const QString& _sender, const QString& _message) {
	if (QThread::currentThreadId() == m_mainThread) {
		slotLog(_sender, _message, InternWarning);
	}
	else {
		QMetaObject::invokeMethod(this, "slotLog", Qt::QueuedConnection, Q_ARG(const QString&, _sender), Q_ARG(const QString&, _message), Q_ARG(int, InternWarning));
	}
}

void AppBase::logError(const QString& _sender, const QString& _message) {
	if (QThread::currentThreadId() == m_mainThread) {
		slotLog(_sender, _message, InternError);
	}
	else {
		QMetaObject::invokeMethod(this, "slotLog", Qt::QueuedConnection, Q_ARG(const QString&, _sender), Q_ARG(const QString&, _message), Q_ARG(int, InternError));
	}
}

void AppBase::slotProcessMessage(const QString& _message) {
	try {
		std::string msgStd = _message.toStdString();
		OT_rJSON_parseDOC(inboundAction, msgStd.c_str());
		if (inboundAction.IsObject()) {

			std::string action = ot::rJSON::getString(inboundAction, OT_ACTION_MEMBER);

			if (action == OT_ACTION_CMD_Log) {
				OT_rJSON_checkMember(inboundAction, OT_ACTION_PARAM_LOG, Object);
				OT_rJSON_val logObj = inboundAction[OT_ACTION_PARAM_LOG].GetObject();

				ot::LogMessage msg;
				msg.setFromJsonObject(logObj);
				msg.setCurrentTimeAsGlobalSystemTime();
				
				m_logger->appendLogMessage(msg);
			}
		}
		else {
			m_statusBar->setErrorInfo("The received message is not a JSON object");
		}
	}
	catch (const std::exception& _e) {
		m_statusBar->setErrorInfo(_e.what());
	}
	catch (...) {
		m_statusBar->setErrorInfo("Unknown error occured while processing message");
	}
}

void AppBase::slotDockWidgetVisibilityChanged(bool _visible) {
	m_outputAction->setChecked(_visible);
}

void AppBase::slotToggleOutput(void) {
	m_outputDock->setVisible(!m_outputDock->isVisible());
}

void AppBase::slotSettings(void) {

}

void AppBase::slotClose(void) {
	close();
}

void AppBase::slotInitialize(void) {
	OTOOLKIT_LOG("OToolkit", "Starting initialize...");

	// Create tools
	m_logger = new LogVisualization;
	m_terminal = new Terminal;

	m_tabWidget->addTab(m_logger->widget(), "Log Visualization");
	m_tabWidget->addTab(m_terminal->widget(), "OTerminal");

	// Setup menu
	m_logger->createMenuBarEntries(m_menuBar);
	m_terminal->createMenuBarEntries(m_menuBar);

	// Setup information
	m_statusBar->setCurrentTool(m_logger);
}

void AppBase::slotLog(const QString& _sender, const QString& _message, int _type) {
	QTextCursor cursor = m_output->textCursor();
	QTextCharFormat format = cursor.charFormat();
	QTextCharFormat formatTime = format;
	QTextCharFormat formatSender = format;
	QTextCharFormat formatWarn = format;
	QTextCharFormat formatError = format;
	formatTime.setForeground(QBrush(QColor(192, 128, 255)));
	formatSender.setForeground(QBrush(QColor(128, 192, 255)));
	formatWarn.setForeground(QBrush(QColor(255, 242, 0)));
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

	if (_type == InternWarning) {
		cursor.insertText("[");
		cursor.setCharFormat(formatWarn);
		cursor.insertText("Warning");
		cursor.setCharFormat(format);
		cursor.insertText("] ");
	} else if (_type == InternError) {
		cursor.insertText("[");
		cursor.setCharFormat(formatError);
		cursor.insertText("Error");
		cursor.setCharFormat(format);
		cursor.insertText("] ");
	}

	cursor.insertText(_message);
	m_output->setTextCursor(cursor);
	m_output->append("");
}

AppBase::AppBase() : m_mainThread(QThread::currentThreadId()) {
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

	// Setup window
	setCentralWidget(m_tabWidget);
	setWindowTitle(APP_BASE_APP_NAME);
	setWindowIcon(QIcon(":/images/OToolkit.png"));

	m_outputDock->setWidget(m_output);
	addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);

	// Setup status
	m_statusBar = new StatusBar;
	setStatusBar(m_statusBar);

	// Setup menu
	m_menuBar = new QMenuBar;
	setMenuBar(m_menuBar);

	QMenu * fileMenu = m_menuBar->addMenu("File");
	m_outputAction = fileMenu->addAction(QIcon(":/images/Log.png"), "Output");
	fileMenu->addSeparator();
	m_settingsAction = fileMenu->addAction(QIcon(":/images/Settings.png"), "Settings");
	fileMenu->addSeparator();
	m_exitAction = fileMenu->addAction(QIcon(":/images/Exit.png"), "Exit");

	m_outputAction->setCheckable(true);
	m_outputAction->setChecked(true);

	// Restore settings
	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	bool isMax = s.value("IsMaximized", false).toBool();
	int sizeX = s.value("SizeX", 800).toInt();
	int sizeY = s.value("SizeY", 600).toInt();
	int posX = s.value("PosX", 0).toInt();
	int posY = s.value("PosY", 0).toInt();

	move(posX, posY);

	if (isMax) showMaximized();
	else {
		resize(sizeX, sizeY);
		showNormal();
	}

	restoreState(s.value("WindowState", "").toByteArray());
	m_outputAction->setChecked(m_outputDock->isVisible());

	connect(m_outputDock, &QDockWidget::visibilityChanged, this, &AppBase::slotDockWidgetVisibilityChanged);
	connect(m_outputAction, &QAction::triggered, this, &AppBase::slotToggleOutput);
	connect(m_settingsAction, &QAction::triggered, this, &AppBase::slotSettings);
	connect(m_exitAction, &QAction::triggered, this, &AppBase::slotClose);

	log("OToolkit", "Welcome to " APP_BASE_APP_NAME " (Build: " __DATE__ " " __TIME__ ")");

	QMetaObject::invokeMethod(this, &AppBase::slotInitialize, Qt::QueuedConnection);
}
