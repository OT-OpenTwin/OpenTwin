//! @file StatusManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qstring.h>
#include <QtWidgets/qstatusbar.h>	// Base class

// std header
#include <map>
#include <list>

class QTimer;
class QLabel;

class StatusManager : public QStatusBar {
	Q_OBJECT
public:
	StatusManager();
	virtual ~StatusManager();

	//! @brief Will set the provided text as information text to the statusbar
	void setInfo(const QString& _text);
	
	//! @brief Will set the provided text as error text to the statusbar
	void setErrorInfo(const QString& _text);

	void setCurrentTool(const QString& _toolName);

	void addTool(const QString& _toolName, const std::list<QWidget*>& _widgets);
	void removeTool(const QString& _toolName);

private slots:
	void slotResetErrorStatus(void);

private:
	bool						m_statusIsError;
	
	QString						m_statusText;

	QTimer *					m_timerErrorStatusReset;
	QLabel *					m_infoLabel;
	QLabel *					m_stretchLabel;

	QString						m_currentTool;
	std::map<QString, std::list<QWidget*>> m_toolWidgets;

};