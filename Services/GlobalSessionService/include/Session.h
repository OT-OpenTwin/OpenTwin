//! @file Session.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTCore/OTClassHelper.h"
#include <string>

class Session {
public:
	Session();
	Session(const std::string& _id);
	Session(const Session& _other);
	virtual ~Session();

	Session& operator = (const Session& _other);

	void setId(const std::string& _id) { m_id = _id; };
	const std::string& id(void) const { return m_id; };

	void setUserName(const std::string& _name) { m_userName = _name; };
	const std::string& userName(void) const { return m_userName; };

private:
	std::string m_id;
	std::string m_userName;
};