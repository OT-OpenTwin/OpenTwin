// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

namespace ot {

	enum class LoginType {
		UsernamePassword,
		SSO
	};

	OT_CORE_API_EXPORT std::string toString(LoginType _type);
	OT_CORE_API_EXPORT LoginType stringToLoginType(const std::string& _str);

}