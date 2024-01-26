#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>

QLocalServer server;
QLocalSocket* socket;
QEventLoop* loop;

void Scream()
{
	std::cout << "Received a message\n";
	std::string receiveMessage("");
	if (socket->canReadLine())
	{
		receiveMessage += (socket->readLine().data());
		socket->write("General Kenobi.");
		socket->flush();
		socket->waitForBytesWritten();
	}

}

void DisConnect()
{
	std::cout << "Disconnected.\n";
	delete socket;
	socket = nullptr;

}

void ConnectNew()
{
	std::cout << "Reconnected.\n";
	socket = server.nextPendingConnection();
	QObject::connect(socket, &QLocalSocket::readyRead, loop, &Scream);
	QObject::connect(socket, &QLocalSocket::disconnected, loop, &DisConnect);
	socket->write("Handshake");
	socket->flush();
	socket->waitForBytesWritten();
}


int main(int argc, char* argv[], char* envp[])
{
	QApplication a(argc, argv);
	loop = new QEventLoop();
	const std::string serverName = argv[1];
	bool success = server.listen(serverName.c_str());

	server.waitForNewConnection(-1);
	socket = server.nextPendingConnection();
	socket->waitForConnected();
	socket->write("Handshake");
	socket->flush();


	QObject::connect(socket, &QLocalSocket::readyRead, loop, &Scream);
	QObject::connect(&server, &QLocalServer::newConnection, loop, &ConnectNew);
	QObject::connect(socket, &QLocalSocket::disconnected, loop, &DisConnect);
	loop->exec();
	return a.exec();
}