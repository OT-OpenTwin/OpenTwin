//! @file LocalSessionService.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>
#include <string>

class Session;
class LocalSessionService : public ot::Serializable {
public:
	LocalSessionService();
	LocalSessionService(const LocalSessionService& _other);
	virtual ~LocalSessionService();

	LocalSessionService& operator = (const LocalSessionService& _other);

	// ###################################################################

	// Getter/Setter

	void setUrl(const std::string& _url) { m_url = _url; };
	const std::string& url(void) const { return m_url; };

	void setSessions(const std::list<Session*>& _sessions) { m_sessions = _sessions; };
	const std::list<Session*>& sessions(void) const { return m_sessions; };

	void setId(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t id(void) const { return m_id; };

	bool addSession(const Session& _session);
	void removeSession(Session * _session);
	size_t sessionCount(void) const;
	Session * getSessionById(const std::string& _sessionId);

	double globalCpuLoad(void) const;
	double globalMemoryLoad(void) const;

	// ###################################################################

	// Data manipulation

	//! @brief Add the object contents to the provided JSON object
	//! @param _object Json object reference
	//! @param _allocator Allocator
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	void clear();

private:
	std::string m_url;
	std::list<Session*> m_sessions;
	ot::serviceID_t m_id;
};