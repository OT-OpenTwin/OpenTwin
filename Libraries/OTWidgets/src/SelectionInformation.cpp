// @otlicense
// File: SelectionInformation.cpp
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
#include "OTWidgets/SelectionInformation.h"

void ot::SelectionInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("UIDs", JsonArray(m_uids, _allocator), _allocator);
}

void ot::SelectionInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_uids = json::getUInt64List(_object, "UIDs");
}

void ot::SelectionInformation::removeSelectedNavigationItem(UID _uid) {
	auto it = std::find(m_uids.begin(), m_uids.end(), _uid);
	if (it != m_uids.end()) {
		m_uids.erase(it);
	}
}
