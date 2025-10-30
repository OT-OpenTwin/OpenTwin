// @otlicense
// File: LogInGSSEntry.h
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

// Qt header
#include <QtCore/qstring.h>

class LogInGSSEntry {
	OT_DECL_DEFCOPY(LogInGSSEntry)
	OT_DECL_DEFMOVE(LogInGSSEntry)
public:
	LogInGSSEntry() = default;
	LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port);

	void setName(const QString& _name) { m_name = _name; };
	const QString& getName(void) const { return m_name; };

	void setUrl(const QString& _url) { m_url = _url; };
	const QString& getUrl(void) const { return m_url; };

	void setPort(const QString& _port) { m_port = _port; };
	const QString& getPort(void) const { return m_port; };

	QString getConnectionUrl(void) const { return m_url + ":" + m_port; };

	void clear(void);

	bool isValid(void) const { return !m_name.isEmpty() && !m_url.isEmpty() && !m_port.isEmpty(); };

	QString getDisplayText(void) const { return m_name + " (" + m_url + ":" + m_port + ")"; };

private:
	QString		m_name;
	QString		m_url;
	QString		m_port;
};