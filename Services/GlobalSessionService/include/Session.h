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

	OT_PROPERTY_REF(std::string, id, setId, id);
	OT_PROPERTY_REF(std::string, userName, setUserName, userName);
};