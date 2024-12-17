// Service header
#include "Timeouts.h"
#include "ActionHandler.h"
#include "CommunicationHandler.h"

// OpenTwin header
#include "OTCore/Logger.h"

// Qt header
#include <QtCore/qeventloop.h>
#include <QtNetwork/qlocalsocket.h>

// std header
#include <thread>

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

	this->connect(m_serverSocket, &QLocalSocket::readyRead, this, &CommunicationHandler::slotDataReceived);
	this->connect(m_serverSocket, &QLocalSocket::disconnected, this, &CommunicationHandler::slotDisconnected);

	OT_LOG_D("Connecting with server " + m_serverName);
	m_serverSocket->connectToServer(QString::fromStdString(m_serverName));

	int timeout = Timeouts::connection / Timeouts::tickRate;
	while (m_serverSocket->state() != QLocalSocket::ConnectedState) {
		if (timeout-- <= 0) {
			OT_LOG_E("Timeout while connecting to server");
			m_serverSocket->disconnect();
			delete m_serverSocket;
			m_serverSocket = nullptr;
			return false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(Timeouts::tickRate));
		this->processNextEvent();
	}
	OT_LOG_D("Connected with server");
	return true;
}

void CommunicationHandler::slotDataReceived(void) {
	if (!m_serverSocket) {
		OT_LOG_EA("No socket set");
		return;
	}

	std::string data = m_serverSocket->readAll().toStdString();

	QMetaObject::invokeMethod(this, "slotProcessMessage", Qt::QueuedConnection, Q_ARG(std::string, data));
}

void CommunicationHandler::slotProcessMessage(std::string _message) {
	OT_LOG(_message, ot::INBOUND_MESSAGE_LOG);
	
	ot::JsonDocument doc;
	if (!doc.fromJson(_message)) {
		OT_LOG_E("Failed to parse document");
	}

	try {
		ot::ReturnMessage returnMessage = ActionHandler::instance().handleAction(doc);
		this->writeToServer(returnMessage.toJson());
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
	catch (...) {
		OT_LOG_E("Unknown error");
	}
	
}

void CommunicationHandler::slotDisconnected(void) {
	m_serverSocket->deleteLater();
	m_serverSocket = nullptr;

	OT_LOG_D("Disconnected from server");

	Q_EMIT disconnected();

	exit(0);
}

bool CommunicationHandler::writeToServer(const std::string& _message) {
	QByteArray data = QByteArray::fromStdString(_message);
	data.append("\n");
	if (!m_serverSocket) {
		OT_LOG_E("Not connected");
		return false;
	}

	OT_LOG(_message, ot::OUTGOING_MESSAGE_LOG);

	m_serverSocket->write(data);
	m_serverSocket->flush();
	return true;
}

void CommunicationHandler::processNextEvent(void) {
	QEventLoop loop;
	loop.processEvents(QEventLoop::ProcessEventsFlag::AllEvents);
}
