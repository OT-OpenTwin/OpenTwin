#include "SubprocessHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"

SubprocessHandler::SubprocessHandler(const std::string& serverName)
	:_serverName(serverName)
{
	_initialisationRoutines.reserve(_numberOfInitialisationRoutines);
	
	_subprocessPath = FindSubprocessPath() + _executableName;
	InitiateProcess();
#ifdef _DEBUG
	OT_LOG_D("Debug");
	_subProcess.start();
	bool processStarted = _subProcess.waitForStarted(_timeoutSubprocessStart);
	_socket.connectToServer(_serverName.c_str());
	bool connected = _socket.waitForConnected(-1);
	assert(connected);
#else
	std::thread workerThread(&SubprocessHandler::RunSubprocess, this);
	workerThread.detach();

#endif // DEBUG

}

SubprocessHandler::~SubprocessHandler()
{
	CloseSubprocess();
}

std::string SubprocessHandler::FindSubprocessPath()
{
#ifdef _DEBUG
	const std::string otRootFolder = ot::os::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
	assert(otRootFolder != "");
	return otRootFolder + "\\Deployment\\";
	
#else
	return ".\\";
	
#endif // DEBUG

}

void SubprocessHandler::InitiateProcess()
{
	QStringList arguments{ QString(_serverName.c_str()) };
	_subProcess.setArguments(arguments);

	//Turning off the default communication channels of the process. Communication will be handled via QLocalServer and QLocalSocket
	_subProcess.setStandardOutputFile(QProcess::nullDevice());
	_subProcess.setStandardErrorFile(QProcess::nullDevice());
	_subProcess.setProgram(_subprocessPath.c_str());
}

void SubprocessHandler::RunSubprocess()
{
	try
	{
		const bool startSuccessfull = StartProcess();
		if (!startSuccessfull)
		{
			std::string message = "Starting Python Subprocess timeout.";
			OT_LOG_E(message);
			throw std::exception(message.c_str());
		}
		OT_LOG_D("Python Subprocess started");

		const bool connectionSuccessfull = ConnectWithSubService();
		if (!connectionSuccessfull)
		{
			std::string message = "Connecting with Python Subprocess timeout.";
			OT_LOG_E(message);
			throw std::exception(message.c_str());
		}
		OT_LOG_D("Connection with Python Subprocess established");
		_initialConnectionEstablished = true;
		InitialiseSubprocess();
	}
	catch (std::exception& e)
	{
		const std::string exceptionMessage("Starting the python subprocess failed due to: " + std::string(e.what())+ ". Shutting down.");
		OT_LOG_E(exceptionMessage);
		exit(0);
	}
}

bool SubprocessHandler::StartProcess()
{
	bool processStarted = false;
	for (int i = 1; i <= _numberOfRetries; i++)
	{
		OT_LOG_D("Starting the subprocess: " + _subProcess.program().toStdString() + " trial: " + std::to_string(i) + "/" + std::to_string(_numberOfRetries));
		_subProcess.start();
		auto state = _subProcess.state();
		if (state == QProcess::NotRunning)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutSubprocessStart));
		}
		else if (state == QProcess::Starting)
		{
			processStarted = _subProcess.waitForStarted(_timeoutSubprocessStart);
			if (processStarted)
			{
				return true;
			}
			else
			{
				std::string errorMessage;
				if (ProcessErrorOccured(errorMessage))
				{
					errorMessage = "Error occured while starting Python Subservice: " + errorMessage;
					throw std::exception(errorMessage.c_str());
				}
			}
		}
		else
		{
			assert(state == QProcess::Running);
			return true;
		}

	}
	return processStarted;
	
}

bool SubprocessHandler::ConnectWithSubService()
{
	bool connectionEstablished = false;
	for (int i = 1; i <= _numberOfRetries; i++)
	{
		OT_LOG_D("Attempting connection with server: " + _serverName + " trial: " + std::to_string(i) + "/"+ std::to_string(_numberOfRetries));
		_socket.connectToServer(_serverName.c_str());

		auto socketState =_socket.state();
		if (socketState == QLocalSocket::UnconnectedState || socketState == QLocalSocket::ClosingState)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutServerConnect));
		}
		else if (socketState == QLocalSocket::ConnectingState)
		{
			connectionEstablished = _socket.waitForConnected(_timeoutServerConnect);
			if (connectionEstablished)
			{
				OT_LOG_D("Connection with subservice established");
				return connectionEstablished;
			}
			else
			{
				std::string errorMessage;
				if (SocketErrorOccured(errorMessage))
				{
					errorMessage = "Error occured while connecting with Python Subservice: " + errorMessage;
					throw std::exception(errorMessage.c_str());
				}
			}
		}
		else
		{
			assert(socketState == QLocalSocket::ConnectedState);
			OT_LOG_D("Connection with subservice established");
			connectionEstablished = true;
			return connectionEstablished;
		}
	}
	return connectionEstablished;
}

ot::ReturnMessage SubprocessHandler::Send(const std::string& message)
{
	while (!_initialConnectionEstablished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutSubprocessStart));
	}
	
	std::string errorMessage;
	bool readyToWrite = SubprocessResponsive(errorMessage);
	if (!readyToWrite)
	{
		_uiComponent->displayMessage("Python Subservice not responsive. Restarting ...\n");
		OT_LOG_D("Python Subservice not responsive. Restarting ...");
		CloseSubprocess();
		RunSubprocess();
	}
	const std::string messageAsLine = message + "\n";
	_socket.write(messageAsLine.c_str());
	_socket.flush();
	bool allIsWritten = _socket.waitForBytesWritten(_timeoutSendingMessage);

	if (!SendSucceeded())
	{
		_uiComponent->displayMessage("Retrying to send message to Python Subservice\n");
		return Send(message);
	}

	const std::string returnString(_socket.readLine().data());

	ot::ReturnMessage returnMessage;
	returnMessage.fromJson(returnString);

	return returnMessage;
	
}

void SubprocessHandler::setDatabase(const std::string& url, const std::string& userName, const std::string& psw, const std::string& collectionName, const std::string& siteID)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_DataBase, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(url, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SITE_ID, ot::JsonString( siteID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(psw, doc.GetAllocator()), doc.GetAllocator());
	_initialisationRoutines.push_back(doc.toJson());
	_initialisationPrepared = _initialisationRoutines.size() == _numberOfInitialisationRoutines;
	InitialiseSubprocess();
}

void SubprocessHandler::ModelComponentWasSet()
{
	const std::string url = _modelComponent->serviceURL();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_MODEL, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(url, doc.GetAllocator()), doc.GetAllocator());
	_initialisationRoutines.push_back(doc.toJson());
	_initialisationPrepared = _initialisationRoutines.size() == _numberOfInitialisationRoutines;
	InitialiseSubprocess();
}

void SubprocessHandler::UIComponentWasSet()
{
	const std::string url = _uiComponent->serviceURL();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(url, doc.GetAllocator()), doc.GetAllocator());
	_initialisationRoutines.push_back(doc.toJson());
	_initialisationPrepared = _initialisationRoutines.size() == _numberOfInitialisationRoutines;
	InitialiseSubprocess();
}

void SubprocessHandler::SetPythonPath()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PATH_To, ot::JsonString(_pythonModulePath, doc.GetAllocator()), doc.GetAllocator());

}

bool SubprocessHandler::SubprocessResponsive(std::string& errorMessage)
{
	bool subProcessResponsive = true;

#ifndef _DEBUG
	if (_subProcess.state() != QProcess::Running)
	{
		subProcessResponsive &= !ProcessErrorOccured(errorMessage);
	}
#endif // _DEBUG

	if (!_socket.isValid())
	{
		subProcessResponsive &= !SocketErrorOccured(errorMessage);
	}
	return subProcessResponsive;
}

void SubprocessHandler::InitialiseSubprocess()
{
	std::unique_lock<std::mutex> lock (_mtx);
	if (_initialisationPrepared && _initialConnectionEstablished)
	{
		_initialisationPrepared = false;
		OT_LOG_D("Initialising Python Subservice.");
		for (std::string& routine: _initialisationRoutines)
		{
			ot::ReturnMessage returnMessage = Send(routine);
			if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Failed)
			{
				std::string errorMessage = "Failed to initialise Python subprocess, due to following error: " + returnMessage.getWhat();
				throw std::exception(errorMessage.c_str());
			}
		}
	}
}


void SubprocessHandler::CloseSubprocess()
{
	_socket.close();
	_socket.waitForDisconnected(_timeoutServerConnect);
	_subProcess.kill();
	_subProcess.waitForFinished(_timeoutSubprocessStart);
	OT_LOG_D("Closed Python Subprocess");
}


bool SubprocessHandler::SendSucceeded()
{
	while (!_socket.canReadLine())
	{
		bool receivedMessage = _socket.waitForReadyRead(_heartBeat);
		if (!receivedMessage)
		{
			std::string errorMessage;
			bool subprocessResponsive = SubprocessResponsive(errorMessage);
			if (!subprocessResponsive)
			{
				const std::string message = "Python Subservice crashed while waiting for response: " + errorMessage;
				_uiComponent->displayMessage(message);
				return false;
			}
		}
	}
	return true;
}


bool SubprocessHandler::ProcessErrorOccured(std::string& message)
{
	const auto error = _subProcess.error();
	bool errorOccured = true;
	if (error == QProcess::FailedToStart)
	{
		message = "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions or resources to invoke the program.\n";
	}
	else if (error == QProcess::Crashed)
	{
		message = "The process crashed some time after starting successfully.\n";
	}
	else if (error == QProcess::Timedout)
	{
		//The last waitFor...() function timed out.The state of QProcess is unchanged, and you can try calling waitFor...() again.
		errorOccured = false;
	}
	else if (error == QProcess::WriteError)
	{
		message = "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.\n";
	}
	else if (error == QProcess::ReadError)
	{
		message = "An error occurred when attempting to read from the process. For example, the process may not be running.\n";
	}
	else if (error == QProcess::UnknownError)
	{
		message = "An unknown error occurred.\n";
	}
	else
	{
		assert(0);
	}
	return errorOccured;
}

bool SubprocessHandler::SocketErrorOccured(std::string& message)
{
	const auto error = _socket.error();
	bool errorOccured = true;
	if (error == QLocalSocket::ConnectionRefusedError)
	{
		message = "The connection was refused by the peer(or timed out).\n";

	}
	else if (error == QLocalSocket::PeerClosedError)
	{
		message = "The remote socket closed the connection.\n";
			//Note that the client socket(i.e., this socket) will be closed after the remote close notification has been sent.";
	}
	else if (error == QLocalSocket::ServerNotFoundError)
	{
		message = "The local socket name was not found.\n";

	}
	else if (error == QLocalSocket::SocketAccessError)
	{
		message = "The socket operation failed because the application lacked the required privileges.\n";
	}
	else if (error == QLocalSocket::SocketResourceError)
	{
		message = "The local system ran out of resources(e.g., too many sockets).\n";
	}
	else if (error == QLocalSocket::SocketTimeoutError)
	{
		//The socket operation timed out.
		errorOccured = false;
	}
	else if (error == QLocalSocket::DatagramTooLargeError)
	{
		message = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).\n";
	}
	else if (error == QLocalSocket::ConnectionError)
	{
		message = "An error occurred with the connection.\n";
	}
	else if (error == QLocalSocket::UnsupportedSocketOperationError)
	{
		message = "The requested socket operation is not supported by the local operating system.\n";
	}
	else if (error == QLocalSocket::OperationError)
	{
		message = "An operation was attempted while the socket was in a state that did not permit it.\n";
	}
	else if (error == QLocalSocket::UnknownSocketError)
	{
		message = "An unidentified error occurred.\n";
	}
	else
	{
		assert(0);
	}
	return errorOccured;
}

//bool SubprocessHandler::SocketStateChanged(std::string& message)
//{
//	const auto state = _socket.state();
//	bool socketInvalid = true;
//	if (state == QLocalSocket::UnconnectedState)
//	{
//		message = "The socket is not connected.\n";
//	}
//	else if (state == QLocalSocket::ConnectingState)
//	{
//		message = "The socket has started establishing a connection.\n";
//	}
//	else if (state == QLocalSocket::ConnectedState)
//	{
//		//	A connection is established.
//		socketInvalid = false;
//	}
//	else if (state == QLocalSocket::ClosingState)
//	{
//		message = "The socket is about to close (data may still be waiting to be written).\n";
//	}
//	else
//	{
//		assert(0);
//	}
//	return socketInvalid;
//}

//bool SubprocessHandler::ProcessStateChanged(std::string& message)
//{
//	const auto state = _subProcess.state();
//	bool processInvalid = true;
//	if (state == QProcess::NotRunning)
//	{
//		message = "The process is not running.\n";
//	}
//	else if (state == QProcess::Starting)
//	{
//		message = "The process is starting, but the program has not yet been invoked.\n";
//	}
//	else if (state == QProcess::Running)
//	{
//		//The process is running and is ready for reading and writing.
//		processInvalid = false;
//	}
//	else
//	{
//		assert(0);
//	}
//	return processInvalid;
//}