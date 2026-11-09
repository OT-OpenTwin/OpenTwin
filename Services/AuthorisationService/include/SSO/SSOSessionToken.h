// @otlicense
// File: SSOSessionToken.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <optional>
#include <string>
#include <chrono>

namespace ot
{
	class SSOSessionToken
	{
		OT_DECL_NOCOPY(SSOSessionToken)
		OT_DECL_DEFMOVE(SSOSessionToken)
	public:
		SSOSessionToken() = default;
		~SSOSessionToken() = default;
		
		std::string generateToken();
		const std::string& getToken() const;

		bool tokenIsValid(bool _useDelta = true) const;
	private:
		std::string m_token;
		std::chrono::steady_clock::time_point m_creationTime;
		std::chrono::minutes m_tokenValidityDurationAbsolute{ 30 }; 
		std::chrono::seconds m_tokenValidityDelta{ 5 }; 
		// std::chrono::minutes m_tokenValidityDurationIdle{ 30 }; Does not make sense since the authentication requests happen rarely
	};
}