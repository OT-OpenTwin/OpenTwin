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
	_serverName = "TestServer";
#endif
	_server.listen(_serverName.c_str());
#ifndef _DEBUG
	std::thread workerThread(&SubprocessHandler::RunSubprocess, this);
	workerThread.detach();
#else
	ConnectWithSubprocess();
	if (WaitForResponse())
	{
		std::string response = _socket->readLine().data();
		assert(response.substr(0,response.size()-1) == OT_ACTION_CMD_CheckStartupCompleted);
		_startupChecked = true;
		InitialiseSubprocess();
	}
	else
	{
		assert(0);
	}
#endif
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

		ConnectWithSubprocess();
		if (WaitForResponse())
		{
			std::string response = _socket->readLine().data();
			_startupChecked = true;
			InitialiseSubprocess();
		}
		else
		{
			throw std::exception("Failed in waiting for python subprocess startup");
		}
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
			else if(_subProcess.error() != QProcess::Timedout)
			{
				std::string errorMessage;
				ProcessErrorOccured(errorMessage);
				errorMessage = "Error occured while starting Python Subservice: " + errorMessage;
				throw std::exception(errorMessage.c_str());
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

void SubprocessHandler::ConnectWithSubprocess()
{
	OT_LOG_D("Waiting for connection servername: " + _serverName);
	bool connected = _server.waitForNewConnection(_timeoutServerConnect);
	if (connected)
	{
		_socket = _server.nextPendingConnection();
		_socket->waitForConnected(_timeoutServerConnect);
		connected = _socket->state() == QLocalSocket::ConnectedState;
		if (connected)
		{
			OT_LOG_D("Connection with subservice established");
		}
		else
		{
			std::string errorMessage;
			SocketErrorOccured(errorMessage);
			errorMessage = "Error occured while connecting with Python Subservice: " + errorMessage;
			throw std::exception(errorMessage.c_str());
		}
	}
	else
	{
		throw std::exception("Timout while waiting for subprocess to connect with server.");
	}
}

ot::ReturnMessage SubprocessHandler::Send(const std::string& message)
{
	while (!_startupChecked)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(_timeoutSubprocessStart));
	}
	
	std::string errorMessage;
	bool readyToWrite = SubprocessResponsive(errorMessage);
	if (!readyToWrite)
	{
		_uiComponent->displayMessage("Python Subservice not responsive. Restarting ...\n");
		OT_LOG_E("Python Subservice not responsive. Due to error: " + errorMessage);

		CloseSubprocess();
		RunSubprocess();
	}
	const std::string messageAsLine = message + "\n";
	_socket->write(messageAsLine.c_str());
	_socket->flush();
	bool allIsWritten = _socket->waitForBytesWritten(_timeoutSendingMessage);

	if (!WaitForResponse())
	{
		_uiComponent->displayMessage("Retrying to send message to Python Subservice\n");
		return Send(message);
	}

	const std::string returnString(_socket->readLine().data());
	ot::ReturnMessage returnMessage;
	returnMessage.fromJson(returnString);

	return returnMessage;	
}

void SubprocessHandler::setDatabase(const std::string& url, const std::string& userName, const std::string& psw, const std::string& collectionName, const std::string& siteID, int sessionID, int serviceID)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_DataBase, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(url, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SITE_ID, ot::JsonString( siteID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(psw, doc.GetAllocator()), doc.GetAllocator());
	
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, sessionID,doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, serviceID, doc.GetAllocator());
	_initialisationRoutines.push_back(doc.toJson());
	_initialisationPrepared = _initialisationRoutines.size() == _numberOfInitialisationRoutines;
	InitialiseSubprocess();
}

void SubprocessHandler::ModelComponentWasSet()
{
	const std::string url = _modelComponent->serviceURL();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
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
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(url, doc.GetAllocator()), doc.GetAllocator());
	_initialisationRoutines.push_back(doc.toJson());
	_initialisationPrepared = _initialisationRoutines.size() == _numberOfInitialisationRoutines;
	InitialiseSubprocess();
}

void SubprocessHandler::SetPythonPath()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PATH_To, ot::JsonString(_pythonModulePath, doc.GetAllocator()), doc.GetAllocator());

}

bool SubprocessHandler::SubprocessResponsive(std::string& errorMessage)
{
	bool subProcessResponsive = true;

#ifndef _DEBUG
	if (_subProcess.state() != QProcess::Running)
	{
		OT_LOG_E("Sending message failed. Process state: " + _subProcess.state());
		subProcessResponsive = false;
		ProcessErrorOccured(errorMessage);
	}
#endif // _DEBUG

	if (!_socket->isValid())
	{
		OT_LOG_E("Sending message failed. Socket state: " + _socket->state());
		subProcessResponsive = false;
		SocketErrorOccured(errorMessage);
	}
	return subProcessResponsive;
}

void SubprocessHandler::InitialiseSubprocess()
{
	std::unique_lock<std::mutex> lock (_mtx);
	if (_initialisationPrepared && _startupChecked)
	{
		_initialisationPrepared = false;
		OT_LOG_D("Initialising Python Subservice.");
		for (std::string& routine: _initialisationRoutines)
		{
			ot::ReturnMessage returnMessage = Send(routine);
			if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Failed)
			{
				OT_LOG_E("Failed to initialise Python subprocess");
				std::string errorMessage = "Failed to initialise Python subprocess, due to following error: " + returnMessage.getWhat();
				throw std::exception(errorMessage.c_str());
			}
		}
	}
}


void SubprocessHandler::CloseSubprocess()
{
	_socket->close();
	_socket->waitForDisconnected(_timeoutServerConnect);
	_subProcess.waitForFinished(_timeoutSubprocessStart);
	OT_LOG_D("Closed Python Subprocess");
}


bool SubprocessHandler::WaitForResponse()
{
	while (!_socket->canReadLine())
	{
		bool receivedMessage = _socket->waitForReadyRead(_heartBeat);
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


void SubprocessHandler::ProcessErrorOccured(std::string& message)
{
	message = _subProcess.errorString().toStdString();
}

void SubprocessHandler::SocketErrorOccured(std::string& message)
{
	message = _socket->errorString().toStdString();
}