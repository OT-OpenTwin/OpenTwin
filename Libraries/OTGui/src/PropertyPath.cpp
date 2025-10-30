// @otlicense
// File: PropertyPath.cpp
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
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyPath.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyPath> propertyPathRegistrar(ot::PropertyPath::propertyTypeString());

ot::PropertyPath::PropertyPath(const PropertyPath* _other)
	: Property(_other), m_browseMode(_other->m_browseMode), m_filters(_other->m_filters), m_path(_other->m_path)
{}

ot::PropertyPath::PropertyPath(const PropertyBase& _base)
	: Property(_base), m_browseMode(PathBrowseMode::ReadFile)
{}

ot::PropertyPath::PropertyPath(PathBrowseMode _mode, PropertyFlags _flags)
	: Property(_flags), m_browseMode(_mode)
{}

ot::PropertyPath::PropertyPath(const std::string& _path, PathBrowseMode _mode, PropertyFlags _flags)
	: Property(_flags), m_browseMode(_mode), m_path(_path)
{}

ot::PropertyPath::PropertyPath(const std::string& _name, const std::string& _path, PathBrowseMode _mode, PropertyFlags _flags)
	: Property(_name, _flags), m_browseMode(_mode), m_path(_path)
{}

void ot::PropertyPath::addFilter(const FilterInfo& _info) {
	m_filters.push_back(_info);
}

void ot::PropertyPath::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyPath* other = dynamic_cast<const PropertyPath*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {
		m_path = other->m_path;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_browseMode = other->m_browseMode;
		m_filters = other->m_filters;
	}
}

ot::Property* ot::PropertyPath::createCopy(void) const {
	return new ot::PropertyPath(this);
}

void ot::PropertyPath::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Path", JsonString(m_path, _allocator), _allocator);
	_object.AddMember("Mode", JsonString(ot::toString(m_browseMode), _allocator), _allocator);
	JsonArray fArr;
	for (const FilterInfo& info : m_filters) {
		JsonObject fObj;
		fObj.AddMember("Ext", JsonString(info.extension, _allocator), _allocator);
		fObj.AddMember("Txt", JsonString(info.text, _allocator), _allocator);
		fArr.PushBack(fObj, _allocator);
	}
	_object.AddMember("Filters", fArr, _allocator);
}

void ot::PropertyPath::setPropertyData(const ot::ConstJsonObject& _object) {
	m_path = json::getString(_object, "Path");
	m_browseMode = ot::stringToPathBrowseMode(json::getString(_object, "Mode"));
	ConstJsonArray fArr = json::getArray(_object, "Filters");
	for (JsonSizeType i = 0; i < fArr.Size(); i++) {
		ConstJsonObject fObj = json::getObject(fArr, i);
		FilterInfo info;
		info.extension = json::getString(fObj, "Ext");
		info.text = json::getString(fObj, "Txt");
		m_filters.push_back(info);
	}
}