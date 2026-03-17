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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Plot/Plot1DCfg.h"
#include "OTGui/Plot/Plot1DAxisCfg.h"

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
		m_quantityScaling == _other.m_quantityScaling &&

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
	
	_object.AddMember("AxisQuantity", JsonString(toString(m_axisQuantity), _allocator), _allocator);
	_object.AddMember("AxisScale", static_cast<uint32_t>(m_axisScaling), _allocator);
	_object.AddMember("QuantityScale", static_cast<uint32_t>(m_quantityScaling), _allocator);

	_object.AddMember("DisplayNumberFormat", static_cast<uint32_t>(m_displayNumberFormat), _allocator);
	_object.AddMember("DisplayNumberPrecision", m_displayNumberPrecision, _allocator);
}

void ot::Plot1DAxisCfg::setFromJsonObject(const ot::ConstJsonObject& _object) 
{
	m_min = json::getDouble(_object, "Min");
	m_max = json::getDouble(_object, "Max");

	m_autoDetermineAxisLabel = json::getBool(_object, "AutoDetermineAxisLabel");
	m_axisLabel = json::getString(_object, "AxisLabel");
	
	m_axisQuantity = stringToAxisQuantity(json::getString(_object, "AxisQuantity"));
	m_axisScaling = static_cast<AxisScaling>(json::getUInt(_object, "AxisScale"));
	m_quantityScaling = static_cast<QuantityScaling>(json::getUInt(_object, "QuantityScale"));

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
		result = String::trim((m_axisQuantity == Plot1DAxisCfg::XData ? _plotCfg.getDataLabelX() : _plotCfg.getDataLabelY()));

		if (!result.empty())
		{
			result.append(" ");
		}

		// Append the quantity name based on the axis quantity.
		switch (m_axisQuantity)
		{
		case ot::Plot1DAxisCfg::Undefined: break;
		case ot::Plot1DAxisCfg::XData: break;
		case ot::Plot1DAxisCfg::Magnitude: result.append("Magnitude"); break;
		case ot::Plot1DAxisCfg::Phase: result.append("Phase"); break;
		case ot::Plot1DAxisCfg::Real: result.append("Real"); break;
		case ot::Plot1DAxisCfg::Imaginary: result.append("Imaginary"); break;
		default:
			OT_LOG_E("Unknown axis quantity (" + std::to_string((int)m_axisQuantity) + ")");
			break;
		}
	}
	return result;
}

std::string ot::Plot1DAxisCfg::getUnitLabel(const Plot1DCfg& _plotCfg) const
{
	return (m_axisQuantity == Plot1DAxisCfg::XData ? _plotCfg.getUnitLabelX() : _plotCfg.getUnitLabelY());
}

std::string ot::Plot1DAxisCfg::getUnitWithScalingLabel(const Plot1DCfg& _plotCfg) const
{
	std::string result = getUnitLabel(_plotCfg);
	if (m_quantityScaling.has(QuantityScalingFlag::DB10)) result.append((result.empty() ? "" : " ") + std::string("(dB 10)"));
	if (m_quantityScaling.has(QuantityScalingFlag::DB20)) result.append((result.empty() ? "" : " ") + std::string("(dB 20)"));
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
