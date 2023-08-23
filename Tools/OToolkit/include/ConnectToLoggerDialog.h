#pragma once

#include "OpenTwinCore/Logger.h"

#include <QtCore/qlist.h>
#include <QtWidgets/qdialog.h>

#include <list>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class ConnectToLoggerDialog : public QDialog {
	Q_OBJECT
public:
	ConnectToLoggerDialog(bool _isAutoConnect);
	virtual ~ConnectToLoggerDialog();

	const QList<ot::LogMessage>& messageBuffer(void) const { return m_messageBuffer; };
	bool success(void) const { return m_success; };

private slots:
	void slotCancel(void);
	void slotConnect(void);
	void slotPingFail(void);
	void slotRegisterFail(void);
	void slotDone(void);

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
	bool						m_success;

	ConnectToLoggerDialog() = delete;
};