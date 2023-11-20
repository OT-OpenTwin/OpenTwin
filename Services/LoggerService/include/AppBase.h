#pragma once

#include "OTCore/rJSON.h"
#include "OTCore/Logger.h"

#include <mutex>
#include <string>
#include <list>

class AppBase {
public:
	static AppBase& instance(void);

	std::string dispatchAction(const std::string& _action, OT_rJSON_doc& _jsonDocument);

private:
	std::string handleGetDebugInfo(void);
	std::string handleClear(void);
	std::string handleLog(OT_rJSON_doc& _jsonDocument);
	std::string handleRegister(OT_rJSON_doc& _jsonDocument);
	std::string handleDeregister(OT_rJSON_doc& _jsonDocument);
	void notifyListeners(const ot::LogMessage& _message);

	void workerNotify(std::list<std::string> _receiver, std::string _message);

	void removeReceiver(const std::string& _receiver);

	std::list<ot::LogMessage>	m_messages;
	unsigned int				m_count;

	std::list<std::string>		m_receiver;
	std::mutex					m_receiverMutex;

	AppBase();
	virtual ~AppBase();
};