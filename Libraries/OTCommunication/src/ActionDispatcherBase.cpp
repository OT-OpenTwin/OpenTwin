//! @file ActionDispatcherBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcherBase.h"
#include "OTCommunication/ActionHandleConnector.h"

ot::ActionDispatcherBase::ActionDispatcherBase() {

}

ot::ActionDispatcherBase::~ActionDispatcherBase() {
	for (auto& it : m_data) {
		it.second->forgetActionDispatcher();
	}
	m_data.clear();
}

void ot::ActionDispatcherBase::add(ActionHandleConnector* _item, bool _overwrite) {
	std::lock_guard<std::mutex> mtxLock(m_mutex);

	for (const std::string& action : _item->actionNames()) {
		auto it = m_data.find(action);
		if (it == m_data.end() || _overwrite) {
			m_data.insert_or_assign(action, _item);
		}
		else {
			OT_LOG_EAS("Handler for \"" + action + "\" already exist. Ignoring");
		}
	}
}

void ot::ActionDispatcherBase::remove(ActionHandleConnector* _item) {
	OTAssertNullptr(_item);

	std::lock_guard<std::mutex> mtxLock(m_mutex);
	
	for (const std::string& action : _item->actionNames()) {
		m_data.erase(action);
	}

	_item->forgetActionDispatcher();
}

char* ot::ActionDispatcherBase::dispatchWrapper(const char* _json, const char* _senderUrl, MessageType _messageType) {
	std::string result = dispatch(_json, _messageType);

	// Return value must be copied to a memory location that will not be destroyed by the application/service
	size_t len = result.length() + 1;
	char* returnValue = new char[len];
	strcpy_s(returnValue, len, result.c_str());
	return returnValue;
}

std::string ot::ActionDispatcherBase::dispatch(const std::string& _json, MessageType _messageType) {
	// Parse the json string and check its main syntax
	JsonDocument doc;
	doc.fromJson(_json);

	return this->dispatch(doc, _messageType);
}

std::string ot::ActionDispatcherBase::dispatch(JsonDocument& _document, MessageType _messageType) {
	// Get the action and dispatch it
	std::string action = json::getString(_document, OT_ACTION_MEMBER);

	if (action.empty()) {
		OT_LOG_EA("Action member not found");
		return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Provided action is empty");
	}

	bool handlerFound = false;
	std::string result = dispatch(action, _document, handlerFound, _messageType);

	// Check if a handler was found or an error occured (non empty result);
	if (!handlerFound && result.empty()) {
		OT_LOG_WAS("Unknown action received: " + action);
		result = ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Unknown action received: \"" + action + "\"");
	}

	return result;
}

std::string ot::ActionDispatcherBase::dispatch(const std::string& _action, JsonDocument& _document, MessageType _messageType) {
	bool tmp = false;
	return dispatch(_action, _document, tmp, _messageType);
}

std::string ot::ActionDispatcherBase::dispatch(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType) {
	// Check for default actions
	if (_action == OT_ACTION_CMD_Ping) {
		_handlerFound = true;
		return OT_ACTION_CMD_Ping;
	}

	std::lock_guard<std::mutex> mtxLock(m_mutex);

	return this->dispatchImpl(_action, _document, _handlerFound, _messageType);
}

std::string ot::ActionDispatcherBase::dispatchImpl(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType) {
	std::string result;

	OT_LOG("Dispatching: \"" + _action + "\"", ot::messageTypeToLogFlag(_messageType));

	try {
		auto it = m_data.find(_action);
		_handlerFound = false;
		if (it != m_data.end()) {
			if (it->second->mayDispatch(_messageType)) {
				result = it->second->forwardDispatch(_document);
				_handlerFound = true;
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		result = ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, _e.what());
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error occured");
		result = ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Unknown error occured");
	}

	if (_handlerFound) {
		OT_LOG("..... Completed: Dispatching: \"" + _action + "\". Returning: \"" + result + "\".", ot::messageTypeToLogFlag(_messageType));
	}
	else {
		OT_LOG("..... Failed: Dispatching \"" + _action + "\" failed: No handler found.", ot::messageTypeToLogFlag(_messageType));
	}

	return result;
}

