#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"


#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qprocess.h>
#include <QtCore/qdebug.h>
#include "QtCore/qobject.h"

#include <atomic>
#include <chrono>



class SubprocessHandler : public BusinessLogicHandler
{
	
public:
	SubprocessHandler(const std::string& serverName, int sessionID, int serviceID);
	~SubprocessHandler();


	bool RunSubprocess();
	bool startSubprocess();
	void stopSubprocess();

	

private:
	
	const std::string m_executableName = "CircuitExecution.exe";
	std::string m_subprocessPath;
	
	
	std::atomic_bool m_isHealthy;
	std::atomic_bool m_initialisationPrepared = false;

	QProcess m_subProcess;
	std::string m_serverName;
	std::string FindSubprocessPath();
	void InitiateProcess();

	const int m_numberOfRetries = 3;
	const int m_timeoutSubprocessStart = 50000;//5 seconds

	const int m_heartBeat = m_timeoutSubprocessStart;
	
	void ProcessErrorOccured(std::string& message);



};