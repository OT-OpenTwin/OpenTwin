#include "Session.h"

Session::Session() {}

Session::Session(const std::string& _id) : m_ID(_id) {}

Session::Session(const Session& _other) : m_ID(_other.m_ID), m_UserName(_other.m_UserName) {}

Session::~Session() {}

Session& Session::operator = (const Session& _other) {
	m_ID = _other.m_ID;
	m_UserName = _other.m_UserName;
	return *this;
}