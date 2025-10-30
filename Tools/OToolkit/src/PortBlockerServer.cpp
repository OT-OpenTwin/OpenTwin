// @otlicense

#include "PortBlockerServer.h"

#include <QtNetwork/qtcpsocket.h>

PortBlockerServer::PortBlockerServer() {
	this->connect(this, &PortBlockerServer::pendingConnectionAvailable, this, &PortBlockerServer::slotNewPendingConnection);
}

void PortBlockerServer::slotNewPendingConnection(void) {
	QTcpSocket* newSocket = this->nextPendingConnection();
	newSocket->close();
}