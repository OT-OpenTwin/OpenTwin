#pragma once

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
	
	bool sendRequest(const ot::JsonDocument& _document, std::string& _response);

private:
	//! @brief Runs the subservice if needed and checks connection with a ping.
	//! @return Return false if the connection could not be established.
	bool ensureSubprocessRunning(void);

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