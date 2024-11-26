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
#define slots Q_SLOTS

QLocalSocket m_socket;

int m_socketConnectionTimeout = 60000; //60 seconds
int m_numberOfServerConnectionTrials = 30;
int m_secondsUntilSearchForServer = 1;

void Send(const std::string& message)
{
	m_socket.write((message + "\n").c_str());
	m_socket.flush();
	bool allIsWritten = m_socket.waitForBytesWritten(-1);
}

void MessageReceived()
{
	if (m_socket.canReadLine())
	{
		try
		{
			const std::string message(m_socket.readLine().data());

			ot::JsonDocument document;
			document.fromJson(message);
			ot::ReturnMessage returnMessage(ot::ReturnMessage::Ok);
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


int main(int argc, char* argv[], char* envp[]){




	QApplication a(argc, argv);
#ifdef _RELEASEDEBUG
	const std::string serverName = "TestServer";
	ot::ServiceLogNotifier::initialize("CircuitSubprocess", "", true);

#else
	const std::string serverName = argv[1];
	ot::ServiceLogNotifier::initialize("CircuitSubprocess", "", false);
#endif // _DEBUG
	try
	{
		OT_LOG_D("Connecting subservice with server: " + serverName);
		int count(0);
		do
		{
			std::this_thread::sleep_for(std::chrono::seconds(m_secondsUntilSearchForServer));
			m_socket.connectToServer(serverName.c_str());
			count++;

		} while (m_socket.error() == QLocalSocket::ServerNotFoundError && count < m_numberOfServerConnectionTrials);

		if (m_socket.state() == QLocalSocket::ConnectingState)
		{
			m_socket.waitForConnected(m_socketConnectionTimeout);
		}

		if (m_socket.state() != QLocalSocket::ConnectedState)
		{
			auto socketError = m_socket.errorString();
			OT_LOG_E("Error while trying to connect to Circuit Service: " + socketError.toStdString());
			OT_LOG_E("Shutting down");
			exit(0);
		}
		OT_LOG_D("Connected with socket");

		

		QEventLoop loop;
		QObject::connect(&m_socket, &QLocalSocket::readyRead, &loop, &MessageReceived);
		QObject::connect(&m_socket, &QLocalSocket::disconnected, &loop, &DisConnect);
		OT_LOG_D("Finished startup. Starting to listen.");
		ot::ReturnMessage msg(ot::ReturnMessage::Ok);
		Send(msg.toJson());
		loop.exec();
		return a.exec();
	}
	catch (const std::exception& e)
	{
		OT_LOG_E("Crash during setup: " + std::string(e.what()));
	}



}