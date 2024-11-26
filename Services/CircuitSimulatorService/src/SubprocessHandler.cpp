
#include "SubprocessHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTSystem/OperatingSystem.h"





SubprocessHandler::SubprocessHandler(const std::string& serverName, int sessionID, int serviceID) :m_serverName(serverName) ,m_isHealthy(false) {
	_initialisationRoutines.reserve(_numberOfInitialisationRoutines);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_CircuitSimulatorService, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, sessionID, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, serviceID, doc.GetAllocator());
	_initialisationRoutines.push_back(doc.toJson());

	m_subprocessPath = FindSubprocessPath() + m_executableName;

	InitiateProcess();

	m_server.listen(serverName.c_str());

	std::thread workerThread(&SubprocessHandler::startSubprocess, this);
	workerThread.detach();
}

SubprocessHandler::~SubprocessHandler() {
	stopSubprocess();
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

bool SubprocessHandler::startSubprocess() {
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

void SubprocessHandler::stopSubprocess() {
	if (m_subProcess.state() == QProcess::Running) {
		OT_LOG_D("Stopping CircuitExecution!");
		m_subProcess.terminate();

		if (!m_subProcess.waitForFinished(5000)) { // Timeout: 5 Sekunden
			OT_LOG_D("CircuitExecution not responding. Killing Process.");
			m_subProcess.kill();
			m_subProcess.waitForFinished();
		}
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
