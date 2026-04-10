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
#include "OTCore/Symbol.h"
#include "OTCore/Logging/Logger.h"
#include "OTGui/Plot/Plot1DCfg.h"
#include "OTGui/Plot/Plot1DAxisCfg.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"

std::string ot::Plot1DAxisCfg::toString(AxisQuantityComponent _quantity) 
{
	switch (_quantity) 
	{
	case AxisQuantityComponent::Undefined: 
		return "Undefined";
	case AxisQuantityComponent::XData: 
		return "X Data";
	case AxisQuantityComponent::Magnitude: 
		return ot::ComplexNumbers::getComponentMagnitude();
	case AxisQuantityComponent::Phase: 
		return ot::ComplexNumbers::getComponentPhase();
	case AxisQuantityComponent::Real: 
		return ot::ComplexNumbers::getComponentReal();
	case AxisQuantityComponent::Imaginary: 
		return ot::ComplexNumbers::getComponentImaginary();
	default:
		OT_LOG_E("Unknown axis quantity (" + std::to_string((int)_quantity) + ")");
		return "Undefined";
	}
}

ot::Plot1DAxisCfg::AxisQuantityComponent ot::Plot1DAxisCfg::stringToAxisQuantity(const std::string& _quantity) 
{
	if (_quantity == toString(AxisQuantityComponent::Undefined)) 
	{
		return AxisQuantityComponent::Undefined;
	}
	else if (_quantity == toString(AxisQuantityComponent::XData)) 
	{
		return AxisQuantityComponent::XData;
	}
	else if (_quantity == toString(AxisQuantityComponent::Magnitude)) 
	{
		return AxisQuantityComponent::Magnitude;
	}
	else if (_quantity == toString(AxisQuantityComponent::Phase)) 
	{
		return AxisQuantityComponent::Phase;
	}
	else if (_quantity == toString(AxisQuantityComponent::Real))
	{
		return AxisQuantityComponent::Real;
	}
	else if (_quantity == toString(AxisQuantityComponent::Imaginary)) 
	{
		return AxisQuantityComponent::Imaginary;
	}
	else {
		OT_LOG_E("Unknown axis quantity \"" + _quantity + "\"");
		return AxisQuantityComponent::Undefined;
	}
}

std::string ot::Plot1DAxisCfg::toString(ValueScalingFlag _scaling)
{
	switch (_scaling)
	{
	case ValueScalingFlag::NoQuantityScaling: return "No Scaling";
	case ValueScalingFlag::DB10: return "dB 10";
	case ValueScalingFlag::DB20: return "dB 20";
	default:
		OT_LOG_E("Unknown quantity scaling (" + std::to_string((int)_scaling) + ")");
		return "No Scaling";
	};
}

ot::Plot1DAxisCfg::ValueScalingFlag ot::Plot1DAxisCfg::stringToQuantityScalingFlag(const std::string& _scaling)
{
	if (_scaling == toString(ValueScalingFlag::NoQuantityScaling)) return ValueScalingFlag::NoQuantityScaling;
	else if (_scaling == toString(ValueScalingFlag::DB10)) return ValueScalingFlag::DB10;
	else if (_scaling == toString(ValueScalingFlag::DB20)) return ValueScalingFlag::DB20;
	else {
		OT_LOG_E("Unknown quantity scaling \"" + _scaling + "\"");
		return ValueScalingFlag::NoQuantityScaling;
	}
}

std::list<std::string> ot::Plot1DAxisCfg::getAxisQuantityStringList() 
{
	return std::list<std::string>({
		toString(AxisQuantityComponent::XData),
		toString(AxisQuantityComponent::Real),
		toString(AxisQuantityComponent::Imaginary),
		toString(AxisQuantityComponent::Magnitude),
		toString(AxisQuantityComponent::Phase),
	});
}

std::list<std::string> ot::Plot1DAxisCfg::getQuantityScalingStringList()
{
	return std::list<std::string>({
		toString(ValueScalingFlag::NoQuantityScaling),
		toString(ValueScalingFlag::DB10),
		toString(ValueScalingFlag::DB20)
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
		m_axisQuantityComponent == _other.m_axisQuantityComponent &&
		m_valueScaling == _other.m_valueScaling &&

		m_displayNumberFormat == _other.m_displayNumberFormat &&
		m_displayNumberPrecision == _other.m_displayNumberPrecision &&

		m_autoDetermineAxisLabel == _other.m_autoDetermineAxisLabel
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

	_object.AddMember("AutoDetermineAxisLabel", m_autoDetermineAxisLabel, _allocator);
	_object.AddMember("AxisLabel", ot::JsonString(m_axisLabel, _allocator), _allocator);
	
	_object.AddMember("AxisQuantityComponent", JsonString(toString(m_axisQuantityComponent), _allocator), _allocator);
	_object.AddMember("AxisScale", static_cast<uint32_t>(m_axisScaling), _allocator);
	_object.AddMember("QuantityScale", static_cast<uint32_t>(m_valueScaling), _allocator);

	_object.AddMember("DisplayNumberFormat", static_cast<uint32_t>(m_displayNumberFormat), _allocator);
	_object.AddMember("DisplayNumberPrecision", m_displayNumberPrecision, _allocator);
}

void ot::Plot1DAxisCfg::setFromJsonObject(const ot::ConstJsonObject& _object) 
{
	m_min = json::getDouble(_object, "Min");
	m_max = json::getDouble(_object, "Max");

	m_autoDetermineAxisLabel = json::getBool(_object, "AutoDetermineAxisLabel");
	m_axisLabel = json::getString(_object, "AxisLabel");
	
	m_axisQuantityComponent = stringToAxisQuantity(json::getString(_object, "AxisQuantityComponent"));
	m_axisScaling = static_cast<AxisScaling>(json::getUInt(_object, "AxisScale"));
	m_valueScaling = static_cast<ValueScaling>(json::getUInt(_object, "QuantityScale"));

	m_displayNumberPrecision = json::getInt(_object, "DisplayNumberPrecision");
	m_displayNumberFormat = static_cast<String::DisplayNumberFormat>(json::getUInt(_object, "DisplayNumberFormat"));
}

std::string ot::Plot1DAxisCfg::getDisplayLabel(const Plot1DCfg& _plotCfg) const
{
	std::string result;

	if (m_autoDetermineAxisLabel)
	{
		result = getQuantityLabel(_plotCfg) + " " + getUnitWithScalingLabel(_plotCfg);
	}
	else
	{
		result = m_axisLabel;
	}
	return result;
}

std::string ot::Plot1DAxisCfg::getValueDisplayString(double _value, const Plot1DCfg& _plotCfg) const
{
	return
		getQuantityLabel(_plotCfg) + 
		" = " +  String::numberToString(_value, m_displayNumberFormat, m_displayNumberPrecision) +
		" " + getUnitWithScalingLabel(_plotCfg);
}

std::string ot::Plot1DAxisCfg::getValueDisplayString(double _value, const Plot1DCfg& _plotCfg, const std::string& _unitPrefix) const
{
	return 
		getQuantityLabel(_plotCfg) + 
		" = " + String::numberToString(_value, m_displayNumberFormat, m_displayNumberPrecision) +
		" " + _unitPrefix + getUnitWithScalingLabel(_plotCfg);
}


std::string ot::Plot1DAxisCfg::getQuantityLabel(const Plot1DCfg& _plotCfg) const
{
	std::string result;

	if (!m_autoDetermineAxisLabel)
	{
		result = m_axisLabel;
	}
	else
	{
		// Determine axis type label based on plot type and axis quantity
		result = String::trim((m_axisQuantityComponent == Plot1DAxisCfg::XData ? _plotCfg.getDataLabelX() : _plotCfg.getDataLabelY()));

		if (!result.empty())
		{
			result.append(" ");
		}

		// Append the quantity name based on the axis quantity.
		switch (m_axisQuantityComponent)
		{
		case ot::Plot1DAxisCfg::Undefined: break;
		case ot::Plot1DAxisCfg::XData: break;
		case ot::Plot1DAxisCfg::Magnitude: result.append("Magnitude"); break;
		case ot::Plot1DAxisCfg::Phase: result.append("Phase"); break;
		case ot::Plot1DAxisCfg::Real: result.append("Real"); break;
		case ot::Plot1DAxisCfg::Imaginary: result.append("Imaginary"); break;
		default:
			OT_LOG_E("Unknown axis quantity (" + std::to_string((int)m_axisQuantityComponent) + ")");
			break;
		}
	}
	return result;
}

std::string ot::Plot1DAxisCfg::getUnitLabel(const Plot1DCfg& _plotCfg) const
{
	return (m_axisQuantityComponent == Plot1DAxisCfg::XData ? _plotCfg.getUnitLabelX() : _plotCfg.getUnitLabelY());
}

std::string ot::Plot1DAxisCfg::getUnitWithScalingLabel(const Plot1DCfg& _plotCfg) const
{
	std::string result = getUnitLabel(_plotCfg);
	if (m_valueScaling.has(ValueScalingFlag::DB10)) result.append((result.empty() ? "" : " ") + std::string("(dB 10)"));
	if (m_valueScaling.has(ValueScalingFlag::DB20)) result.append((result.empty() ? "" : " ") + std::string("(dB 20)"));
	return result;
}

std::string ot::Plot1DAxisCfg::getXDataLabelWithUnit(const Plot1DCfg& _plotCfg) const
{
	std::string result = _plotCfg.getDataLabelX();

	if (!_plotCfg.getUnitLabelX().empty())
	{
		result.append(" " + _plotCfg.getUnitLabelX());
	}

	return result;
}

std::string ot::Plot1DAxisCfg::getYDataLabelWithUnit(const Plot1DCfg& _plotCfg) const
{
	std::string result = _plotCfg.getDataLabelY();

	if (!_plotCfg.getUnitLabelY().empty())
	{
		result.append(" " + _plotCfg.getUnitLabelY());
	}

	return result;
}
