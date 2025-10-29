// @otlicense

#pragma once

// Service header
#include "DataBaseInfo.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <mutex>

class Application;
class SubprocessHandler;
class CommunicationHandler;

namespace std { class thread; }

class SubprocessManager {
	OT_DECL_NOCOPY(SubprocessManager)
	OT_DECL_NODEFAULT(SubprocessManager)
public:
	SubprocessManager(Application* _app);
	~SubprocessManager();
	
	void setModelUrl(const std::string& _url);
	void setFrontendUrl(const std::string& _url);
	void setDataBaseInfo(const DataBaseInfo& _info);

	//! @brief Sends the request to the Subprocess.
	//! Will start the subprocess if needed.
	bool sendRequest(const ot::JsonDocument& _document, std::string& _response);

	//! @brief Sends the request to the Subprocess.
	//! Will start the subprocess if needed.
	//! Will shutdown the subprocess after finished.
	bool sendSingleRequest(const ot::JsonDocument& _document, std::string& _response);

	bool ensureSubprocessRunning(void);

	//! @brief Stops the connection handler.
	void stopConnectionHandlingAfterCrash(void);

	bool isConnected();

private:
	void shutdownSubprocess(void);

	//! @brief Runs the subservice if needed and checks connection with a ping.
	//! @return Return false if the connection could not be established.
	bool ensureWorkerRunning(void);

	//! @brief Returns true if the worker has started successfully.
	//! Starting the working will only start the event loop and communication handler.
	bool isWorkerAlive(void);

	//! @brief Event loop worker.
	void worker(std::string _projectName);

	std::thread* m_workerThread;
	std::mutex m_mutex;

	Application* m_app;

	SubprocessHandler* m_subprocessHandler;
	CommunicationHandler* m_communicationHandler;
};