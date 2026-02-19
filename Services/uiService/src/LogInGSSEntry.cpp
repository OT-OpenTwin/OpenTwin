// @otlicense
// File: LogInGSSEntry.cpp
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

// Frontend header
#include "LogInGSSEntry.h"
#include "OTCore/Logging/Logger.h"

LogInGSSEntry::LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port)
	: m_name(_name), m_url(_url), m_port(_port)
{}

void LogInGSSEntry::clear() {
	m_name.clear();
	m_url.clear();
	m_port.clear();
}

QString LogInGSSEntry::getDisplayText() const {
	QString txt = m_name + " (";

	switch (m_loginType) {
	case ot::LoginType::SSO:
		txt.append("SSO ");
		break;
	case ot::LoginType::UsernamePassword:
		break;
	default:
		OT_LOG_E("Unknown login type (" + std::to_string(static_cast<int>(m_loginType)) + ")");
		break;
	}
	txt.append(" (" + getConnectionUrl() + ")");

	return txt;
}