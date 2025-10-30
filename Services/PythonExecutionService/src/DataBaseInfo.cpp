// @otlicense
// File: DataBaseInfo.cpp
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

#include "DataBaseInfo.h"

DataBaseInfo::DataBaseInfo(const DataBaseInfo& _other) {
	*this = _other;
}

DataBaseInfo& DataBaseInfo::operator=(const DataBaseInfo& _other) {
	if (this != &_other) {
		m_dbUrl = _other.m_dbUrl;
		m_siteID = _other.m_siteID;
		m_collectionName = _other.m_collectionName;

		m_userName = _other.m_userName;
		m_userPassword = _other.m_userPassword;
	}

	return *this;
}

bool DataBaseInfo::hasInfoSet(void) const {
	return !m_dbUrl.empty() && !m_siteID.empty() && !m_collectionName.empty() && !m_userName.empty() && !m_userPassword.empty();
}
