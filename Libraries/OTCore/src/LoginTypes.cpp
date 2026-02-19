// @otlicense

// OpenTwin header
#include "OTCore/LoginTypes.h"
#include "OTCore/Logging/Logger.h"

std::string ot::toString(LoginType _type) {
	switch (_type) {
	case ot::LoginType::UsernamePassword: 
		return "Username/Password";

	case ot::LoginType::SSO:
		return "Single Sign-on";

	default:
		std::string msg = "Unknown LoginType (" + std::to_string(static_cast<int>(_type)) + ")";
		OT_LOG_E(msg);
		throw Exception::InvalidArgument(msg);
	}
}

ot::LoginType ot::stringToLoginType(const std::string& _str) {
	if (_str == toString(LoginType::UsernamePassword)) {
		return LoginType::UsernamePassword;
	}
	else if (_str == toString(LoginType::SSO)) {
		return LoginType::SSO;
	}
	else {
		std::string msg = "Unknown LoginType string \"" + _str + "\".";
		OT_LOG_E(msg);
		throw Exception::InvalidType(msg);
	}
}
