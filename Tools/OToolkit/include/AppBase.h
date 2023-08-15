#pragma once

// Qt header
#include <QtCore/qthread.h>
#include <QtWidgets/qmainwindow.h>

#define APP_BASE_APP_NAME "OToolkit"

class StatusBar;
class LogVisualization;
class Terminal;
class TextFinder;

class QTabWidget;
class QMenuBar;
class QDockWidget;
class QTextEdit;
class QAction;
class QApplication;

#define OTOOLKIT_LOG(___sender, ___message) AppBase::instance()->log(___sender, ___message)
#define OTOOLKIT_LOGW(___sender, ___message) AppBase::instance()->logWarning(___sender, ___message)
#define OTOOLKIT_LOGE(___sender, ___message) AppBase::instance()->logError(___sender, ___message)

class AppBase : public QMainWindow {
	Q_OBJECT
public:
	static AppBase * instance(void);

	StatusBar * sb(void) { return m_statusBar; };

	virtual void closeEvent(QCloseEvent * _event) override;

	void setUrl(const QString& _url) { m_url = _url; };
	const QString& url(void) const { return m_url; };

	void log(const QString& _sender, const QString& _message);
	void logWarning(const QString& _sender, const QString& _message);
	void logError(const QString& _sender, const QString& _message);

	void setApplicationInstance(QApplication* _app) { m_app = _app; };
	QApplication* applicationInstance(void) { return m_app; };

public slots:
	void slotProcessMessage(const QString& _message);
	void slotDockWidgetVisibilityChanged(bool _visible);
	void slotToggleOutput(void);
	void slotSettings(void);
	void slotClose(void);

private slots:
	void slotInitialize(void);
	void slotLog(const QString& _sender, const QString& _message, int _type);

private:
	AppBase();
	
	Qt::HANDLE			m_mainThread;

	QString				m_url;

	QTabWidget *		m_tabWidget;
	StatusBar *			m_statusBar;
	QMenuBar *			m_menuBar;

	LogVisualization *	m_logger;
	Terminal *			m_terminal;
	TextFinder* m_textFinder;

	QDockWidget *		m_outputDock;
	QTextEdit *			m_output;

	QAction *			m_outputAction;
	QAction *			m_settingsAction;
	QAction *			m_exitAction;
	QApplication* m_app;
};