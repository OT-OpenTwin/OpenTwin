// Service header
#include "CommunicationHandler.h"

// OpenTwin header
#include "OTCore/Logger.h"

// Qt header
#include <QtNetwork/qlocalsocket.h>

CommunicationHandler::CommunicationHandler() 
	: m_serverSocket(nullptr)
{

}

CommunicationHandler::~CommunicationHandler() {

}

bool CommunicationHandler::ensureConnectionToServer(void) {
	if (m_serverSocket) {
		return true;
	}
	
	m_serverSocket = new QLocalSocket;

	m_serverSocket->connectToServer(QString::fromStdString(m_serverName));
}

void CommunicationHandler::slotDataReceived(void) {
	if (!m_serverSocket) {
		OT_LOG_EA("No socket set");
		return;
	}

	std::string data = m_serverSocket->readAll().toStdString();

	QMetaObject::invokeMethod(this, "slotProcessMessage", Qt::QueuedConnection, Q_ARG(std::string, data));
}

void CommunicationHandler::slotProcessMessage(std::string _data) {
	Q_EMIT dataReceived(_data);
}

void CommunicationHandler::slotDisconnected(void) {
	m_serverSocket->deleteLater();
	m_serverSocket = nullptr;

	Q_EMIT disconnected();
}
