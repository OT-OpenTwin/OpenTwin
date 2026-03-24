// @otlicense
// File: TableHeaderItemCfg.cpp
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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Widgets/TableHeaderItemCfg.h"

std::string ot::TableHeaderItemCfg::toString(FilterBehavior _behavior)
{
	switch (_behavior)
	{
	case ot::TableHeaderItemCfg::NoFilter: return "None";
	case ot::TableHeaderItemCfg::UseText: return "Text";
	case ot::TableHeaderItemCfg::RequestData: return "Request";
	default:
		OT_LOG_EAS("Unknown filter behavior (" + std::to_string((int)_behavior) + ")");
		return "None";
	}
}

ot::TableHeaderItemCfg::FilterBehavior ot::TableHeaderItemCfg::stringToFilterBehavior(const std::string& _behavior)
{
	if (_behavior == toString(FilterBehavior::NoFilter)) return FilterBehavior::NoFilter;
	else if (_behavior == toString(FilterBehavior::UseText)) return FilterBehavior::UseText;
	else if (_behavior == toString(FilterBehavior::RequestData)) return FilterBehavior::RequestData;
	else {
		OT_LOG_EAS("Unknown filter behavior \"" + _behavior + "\"");
		return FilterBehavior::NoFilter;
	}
}

ot::TableHeaderItemCfg::TableHeaderItemCfg(const std::string& _text)
	: m_text(_text)
{

}

void ot::TableHeaderItemCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("FilterEnabled", JsonString(toString(m_filterBehavior), _allocator), _allocator);
}

void ot::TableHeaderItemCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_text = json::getString(_object, "Text");
	m_filterBehavior = stringToFilterBehavior(json::getString(_object, "FilterEnabled"));
}
