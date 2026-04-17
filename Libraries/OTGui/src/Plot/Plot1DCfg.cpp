// @otlicense
// File: Plot1DCfg.cpp
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

#pragma once

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Plot/Plot1DCfg.h"
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"

std::string ot::Plot1DCfg::toString(PlotType _type) {
	switch (_type) 
	{
	case Plot1DCfg::Cartesian: 
		return ComplexNumbers::getFormatString(ComplexNumberFormat::Cartesian);
	case Plot1DCfg::Polar: 
		return ComplexNumbers::getFormatString(ComplexNumberFormat::Polar);
	default:
		OT_LOG_E("Unknown plot type (" + std::to_string((int)_type) + ")");
		return "Cartesian";
	}
}

ot::Plot1DCfg::PlotType ot::Plot1DCfg::stringToPlotType(const std::string& _type) 
{
	if (ComplexNumbers::getFormatFromString(_type) == ComplexNumberFormat::Cartesian)
	{
		return Plot1DCfg::Cartesian;
	}
	else if (ComplexNumbers::getFormatFromString(_type) == ComplexNumberFormat::Polar)
	{
		return Plot1DCfg::Polar;
	}
	else 
	{
		OT_LOG_E("Unknown plot type \"" + _type + "\"");
		return Plot1DCfg::Cartesian;
	}
}

std::list<std::string> ot::Plot1DCfg::getPlotTypeStringList() {
	return std::list<std::string>(
		{
			toString(Plot1DCfg::Cartesian),
			toString(Plot1DCfg::Polar)
		}
	);
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::Plot1DCfg::Plot1DCfg() : 
	WidgetViewBase(WidgetViewBase::View1D, WidgetViewBase::ViewIsCentral | WidgetViewBase::ViewIsCloseable | WidgetViewBase::ViewIsPinnable | WidgetViewBase::ViewNameAsTitle | WidgetViewBase::ViewCloseOnEmptySelection)
{
	m_xAxis.setQuantityComponent(Plot1DAxisCfg::AxisQuantityComponent::XData);
	m_yAxis.setQuantityComponent(Plot1DAxisCfg::AxisQuantityComponent::Real);
}

ot::Plot1DCfg::~Plot1DCfg() {}

void ot::Plot1DCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	OTAssert(invariant(), "Plot1DCfg object is in an invalid state before serialization.");

	WidgetViewBase::addToJsonObject(_object, _allocator);

	_object.AddMember("CollectionName", JsonString(m_collectionName, _allocator), _allocator);
	_object.AddMember("Type", JsonString(this->toString(m_type), _allocator), _allocator);
	
	_object.AddMember("GridVisible", m_gridVisible, _allocator);
	_object.AddMember("GridColor", JsonObject(m_gridColor, _allocator), _allocator);
	_object.AddMember("GridWidth", m_gridWidth, _allocator);

	_object.AddMember("Hidden", m_isHidden, _allocator);
	_object.AddMember("LegendVisible", m_legendVisible, _allocator);

	JsonObject treeIconsObject;
	m_treeIcons.addToJsonObject(treeIconsObject, _allocator);
	_object.AddMember("OldTreeIcons", treeIconsObject, _allocator);

	JsonObject xAxisObject;
	m_xAxis.addToJsonObject(xAxisObject, _allocator);
	_object.AddMember("XAxis", xAxisObject, _allocator);

	JsonObject yAxisObject;
	m_yAxis.addToJsonObject(yAxisObject, _allocator);
	_object.AddMember("YAxis", yAxisObject, _allocator);

	_object.AddMember("DataLabelX", JsonString(m_dataLabelX, _allocator), _allocator);
	_object.AddMember("DataLabelY", JsonString(m_dataLabelY, _allocator), _allocator);

	_object.AddMember("UnitLabelX", JsonString(m_unitLabelX, _allocator), _allocator);
	_object.AddMember("UnitLabelY", JsonString(m_unitLabelY, _allocator), _allocator);

	_object.AddMember("xAxisParameter", JsonString(m_xAxisParameter, _allocator), _allocator);
	
	JsonArray secondParameter(m_secondaryParameter, _allocator);
	_object.AddMember("SecondaryParameter", secondParameter, _allocator);

	_object.AddMember("QueryQuantity", JsonString(m_queryQuantity, _allocator), _allocator);

	_object.AddMember("UseLimitOfCurves", m_useLimit, _allocator);
	_object.AddMember("NbCurveLimit", m_curveLimit, _allocator);

	_object.AddMember("ShowEntireMatrix", m_showEntireMatrix, _allocator);
	_object.AddMember("ShowMatrixRow", m_showMatrixRowEntry, _allocator);
	_object.AddMember("ShowMatrixColumn", m_showMatrixColumnEntry, _allocator);

	JsonArray allQueries;
	for(const ValueComparisonDescription& query : m_queries)
	{
		JsonObject querySerialised;
		query.addToJsonObject(querySerialised, _allocator);
		allQueries.PushBack(querySerialised, _allocator);
	}
	_object.AddMember("Queries", allQueries, _allocator);

	_object.AddMember("PolarDegreeOrigin", m_polarDegreeOrigin, _allocator);

	JsonArray fixedDatasetLabelInfosArr;
	for (const auto& labelInfo : m_fixedDatasetLabelInfos)
	{
		JsonObject labelInfoObj;
		labelInfoObj.AddMember("Label", JsonString(labelInfo.label, _allocator), _allocator);
		labelInfoObj.AddMember("Behavior", static_cast<uint32_t>(labelInfo.behavior), _allocator);
		fixedDatasetLabelInfosArr.PushBack(labelInfoObj, _allocator);
	}
	_object.AddMember("FixedDatasetLabelInfos", fixedDatasetLabelInfosArr, _allocator);
}

void ot::Plot1DCfg::setFromJsonObject(const ConstJsonObject& _object) {
	WidgetViewBase::setFromJsonObject(_object);

	m_collectionName = json::getString(_object, "CollectionName");
	m_type = this->stringToPlotType(json::getString(_object, "Type"));
	
	m_gridVisible = json::getBool(_object, "GridVisible");
	setGridColor(Painter2DFactory::create(json::getObject(_object, "GridColor")));
	m_gridWidth = json::getDouble(_object, "GridWidth");

	m_isHidden = json::getBool(_object, "Hidden");
	m_legendVisible = json::getBool(_object, "LegendVisible");

	m_treeIcons.setFromJsonObject(json::getObject(_object, "OldTreeIcons"));

	m_xAxis.setFromJsonObject(json::getObject(_object, "XAxis"));
	m_yAxis.setFromJsonObject(json::getObject(_object, "YAxis"));

	m_dataLabelX = json::getString(_object, "DataLabelX");
	m_dataLabelY = json::getString(_object, "DataLabelY");

	m_unitLabelX = json::getString(_object, "UnitLabelX");
	m_unitLabelY = json::getString(_object, "UnitLabelY");

	m_xAxisParameter = json::getString(_object, "xAxisParameter");
	m_queryQuantity = json::getString(_object, "QueryQuantity");

	m_showEntireMatrix = json::getBool(_object, "ShowEntireMatrix");
	m_showMatrixRowEntry = json::getInt(_object, "ShowMatrixRow");
	m_showMatrixColumnEntry = json::getInt(_object, "ShowMatrixColumn");

	m_secondaryParameter = ot::json::getStringList(_object, "SecondaryParameter");

	const auto& allQueries = json::getArray(_object, "Queries");
	for (uint32_t i = 0; i < allQueries.Size(); i++)
	{
		const auto& query = json::getObject(allQueries, i);
		ValueComparisonDescription valueDescription;
		valueDescription.setFromJsonObject(query);
		m_queries.push_back(valueDescription);
	}

	m_useLimit = json::getBool(_object,"UseLimitOfCurves");
	m_curveLimit = json::getInt(_object, "NbCurveLimit");

	m_polarDegreeOrigin = json::getDouble(_object, "PolarDegreeOrigin");

	m_fixedDatasetLabelInfos.clear();
	for (const ConstJsonObject& labelInfoObj : json::getObjectList(_object, "FixedDatasetLabelInfos"))
	{
		DependencyLabelInfo labelInfo;
		labelInfo.label = json::getString(labelInfoObj, "Label");
		labelInfo.behavior = static_cast<Plot1DCfg::DependencyLabelBehavior>(json::getUInt(labelInfoObj, "Behavior"));
		m_fixedDatasetLabelInfos.push_back(labelInfo);
	}

	OTAssert(invariant(), "Plot1DCfg object is in an invalid state after deserialization.");
}

bool ot::Plot1DCfg::operator==(const Plot1DCfg& _other) const {
	return (WidgetViewBase::operator==(_other)) &&
		(m_collectionName == _other.m_collectionName) &&
		(m_type == _other.m_type) &&

		(m_gridVisible == _other.m_gridVisible) &&
		(m_gridColor->isEqualTo(*_other.m_gridColor)) &&
		(m_gridWidth == _other.m_gridWidth) &&

		(m_isHidden == _other.m_isHidden) &&
		(m_legendVisible == _other.m_legendVisible) &&

		(m_treeIcons == _other.m_treeIcons) &&

		(m_xAxisParameter == _other.m_xAxisParameter) &&
		(m_queryQuantity == _other.m_queryQuantity) &&

		(m_dataLabelX == _other.m_dataLabelX) &&
		(m_dataLabelY == _other.m_dataLabelY) &&

		(m_unitLabelX == _other.m_unitLabelX) &&
		(m_unitLabelY == _other.m_unitLabelY) &&

		(m_xAxis == _other.m_xAxis) &&
		(m_yAxis == _other.m_yAxis) &&

		(m_secondaryParameter == _other.m_secondaryParameter)&&

		(m_polarDegreeOrigin == _other.m_polarDegreeOrigin);


}

void ot::Plot1DCfg::setTupleSettings(const std::string& _tupleType, const std::string& _tupleFormat)
{
	m_tupleType = _tupleType;
	m_tupleFormat = _tupleFormat;
}


bool ot::Plot1DCfg::operator!=(const Plot1DCfg& _other) const {
	return !Plot1DCfg::operator==(_other);
}

bool ot::Plot1DCfg::invariant() const
{
	return m_polarDegreeOrigin > (-360.) && m_polarDegreeOrigin < 360.;
}
