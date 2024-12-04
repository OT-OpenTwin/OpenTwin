#pragma once
#include "QtNetwork/qlocalsocket.h"
#include "QtCore/qobject.h"


class ConnectionManager : public QObject {
	Q_OBJECT
public: 
	ConnectionManager(QObject* parent = (QObject*)nullptr);
	//~ConnectionManager();

	//void connectoToServer(std::string &serverName);

	
	

};