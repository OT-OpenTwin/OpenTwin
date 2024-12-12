#pragma once

// Qt header
#include <QtCore/qobject.h>

// std header
#include <string>

class QLocalSocket;

class CommunicationHandler : public QObject {
	Q_OBJECT
public:
	CommunicationHandler();
	virtual ~CommunicationHandler();

	void setServerName(const std::string& _serverName) { m_serverName = _serverName; };
	const std::string& getServerName(void) const { return m_serverName; };

	bool ensureConnectionToServer(void);

Q_SIGNALS:
	void disconnected(void);
	void dataReceived(std::string _data);

private Q_SLOTS:
	void slotDataReceived(void);
	void slotProcessMessage(std::string _data);
	void slotDisconnected(void);

private:
	std::string m_serverName;
	QLocalSocket* m_serverSocket;

};