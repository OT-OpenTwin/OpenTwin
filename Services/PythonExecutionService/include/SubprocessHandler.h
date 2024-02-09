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
	SubprocessHandler(const std::string& serverName);
	~SubprocessHandler();
	ot::ReturnMessage Send(const std::string& message);
	
	void setDatabase(const std::string& url, const std::string& userName, const std::string& psw, const std::string& collectionName, const std::string& siteID, int sessionID, int serviceID);

private:
	std::string _serverName;
	const std::string _executableName = "PythonExecution.exe";
	std::string _subprocessPath;
	std::string _pythonModulePath;

	std::vector<std::string> _initialisationRoutines;
	const int _numberOfInitialisationRoutines = 3;
	
	std::atomic_bool _startupChecked = false;
	std::atomic_bool _initialisationPrepared = false;

	std::mutex _mtx;

	const int _numberOfRetries = 3;
	const int _timeoutSubprocessStart = 50000;//5 seconds
	const int _timeoutServerConnect = _timeoutSubprocessStart;
	const int _timeoutSendingMessage = _timeoutSubprocessStart;
	const int _heartBeat = _timeoutSubprocessStart;

	QProcess _subProcess;
	QLocalServer _server;
	QLocalSocket* _socket = nullptr;
	
	void ModelComponentWasSet() override;
	void UIComponentWasSet() override;
	void SetPythonPath();

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
