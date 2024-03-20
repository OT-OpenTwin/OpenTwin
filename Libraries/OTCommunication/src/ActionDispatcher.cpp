//! @file ActionDispatcher.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/otAssert.h"
#include "OTCore/Logger.h"

#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionHandlerBase.h"

ot::ActionDispatcher& ot::ActionDispatcher::instance(void) {
	static ActionDispatcher g_instance;
	return g_instance;
}

ot::ActionDispatcher::ActionDispatcher() {

}

ot::ActionDispatcher::~ActionDispatcher() {

}

void ot::ActionDispatcher::add(ActionHandlerBase* _item, bool _overwrite) {
	m_mutex.lock();

	for (const std::string& action : _item->actionNames()) {
		auto it = m_data.find(action);
		if (it == m_data.end() || _overwrite) {
			m_data.insert_or_assign(action, _item);
		}
		else {
			OTAssert(0, "Handler for the given action already exist");
		}
	}

	m_mutex.unlock();
}

void ot::ActionDispatcher::remove(ActionHandlerBase* _item) {
	m_mutex.lock();

	for (const std::string& action : _item->actionNames()) {
		m_data.erase(action);
	}

	m_mutex.unlock();
}

char* ot::ActionDispatcher::dispatchWrapper(const char* _json, const char* _senderUrl, ot::MessageType _messageType) {
	std::string result = dispatch(_json, _messageType);

	// Return value must be copied to a memory location that will not be destroyed by the application/service
	size_t len = result.length() + 1;
	char* returnValue = new char[len];
	strcpy_s(returnValue, len, result.c_str());
	return returnValue;
}

std::string ot::ActionDispatcher::dispatch(const std::string& _json, ot::MessageType _messageType) {
	// Parse the json string and check its main syntax
	JsonDocument doc;
	doc.fromJson(_json);

	return this->dispatch(doc, _messageType);
}

std::string ot::ActionDispatcher::dispatch(JsonDocument& _document, MessageType _messageType) {
	// Get the action and dispatch it
	std::string action = json::getString(_document, OT_ACTION_MEMBER);

	if (action.empty()) {
		OT_LOG_EA("Action member not found");
		return OT_ACTION_RETURN_INDICATOR_Error "Action syntax error";
	}

	bool handlerFound = false;
	std::string result = dispatch(action, _document, handlerFound, _messageType);

	// Check if a handler was found or an error occured (non empty result);
	if (!handlerFound && result.empty()) {
		OT_LOG_WAS("Unknown action received: " + action);
		result = OT_ACTION_RETURN_UnknownAction;
	}

	return result;
}

std::string ot::ActionDispatcher::dispatch(const std::string& _action, JsonDocument& _document, ot::MessageType _messageType) {
	bool tmp = false;
	return dispatch(_action, _document, tmp, _messageType);
}

std::string ot::ActionDispatcher::dispatch(const std::string& _action, JsonDocument& _document, bool& _handlerFound, ot::MessageType _messageType) {
	// Check for default actions
	if (_action == OT_ACTION_CMD_Ping) {
		_handlerFound = true;
		return OT_ACTION_CMD_Ping;
	}

	m_mutex.lock();

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
		else if (_action == OT_ACTION_CMD_SetLoggingOptions) {
			// todo: add global logging options logic
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		result = std::string(OT_ACTION_RETURN_INDICATOR_Error) + _e.what();
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error occured");
		result = OT_ACTION_RETURN_INDICATOR_Error "Unknown error";
	}

	if (_handlerFound) {
		OT_LOG("..... Completed: Dispatching: \"" + _action + "\". Returning: \"" + result + "\"", ot::messageTypeToLogFlag(_messageType));
	}
	else {
		OT_LOG("..... Completed: Dispatching: \"" + _action + "\". FAILED: No handler found. Returning: \"" + result + "\"", ot::messageTypeToLogFlag(_messageType));
	}

	m_mutex.unlock();
	return result;
}
