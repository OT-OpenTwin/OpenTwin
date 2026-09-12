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
#include "OTCore/LoginTypes.h"
#include "OTCore/Serializable.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qurlquery.h>

// std header
#include <optional>
 
class LogInGSSEntry : public ot::Serializable {
	OT_DECL_DEFCOPY(LogInGSSEntry)
	OT_DECL_DEFMOVE(LogInGSSEntry)
public:
	LogInGSSEntry() = default;
	LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port);

	// ###########################################################################################################################################################################################################################################################################################################################

	// JSON serialization

	virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
	virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Query serialization

	void addToQuery(QUrlQuery& _query, bool _includeName = false) const;
	void addToQuery(QUrlQuery& _query, const QString& _primaryKey, bool _includeName = false) const;

	OT_DECL_NODISCARD std::optional<QString> readFromQuery(const QUrlQuery& _query);
	OT_DECL_NODISCARD std::optional<QString> readFromQuery(const QUrlQuery& _query, const QString& _primaryKey);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	bool isValid(bool _nameMayBeEmpty = true) const { return (_nameMayBeEmpty || !m_name.isEmpty()) && !m_url.isEmpty() && !m_port.isEmpty(); };

	void clear();

	void setName(const QString& _name) { m_name = _name; };
	const QString& getName() const { return m_name; };

	void setUrl(const QString& _url) { m_url = _url; };
	const QString& getUrl() const { return m_url; };

	void setPort(const QString& _port) { m_port = _port; };
	const QString& getPort() const { return m_port; };

	void setLoginType(const ot::LoginType& _loginType) { m_loginType = _loginType; };
	const ot::LoginType& getLoginType() const { return m_loginType; };

	QString getConnectionUrl() const { return m_url + ":" + m_port; };

	//! @brief Returns a string with the current name and connection information.
	//! The format is: name (url:port (login_mode))
	QString getDisplayText() const;

private:
	QString		m_name;
	QString		m_url;
	QString		m_port;
	ot::LoginType m_loginType = ot::LoginType::UsernamePassword;
};