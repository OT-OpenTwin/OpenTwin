#pragma once

#include "OTCore/CoreTypes.h"
#include "OTCore/rJSON.h"
#include <string>
#include <list>

class Session;
class SessionService {
public:
	SessionService();
	SessionService(const SessionService& _other);
	virtual ~SessionService();

	SessionService& operator = (const SessionService& _other);

	// ###################################################################

	// Getter/Setter

	void setURL(const std::string& _url) { m_url = _url; }
	const std::string& url(void) const { return m_url; }

	void setID(ot::serviceID_t _uid) { m_id = _uid; }
	ot::serviceID_t id(void) const { return m_id; }

	bool addSession(const Session& _session);
	void removeSession(Session * _session);
	const std::list<Session *> sessions(void) const { return m_sessions; }
	size_t sessionCount(void) const;
	Session * getSessionById(const std::string& _sessionId);

	// ###################################################################

	// Data manipulation

	bool setFromDocument(OT_rJSON_doc& _document);

	void clear();

private:
	std::string				m_url;
	ot::serviceID_t			m_id;

	std::list<Session *>	m_sessions;
};