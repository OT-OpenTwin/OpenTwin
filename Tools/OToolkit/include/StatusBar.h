//! @file StatusBar.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qlist.h>
#include <QtWidgets/qstatusbar.h>	// Base class

// std header
#include <map>

class QTimer;
class QLabel;

class StatusBar : public QStatusBar {
	Q_OBJECT
public:
	StatusBar();

	//! @brief Will set the provided text as information text to the statusbar
	void setInfo(const QString& _text);
	
	//! @brief Will set the provided text as error text to the statusbar
	void setErrorInfo(const QString& _text);

private slots:
	void slotResetErrorStatus(void);

private:
	bool						m_statusIsError;
	
	QString						m_statusText;

	QTimer *					m_timerErrorStatusReset;
	QLabel *					m_infoLabel;
	QLabel *					m_stretchLabel;


};