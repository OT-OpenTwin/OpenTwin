#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"
#include "ConnectionManager.h"

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


	void RunSubprocess();
	bool startSubprocess();
	void stopSubprocess();
	bool isSubprocessHealthy();
	void restartSubprocess();

	

private:
	
	const std::string m_executableName = "CircuitExecution.exe";
	std::string m_subprocessPath;
	
	
	std::atomic_bool m_isHealthy;
	std::atomic_bool m_initialisationPrepared = false;

	ConnectionManager *m_connectionManager;
	QProcess m_subProcess;
	QLocalServer m_server;
	std::string m_serverName;


	std::string FindSubprocessPath();
	void InitiateProcess();

	const int m_numberOfRetries = 3;
	const int m_timeoutSubprocessStart = 50000;//5 seconds

	const int m_heartBeat = m_timeoutSubprocessStart;
	
	void ProcessErrorOccured(std::string& message);



};