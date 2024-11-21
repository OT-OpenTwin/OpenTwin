#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qobject.h>
#include <QtCore/qprocess.h>

#include <atomic>
#include <chrono>

class SubprocessHandler : public BusinessLogicHandler
{
public:
	SubprocessHandler(const std::string& serverName, int sessionID, int serviceID);
	~SubprocessHandler();
	std::string Send(const std::string& message);


private:
	std::string m_serverName;
	const std::string m_executableName = "CircuitExecution.exe";
	std::string m_subprocessPath;

	std::vector<std::string> m_initialisationRoutines;
	const int m_numberOfInitialisationRoutines = 4;

	std::atomic_bool m_startupChecked = false;
	std::atomic_bool m_initialisationPrepared = false;

	std::mutex _mtx;


	const int m_numberOfRetries = 3;
	const int m_timeoutSubprocessStart = 50000;//5 seconds
	const int m_timeoutServerConnect = m_timeoutSubprocessStart;
	const int m_timeoutSendingMessage = m_timeoutSubprocessStart;
	const int m_heartBeat = m_timeoutSubprocessStart;

	QProcess m_subProcess;
	QLocalServer m_server;
	QLocalSocket* m_socket = nullptr;

	std::string FindSubprocessPath();
	void InitiateProcess();

	void RunSubprocess();
	bool StartProcess();
	void ConnectWithSubprocess();
	void CloseSubprocess();

	bool WaitForResponse();
	bool SubprocessResponsive(std::string& errorMessage);
	void ProcessErrorOccured(std::string& message);
	void SocketErrorOccured(std::string& message);

	void InitialiseSubprocess();
};