
#include "SubprocessHandler.h"
#include "SimulationResults.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logger.h"

#include "QtCore/qcoreapplication.h"
#include "QtCore/qtimer.h"
#include "QtCore/qdatetime.h"

SubprocessHandler::SubprocessHandler(const std::string& serverName, int sessionID, int serviceID) :m_serverName(serverName) ,m_isHealthy(false) {

	

	m_subprocessPath = FindSubprocessPath() + m_executableName;

	InitiateProcess();


	std::thread workerThread(&SubprocessHandler::RunSubprocess, this);
	workerThread.detach();

}

SubprocessHandler::~SubprocessHandler() {
	stopSubprocess();
}


std::string SubprocessHandler::FindSubprocessPath()
{
#ifdef _DEBUG
	const std::string otRootFolder = ot::OperatingSystem::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
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
		const bool startSuccessfull = startSubprocess();
		if (!startSuccessfull)
		{
			std::string message = "Starting Circuit Subprocess timeout.";
			OT_LOG_E(message);
			throw std::exception(message.c_str());
		}
		OT_LOG_D("Circuit Subprocess started");
		
		

	}
	catch (std::exception& e)
	{
		const std::string exceptionMessage("Starting the circuit subprocess failed due to: " + std::string(e.what()) + ". Shutting down.");
		OT_LOG_E(exceptionMessage);
		exit(0);
	}
}

bool SubprocessHandler::startSubprocess() {
	bool processStarted = false;
	for (int i = 1; i <= m_numberOfRetries; i++)
	{
		OT_LOG_D("Starting the subprocess: " + m_subProcess.program().toStdString() + " trial: " + std::to_string(i) + "/" + std::to_string(m_numberOfRetries));
		m_subProcess.start();

		//Time of process started
		QDateTime finishedStartTime = QDateTime::currentDateTime();
		SimulationResults::getInstance()->handleCircuitExecutionTiming(finishedStartTime, "finishedInitTime");
		

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

void SubprocessHandler::stopSubprocess() {
	if (m_subProcess.state() == QProcess::Running) {
		//OT_LOG_D("Stopping CircuitExecution!");
		//m_subProcess.terminate();

		//m_subProcess.waitForFinished(5000);
		OT_LOG_D("CircuitExecution not responding. Killing Process.");
		m_subProcess.kill();
		m_subProcess.waitForFinished();
		
	}

	m_isHealthy = false;
	OT_LOG_D("Stopped CircuitExecution");

}

bool SubprocessHandler::isSubprocessHealthy() {
	return m_isHealthy && (m_subProcess.state() == QProcess::Running);
}

void SubprocessHandler::restartSubprocess() {
	stopSubprocess();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	startSubprocess();
}



void SubprocessHandler::ProcessErrorOccured(std::string& message)
{
	message = m_subProcess.errorString().toStdString();
}
