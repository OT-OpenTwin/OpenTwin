#pragma once

// OpenTwin header
#include "OpenTwinCore/Serializable.h"

#include <string>

class SessionInformation : public ot::Serializable {
public:
	SessionInformation();
	SessionInformation(const std::string& _id, const std::string& _sessionServiceUrl);
	SessionInformation(const SessionInformation& _other);
	virtual ~SessionInformation();

	SessionInformation& operator = (const SessionInformation& _other);
	bool operator == (const SessionInformation& _other) const;
	bool operator != (const SessionInformation& _other) const;
	bool operator < (const SessionInformation& _other) const;
	bool operator <= (const SessionInformation& _other) const;
	bool operator > (const SessionInformation& _other) const;
	bool operator >= (const SessionInformation& _other) const;

	//! @brief Add the object contents to the provided JSON object
	//! @param _document The JSON document (used to get the allocator)
	//! @param _object The JSON object to add the contents to
	virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	void setID(const std::string& _id) { m_id = _id; }
	const std::string& id(void) const { return m_id; }

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceUrl = _url; }
	const std::string& sessionServiceURL(void) const { return m_sessionServiceUrl; }

private:
	std::string		m_id;
	std::string		m_sessionServiceUrl;
};