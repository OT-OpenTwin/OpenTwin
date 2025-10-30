// @otlicense
// File: ThisService.cpp
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
#include "OTCore/ThisService.h"

ot::ThisService& ot::ThisService::instance(void) {
	static ThisService g_instance;
	return g_instance;
}

void ot::ThisService::addIdToJsonDocument(JsonDocument& _document) {
	_document.AddMember("Service.ID", ThisService::instance().getServiceID(), _document.GetAllocator());
}

ot::serviceID_t ot::ThisService::getIdFromJsonDocument(const JsonDocument& _document) {
	serviceID_t id = invalidServiceID;
	id = (serviceID_t)json::getUInt(_document, "Service.ID");
	return id;
}