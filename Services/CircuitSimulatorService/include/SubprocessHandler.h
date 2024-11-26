#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qprocess.h>
#include <QtCore/qdebug.h>


#include <atomic>
#include <chrono>

class SubprocessHandler : public BusinessLogicHandler
{
public:
	SubprocessHandler(const std::string& serverName, int sessionID, int serviceID);
	~SubprocessHandler();

	bool startSubprocess();
	void stopSubprocess();
	bool isSubprocessHealthy();
	void restartSubprocess();


private:
	std::string m_serverName;
	const std::string m_executableName = "CircuitExecution.exe";
	std::string m_subprocessPath;
	QProcess m_subProcess;
	QLocalServer m_server;
	std::atomic_bool m_isHealthy;

	std::vector<std::string> _initialisationRoutines;
	const int _numberOfInitialisationRoutines = 4;

	std::string FindSubprocessPath();
	void InitiateProcess();

	const int m_numberOfRetries = 3;
	const int m_timeoutSubprocessStart = 50000;//5 seconds
	const int m_timeoutServerConnect = m_timeoutSubprocessStart;
	const int m_timeoutSendingMessage = m_timeoutSubprocessStart;
	const int m_heartBeat = m_timeoutSubprocessStart;
	
	void ProcessErrorOccured(std::string& message);



};