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
#include "ToolManager.h"
#include "MenuManager.h"
#include "ImageEditor.h"
#include "StatusManager.h"
#include "ToolViewManager.h"
#include "SettingsManager.h"
#include "ColorStyleEditor.h"
#include "ToolBarManager.h"
#include "WidgetTest.h"
#include "GraphicsItemDesigner.h"
#include "NetworkTools.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/CheckerboardPainter2D.h"
#include "OTCommunication/actionTypes.h"
#include "OTWidgets/IconManager.h"
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
	m_output->appendPlainText("");
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
	m_output->appendPlainText("");
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
	m_output->appendPlainText("");
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
	m_logger = new Logging;
	
	m_toolManager->addTool(m_logger);
	m_toolManager->addTool(new Terminal);
	m_toolManager->addTool(new FAR);
	m_toolManager->addTool(new Randomizer);
	m_toolManager->addTool(new ColorStyleEditor);
	m_toolManager->addTool(new ImageEditor);
	m_toolManager->addTool(new GraphicsItemDesigner);
	m_toolManager->addTool(new NetworkTools);

	QByteArray arr = qgetenv("OPENTWIN_DEV_ROOT");
	if (!arr.isEmpty()) {
		m_toolManager->addTool(new WidgetTest);
	}

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
		m_toolManager->getToolViewManager()->slotViewFocused(currentView);
	}
	
	this->setEnabled(true);
}

void AppBase::slotColorStyleChanged(const ot::ColorStyle& _style) {
	this->updateTransparentColorStyleValue();
	//this->hide();
	//QMetaObject::invokeMethod(this, &AppBase::show, Qt::QueuedConnection);
}

AppBase::AppBase(QApplication* _app) 
	: m_mainThread(QThread::currentThreadId()), m_app(_app), m_logger(nullptr), m_replaceTransparentColorStyleValue(true)
{
	// Create the output first so log messages can be created
	m_output = new ot::PlainTextEditView;

	this->setDeleteLogNotifierLater(true);
	ot::LogDispatcher::instance().addReceiver(this);
	
	// Initialize Toolkit API
	otoolkit::api::initialize(this);

	ads::CDockManager::setAutoHideConfigFlag(ads::CDockManager::AutoHideFeatureEnabled);
	ads::CDockManager::setAutoHideConfigFlag(ads::CDockManager::AutoHideButtonTogglesArea);
	ads::CDockManager::setAutoHideConfigFlag(ads::CDockManager::AutoHideShowOnMouseOver);
	ads::CDockManager::setAutoHideConfigFlag(ads::CDockManager::DockAreaHasAutoHideButton);
	ot::WidgetViewManager::instance().initialize();

	// Create tool manager
	m_toolManager = new ToolManager(this);
	this->setMenuBar(m_toolManager->getMenuManager());
	this->setStatusBar(m_toolManager->getStatusManager());
	
	// Create output
	m_output->setViewData(ot::WidgetViewBase("Output", "Output", ot::WidgetViewBase::Bottom, ot::WidgetViewBase::ViewFlag::ViewIsSide));
	m_output->setObjectName("OToolkit_Output");
	m_output->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_toolManager->getToolViewManager()->addIgnoredView(m_output);

	QFont f = m_output->font();
	f.setFamily("Consolas");
	m_output->setFont(f);
	m_output->setReadOnly(true);
	
	this->setCentralWidget(ot::WidgetViewManager::instance().getDockManager());
	this->setWindowTitle("OToolkit");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());

	ot::WidgetViewManager::instance().addView(ot::BasicServiceInformation(), m_output);

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

	QMetaObject::invokeMethod(this, &AppBase::slotInitialize, Qt::QueuedConnection);

	OT_LOG_I("Initializing");
}
