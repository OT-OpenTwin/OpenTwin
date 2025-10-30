// @otlicense
// File: ServiceDataUi.h
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
#include "OTCore/ServiceBase.h"

class ServiceDataUi : public ot::ServiceBase {
public:
	ServiceDataUi();
	ServiceDataUi(const std::string& _name, const std::string& _type, const std::string& _url, ot::serviceID_t _id);
	virtual ~ServiceDataUi();

	void setUiInitializationCompleted(bool _completed = true) { m_uiInitCompleted = _completed; };
	bool isUiInitializationCompleted(void) const { return m_uiInitCompleted; };

private:
	bool m_uiInitCompleted;
};
