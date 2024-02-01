//! @file ConnectToLoggerDialog.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTCore/Logger.h"
#include "OTWidgets/Dialog.h"

#include <QtCore/qlist.h>

#include <list>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class ConnectToLoggerDialog : public ot::Dialog {
	Q_OBJECT
public:
	ConnectToLoggerDialog();
	virtual ~ConnectToLoggerDialog();

	const QList<ot::LogMessage>& messageBuffer(void) const { return m_messageBuffer; };

	QString loggerServiceUrl(void) const;

	void queueConnectRequest(void);
	void queueRecenterRequest(void);

private slots:
	void slotCancel(void);
	void slotConnect(void);
	void slotPingFail(void);
	void slotRegisterFail(void);
	void slotDone(void);
	void slotRecenter(void);

private:
	void worker(QString _url);

	QVBoxLayout *				m_centralLayout;
	QGridLayout *				m_inputLayout;
	QHBoxLayout *				m_buttonLayout;

	QLabel *					m_urlL;
	QLineEdit *					m_url;

	QPushButton *				m_btnCancel;
	QPushButton *				m_btnConnect;

	QList<ot::LogMessage>	m_messageBuffer;
};