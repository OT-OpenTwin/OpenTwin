#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>
#include <QtCore/qprocess.h>


class SubprocessHandler : public BusinessLogicHandler, public QEventLoop
{
public:
	SubprocessHandler(const std::string& serverName, int argc, char* argv[]);

	bool Send(const std::string& message);
		
private:
	const std::string _serverName;
	const std::string _executableName = "PythonExecution.exe";
	std::string _subprocessPath;

	const int _timeoutSubprocessStart = 5000;//5 seconds
	const int _timeoutServerConnect = _timeoutSubprocessStart;
	const int _timeoutSendingMessage = _timeoutSubprocessStart;

	QCoreApplication _qtApplication;
	//QEventLoop _eventLoop;
	QProcess _subProcess;
	QLocalSocket _socket;

	std::string FindSubprocessPath();
	void InitiateProcess();
	void StartProcess();
	void ConnectWithSubService();


	void ProcessErrorOccured();
	void ProcessStateChanged();
	void SocketErrorOccured();
	void SocketStateChanged();

	void SocketRecievedMessage();
	void SocketDisconnected();

};
