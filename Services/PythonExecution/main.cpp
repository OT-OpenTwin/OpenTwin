#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>

#include "OTCore/Logger.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "OTCommunication/ActionTypes.h"
#include "Application.h"
#include "DataBase.h"

#undef slots
#include "PythonAPI.h"
#define slots Q_SLOTS

QLocalSocket* _socket = nullptr;
PythonAPI* _pythonAPI = nullptr;
int _socketConnectionTimeout = 60000; //30 seconds


ot::ReturnMessage HandleMessage(ot::JsonDocument& message)
{
	const std::string action = ot::json::getString(message, OT_ACTION_MEMBER);
	if (action == OT_ACTION_CMD_Init)
	{
		const std::string serviceName = ot::json::getString(message, OT_ACTION_PARAM_SERVICE_NAME);
		if (serviceName == OT_INFO_SERVICE_TYPE_DataBase)
		{
			OT_LOG_D("Initialise Database Connection");
			const std::string url = ot::json::getString(message, OT_ACTION_PARAM_SERVICE_URL);
			const std::string userName = ot::json::getString(message, OT_PARAM_AUTH_USERNAME);
			const std::string psw = ot::json::getString(message, OT_PARAM_AUTH_PASSWORD);
			const std::string collectionName = ot::json::getString(message, OT_ACTION_PARAM_COLLECTION_NAME);
			const std::string siteID = ot::json::getString(message, OT_ACTION_PARAM_SITE_ID);
			DataBase::GetDataBase()->setProjectName(collectionName);
			DataBase::GetDataBase()->InitializeConnection(url, siteID);;
			DataBase::GetDataBase()->setUserCredentials(userName, psw);
		}
		else if (serviceName == OT_INFO_SERVICE_TYPE_MODEL)
		{
			OT_LOG_D("Connecting with modelService");
			const std::string url = ot::json::getString(message, OT_ACTION_PARAM_SERVICE_URL);
			Application::instance()->setModelServiceURL(url);
		}
		else if (serviceName == OT_INFO_SERVICE_TYPE_UI)
		{
			OT_LOG_D("Connecting with uiService");
			const std::string url = ot::json::getString(message, OT_ACTION_PARAM_SERVICE_URL);
			Application::instance()->setUIServiceURL(url);
		}
		else if (serviceName == OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE)
		{
			OT_LOG_D("Connecting with uiService");
			const std::string pythonPath = ot::json::getString(message, OT_ACTION_PARAM_PATH_To);
		}
		else
		{
			OT_LOG_D("Not supported initialisation routine.");
			assert(0);
		}
		return ot::ReturnMessage();
	}
	else if (action == OT_ACTION_CMD_Run)
	{
		OT_LOG_D("Python Subservice running task.");
		return ot::ReturnMessage();
	}
	else
	{
		OT_LOG_D("Not supported action.");
		assert(0);
	}
}

void Send(const std::string& message)
{
	_socket->write(message.c_str());
	_socket->flush();
	bool allIsWritten = _socket->waitForBytesWritten(-1);
}

void MessageReceived()
{
	if(_socket->canReadLine())
	{
		const std::string message(_socket->readLine().data());

		ot::JsonDocument document;
		document.fromJson(message);
		ot::ReturnMessage returnMessage = HandleMessage(document);
		Send(returnMessage.toJson() + "\n");

	}
}


void DisConnect()
{
	OT_LOG_D("Socket disconnected. Shutting down.");
	exit(0);
}


int main(int argc, char* argv[], char* envp[])
{
	QApplication a(argc, argv);
#ifdef _RELEASEDEBUG
	const std::string serverName = "TestServer";
	ot::ServiceLogNotifier::initialize("PythonSubprocess", "", true);
	
#else
	const std::string serverName = argv[1];
	ot::ServiceLogNotifier::initialize("PythonSubprocess", "", false);
#endif // _DEBUG

	QLocalServer _server;
	OT_LOG_D("Starting subservice with server: " + serverName);
	bool success = _server.listen(serverName.c_str());

	bool connectionReceived = _server.waitForNewConnection(_socketConnectionTimeout);
	if (!connectionReceived)
	{
		OT_LOG_E("Timeout while waiting for socket connection. Shutting down.");
		exit(0);
	}
	_socket = _server.nextPendingConnection();
	_socket->waitForConnected();
	OT_LOG_D("Connected with socket");

	QEventLoop loop;
	QObject::connect(_socket, &QLocalSocket::readyRead, &loop, &MessageReceived);
	QObject::connect(_socket, &QLocalSocket::disconnected, &loop, &DisConnect);
		
	loop.exec();
	return a.exec();
}