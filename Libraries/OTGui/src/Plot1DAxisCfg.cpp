// @otlicense
// File: Plot1DAxisCfg.cpp
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
#include "OTGui/Plot1DAxisCfg.h"

std::string ot::Plot1DAxisCfg::toString(AxisQuantity _quantity) {
	switch (_quantity) {
	case AxisQuantity::Undefined: return "Undefined";
	case AxisQuantity::XData: return "X Data";
	case AxisQuantity::Magnitude: return "Magnitude";
	case AxisQuantity::Phase: return "Phase";
	case AxisQuantity::Real: return "Real";
	case AxisQuantity::Imaginary: return "Imaginary";
	default:
		OT_LOG_EAS("Unknown axis quantity (" + std::to_string((int)_quantity) + ")");
		return "Undefined";
	}
}

ot::Plot1DAxisCfg::AxisQuantity ot::Plot1DAxisCfg::stringToAxisQuantity(const std::string& _quantity) {
	if (_quantity == toString(AxisQuantity::Undefined)) return AxisQuantity::Undefined;
	else if (_quantity == toString(AxisQuantity::XData)) return AxisQuantity::XData;
	else if (_quantity == toString(AxisQuantity::Magnitude)) return AxisQuantity::Magnitude;
	else if (_quantity == toString(AxisQuantity::Phase)) return AxisQuantity::Phase;
	else if (_quantity == toString(AxisQuantity::Real)) return AxisQuantity::Real;
	else if (_quantity == toString(AxisQuantity::Imaginary)) return AxisQuantity::Imaginary;
	else {
		OT_LOG_EAS("Unknown axis quantity \"" + _quantity + "\"");
		return AxisQuantity::Undefined;
	}
}

std::list<std::string> ot::Plot1DAxisCfg::getAxisQuantityStringList() {
	return std::list<std::string>({
		toString(AxisQuantity::XData),
		toString(AxisQuantity::Real),
		toString(AxisQuantity::Imaginary),
		toString(AxisQuantity::Magnitude),
		toString(AxisQuantity::Phase),
	});
}

ot::Plot1DAxisCfg::Plot1DAxisCfg()
	: m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max())
{
}

ot::Plot1DAxisCfg::~Plot1DAxisCfg() {}


bool ot::Plot1DAxisCfg::operator==(const Plot1DAxisCfg& _other) const {
	return m_min == _other.m_min &&
		m_max == _other.m_max &&
		m_axisQuantity == _other.m_axisQuantity &&
		m_isLogScale == _other.m_isLogScale &&
		m_axisLabel == _other.m_axisLabel &&
		m_autoDetermineAxisLabel == _other.m_autoDetermineAxisLabel && 
		m_isAutoScale == _other.m_isAutoScale;
}

bool ot::Plot1DAxisCfg::operator!=(const Plot1DAxisCfg& _other) const {
	return !(*this == _other);
}

void ot::Plot1DAxisCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Min", m_min, _allocator);
	_object.AddMember("Max", m_max, _allocator);
	_object.AddMember("Quantity", JsonString(toString(m_axisQuantity), _allocator), _allocator);
	_object.AddMember("LogScale", m_isLogScale, _allocator);
	_object.AddMember("AutoScale", m_isAutoScale, _allocator);
	_object.AddMember("AutoDetectAxisLabel", m_autoDetermineAxisLabel, _allocator);
	_object.AddMember("AxisLabel",ot::JsonString( m_axisLabel,_allocator), _allocator);
}

void ot::Plot1DAxisCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_min = json::getDouble(_object, "Min");
	m_max = json::getDouble(_object, "Max");
	m_axisQuantity = stringToAxisQuantity(json::getString(_object, "Quantity"));
	m_isLogScale = json::getBool(_object, "LogScale");
	m_isAutoScale = json::getBool(_object, "AutoScale");
	m_autoDetermineAxisLabel = json::getBool(_object, "AutoDetectAxisLabel");
	m_axisLabel = json::getString(_object, "AxisLabel");
}
