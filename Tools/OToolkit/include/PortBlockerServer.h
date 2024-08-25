//! @file PortBlockerServer.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtNetwork/qtcpserver.h>

class PortBlockerServer : public QTcpServer {
	Q_OBJECT
public:
	PortBlockerServer();
	virtual ~PortBlockerServer() {};

private Q_SLOTS:
	void slotNewPendingConnection(void);

private:

};