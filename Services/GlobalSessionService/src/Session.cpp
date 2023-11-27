//! @file Session.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GSS header
#include "Session.h"

Session::Session() {}

Session::Session(const std::string& _id) : m_id(_id) {}

Session::Session(const Session& _other) : m_id(_other.m_id), m_userName(_other.m_userName) {}

Session::~Session() {}

Session& Session::operator = (const Session& _other) {
	m_id = _other.m_id;
	m_userName = _other.m_userName;
	return *this;
}