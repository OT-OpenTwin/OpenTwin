//! @file AppBase.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/ColorStyle.h"

// Qt header
#include <QtCore/qthread.h>
#include <QtWidgets/qmainwindow.h>

class ToolManager;
class LogVisualization;
class Terminal;

class QTabWidget;
class QTextEdit;
class QAction;
class QApplication;
class QShortcut;

class AppBase : public QMainWindow, public otoolkit::APIInterface, public ot::AbstractLogNotifier {
	Q_OBJECT
public:
	// Static functions

	static AppBase * instance(QApplication* _app = (QApplication*)nullptr);

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	virtual void log(const ot::LogMessage& _message) override;

	virtual void log(const QString& _sender, otoolkit::APIInterface::InterfaceLogType _type, const QString& _message) override;

	virtual bool addTool(otoolkit::Tool* _tool) override;

	virtual void updateStatusString(const QString& _statusText) override;

	virtual void updateStatusStringAsError(const QString& _statusText) override;

	virtual void registerToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) override;

	virtual void removeToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) override;

	virtual otoolkit::SettingsRef createSettingsInstance(void) override;

	virtual const QWidget* rootWidget(void) const override { return this; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Qt base functions

	virtual void closeEvent(QCloseEvent* _event) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setUrl(const QString& _url);
	const QString& url(void) const { return m_url; };

	void setApplicationInstance(QApplication* _app) { m_app = _app; };
	QApplication* applicationInstance(void) { return m_app; };

public slots:
	void slotProcessMessage(const QString& _json);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private slots:
	void slotLogMessage(const QString& _sender, const QString& _message);
	void slotLogWarning(const QString& _sender, const QString& _message);
	void slotLogError(const QString& _sender, const QString& _message);
	void slotSetStatus(const QString& _text);
	void slotSetErrorStatus(const QString& _text);
	void slotInitialize(void);
	void slotInitializeTools(void);
	void slotRecenter(void);
	void slotColorStyleChanged(const ot::ColorStyle& _style);

private:
	AppBase(QApplication* _app = (QApplication*)nullptr);
	
	Qt::HANDLE			m_mainThread;

	QString				m_url;

	ToolManager*        m_toolManager;
	
	LogVisualization *	m_logger;
	
	QTextEdit *			m_output;
	QApplication*		m_app;

	QShortcut* m_recenterShortcut;
};