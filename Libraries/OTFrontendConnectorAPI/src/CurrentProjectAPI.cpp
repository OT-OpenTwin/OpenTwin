// @otlicense
// File: CurrentProjectAPI.cpp
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

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/LogDispatcher.h"
#include "OTFrontendConnectorAPI/CurrentProjectAPI.h"

// Public API

void ot::CurrentProjectAPI::activateModelVersion(const std::string& _versionName) {
	CurrentProjectAPI::instance()->activateModelVersionAPI(_versionName);
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::CurrentProjectAPI::CurrentProjectAPI() {
	if (getInstanceReference()) {
		OT_LOG_EA("CurrentProjectAPI instance already exists");
		throw Exception::ObjectAlreadyExists("CurrentProjectAPI instance already exists");
	}
	getInstanceReference() = this;
}

ot::CurrentProjectAPI::~CurrentProjectAPI() {
	OTAssert(getInstanceReference(), "CurrentProjectAPI instance does not exist");
	getInstanceReference() = nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Instance management

ot::CurrentProjectAPI* ot::CurrentProjectAPI::instance() {
	CurrentProjectAPI* instance = getInstanceReference();
	if (!instance) {
		OT_LOG_EA("CurrentProjectAPI instance not available");
		throw Exception::ObjectNotFound("CurrentProjectAPI instance not available");
	}
	return instance;
}

ot::CurrentProjectAPI*& ot::CurrentProjectAPI::getInstanceReference() {
	static CurrentProjectAPI* instance = nullptr;
	return instance;
}