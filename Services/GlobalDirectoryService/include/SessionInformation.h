#pragma once

// C++ header
#include <string>

class SessionInformation {
public:
	SessionInformation();
	SessionInformation(const std::string& _id, const std::string& _sessionServiceURL);
	SessionInformation(const SessionInformation& _other);
	virtual ~SessionInformation();

	SessionInformation& operator = (const SessionInformation& _other);
	bool operator == (const SessionInformation& _other) const;
	bool operator != (const SessionInformation& _other) const;

	void setID(const std::string& _id) { m_id = _id; }
	const std::string& id(void) const { return m_id; }

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceURL = _url; }
	const std::string& sessionServiceURL(void) const { return m_sessionServiceURL; }

private:
	std::string		m_id;
	std::string		m_sessionServiceURL;
};