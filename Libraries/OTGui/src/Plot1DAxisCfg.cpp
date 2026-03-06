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
#include "OTGui/Plot1DCfg.h"
#include "OTGui/Plot1DAxisCfg.h"

std::string ot::Plot1DAxisCfg::toString(AxisQuantity _quantity) 
{
	switch (_quantity) 
	{
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

ot::Plot1DAxisCfg::AxisQuantity ot::Plot1DAxisCfg::stringToAxisQuantity(const std::string& _quantity) 
{
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

std::string ot::Plot1DAxisCfg::toString(QuantityScalingFlag _scaling)
{
	switch (_scaling)
	{
	case QuantityScalingFlag::NoQuantityScaling: return "No Scaling";
	case QuantityScalingFlag::DB10: return "dB 10";
	case QuantityScalingFlag::DB20: return "dB 20";
	default:
		OT_LOG_EAS("Unknown quantity scaling (" + std::to_string((int)_scaling) + ")");
		return "No Scaling";
	};
}

ot::Plot1DAxisCfg::QuantityScalingFlag ot::Plot1DAxisCfg::stringToQuantityScalingFlag(const std::string& _scaling)
{
	if (_scaling == toString(QuantityScalingFlag::NoQuantityScaling)) return QuantityScalingFlag::NoQuantityScaling;
	else if (_scaling == toString(QuantityScalingFlag::DB10)) return QuantityScalingFlag::DB10;
	else if (_scaling == toString(QuantityScalingFlag::DB20)) return QuantityScalingFlag::DB20;
	else {
		OT_LOG_EAS("Unknown quantity scaling \"" + _scaling + "\"");
		return QuantityScalingFlag::NoQuantityScaling;
	}
}

std::list<std::string> ot::Plot1DAxisCfg::getAxisQuantityStringList() 
{
	return std::list<std::string>({
		toString(AxisQuantity::XData),
		toString(AxisQuantity::Real),
		toString(AxisQuantity::Imaginary),
		toString(AxisQuantity::Magnitude),
		toString(AxisQuantity::Phase),
	});
}

std::list<std::string> ot::Plot1DAxisCfg::getQuantityScalingStringList()
{
	return std::list<std::string>({
		toString(QuantityScalingFlag::NoQuantityScaling),
		toString(QuantityScalingFlag::DB10),
		toString(QuantityScalingFlag::DB20)
		});
}

ot::Plot1DAxisCfg::Plot1DAxisCfg()
	: m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max())
{}

ot::Plot1DAxisCfg::~Plot1DAxisCfg() {}


bool ot::Plot1DAxisCfg::operator==(const Plot1DAxisCfg& _other) const
{
	return
		m_min == _other.m_min &&
		m_max == _other.m_max &&

		m_axisLabel == _other.m_axisLabel &&
		
		m_axisScaling == _other.m_axisScaling &&
		m_axisQuantity == _other.m_axisQuantity &&
		m_quantityScaling == _other.m_quantityScaling
		;
}

bool ot::Plot1DAxisCfg::operator!=(const Plot1DAxisCfg& _other) const
{
	return !(*this == _other);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::Plot1DAxisCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const 
{
	_object.AddMember("Min", m_min, _allocator);
	_object.AddMember("Max", m_max, _allocator);

	_object.AddMember("AxisLabel", ot::JsonString(m_axisLabel, _allocator), _allocator);
	
	_object.AddMember("AxisQuantity", JsonString(toString(m_axisQuantity), _allocator), _allocator);
	_object.AddMember("AxisScale", static_cast<uint32_t>(m_axisScaling), _allocator);
	_object.AddMember("QuantityScale", static_cast<uint32_t>(m_quantityScaling), _allocator);
}

void ot::Plot1DAxisCfg::setFromJsonObject(const ot::ConstJsonObject& _object) 
{
	m_min = json::getDouble(_object, "Min");
	m_max = json::getDouble(_object, "Max");

	m_axisLabel = json::getString(_object, "AxisLabel");
	
	m_axisQuantity = stringToAxisQuantity(json::getString(_object, "AxisQuantity"));
	m_axisScaling = static_cast<AxisScaling>(json::getUInt(_object, "AxisScale"));
	m_quantityScaling = static_cast<QuantityScaling>(json::getUInt(_object, "QuantityScale"));
}

std::string ot::Plot1DAxisCfg::getDisplayAxisLabel(const Plot1DCfg& _plotCfg) const
{
	std::string result;

	if (!m_autoDetermineAxisLabel)
	{
		result = m_axisLabel;
	}
	else
	{
		switch (m_axisQuantity)
		{
		case ot::Plot1DAxisCfg::Undefined: break;
		case ot::Plot1DAxisCfg::XData: result.append(getXDataLabel(_plotCfg)); break;
		case ot::Plot1DAxisCfg::Magnitude: result.append(getYDataLabel(_plotCfg) + " Magnitude"); break;
		case ot::Plot1DAxisCfg::Phase: result.append(getYDataLabel(_plotCfg) + " Phase"); break;
		case ot::Plot1DAxisCfg::Real: result.append(getYDataLabel(_plotCfg) + " Real"); break;
		case ot::Plot1DAxisCfg::Imaginary: result.append(getYDataLabel(_plotCfg) + " Imaginary"); break;
		default:
			OT_LOG_E("Unknown axis quantity (" + std::to_string((int)m_axisQuantity) + ")");
			break;
		}

		if (m_quantityScaling.has(QuantityScalingFlag::DB10)) result.append(" (dB 10)");
		if (m_quantityScaling.has(QuantityScalingFlag::DB20)) result.append(" (dB 20)");

	}
	return result;
}

std::string ot::Plot1DAxisCfg::getXDataLabel(const Plot1DCfg& _plotCfg) const
{
	std::string result = _plotCfg.getDataLabelX();

	if (!_plotCfg.getUnitLabelX().empty())
	{
		result.append(" " + _plotCfg.getUnitLabelX());
	}

	return result;
}

std::string ot::Plot1DAxisCfg::getYDataLabel(const Plot1DCfg& _plotCfg) const
{
	std::string result = _plotCfg.getDataLabelY();

	if (!_plotCfg.getUnitLabelY().empty())
	{
		result.append(" " + _plotCfg.getUnitLabelY());
	}

	return result;
}
