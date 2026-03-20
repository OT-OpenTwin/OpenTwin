// @otlicense
// File: AbstractPlot.cpp
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
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/AbstractPlot.h"

ot::AbstractPlot::AbstractPlot(PlotBase* _ownerPlot) :
	m_axisXBottom(nullptr), m_axisXTop(nullptr), m_axisYLeft(nullptr), m_axisYRight(nullptr), m_owner(_ownerPlot)
{
}

ot::AbstractPlot::~AbstractPlot() {}

const ot::Plot1DCfg& ot::AbstractPlot::getConfig() const
{
	const PlotBase* base = getOwner();
	OTAssertNullptr(base);
	return base->getConfig();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Axis

void ot::AbstractPlot::setPlotAxis(AbstractPlotAxis* _axisXBottom, AbstractPlotAxis* _axisXTop, AbstractPlotAxis* _axisYLeft, AbstractPlotAxis* _axisYRight)
{
	m_axisXBottom = _axisXBottom;
	m_axisXTop = _axisXTop;
	m_axisYLeft = _axisYLeft;
	m_axisYRight = _axisYRight;
}

bool ot::AbstractPlot::hasPlotAxis(AbstractPlotAxis::AxisID _id) const
{
	switch (_id)
	{
	case ot::AbstractPlotAxis::yLeft:   return m_axisYLeft != nullptr;
	case ot::AbstractPlotAxis::yRight:  return m_axisYRight != nullptr;
	case ot::AbstractPlotAxis::xBottom: return m_axisXBottom != nullptr;
	case ot::AbstractPlotAxis::xTop:    return m_axisXTop != nullptr;
	default:
		OT_LOG_EAS("Invalid plot axis (" + std::to_string(_id) + ")");
		return false;
	}
}

ot::AbstractPlotAxis* ot::AbstractPlot::getPlotAxis(AbstractPlotAxis::AxisID _id)
{
	ot::AbstractPlotAxis* result = nullptr;

	switch (_id)
	{
	case AbstractPlotAxis::yLeft:
		result = m_axisYLeft;
		break;

	case AbstractPlotAxis::yRight:
		result = m_axisYRight;
		break;

	case AbstractPlotAxis::xBottom:
		result = m_axisXBottom;
		break;

	case AbstractPlotAxis::xTop:
		result = m_axisXTop;
		break;

	default:
		const std::string msg = "Invalid plot axis (" + std::to_string(_id) + ")";
		OT_LOG_EAS(msg);
		throw Exception::InvalidArgument(msg);
	}

	if (result == nullptr)
	{
		OT_LOG_EAS("Plot axis not set for axis id " + std::to_string(_id));
		throw Exception::ObjectNotFound("Plot axis not set for axis id " + std::to_string(_id));
	}

	return result;
}

void ot::AbstractPlot::setPlotAxisTitle(AbstractPlotAxis::AxisID _axis, const QString& _title)
{
	getPlotAxis(_axis)->setTitle(_title);
}

void ot::AbstractPlot::setPlotAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _isAutoScale)
{
	getPlotAxis(_axis)->setIsAutoScale(_isAutoScale);
}

void ot::AbstractPlot::setPlotAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _isLogScale)
{
	getPlotAxis(_axis)->setIsLogScale(_isLogScale);
}

void ot::AbstractPlot::setPlotAxisMin(AbstractPlotAxis::AxisID _axis, double _minValue)
{
	getPlotAxis(_axis)->setMin(_minValue);
}

void ot::AbstractPlot::setPlotAxisMax(AbstractPlotAxis::AxisID _axis, double _maxValue)
{
	getPlotAxis(_axis)->setMax(_maxValue);
}

void ot::AbstractPlot::repaintPlotAxis(AbstractPlotAxis::AxisID _axis)
{
	getPlotAxis(_axis)->updateAxis();
}

