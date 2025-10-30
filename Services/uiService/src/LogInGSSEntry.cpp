// @otlicense

// Frontend header
#include "LogInGSSEntry.h"

LogInGSSEntry::LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port)
	: m_name(_name), m_url(_url), m_port(_port)
{}

void LogInGSSEntry::clear(void) {
	m_name.clear();
	m_url.clear();
	m_port.clear();
}