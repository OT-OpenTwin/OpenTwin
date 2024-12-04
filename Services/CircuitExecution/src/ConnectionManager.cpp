#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(QObject* parent) :QObject(parent) {

    m_socket = new QLocalSocket(this);

    QObject::connect(m_socket, &QLocalSocket::connected, this, &ConnectionManager::sendHello);
	QObject::connect(m_socket, &QLocalSocket::readyRead, this, &ConnectionManager::receiveResponse);
    QObject::connect(m_socket, &QLocalSocket::errorOccurred, this, &ConnectionManager::handleError);
}


void ConnectionManager::connectToCircuitSimulatorService(const QString& serverName) {
    m_socket->connectToServer(serverName);
    if (!m_socket->waitForConnected()) {
        OT_LOG_E("Failed to connect to server:" + m_socket->errorString().toStdString());
    }
    else {
        OT_LOG_D("Connected to Server");
    }

    m_socket->write("Hello");
    m_socket->flush();
}

void ConnectionManager::receiveResponse() {
    QByteArray response = m_socket->readAll();
    OT_LOG_D("Answer from CircuitSimulatorService: " + response.toStdString());
}

void ConnectionManager::sendHello() {
    OT_LOG_D("Connection to CircuitSimulatorService is established. Sending.");
    QByteArray message = "Hello";
    m_socket->write(message);
    m_socket->flush();
}

void ConnectionManager::handleError(QLocalSocket::LocalSocketError error) {
    OT_LOG_E("Error in establishing connection to CircuitSimulatorService: " + m_socket->errorString().toStdString());
}