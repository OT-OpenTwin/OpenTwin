#include "AppBase.h"

#include "OTCore/rJSONHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

#include <thread>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#define OT_LOG_BUFFER_LIMIT 10000

AppBase& AppBase::instance(void) {
	static AppBase g_instance;
	return g_instance;
}

std::string AppBase::dispatchAction(const std::string& _action, OT_rJSON_doc& _jsonDocument) {
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
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, "MessageCount", m_count);

	OT_rJSON_createValueArray(receiverArr);
	m_receiverMutex.lock();
	for (auto r : m_receiver) {
		receiverArr.PushBack(rapidjson::Value(r.c_str(), doc.GetAllocator()), doc.GetAllocator());
	}
	m_receiverMutex.unlock();
	ot::rJSON::add(doc, "Receivers", receiverArr);

	return ot::rJSON::toJSON(doc);
}

std::string AppBase::handleClear(void) {
	m_messages.clear();
	m_count = 0;
	m_receiver.clear();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string AppBase::handleLog(OT_rJSON_doc& _jsonDocument) {
	OT_rJSON_checkMember(_jsonDocument, OT_ACTION_PARAM_LOG, Object);
	OT_rJSON_val logObject = _jsonDocument[OT_ACTION_PARAM_LOG].GetObject();

	ot::LogMessage msg;
	msg.setFromJsonObject(logObject);

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

std::string AppBase::handleRegister(OT_rJSON_doc& _jsonDocument) {
	std::string receiverUrl = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);

	m_receiverMutex.lock();
	m_receiver.push_back(receiverUrl);
#ifdef _DEBUG
	std::cout << "New Receiver: " << receiverUrl << std::endl;
#endif // _DEBUG

	m_receiverMutex.unlock();

	OT_rJSON_doc doc;
	doc.SetArray();
	for (auto msg : m_messages) {
		OT_rJSON_createValueObject(msgObj);
		msg.addToJsonObject(doc, msgObj);
		doc.PushBack(msgObj, doc.GetAllocator());
	}
	return ot::rJSON::toJSON(doc);
}

std::string AppBase::handleDeregister(OT_rJSON_doc& _jsonDocument) {
	std::string receiverUrl = ot::rJSON::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);
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
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_Log);

	OT_rJSON_createValueObject(logObj);
	_message.addToJsonObject(doc, logObj);
	ot::rJSON::add(doc, OT_ACTION_PARAM_LOG, logObj);

	std::string msg = ot::rJSON::toJSON(doc);

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