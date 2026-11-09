// @otlicense
// File: LoginTypes.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
