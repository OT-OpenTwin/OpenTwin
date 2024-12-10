#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#include <QtNetwork/qlocalsocket.h>

#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>

#include "OTCore/Logger.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include <thread>

#undef slots
#include "ActionHandler.h"
#define slots Q_SLOTS

QLocalSocket _socket;


int _socketConnectionTimeout = 60000; //60 seconds
int _numberOfServerConnectionTrials = 30;
int _secondsUntilSearchForServer = 1;

ActionHandler* _actionHandler = nullptr;

ot::ReturnMessage HandleMessage(ot::JsonDocument& message)
{
	ot::ReturnMessage returnMessage = _actionHandler->Handle(message);
	return returnMessage;
}

void Send(const std::string& message)
{
	OT_LOG(message, ot::OUTGOING_MESSAGE_LOG);

	_socket.write((message + "\n").c_str());
	_socket.flush();
	bool allIsWritten = _socket.waitForBytesWritten(-1);
}

void MessageReceived() {
	if(_socket.canReadLine()) {
		try
		{
			const std::string message(_socket.readLine().data());

			OT_LOG(message, ot::INBOUND_MESSAGE_LOG);

			ot::JsonDocument document;
			document.fromJson(message);
			ot::ReturnMessage returnMessage(HandleMessage(document));
			Send(returnMessage.toJson());
		}
		catch (std::exception e)
		{
			OT_LOG_E("Failed to handle incoming message due to: " + std::string(e.what()));
		}
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
	const std::string serverName = "TestServerPython";
	ot::ServiceLogNotifier::initialize("PythonSubprocess", "", true);
	
#else
	ot::ServiceLogNotifier::initialize("PythonSubprocess", "", false);

	std::string serverName;
	if (argc < 2) {
		OT_LOG_E("Missing arguments");
		return 1;
	}
	serverName = argv[1];
#endif // _DEBUG
	try
	{
		OT_LOG_D("Connecting subservice with server: " + serverName);
		int count(0);
		do
		{
			std::this_thread::sleep_for(std::chrono::seconds(_secondsUntilSearchForServer));
			_socket.connectToServer(serverName.c_str());
			count++;

		} while (_socket.error() == QLocalSocket::ServerNotFoundError && count < _numberOfServerConnectionTrials);

		if (_socket.state() == QLocalSocket::ConnectingState)
		{
			_socket.waitForConnected(_socketConnectionTimeout);
		}

		if (_socket.state() != QLocalSocket::ConnectedState)
		{
			auto socketError = _socket.errorString();
			OT_LOG_E("Error while trying to connect to Python Service: " + socketError.toStdString());
			OT_LOG_E("Shutting down");
			exit(0);
		}
		OT_LOG_D("Connected with socket");

		_actionHandler = new ActionHandler();

		QEventLoop loop;
		QObject::connect(&_socket, &QLocalSocket::readyRead, &loop, &MessageReceived);
		QObject::connect(&_socket, &QLocalSocket::disconnected, &loop, &DisConnect);
		OT_LOG_D("Finished startup. Starting to listen.");
		ot::ReturnMessage msg(ot::ReturnMessage::Ok, OT_ACTION_CMD_CheckStartupCompleted);
		Send(msg.toJson());
		loop.exec();
		return a.exec();
	}
	catch (const std::exception& e)
	{
		OT_LOG_E("Crash during setup: " + std::string(e.what()));
	}
}