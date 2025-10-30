// @otlicense
// File: DataBaseInfo.h
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

// std header
#include <string>

class DataBaseInfo {
public:
	DataBaseInfo() {};
	DataBaseInfo(const DataBaseInfo& _other);
	~DataBaseInfo() {};

	DataBaseInfo& operator = (const DataBaseInfo& _other);

	bool hasInfoSet(void) const;

	void setDataBaseUrl(const std::string& _url) { m_dbUrl = _url; };
	const std::string& getDataBaseUrl(void) const { return m_dbUrl; };

	void setSiteID(const std::string& _id) { m_siteID = _id; };
	const std::string& getSiteID(void) const { return m_siteID; };

	void setCollectionName(const std::string& _collectionName) { m_collectionName = _collectionName; };
	const std::string& getCollectionName(void) const { return m_collectionName; };

	void setUserName(const std::string& _userName) { m_userName = _userName; };
	const std::string& getUserName(void) const { return m_userName; };

	void setUserPassword(const std::string& _userPassword) { m_userPassword = _userPassword; };
	const std::string& getUserPassword(void) const { return m_userPassword; };

private:
	std::string m_dbUrl;
	std::string m_siteID;
	std::string m_collectionName;

	std::string m_userName;
	std::string m_userPassword;

};