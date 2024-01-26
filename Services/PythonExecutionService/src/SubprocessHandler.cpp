#include "SubprocessHandler.h"

#include "OTSystem/OperatingSystem.h"

SubprocessHandler::SubprocessHandler(const std::string& serverName, int argc, char* argv[])
	:_serverName(serverName), _qtApplication(argc,argv)
{
	_subprocessPath = FindSubprocessPath() + _executableName;
	OT_LOG_D("Starting to create the subprocess: " + _subprocessPath);
	InitiateProcess();
	StartProcess();
	ConnectWithSubService();
}

bool SubprocessHandler::Send(const std::string& message)
{
	_socket.write(message.c_str());
	_socket.flush();
	bool allIsWritten = _socket.waitForBytesWritten(_timeoutSendingMessage);
	if (!allIsWritten)
	{
		SocketErrorOccured();
	}
	return allIsWritten;
}

std::string SubprocessHandler::FindSubprocessPath()
{
#ifdef _DEBUG
		const std::string executablePathBase = ot::os::getEnvironmentVariable("OT_PYTHON_EXECUTION_ROOT");
		const std::string debugBinFolder = ot::os::getEnvironmentVariable("OT_DLLD");
		if (debugBinFolder == "" || executablePathBase == "")
		{
			assert(0);
		}
		return executablePathBase + "\\x64\\ReleaseDebug\\";
#else
	const std::string deploymentFolder = ot::os::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
	if (deploymentFolder != "")
	{
		return deploymentFolder + "\\Deployment\\";
	}
	else
	{
		return ".\\";
	}

#endif // DEBUG

}

void SubprocessHandler::InitiateProcess()
{
	QStringList arguments{ QString(_serverName.c_str()) };
	_subProcess.setArguments(arguments);

	//Turning off the default communication channels of the process
	_subProcess.setStandardOutputFile(QProcess::nullDevice());
	_subProcess.setStandardErrorFile(QProcess::nullDevice());
	_subProcess.setProgram(_subprocessPath.c_str());
}

void SubprocessHandler::StartProcess()
{
	_subProcess.start();
	_subProcess.waitForStarted(_timeoutSubprocessStart);
	ProcessStateChanged();

	QObject::connect(&_subProcess, &QProcess::errorOccurred, this, &SubprocessHandler::ProcessErrorOccured);
	QObject::connect(&_subProcess, &QProcess::stateChanged, this, &SubprocessHandler::ProcessStateChanged);
}

void SubprocessHandler::ConnectWithSubService()
{
	_socket.connectToServer(_serverName.c_str());
	bool connected = _socket.waitForConnected(_timeoutServerConnect);
	if (!connected)
	{
		SocketErrorOccured();
	}
	QObject::connect(&_socket, &QLocalSocket::readyRead, this, &SubprocessHandler::SocketRecievedMessage);
	QObject::connect(&_socket, &QLocalSocket::disconnected, this, &SubprocessHandler::SocketDisconnected);
	QObject::connect(&_socket, &QLocalSocket::errorOccurred, this, &SubprocessHandler::SocketErrorOccured);
	QObject::connect(&_socket, &QLocalSocket::stateChanged, this, &SubprocessHandler::SocketStateChanged);
}

void SubprocessHandler::ProcessErrorOccured()
{
	const auto error = _subProcess.error();
	if (error == QProcess::FailedToStart)
	{
		//The process failed to start.Either the invoked program is missing, or you may have insufficient permissions or resources to invoke the program.
	}
	else if (error == QProcess::Crashed)
	{
		//The process crashed some time after starting successfully.
	}
	else if (error == QProcess::Timedout)
	{
		//The last waitFor...() function timed out.The state of QProcess is unchanged, and you can try calling waitFor...() again.
	}
	else if (error == QProcess::WriteError)
	{
		//An error occurred when attempting to write to the process.For example, the process may not be running, or it may have closed its input channel.
	}
	else if (error == QProcess::ReadError)
	{
		//An error occurred when attempting to read from the process.For example, the process may not be running.
	}
	else if (error == QProcess::UnknownError)
	{
		//An unknown error occurred.This is the default return value of error().
	}
	else
	{
		assert(0);
	}
}

void SubprocessHandler::ProcessStateChanged()
{
	const auto state = _subProcess.state();
	if (state == QProcess::NotRunning)
	{
		//The process is not running.
	}
	else if(state == QProcess::Starting)
	{
		//The process is starting, but the program has not yet been invoked.
	}
	else if (state == QProcess::Running)
	{
		//The process is running and is ready for reading and writing.
	}
	else
	{
		assert(0);
	}

}

void SubprocessHandler::SocketErrorOccured()
{
	const auto error = _socket.error();
	if (error == QLocalSocket::ConnectionRefusedError)
	{
		//	The connection was refused by the peer (or timed out).

	}
	else if (error == QLocalSocket::PeerClosedError)
	{
		//The remote socket closed the connection.Note that the client socket(i.e., this socket) will be closed after the remote close notification has been sent.		
	}
	else if (error == QLocalSocket::ServerNotFoundError)
	{
		//The local socket name was not found.

	}
	else if (error == QLocalSocket::SocketAccessError)
	{
		//The socket operation failed because the application lacked the required privileges.
	}
	else if (error == QLocalSocket::SocketResourceError)
	{
		//The local system ran out of resources(e.g., too many sockets).
	}
	else if (error == QLocalSocket::SocketTimeoutError)
	{
		//The socket operation timed out.
	}
	else if (error == QLocalSocket::DatagramTooLargeError)
	{
		//The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).
	}
	else if (error == QLocalSocket::ConnectionError)
	{
		//An error occurred with the connection.
	}
	else if (error == QLocalSocket::UnsupportedSocketOperationError)
	{
		//The requested socket operation is not supported by the local operating system.
	}
	else if (error == QLocalSocket::OperationError)
	{
		//An operation was attempted while the socket was in a state that did not permit it.
	}
	else if (error == QLocalSocket::UnknownSocketError)
	{
		//An unidentified error occurred.
	}
	else
	{
		assert(0);
	}

}

void SubprocessHandler::SocketStateChanged()
{
	const auto state = _socket.state();
	if (state == QLocalSocket::UnconnectedState)
	{
		//The socket is not connected.
	}
	else if (state == QLocalSocket::ConnectingState)
	{
		//The socket has started establishing a connection.
	}
	else if (state == QLocalSocket::ConnectedState)
	{
		//	A connection is established.
	}
	else if (state == QLocalSocket::ClosingState)
	{
		//	The socket is about to close (data may still be waiting to be written).
	}
	else
	{
		assert(0);
	}
}

void SubprocessHandler::SocketRecievedMessage()
{


}

void SubprocessHandler::SocketDisconnected()
{

}
