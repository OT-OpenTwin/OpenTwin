// @otlicense
// File: SupportedService.h
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
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

class SupportedService {
	OT_DECL_DEFCOPY(SupportedService)
	OT_DECL_DEFMOVE(SupportedService)
public:
	SupportedService();
	SupportedService(const std::string& _name, const std::string& _type);
	~SupportedService() {};

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setType(const std::string& _type) { m_type = _type; };
	const std::string& getType() const { return m_type; };

	void setMaxCrashRestarts(unsigned int _restarts) { m_maxCrashRestarts = _restarts; };
	unsigned int getMaxCrashRestarts() const { return m_maxCrashRestarts; };

	void setMaxStartupRestarts(unsigned int _restarts) { m_maxStartupRestarts = _restarts; };
	unsigned int getMaxStartupRestarts() const { return m_maxStartupRestarts; };

private:
	std::string  m_name;
	std::string  m_type;
	unsigned int m_maxCrashRestarts;
	unsigned int m_maxStartupRestarts;
};