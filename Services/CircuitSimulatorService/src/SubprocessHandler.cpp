#include "SubprocessHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"

SubprocessHandler::SubprocessHandler(const std::string& serverName, int sessionID, int serviceID) :m_serverName(serverName) {


	m_initialisationRoutines.reserve(m_numberOfInitialisationRoutines);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, sessionID, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, serviceID, doc.GetAllocator());
	m_initialisationRoutines.push_back(doc.toJson());

	m_subprocessPath = FindSubprocessPath() + m_executableName;
	InitiateProcess();

#ifdef _DEBUG
	m_serverName = "TestServer";
#endif
	m_server.listen(m_serverName.c_str());
#ifndef _DEBUG
	std::thread workerThread(&SubprocessHandler::RunSubprocess, this);
	workerThread.detach();
#else
	ConnectWithSubprocess();
	if (WaitForResponse())
	{
		std::string response = m_socket->readLine().data();
		ot::ReturnMessage msg;
		msg.fromJson(response);
		assert(msg.getStatus() == ot::ReturnMessage::Ok);
		m_startupChecked = true;
		OT_LOG_D("Circuit Subprocess signalized readiness");
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
	QStringList arguments{ QString(m_serverName.c_str()) };
	m_subProcess.setArguments(arguments);

	//Turning off the default communication channels of the process. Communication will be handled via QLocalServer and QLocalSocket
	m_subProcess.setStandardOutputFile(QProcess::nullDevice());
	m_subProcess.setStandardErrorFile(QProcess::nullDevice());
	m_subProcess.setProgram(m_subprocessPath.c_str());
}


void SubprocessHandler::RunSubprocess()
{
	try
	{
		const bool startSuccessfull = StartProcess();
		if (!startSuccessfull)
		{
			std::string message = "Starting circuit Subprocess timeout.";
			OT_LOG_E(message);
			throw std::exception(message.c_str());
		}
		OT_LOG_D("Circuit Subprocess started");

		ConnectWithSubprocess();
		if (WaitForResponse())
		{
			std::string response = m_socket->readLine().data();
			m_startupChecked = true;
			InitialiseSubprocess();
		}
		else
		{
			throw std::exception("Failed in waiting for circuit subprocess startup");
		}
	}
	catch (std::exception& e)
	{
		const std::string exceptionMessage("Starting the circuit subprocess failed due to: " + std::string(e.what()) + ". Shutting down.");
		OT_LOG_E(exceptionMessage);
		exit(0);
	}
}


bool SubprocessHandler::StartProcess()
{
	bool processStarted = false;
	for (int i = 1; i <= m_numberOfRetries; i++)
	{
		OT_LOG_D("Starting the subprocess: " + m_subProcess.program().toStdString() + " trial: " + std::to_string(i) + "/" + std::to_string(m_numberOfRetries));
		m_subProcess.start();
		auto state = m_subProcess.state();
		if (state == QProcess::NotRunning)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(m_timeoutSubprocessStart));
		}
		else if (state == QProcess::Starting)
		{
			processStarted = m_subProcess.waitForStarted(m_timeoutSubprocessStart);
			if (processStarted)
			{
				return true;
			}
			else if (m_subProcess.error() != QProcess::Timedout)
			{
				std::string errorMessage;
				ProcessErrorOccured(errorMessage);
				errorMessage = "Error occured while starting Circuit Subservice: " + errorMessage;
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
	OT_LOG_D("Waiting for connection servername: " + m_serverName);
	bool connected = m_server.waitForNewConnection(m_timeoutServerConnect);
	if (connected)
	{
		m_socket = m_server.nextPendingConnection();
		m_socket->waitForConnected(m_timeoutServerConnect);
		connected = m_socket->state() == QLocalSocket::ConnectedState;
		if (connected)
		{
			OT_LOG_D("Connection with subservice established");
		}
		else
		{
			std::string errorMessage;
			SocketErrorOccured(errorMessage);
			errorMessage = "Error occured while connecting with Circuit Subservice: " + errorMessage;
			throw std::exception(errorMessage.c_str());
		}
	}
	else
	{
		throw std::exception("Timout while waiting for subprocess to connect with server.");
	}
}


std::string SubprocessHandler::Send(const std::string& message)
{
	while (!m_startupChecked)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(m_timeoutSubprocessStart));
	}

	std::string errorMessage;
	bool readyToWrite = SubprocessResponsive(errorMessage);
	if (!readyToWrite)
	{
		_uiComponent->displayMessage("Circuit Subservice not responsive. Restarting ...\n");
		OT_LOG_E("Circuit Subservice not responsive. Due to error: " + errorMessage);

		CloseSubprocess();
		RunSubprocess();
	}
	const std::string messageAsLine = message + "\n";
	m_socket->write(messageAsLine.c_str());
	m_socket->flush();
	bool allIsWritten = m_socket->waitForBytesWritten(m_timeoutSendingMessage);

	if (!WaitForResponse())
	{
		_uiComponent->displayMessage("Retrying to send message to Circuit Subservice\n");
		return Send(message);
	}

	const std::string returnString(m_socket->readLine().data());

	return returnString;
}

bool SubprocessHandler::SubprocessResponsive(std::string& errorMessage)
{
	bool subProcessResponsive = true;

#ifndef _DEBUG
	if (m_subProcess.state() != QProcess::Running)
	{
		OT_LOG_E("Sending message failed. Process state: " + m_subProcess.state());
		subProcessResponsive = false;
		ProcessErrorOccured(errorMessage);
	}
#endif // _DEBUG

	if (!m_socket->isValid())
	{
		OT_LOG_E("Sending message failed. Socket state: " + m_socket->state());
		subProcessResponsive = false;
		SocketErrorOccured(errorMessage);
	}
	return subProcessResponsive;
}

void SubprocessHandler::InitialiseSubprocess()
{
	std::unique_lock<std::mutex> lock(_mtx);
	if (m_initialisationPrepared && m_startupChecked)
	{
		m_initialisationPrepared = false;
		OT_LOG_D("Initialising Circuit Subservice.");
		for (std::string& routine : m_initialisationRoutines)
		{
			ot::ReturnMessage returnMessage;
			returnMessage.fromJson(Send(routine));
			if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Failed)
			{
				OT_LOG_E("Failed to initialise Circuit subprocess");
				std::string errorMessage = "Failed to initialise Circuit subprocess, due to following error: " + returnMessage.getWhat();
				throw std::exception(errorMessage.c_str());
			}
		}
	}
}

void SubprocessHandler::CloseSubprocess()
{
	m_socket->close();
	m_socket->waitForDisconnected(m_timeoutServerConnect);
	m_subProcess.waitForFinished(m_timeoutSubprocessStart);
	OT_LOG_D("Closed Circuit Subprocess");
}


bool SubprocessHandler::WaitForResponse()
{
	while (!m_socket->canReadLine())
	{
		bool receivedMessage = m_socket->waitForReadyRead(m_heartBeat);
		if (!receivedMessage)
		{
			std::string errorMessage;
			bool subprocessResponsive = SubprocessResponsive(errorMessage);
			if (!subprocessResponsive)
			{
				const std::string message = "Circuit Subservice crashed while waiting for response: " + errorMessage;
				_uiComponent->displayMessage(message);
				return false;
			}
		}
	}
	return true;
}


void SubprocessHandler::ProcessErrorOccured(std::string& message)
{
	message = m_subProcess.errorString().toStdString();
}

void SubprocessHandler::SocketErrorOccured(std::string& message)
{
	message = m_socket->errorString().toStdString();
}