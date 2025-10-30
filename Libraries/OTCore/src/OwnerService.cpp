// @otlicense
// File: OwnerService.cpp
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

#include "OTCore/OwnerService.h"


bool ot::OwnerService::getIdFromJson(JsonValue& _object, ot::serviceID_t& _id) {
	if (!_object.HasMember(m_jsonMemberName.c_str())) {
		OT_LOG_EA("JSON object member for owner Id is missing ");
		return false;
	}

	if (!_object[m_jsonMemberName.c_str()].IsUint()) {
		OT_LOG_EA("JSON object member for owner Id has wrong format");
		return false;
	}

	_id = (ot::serviceID_t)_object[m_jsonMemberName.c_str()].GetUint();
	return true;
}

ot::OwnerService ot::OwnerService::ownerFromJson(JsonValue& _object) {
	if (!_object.HasMember(m_jsonMemberName.c_str())) {
		OT_LOG_EA("JSON object member for owner Id is missing ");
		return false;
	}

	if (!_object[m_jsonMemberName.c_str()].IsUint()) {
		OT_LOG_EA("JSON object member for owner Id has wrong format");
		return false;
	}
	return ot::OwnerService((ot::serviceID_t)_object[m_jsonMemberName.c_str()].GetUint());
}

void ot::OwnerService::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) {
	_object.AddMember(JsonValue(m_jsonMemberName.c_str(), _allocator), JsonValue(this->getId()), _allocator);
}

