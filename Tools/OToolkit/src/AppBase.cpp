//! @file AppBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "FAR.h"
#include "AppBase.h"
#include "Logging.h"
#include "Terminal.h"
#include "Randomizer.h"
#include "BackendInfo.h"
#include "ToolManager.h"
#include "MenuManager.h"
#include "ImageEditor.h"
#include "StatusManager.h"
#include "SVGBrowser.h"
#include "ToolViewManager.h"
#include "SettingsManager.h"
#include "ColorStyleEditor.h"
#include "ExternalLibraryManager.h"
#include "ToolBarManager.h"
#include "WidgetTest.h"
#include "GraphicsItemDesigner.h"
#include "NetworkTools.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/BasicScopedBoolWrapper.h"
#include "OTCore/String.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/CheckerboardPainter2D.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PlainTextEdit.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/PlainTextEditView.h"

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

#define BUILD_INFO "Open Twin - Build " + QString(__DATE__) + " - " + QString(__TIME__) + "\n\n"

#undef FAR

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
	switch (_message.getFlags().data())
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
	this->log(QString::fromStdString(_message.getFunctionName()), typ, QString::fromStdString(_message.getText()));
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
			QMetaObject::invokeMethod(this, &AppBase::slotLogMessage, Qt::QueuedConnection, _sender, _message);
			break;
		case otoolkit::APIInterface::Warning:
			QMetaObject::invokeMethod(this, &AppBase::slotLogWarning, Qt::QueuedConnection, _sender, _message);
			break;
		case otoolkit::APIInterface::Error:
			QMetaObject::invokeMethod(this, &AppBase::slotLogError, Qt::QueuedConnection, _sender, _message);
			break;
		default:
			OTAssert(0, "Unknown log type");
			QMetaObject::invokeMethod(this, &AppBase::slotLogError, Qt::QueuedConnection, _sender, QString("Unknown log type for message { \"Sender\": \"" + _sender + "\", \"Message\": \"" + _message + "\" }"));
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
		QMetaObject::invokeMethod(this, &AppBase::slotSetStatus, Qt::QueuedConnection, _statusText);
	}
}

void AppBase::updateStatusStringAsError(const QString& _statusText) {
	if (QThread::currentThreadId() == m_mainThread) {
		this->slotSetErrorStatus(_statusText);
	}
	else {
		QMetaObject::invokeMethod(this, &AppBase::slotSetErrorStatus, Qt::QueuedConnection, _statusText);
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
	settings->setValue("ViewState", QByteArray::fromStdString(ot::WidgetViewManager::instance().saveState()));

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
	OT_LOG_D("OToolkit url set to: " + m_url.toStdString());
}

void AppBase::parseStartArgs(const std::string& _args) {
	std::list<std::string> tmp = ot::String::split(_args, ' ', true);
	for (const std::string& arg : tmp) {
		if (arg == "-logexport") {
			m_startArgs.push_back(StartOption::LogExport);
		}
		else if (arg == "-noauto") {
			m_startArgs.push_back(StartOption::NoAutoStart);
		}
		else {
			OT_LOG_W("Unknown start argument \"" + arg + "\"");
		}
	}
}

void AppBase::setUpdateTransparentColorStyleValueEnabled(bool _enabled) {
	m_replaceTransparentColorStyleValue = _enabled;
	this->updateTransparentColorStyleValue();
}

void AppBase::updateTransparentColorStyleValue(void) {
	ot::ColorStyle newStyle = ot::GlobalColorStyle::instance().getCurrentStyle();
	ot::ColorStyleValue newValue;
	newValue.setEntryKey(ot::ColorStyleValueEntry::Transparent);

	if (m_replaceTransparentColorStyleValue) {
		newValue.setPainter(
			new ot::CheckerboardPainter2D(
				new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::WidgetAlternateBackground),
				new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::WidgetBackground)
			)
		);
	}
	else {
		newValue.setColor(ot::Transparent);
	}
	newStyle.addValue(newValue, true);
	ot::GlobalColorStyle::instance().blockSignals(true);
	ot::GlobalColorStyle::instance().addStyle(newStyle, true);
	ot::GlobalColorStyle::instance().blockSignals(false);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void AppBase::slotProcessMessage(const QString& _json) {
	ot::ActionDispatcher::instance().dispatch(_json.toStdString(), ot::EXECUTE);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void AppBase::slotLogMessage(const QString& _sender, const QString& _message) {
	if (!m_output) {
		return;
	}

	m_logMutex.lock();
	
	QTextCursor cursor = m_output->getPlainTextEdit()->textCursor();
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
	m_output->getPlainTextEdit()->setTextCursor(cursor);
	m_output->getPlainTextEdit()->appendPlainText("");
	m_logMutex.unlock();
}

void AppBase::slotLogWarning(const QString& _sender, const QString& _message) {
	m_logMutex.lock();
	
	QTextCursor cursor = m_output->getPlainTextEdit()->textCursor();
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
	m_output->getPlainTextEdit()->setTextCursor(cursor);
	m_output->getPlainTextEdit()->appendPlainText("");
	m_logMutex.unlock();
}

void AppBase::slotLogError(const QString& _sender, const QString& _message) {
	m_logMutex.lock();
	
	QTextCursor cursor = m_output->getPlainTextEdit()->textCursor();
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
	m_output->getPlainTextEdit()->setTextCursor(cursor);
	m_output->getPlainTextEdit()->appendPlainText("");
	m_logMutex.unlock();
}

void AppBase::slotSetStatus(const QString& _text) {
	m_toolManager->getStatusManager()->setInfo(_text);
}

void AppBase::slotSetErrorStatus(const QString& _text) {
	m_toolManager->getStatusManager()->setErrorInfo(_text);
}

void AppBase::slotInitialize(void) {
	QMetaObject::invokeMethod(this, &AppBase::slotInitializeTools, Qt::QueuedConnection);
}

void AppBase::slotInitializeTools(void) {
	// Create tools
	ot::BasicScopedBoolWrapper autoStarting(m_ignoreToolAutoStart, m_ignoreToolAutoStart);

	for (StartOption opt : m_startArgs) {
		if (opt == AppBase::NoAutoStart) {
			m_ignoreToolAutoStart = true;
			break;
		}
	}

	m_logger = new Logging;
	
	m_toolManager->addTool(m_logger);
	m_toolManager->addTool(new Terminal);
	m_toolManager->addTool(new FAR);
	m_toolManager->addTool(new BackendInfo);
	m_toolManager->addTool(new Randomizer);
	m_toolManager->addTool(new ColorStyleEditor);
	m_toolManager->addTool(new ImageEditor);
	m_toolManager->addTool(new GraphicsItemDesigner);
	m_toolManager->addTool(new NetworkTools);
	m_toolManager->addTool(new SVGBrowser);

	QByteArray arr = qgetenv("OPENTWIN_DEV_ROOT");
	if (!arr.isEmpty()) {
		m_toolManager->addTool(new WidgetTest);
	}

	m_externalLibraryManager->importTools(m_toolManager);

	OT_LOG_I("Welcome to OToolkit (Build: " __DATE__ " " __TIME__ ")");

	QMetaObject::invokeMethod(this, &AppBase::slotFinalizeInit, Qt::QueuedConnection);
}

void AppBase::slotRecenter(void) {
	this->move(0, 0);
	this->resize(800, 600);
}

void AppBase::slotFinalizeInit(void) {
	ot::WidgetViewManager::instance().restoreState(this->createSettingsInstance()->value("ViewState", QByteArray()).toByteArray().toStdString());
	
	// Check current view to correctly display toolbar, statusbar and so on
	ot::WidgetView* currentView = ot::WidgetViewManager::instance().getCurrentlyFocusedView();
	if (currentView) {
		m_toolManager->getToolViewManager()->slotViewFocusChanged(currentView, nullptr);
	}
	
	this->setEnabled(true);

	for (StartOption opt : m_startArgs) {
		switch (opt) {
		case AppBase::LogExport:
		{
			OTAssertNullptr(m_logger);
			if (!m_logger->getToolIsRunning()) {
				m_toolManager->runTool(m_logger->getToolName());
			}

			if (m_logger->getToolIsRunning()) {
				QMetaObject::invokeMethod(m_logger, &Logging::runQuickExport, Qt::QueuedConnection);
			}
			else {
				OT_LOG_E("Failed to start logger");
			}
		}
			// Auto start logger
			break;
		default:
			break;
		}
	}
}

void AppBase::slotColorStyleChanged(void) {
	this->updateTransparentColorStyleValue();
}

void AppBase::handleDisplayData(ot::JsonDocument& _doc) {
	std::string data = ot::json::getString(_doc, OT_ACTION_PARAM_Data);

	this->log("DisplayData Request", otoolkit::APIInterface::Information, QString::fromStdString(data));
}

AppBase::AppBase(QApplication* _app)
	: m_mainThread(QThread::currentThreadId()), m_app(_app), m_logger(nullptr), m_replaceTransparentColorStyleValue(true),
	m_ignoreToolAutoStart(false)
{
	this->setCustomDeleteLogNotifier(true);
	ot::LogDispatcher::instance().addReceiver(this);

	// Initialize dock manager
	ot::WidgetViewManager::instance().initialize();

	// Create output
	m_output = new ot::PlainTextEditView;

	// Initialize Toolkit API
	otoolkit::api::initialize(this);

	// Create tool manager
	m_toolManager = new ToolManager(this);
	this->setMenuBar(m_toolManager->getMenuManager());
	this->setStatusBar(m_toolManager->getStatusManager());
	
	// Create external tool manager
	m_externalLibraryManager = new ExternalLibraryManager;

	ot::PlainTextEditView* defaultView = new ot::PlainTextEditView;
	defaultView->setViewData(ot::WidgetViewBase("Debug", "OpenTwin", ot::WidgetViewBase::Default, ot::WidgetViewBase::ViewText, ot::WidgetViewBase::ViewIsCentral));
	defaultView->setViewIsPermanent(true);
	defaultView->getPlainTextEdit()->setPlainText(BUILD_INFO);
	defaultView->getViewDockWidget()->setFeature(ads::CDockWidget::NoTab, true);
	m_toolManager->getToolViewManager()->addIgnoredView(defaultView);

	// Create output
	m_output->setViewData(ot::WidgetViewBase("Output", "Output", ot::WidgetViewBase::Bottom, ot::WidgetViewBase::ViewText, ot::WidgetViewBase::ViewFlag::ViewIsSide | ot::WidgetViewBase::ViewFlag::ViewIsCloseable | ot::WidgetViewBase::ViewFlag::ViewDefaultCloseHandling));
	m_output->getPlainTextEdit()->setObjectName("OToolkit_Output");
	m_output->getPlainTextEdit()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_toolManager->getToolViewManager()->addIgnoredView(m_output);

	QFont f = m_output->getPlainTextEdit()->font();
	f.setFamily("Consolas");
	m_output->getPlainTextEdit()->setFont(f);
	m_output->getPlainTextEdit()->setReadOnly(true);
	
	this->setCentralWidget(ot::WidgetViewManager::instance().getDockManager());
	this->setWindowTitle("OToolkit");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());

	ot::WidgetViewManager::instance().addView(ot::BasicServiceInformation(), defaultView);
	ot::WidgetViewManager::instance().addView(ot::BasicServiceInformation(), m_output);

	ot::WidgetViewManager::instance().setConfigFlags(
		ot::WidgetViewManager::InputFocusCentralViewOnFocusChange |
		ot::WidgetViewManager::UseBestAreaFinderOnViewInsert
	);

	// Setup global shortcuts
	m_recenterShortcut = new QShortcut(QKeySequence("F11"), this, nullptr, nullptr, Qt::WindowShortcut);

	// Restore settings
	this->setObjectName("OToolkit_MainWindow");
	this->setEnabled(false);

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
	this->connect(m_toolManager->getMenuManager(), &MenuManager::exitRequested , this, &AppBase::close);
	this->connect(m_recenterShortcut, &QShortcut::activated, this, &AppBase::slotRecenter);
	this->connect(&ot::GlobalColorStyle::instance(), &ot::GlobalColorStyle::currentStyleChanged, this, &AppBase::slotColorStyleChanged);

	ot::ActionDispatcher::instance().setDefaultMessageTypes(ot::ALL_MESSAGE_TYPES);
	connectAction("DisplayData", this, &AppBase::handleDisplayData);

	QMetaObject::invokeMethod(this, &AppBase::slotInitialize, Qt::QueuedConnection);

	OT_LOG_I("Initializing");
}
