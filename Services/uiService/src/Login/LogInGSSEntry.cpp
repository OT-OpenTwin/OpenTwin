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
#include "Login/LogInGSSEntry.h"
#include "OTCore/Logging/Logger.h"

LogInGSSEntry::LogInGSSEntry(const QString& _name, const QString& _url, const QString& _port)
	: m_name(_name), m_url(_url), m_port(_port)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// JSON serialization

void LogInGSSEntry::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("Name", ot::JsonString(m_name.toStdString(), _allocator), _allocator);
	_jsonObject.AddMember("Url", ot::JsonString(m_url.toStdString(), _allocator), _allocator);
	_jsonObject.AddMember("Port", ot::JsonString(m_port.toStdString(), _allocator), _allocator);
	_jsonObject.AddMember("Auth", ot::JsonString(ot::toString(m_loginType), _allocator), _allocator);
}

void LogInGSSEntry::setFromJsonObject(const ot::ConstJsonObject& _jsonObject)
{
	m_name = QString::fromStdString(ot::json::getString(_jsonObject, "Name"));
	m_url = QString::fromStdString(ot::json::getString(_jsonObject, "Url"));
	m_port = QString::fromStdString(ot::json::getString(_jsonObject, "Port"));
	m_loginType = ot::stringToLoginType(ot::json::getString(_jsonObject, "Auth"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Query serialization

void LogInGSSEntry::addToQuery(QUrlQuery& _query, bool _includeName) const
{
	this->addToQuery(_query, QString("gss"), _includeName);
}

void LogInGSSEntry::addToQuery(QUrlQuery& _query, const QString& _primaryKey, bool _includeName) const
{
	OTAssert(!_primaryKey.isEmpty(), "No primary key set");
	OTAssert(this->isValid(), "Invalid GSS entry");

	if (_includeName)
	{
		_query.addQueryItem(_primaryKey + ".name", m_name);
	}
	_query.addQueryItem(_primaryKey + ".url", m_url);
	_query.addQueryItem(_primaryKey + ".port", m_port);
	_query.addQueryItem(_primaryKey + ".auth", QString::fromStdString(ot::toString(m_loginType)));
}

std::optional<QString> LogInGSSEntry::readFromQuery(const QUrlQuery& _query)
{
	return this->readFromQuery(_query, QString("gss"));
}

std::optional<QString> LogInGSSEntry::readFromQuery(const QUrlQuery& _query, const QString& _primaryKey)
{
	OTAssert(!_primaryKey.isEmpty(), "No primary key set");

	QStringList mandatoryKeys{
		_primaryKey + ".url",
		_primaryKey + ".port",
		_primaryKey + ".auth"
	};

	for (const QString& key : mandatoryKeys)
	{
		if (!_query.hasQueryItem(key))
		{
			return "\"" + key + "\" is missing";
		}
	}

	m_url = _query.queryItemValue(_primaryKey + ".url");
	m_port = _query.queryItemValue(_primaryKey + ".port");
	m_loginType = ot::stringToLoginType(_query.queryItemValue(_primaryKey + ".auth").toStdString());

	if (_query.hasQueryItem(_primaryKey + ".name"))
	{
		m_name = _query.queryItemValue(_primaryKey + ".name");
	}

	return std::nullopt;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void LogInGSSEntry::clear()
{
	m_name.clear();
	m_url.clear();
	m_port.clear();
}

QString LogInGSSEntry::getDisplayText() const
{
	QString txt = m_name + " (" + getConnectionUrl();

	switch (m_loginType)
	{
	case ot::LoginType::SSO:
		txt.append(" (SSO)");
		break;
	case ot::LoginType::UsernamePassword:
		break;
	default:
		OT_LOG_E("Unknown login type (" + std::to_string(static_cast<int>(m_loginType)) + ")");
		break;
	}
	txt.append(")");

	return txt;
}
