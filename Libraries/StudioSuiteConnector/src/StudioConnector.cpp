#include "StudioSuiteConnector/StudioConnector.h"

#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"

#include <cassert>

#include "quuid.h"

//#define DEBUG_PYTHON_SERVER

void StudioConnector::openProject(const std::string& fileName)
{
	// First, we need to ensure that the python subservice is running
	startSubprocess();

	// Now we need to get a list of all process ids of running CST STUDIO SUITE DESIGN ENVIRONMENTS


	// Connect to each of them and check whether the project is open there



	// Here the project is not open yet, so we create a new instance and open the project there







}

void StudioConnector::saveProject()
{

}

void StudioConnector::extractInformation()
{

}

StudioConnector::~StudioConnector()
{
	closeSubprocess();
}

void StudioConnector::startSubprocess()
{
	if (subProcessRunning)
	{
		closeSubprocess();
	}

	// Determine new unique server name
	QUuid uidGenerator = QUuid::createUuid();
	serverName = uidGenerator.toString().toStdString();

	subprocessPath = findSubprocessPath() + executableName;
	initiateProcess();

#ifdef DEBUG_PYTHON_SERVER
	serverName = "TestServer";
#endif
	server.listen(serverName.c_str());
#ifndef DEBUG_PYTHON_SERVER
	std::thread workerThread(&StudioConnector::runSubprocess, this);
	workerThread.detach();
#endif
	connectWithSubprocess();

	if (waitForResponse())
	{
		std::string response = socket->readLine().data();
		startupChecked = true;
	}
	else
	{
		assert(0);
	}

	subProcessRunning = true;
}

std::string StudioConnector::findSubprocessPath()
{
#ifdef _DEBUG
	const std::string otRootFolder = ot::os::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
	assert(otRootFolder != "");
	return otRootFolder + "\\Deployment\\";

#else
	return ".\\";

#endif // DEBUG

}

void StudioConnector::initiateProcess()
{
	QStringList arguments{ QString(serverName.c_str()) };
	subProcess.setArguments(arguments);

	//Turning off the default communication channels of the process. Communication will be handled via QLocalServer and QLocalSocket
	subProcess.setStandardOutputFile(QProcess::nullDevice());
	subProcess.setStandardErrorFile(QProcess::nullDevice());
	subProcess.setProgram(subprocessPath.c_str());
}

void StudioConnector::runSubprocess()
{
	try
	{
		const bool startSuccessfull = startProcess();
		if (!startSuccessfull)
		{
			std::string message = "Starting Python Subprocess timeout.";
			OT_LOG_E(message);
			throw std::string(message.c_str());
		}
		OT_LOG_D("Python Subprocess started");
	}
	catch (std::exception& e)
	{
		const std::string exceptionMessage("Starting the python subprocess failed due to: " + std::string(e.what()) + ". Shutting down.");
		OT_LOG_E(exceptionMessage);
		exit(0);
	}
}

bool StudioConnector::startProcess()
{
	bool processStarted = false;
	for (int i = 1; i <= numberOfRetries; i++)
	{
		OT_LOG_D("Starting the subprocess: " + subProcess.program().toStdString() + " trial: " + std::to_string(i) + "/" + std::to_string(numberOfRetries));
		subProcess.start();
		auto state = subProcess.state();
		if (state == QProcess::NotRunning)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(timeoutSubprocessStart));
		}
		else if (state == QProcess::Starting)
		{
			processStarted = subProcess.waitForStarted(timeoutSubprocessStart);
			if (processStarted)
			{
				return true;
			}
			else if (subProcess.error() != QProcess::Timedout)
			{
				std::string errorMessage;
				getProcessErrorOccured(errorMessage);
				errorMessage = "Error occured while starting Python Subservice: " + errorMessage;
				throw std::string(errorMessage.c_str());
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

void StudioConnector::connectWithSubprocess()
{
	OT_LOG_D("Waiting for connection servername: " + serverName);
	bool connected = server.waitForNewConnection(timeoutServerConnect);
	if (connected)
	{
		socket = server.nextPendingConnection();
		socket->waitForConnected(timeoutServerConnect);
		connected = socket->state() == QLocalSocket::ConnectedState;
		if (connected)
		{
			OT_LOG_D("Connection with subservice established");
		}
		else
		{
			std::string errorMessage;
			getSocketErrorOccured(errorMessage);
			errorMessage = "Error occured while connecting with Python Subservice: " + errorMessage;
			throw std::string(errorMessage.c_str());
		}
	}
	else
	{
		throw std::string("Timout while waiting for subprocess to connect with server.");
	}
}

ot::ReturnMessage StudioConnector::send(const std::string& message)
{
	while (!startupChecked)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(timeoutSubprocessStart));
	}

	std::string errorMessage;
	bool readyToWrite = checkSubprocessResponsive(errorMessage);
	if (!readyToWrite)
	{
		OT_LOG_E("Python Subservice not responsive. Due to error: " + errorMessage);

		closeSubprocess();
		runSubprocess();
	}
	const std::string messageAsLine = message + "\n";
	socket->write(messageAsLine.c_str());
	socket->flush();
	bool allIsWritten = socket->waitForBytesWritten(timeoutSendingMessage);

	if (!waitForResponse())
	{
		return send(message);
	}

	const std::string returnString(socket->readLine().data());
	ot::ReturnMessage returnMessage;
	returnMessage.fromJson(returnString);

	return returnMessage;
}

bool StudioConnector::checkSubprocessResponsive(std::string& errorMessage)
{
	bool subProcessResponsive = true;

#ifndef DEBUG_PYTHON_SERVER
	if (subProcess.state() != QProcess::Running)
	{
		OT_LOG_E("Sending message failed. Process state: " + subProcess.state());
		subProcessResponsive = false;
		getProcessErrorOccured(errorMessage);
	}
#endif // DEBUG_PYTHON_SERVER

	if (!socket->isValid())
	{
		OT_LOG_E("Sending message failed. Socket state: " + socket->state());
		subProcessResponsive = false;
		getSocketErrorOccured(errorMessage);
	}
	return subProcessResponsive;
}

void StudioConnector::closeSubprocess()
{
	socket->close();
	socket->waitForDisconnected(timeoutServerConnect);
	subProcess.waitForFinished(timeoutSubprocessStart);
	OT_LOG_D("Closed Python Subprocess");
}

bool StudioConnector::waitForResponse()
{
	while (!socket->canReadLine())
	{
		bool receivedMessage = socket->waitForReadyRead(heartBeat);
		if (!receivedMessage)
		{
			std::string errorMessage;
			bool subprocessResponsive = checkSubprocessResponsive(errorMessage);
			if (!subprocessResponsive)
			{
				const std::string message = "Python Subservice crashed while waiting for response: " + errorMessage;
//				_uiComponent->displayMessage(message);
				return false;
			}
		}
	}
	return true;
}


void StudioConnector::getProcessErrorOccured(std::string& message)
{
	message = subProcess.errorString().toStdString();
}

void StudioConnector::getSocketErrorOccured(std::string& message)
{
	message = socket->errorString().toStdString();
}
