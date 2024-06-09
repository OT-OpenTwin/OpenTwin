#include "AppBase.h"

#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#include <thread>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#define OT_LOG_BUFFER_LIMIT 10000

namespace intern {

	void replacePasswords(std::string& _string) {
		
		size_t passwordIx = _string.find(OT_ACTION_PASSWORD_SUBTEXT);
		size_t lastIx = 0; // Last index of previous match

		while (passwordIx != std::string::npos) {
			// Ensure the password is inside double quotes
			size_t checkIx = _string.find('\"', lastIx);
			if (checkIx > passwordIx) break;


			// Closing quote
			checkIx = _string.find('\"', passwordIx + 1);
			if (checkIx == std::string::npos) break;

			// Colon
			checkIx = _string.find(':', checkIx + 1);
			if (checkIx == std::string::npos) break;

			// Value opening quote
			checkIx = _string.find('\"', checkIx + 1);
			if (checkIx == std::string::npos) break;

			// Value closing quote
			lastIx = _string.find('\"', checkIx + 1);
			if (lastIx == std::string::npos) break;

			_string.replace(checkIx + 1, (lastIx - checkIx) - 1, "****");
			
			lastIx = _string.find('\"', checkIx + 1) + 1; // Last index is now invalid after replacing
			passwordIx = _string.find(OT_ACTION_PASSWORD_SUBTEXT, lastIx);
		}
	}
}

AppBase& AppBase::instance(void) {
	static AppBase g_instance;
	return g_instance;
}

std::string AppBase::dispatchAction(const std::string& _action, ot::JsonDocument& _jsonDocument) {
	if (_action == OT_ACTION_CMD_Log) return handleLog(_jsonDocument);
	else if (_action == OT_ACTION_CMD_Ping) return OT_ACTION_CMD_Ping;
	else if (_action == OT_ACTION_CMD_RegisterNewService) return handleRegister(_jsonDocument);
	else if (_action == OT_ACTION_CMD_RemoveService) return handleDeregister(_jsonDocument);
	else if (_action == OT_ACTION_CMD_Reset) return handleClear();
	else if (_action == OT_ACTION_CMD_GetDebugInformation) return handleGetDebugInfo();
	else if (_action == OT_ACTION_CMD_ServiceShutdown) exit(0);
	else if (_action == OT_ACTION_CMD_ServiceEmergencyShutdown) exit(-100);
	else return OT_ACTION_RETURN_UnknownAction;
}

std::string AppBase::handleGetDebugInfo(void) {
	ot::JsonDocument doc;
	doc.AddMember("MessageCount", m_count, doc.GetAllocator());

	m_receiverMutex.lock();
	doc.AddMember("Receivers", ot::JsonArray(m_receiver, doc.GetAllocator()), doc.GetAllocator());
	m_receiverMutex.unlock();

	return doc.toJson();
}

std::string AppBase::handleClear(void) {
	m_messages.clear();
	m_count = 0;
	m_receiver.clear();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string AppBase::handleLog(ot::JsonDocument& _jsonDocument) {
	ot::ConstJsonObject obj = ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_LOG);

	ot::LogMessage msg;
	msg.setFromJsonObject(obj);

	if (msg.getText().find(OT_ACTION_PASSWORD_SUBTEXT) != std::string::npos) {
		std::string newText = msg.getText();
		intern::replacePasswords(newText);
		msg.setText(newText);
	}
	
	msg.setCurrentTimeAsGlobalSystemTime();

	m_messages.push_back(msg);

	if (m_count >= OT_LOG_BUFFER_LIMIT) {
		m_messages.pop_front();
	}
	else {
		m_count++;
	}

	notifyListeners(msg);

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string AppBase::handleRegister(ot::JsonDocument& _jsonDocument) {
	std::string receiverUrl = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);

	m_receiverMutex.lock();
	m_receiver.push_back(receiverUrl);
#ifdef _DEBUG
	std::cout << "New Receiver: " << receiverUrl << std::endl;
#endif // _DEBUG

	m_receiverMutex.unlock();

	ot::JsonDocument doc(rapidjson::kArrayType);
	for (auto msg : m_messages) {
		ot::JsonObject msgObj;
		msg.addToJsonObject(msgObj, doc.GetAllocator());
		doc.PushBack(msgObj, doc.GetAllocator());
	}
	return doc.toJson();
}

std::string AppBase::handleDeregister(ot::JsonDocument& _jsonDocument) {
	std::string receiverUrl = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);
	std::string ret = OT_ACTION_RETURN_VALUE_FAILED;
	m_receiverMutex.lock();
	auto it = std::find(m_receiver.begin(), m_receiver.end(), receiverUrl);
	if (it != m_receiver.end()) {
		m_receiver.erase(it);
		ret = OT_ACTION_RETURN_VALUE_OK;
#ifdef _DEBUG
		std::cout << "Removed Receiver: " << receiverUrl << std::endl;
#endif // _DEBUG
	}
	m_receiverMutex.unlock();
	return ret;
}

void AppBase::notifyListeners(const ot::LogMessage& _message) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Log, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject logObj;
	_message.addToJsonObject(logObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_LOG, logObj, doc.GetAllocator());
	
	std::string msg = doc.toJson();

	m_receiverMutex.lock();
	std::list<std::string> receiverList = m_receiver;
	m_receiverMutex.unlock();

	std::thread t(&AppBase::workerNotify, this, receiverList, msg);
	t.detach();
}

void AppBase::workerNotify(std::list<std::string> _receiver, std::string _message) {
	std::list<std::string> failed;
	for (auto r : _receiver) {
		std::string response;
		if (!ot::msg::send("", r, ot::EXECUTE, _message, response)) {
			failed.push_back(r);
		}
		else if (response != OT_ACTION_RETURN_VALUE_OK) {
			failed.push_back(r);
		}
	}
	for (auto f : failed) {
		removeReceiver(f);
	}
}

void AppBase::removeReceiver(const std::string& _receiver) {
	m_receiverMutex.lock();
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_receiver.begin(); it != m_receiver.end(); it++) {
			if (*it == _receiver) {
				m_receiver.erase(it);
				erased = true;
				break;
			}
		}
	}

#ifdef _DEBUG
	std::cout << "Receiver removed: " << _receiver << std::endl;
#endif // _DEBUG

	m_receiverMutex.unlock();
}

AppBase::AppBase() : m_count(0) {}

AppBase::~AppBase() {}