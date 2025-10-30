// @otlicense

#pragma once

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/AbstractLogNotifier.h"
#include "OTCommunication/ActionHandler.h"

// Qt header
#include <QtCore/qthread.h>
#include <QtWidgets/qmainwindow.h>

// std header
#include <mutex>

class Logging;
class Terminal;
class ToolManager;
class ExternalLibraryManager;

class QTabWidget;
class QAction;
class QApplication;
class QShortcut;

namespace ot { class PlainTextEditView; }

class AppBase : public QMainWindow, public otoolkit::APIInterface, public ot::AbstractLogNotifier, public ot::ActionHandler {
	Q_OBJECT
public:
	enum StartOption {
		LogExport,
		NoAutoStart
	};

	// Static functions

	static AppBase * instance(QApplication* _app = (QApplication*)nullptr);

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	virtual void log(const ot::LogMessage& _message) override;

	virtual void log(const QString& _sender, InterfaceLogType _type, const QString& _message) override;

	virtual bool addTool(otoolkit::Tool* _tool) override;

	virtual void updateStatusString(const QString& _statusText) override;

	virtual void updateStatusStringAsError(const QString& _statusText) override;

	virtual void registerToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) override;

	virtual void removeToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) override;

	virtual otoolkit::SettingsRef createSettingsInstance() override;

	virtual const QWidget* rootWidget() const override { return this; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Qt base functions

	virtual void closeEvent(QCloseEvent* _event) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setUrl(const QString& _url);
	const QString& url() const { return m_url; };

	void parseStartArgs(const std::string& _args);

	void setApplicationInstance(QApplication* _app) { m_app = _app; };
	QApplication* applicationInstance() { return m_app; };

	void setUpdateTransparentColorStyleValueEnabled(bool _enabled);
	bool getUpdateTransparentColorStyleValueEnabled() const { return m_replaceTransparentColorStyleValue; };
	void updateTransparentColorStyleValue();

	bool getIgnoreToolAutoStart() const { return m_ignoreToolAutoStart; };

public Q_SLOTS:
	void slotProcessMessage(const QString& _json);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotLogMessage(const QString& _sender, const QString& _message);
	void slotLogWarning(const QString& _sender, const QString& _message);
	void slotLogError(const QString& _sender, const QString& _message);
	void slotSetStatus(const QString& _text);
	void slotSetErrorStatus(const QString& _text);
	void slotInitialize();
	void slotInitializeTools();
	void slotRecenter();
	void slotFinalizeInit();
	void slotColorStyleChanged();

private:
	void handleDisplayData(ot::JsonDocument& _doc);

	AppBase(QApplication* _app = (QApplication*)nullptr);

	Qt::HANDLE				m_mainThread;

	bool                    m_ignoreToolAutoStart;

	QString					m_url;

	std::list<StartOption> m_startArgs;

	ToolManager*			m_toolManager;
	
	ExternalLibraryManager* m_externalLibraryManager;

	Logging*				m_logger;
	
	ot::PlainTextEditView*	m_output;
	QApplication*			m_app;

	QShortcut* m_recenterShortcut;

	std::mutex m_logMutex;

	bool m_replaceTransparentColorStyleValue;
};
