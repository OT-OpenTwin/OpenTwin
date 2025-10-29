// @otlicense

//! @file NetworkTools.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "PortBlockerServer.h"

#include <QtNetwork/qtcpsocket.h>

PortBlockerServer::PortBlockerServer() {
	this->connect(this, &PortBlockerServer::pendingConnectionAvailable, this, &PortBlockerServer::slotNewPendingConnection);
}

void PortBlockerServer::slotNewPendingConnection(void) {
	QTcpSocket* newSocket = this->nextPendingConnection();
	newSocket->close();
}