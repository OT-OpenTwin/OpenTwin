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

	OT_PROPERTY(std::string, ID);
	OT_PROPERTY(std::string, UserName);
};