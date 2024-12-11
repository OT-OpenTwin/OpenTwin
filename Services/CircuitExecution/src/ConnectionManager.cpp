#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(QObject* parent) :QObject(parent) {

    m_socket = new QLocalSocket(this);

    QObject::connect(m_socket, &QLocalSocket::connected, this, &ConnectionManager::sendHello);
	QObject::connect(m_socket, &QLocalSocket::readyRead, this, &ConnectionManager::receiveResponse);
    QObject::connect(m_socket, &QLocalSocket::errorOccurred, this, &ConnectionManager::handleError);
    QObject::connect(m_socket, &QLocalSocket::disconnected, this, &ConnectionManager::handleDisconnected);
}


void ConnectionManager::connectToCircuitSimulatorService(const QString& serverName) {

    /*OT_LOG_D("Connecting to Server:" + serverName.toStdString());*/
    m_socket->connectToServer(serverName);
    if (!m_socket->waitForConnected()) {
        OT_LOG_E("Failed to connect to server:" + m_socket->errorString().toStdString());
    }
    else {
        OT_LOG_D("Connected to Server");
    }

}

void ConnectionManager::receiveResponse() {
    QByteArray response = m_socket->readAll();
    OT_LOG_D("Got following netlist: " + response.toStdString());
}

void ConnectionManager::sendHello() {
    OT_LOG_D("Connected");
    QByteArray message = "Connected";
    m_socket->write(message);
    m_socket->flush();
}

void ConnectionManager::handleError(QLocalSocket::LocalSocketError error) {
    OT_LOG_E("Error in establishing connection to CircuitSimulatorService: " + m_socket->errorString().toStdString());
}

void ConnectionManager::handleDisconnected() {
    OT_LOG_D("Client disconnected from server");
    exit(0);
}
