// @otlicense

//! @file Session.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GSS header
#include "Session.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"

Session::Session() : m_state(NoStateFlags) {}

Session::Session(const std::string& _id) : m_state(NoStateFlags), m_id(_id) {}

Session::Session(const Session& _other) : m_state(_other.m_state), m_id(_other.m_id), m_userName(_other.m_userName) {}

Session::Session(Session&& _other) noexcept : m_state(std::move(_other.m_state)), m_id(std::move(_other.m_id)), m_userName(std::move(_other.m_userName)) {}

Session::~Session() {}

Session& Session::operator = (const Session& _other) {
	if (this != &_other) {
		m_state = _other.m_state;
		m_id = _other.m_id;
		m_userName = _other.m_userName;
	}

	return *this;
}

Session& Session::operator=(Session&& _other) noexcept {
	if (this != &_other) {
		m_state = std::move(_other.m_state);
		m_id = std::move(_other.m_id);
		m_userName = std::move(_other.m_userName);
	}

	return *this;
}

bool Session::operator==(const Session& _other) const {
	return m_id == _other.m_id && m_userName == _other.m_userName;
}

bool Session::operator!=(const Session& _other) const {
	return m_id != _other.m_id || m_userName != _other.m_userName;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void Session::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::JsonArray stateArr;
	if (m_state & SessionStateFlag::LssConfirmed) { stateArr.PushBack(ot::JsonString("LssConfirmed", _allocator), _allocator); }

	_object.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_id, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SESSION_USER, ot::JsonString(m_userName, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_State, stateArr, _allocator);
}

void Session::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_state = NoStateFlags;
	for (const std::string& state : ot::json::getStringList(_object, OT_ACTION_PARAM_State)) {
		if (state == "LssConfirmed") { m_state.setFlag(SessionStateFlag::LssConfirmed); }
		else {
			OT_LOG_EAS("Unknown state flag \"" + state + "\"");
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

ot::GSSDebugInfo::SessionData Session::getDebugInformation() const {
	ot::GSSDebugInfo::SessionData data;
	data.sessionID = m_id;
	data.userName = m_userName;

	std::list<std::string> flagsArr;
	if (m_state & SessionStateFlag::LssConfirmed) { flagsArr.push_back("LssConfirmed"); }

	data.flags = std::move(flagsArr);

	return data;
}