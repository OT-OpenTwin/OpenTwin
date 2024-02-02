#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>

#include "OTCore/Logger.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ServiceLogNotifier.h"


#undef slots
#include "ActionHandler.h"
#define slots Q_SLOTS

QLocalSocket* _socket = nullptr;
ActionHandler _actionHandler;

int _socketConnectionTimeout = 60000; //60 seconds


ot::ReturnMessage HandleMessage(ot::JsonDocument& message)
{
	ot::ReturnMessage returnMessage = _actionHandler.Handle(message);
	return returnMessage;
}

void Send(const std::string& message)
{
	_socket->write(message.c_str());
	_socket->flush();
	bool allIsWritten = _socket->waitForBytesWritten(-1);
}

void MessageReceived()
{
	if(_socket->canReadLine())
	{
		const std::string message(_socket->readLine().data());

		ot::JsonDocument document;
		document.fromJson(message);
		ot::ReturnMessage returnMessage = HandleMessage(document);
		Send(returnMessage.toJson() + "\n");

	}
}


void DisConnect()
{
	OT_LOG_D("Socket disconnected. Shutting down.");
	exit(0);
}


int main(int argc, char* argv[], char* envp[])
{
	QApplication a(argc, argv);
#ifdef _RELEASEDEBUG
	const std::string serverName = "TestServer";
	ot::ServiceLogNotifier::initialize("PythonSubprocess", "", true);
	
#else
	const std::string serverName = argv[1];
	ot::ServiceLogNotifier::initialize("PythonSubprocess", "", false);
#endif // _DEBUG

	QLocalServer _server;
	OT_LOG_D("Starting subservice with server: " + serverName);
	bool success = _server.listen(serverName.c_str());
	
	bool connectionReceived = _server.waitForNewConnection(_socketConnectionTimeout);
	if (!connectionReceived)
	{
		OT_LOG_E("Timeout while waiting for socket connection. Shutting down.");
		exit(0);
	}
	_socket = _server.nextPendingConnection();
	_socket->waitForConnected();
	OT_LOG_D("Connected with socket");

	QEventLoop loop;
	QObject::connect(_socket, &QLocalSocket::readyRead, &loop, &MessageReceived);
	QObject::connect(_socket, &QLocalSocket::disconnected, &loop, &DisConnect);
		
	loop.exec();
	return a.exec();
}