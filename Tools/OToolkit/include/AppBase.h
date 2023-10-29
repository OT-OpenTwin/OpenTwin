//! @file AppBase.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qthread.h>
#include <QtWidgets/qmainwindow.h>

#define APP_BASE_APP_NAME "OToolkit"

class StatusBar;
class LogVisualization;
class Terminal;

class QTabWidget;
class QMenuBar;
class QDockWidget;
class QTextEdit;
class QAction;
class QApplication;
class QShortcut;

class AppBase : public QMainWindow, public otoolkit::APIInterface {
	Q_OBJECT
public:
	// Static functions

	static AppBase * instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	virtual void log(const QString& _sender, otoolkit::APIInterface::InterfaceLogType _type, const QString& _message) override;

	virtual bool addTool(otoolkit::Tool* _tool) override;

	virtual void updateStatusString(const QString& _statusText) override;

	virtual void updateStatusStringAsError(const QString& _statusText) override;

	virtual void registerToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) override;

	virtual void removeToolActivityNotifier(otoolkit::ToolActivityNotifier* _notifier) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Qt base functions

	virtual void closeEvent(QCloseEvent* _event) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	StatusBar * sb(void) { return m_statusBar; };

	void setUrl(const QString& _url) { m_url = _url; };
	const QString& url(void) const { return m_url; };

	void setApplicationInstance(QApplication* _app) { m_app = _app; };
	QApplication* applicationInstance(void) { return m_app; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private slots:
	void slotLogMessage(const QString& _sender, const QString& _message);
	void slotLogWarning(const QString& _sender, const QString& _message);
	void slotLogError(const QString& _sender, const QString& _message);
	void slotInitialize(void);
	void slotRecenter(void);

private:
	AppBase();
	
	Qt::HANDLE			m_mainThread;

	QString				m_url;

	QTabWidget *		m_tabWidget;
	StatusBar *			m_statusBar;
	QMenuBar *			m_menuBar;

	LogVisualization *	m_logger;
	Terminal *			m_terminal;

	QDockWidget *		m_outputDock;
	QTextEdit *			m_output;

	QAction *			m_settingsAction;
	QApplication*		m_app;

	QShortcut* m_recenterShortcut;
};