#include "SubprocessHandler.h"

#include "OTSystem/Application.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/TypeNames.h"
#include "OTCore/Logger.h"

#include <assert.h>



SubprocessHandler::SubprocessHandler(const std::string& urlThisService)
	:_urlThisProcess(urlThisService)
{
	std::string baseDirectory = ot::os::getExecutablePath();
	
	if (baseDirectory == "")
	{
		throw std::exception("Failed to determine executable path.");
	}

	_launcherPath = baseDirectory + "\\open_twin.exe";
	_subprocessPath = baseDirectory + "\\PythonExecution.dll";
	
	OT_rJSON_createDOC(pingDoc);
	ot::rJSON::add(pingDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
	_pingCommand = ot::rJSON::toJSON(pingDoc);
}

SubprocessHandler::~SubprocessHandler()
{
	Close();
}

std::string SubprocessHandler::SendExecutionOrder(OT_rJSON_doc& scriptsAndParameter)
{
	if (/*CheckAlive(_subprocess) && */PingSubprocess())
	{
		ot::rJSON::add(scriptsAndParameter, OT_ACTION_PARAM_MODEL_ActionName, OT_ACTION_CMD_PYTHON_EXECUTE);
		ot::rJSON::add(scriptsAndParameter, OT_ACTION_PARAM_SENDER_URL, _urlThisProcess);
		
		std::string response;
		std::string messageBody =  ot::rJSON::toJSON(scriptsAndParameter);
		if (!ot::msg::send("", _urlSubprocess, ot::EXECUTE, messageBody, response, 0))
		{
			OT_LOG_D("Failed to execute python script");
			assert(0); //What now?
		}
		OT_LOG_D("Python script successfully executed");
		return response;
	}
	else
	{
		OT_LOG_D("Subprocess not responding");
		Close();
		RunWithNextFreeURL(_urlThisProcess);
		ot::ReturnMessage message(ot::ReturnMessage::Failed, "Process not reachable."); //ToDo: Maybe better another trial. With max of 3 trials?
		return message.toJson();
	}
}

void SubprocessHandler::Create(const std::string& urlThisProcess)
{
	try
	{
		assert(_launcherPath != "" && _subprocessPath != "");

		if (CheckAlive(_subprocess))
		{
			if (PingSubprocess())
			{
				Close();
			}
		}

		RunWithNextFreeURL(urlThisProcess);
	}
	catch (std::exception& e)
	{
		OT_LOG_E(e.what());
		throw e;
	}
}

void SubprocessHandler::RunWithNextFreeURL(const std::string& urlThisService)
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);

	int counter = 0;
	OT_LOG_D("Trying to start subprocess with launcher path:" + _launcherPath);
	OT_LOG_D("Trying to start subprocess with subprocess path:" + _subprocessPath);

	while(true)
	{
		std::string urlSubprocess = urlThisService.substr(0, urlThisService.find(':') + 1) + std::to_string(_startPort + counter);
		std::string commandLine = _launcherPath + " \"" + _subprocessPath + "\" \"" + urlSubprocess + "\" \"" + urlThisService+ "\" \"unused\" \"unused\" ";
		
		ot::app::RunResult result = ot::app::GeneralError;
		OT_LOG_D("Trying to launch subprocess as " + urlSubprocess);
		result = ot::app::runApplication(_launcherPath, commandLine, _subprocess, false, 0);

		assert(result == ot::app::OK); //ToDo: When would this case occure?
		
		if (CheckAlive(_subprocess))
		{
			auto now = std::chrono::system_clock::now();
			bool receivedNotification = _waitForInitializationRequest.wait_until(lock, now + _processCreationTimeOut, [this]() {return this->getReceivedInitializationRequest(); });
			if (receivedNotification)
			{
				_urlSubprocess = urlSubprocess;
				OT_LOG_D("Subprocess started successfully");
				break;
			}
		}

		CloseProcess(urlSubprocess);
		counter++;
	}
}


bool SubprocessHandler::CheckAlive(OT_PROCESS_HANDLE& handle)
{
#if defined(OT_OS_WINDOWS)
	// Checking the exit code of the service
	if (handle == OT_INVALID_PROCESS_HANDLE) return false;
	
	DWORD exitCode = STILL_ACTIVE;
	
	if (GetExitCodeProcess(handle, &exitCode))
	{
		if (exitCode != STILL_ACTIVE) {
			CloseHandle(handle);
			handle = OT_INVALID_PROCESS_HANDLE;
			return false;
		}
		else {
			return true;
		}
	}
	else {

		CloseHandle(handle);
		handle = OT_INVALID_PROCESS_HANDLE;
		return false;
	}
#else
	OT_LOG_D("Failed to detect Windows os")
	#error ("Function is implemented only for Windows OS");
#endif // OT_OS_WINDOWS

}

bool SubprocessHandler::PingSubprocess()
{
	for (int pingAttempt = 0; pingAttempt < _maxPingAttempts; pingAttempt++)
	{
		std::string response;
		if (ot::msg::send("", _urlSubprocess, ot::EXECUTE, _pingCommand, response, 1000))
		{
			return true;
		}
	}
	return false;
}

bool SubprocessHandler::CloseProcess(const std::string& url)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceShutdown);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, _urlThisProcess);
	std::string response;
	ot::msg::send("", url, ot::EXECUTE, ot::rJSON::toJSON(doc), response, 0);
	
	CloseHandle(_subprocess);
	_subprocess = OT_INVALID_PROCESS_HANDLE;

	return true;
}

void SubprocessHandler::setReceivedInitializationRequest()
{
	_receivedInitializationRequest = true;
	_waitForInitializationRequest.notify_all();
}

bool SubprocessHandler::Close()
{
	return CloseProcess(_urlSubprocess);
}
