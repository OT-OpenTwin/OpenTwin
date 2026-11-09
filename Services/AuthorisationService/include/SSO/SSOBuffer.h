// @otlicense
// File: SSOBuffer.h
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

#pragma once

// Auth header
#include "SSO/SSOUser.h"
#include "SSO/SSOAuthenticationProcessID.h"

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTSystem/SingleSignOn/SingleSignOn_Server.h"

// std header
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

class SSOBuffer
{
public:
	bool handleRequest(std::string& _determinedUsername, const std::string& _authenticationProcessID, const std::string& _receivedToken, bool _initialToken, ot::JsonDocument& _returnMessage);
	SSOUser* getUser (const std::string& _token);
	void removeUser(const std::string& _token);
	void clearExpiredProcessIDs();
	bool validate(const std::string& _token);
private:
	//! @brief Here we keep the current state of the SSO authentication process per username.
	std::unordered_map<ot::SSOAuthenticationProcessID, std::unique_ptr<ot::SingleSignOn_Server>> m_serverSignOnByProcessID;

	//! @brief Here we buffer the logged in sso users
	std::map<std::string, SSOUser> m_loggedInSSOUsersByToken;
};
