//! @file LogInGSSEntry.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "LogInGSSEntry.h"

LogInGSSEntry::LogInGSSEntry() {}

LogInGSSEntry::LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port)
	: m_name(_name), m_url(_url), m_port(_port)
{}

LogInGSSEntry::LogInGSSEntry(const LogInGSSEntry& _other) {
	*this = _other;
}

LogInGSSEntry& LogInGSSEntry::operator = (const LogInGSSEntry& _other) {
	if (this == &_other) return *this;

	m_name = _other.m_name;
	m_url = _other.m_url;
	m_port = _other.m_port;

	return *this;
}

void LogInGSSEntry::clear(void) {
	m_name.clear();
	m_url.clear();
	m_port.clear();
}