//! @file Session.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GSS header
#include "Session.h"

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
