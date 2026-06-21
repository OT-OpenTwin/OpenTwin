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
#include "OTModelEntities/Lms/UserLibraryElement.h"

bool ot::UserLibraryElement::operator==(const UserLibraryElement& _other) const {
	return ot::LibraryElement::operator==(_other) && m_owner == _other.m_owner;
}

bool ot::UserLibraryElement::isSameElement(UserLibraryElement& _other) const {

	return ot::LibraryElement::isSameElement(_other);
}

void ot::UserLibraryElement::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::LibraryElement::addToJsonObject(_object, _allocator); // Call base class implementation to add common members
	_object.AddMember("Owner", ot::JsonString(m_owner, _allocator), _allocator);
}

void ot::UserLibraryElement::setFromJsonObject(const ot::ConstJsonObject& _object) {
	ot::LibraryElement::setFromJsonObject(_object); // Call base class implementation to set common members

	if (_object.HasMember("Owner")) {
		m_owner = ot::json::getString(_object, "Owner");
	}
	else {
		m_owner = "";
	}
}

ot::UserLibraryElement ot::UserLibraryElement::fromJson(const std::string& _jsonString) {
	ot::JsonDocument doc;
	doc.fromJson(_jsonString);

	ot::UserLibraryElement element;
	element.setFromJsonObject(doc.getConstObject());
	return element;
}

