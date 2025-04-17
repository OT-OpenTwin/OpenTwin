//! @file Plot1DCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Plot1DCfg.h"

std::string ot::Plot1DCfg::plotTypeToString(PlotType _type) {
	switch (_type) {
	case ot::Plot1DCfg::Cartesian: return "Cartesian";
	case ot::Plot1DCfg::Polar: return "Polar";
	default:
		OT_LOG_EAS("Unknown plot type (" + std::to_string((int)_type) + ")");
		return "Cartesian";
	}
}

ot::Plot1DCfg::PlotType ot::Plot1DCfg::stringToPlotType(const std::string& _type) {
	if (_type == Plot1DCfg::plotTypeToString(Plot1DCfg::Cartesian)) return Plot1DCfg::Cartesian;
	else if (_type == Plot1DCfg::plotTypeToString(Plot1DCfg::Polar)) return Plot1DCfg::Polar;
	else {
		OT_LOG_EAS("Unknown plot type \"" + _type + "\"");
		return Plot1DCfg::Cartesian;
	}
}

std::string ot::Plot1DCfg::axisQuantityToString(AxisQuantity _quantity) {
	switch (_quantity) {
	case ot::Plot1DCfg::Magnitude: return "Magnitude";
	case ot::Plot1DCfg::Phase: return "Phase";
	case ot::Plot1DCfg::Real: return "Real";
	case ot::Plot1DCfg::Imaginary: return "Imaginary";
	case ot::Plot1DCfg::Complex: return "Complex";
	default:
		OT_LOG_EAS("Unknown axis quantity (" + std::to_string((int)_quantity) + ")");
		return "Magnitude";
	}
}

ot::Plot1DCfg::AxisQuantity ot::Plot1DCfg::stringToAxisQuantity(const std::string& _quantity) {
	if (_quantity == Plot1DCfg::axisQuantityToString(Plot1DCfg::Magnitude)) return Plot1DCfg::Magnitude;
	else if (_quantity == Plot1DCfg::axisQuantityToString(Plot1DCfg::Phase)) return Plot1DCfg::Phase;
	else if (_quantity == Plot1DCfg::axisQuantityToString(Plot1DCfg::Real)) return Plot1DCfg::Real;
	else if (_quantity == Plot1DCfg::axisQuantityToString(Plot1DCfg::Imaginary)) return Plot1DCfg::Imaginary;
	else if (_quantity == Plot1DCfg::axisQuantityToString(Plot1DCfg::Complex)) return Plot1DCfg::Complex;
	else {
		OT_LOG_EAS("Unknown axis quantity \"" + _quantity + "\"");
		return Plot1DCfg::Magnitude;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::Plot1DCfg::Plot1DCfg() : 
	WidgetViewBase(WidgetViewBase::View1D), m_type(Plot1DCfg::Cartesian), m_axisQuantity(Plot1DCfg::Real),
	m_gridVisible(true), m_gridWidth(1.), m_isHidden(false), m_legendVisible(true)
{}

ot::Plot1DCfg::Plot1DCfg(const Plot1DCfg& _other)
	: WidgetViewBase(WidgetViewBase::View1D)
{
	*this = _other;
}

ot::Plot1DCfg::~Plot1DCfg() {}

void ot::Plot1DCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	WidgetViewBase::addToJsonObject(_object, _allocator);

	_object.AddMember("ProjectName", JsonString(m_projectName, _allocator), _allocator);
	_object.AddMember("Type", JsonString(this->plotTypeToString(m_type), _allocator), _allocator);
	_object.AddMember("AxisQuantity", JsonString(this->axisQuantityToString(m_axisQuantity), _allocator), _allocator);

	_object.AddMember("GridVisible", m_gridVisible, _allocator);
	JsonObject gridColorObject;
	m_gridColor.addToJsonObject(gridColorObject, _allocator);
	_object.AddMember("GridColor", gridColorObject, _allocator);
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

	_object.AddMember("XAxisParameter", ot::JsonString(m_xAxisParameter, _allocator), _allocator);

	_object.AddMember("UseLimitOfCurves", m_useLimit, _allocator);
	_object.AddMember("NbCurveLimit", m_curveLimit, _allocator);

	JsonArray allQueries;
	for(const ValueComparisionDefinition& query : m_queries)
	{
		JsonObject querySerialised;
		query.addToJsonObject(querySerialised, _allocator);
		allQueries.PushBack(querySerialised, _allocator);
	}
	_object.AddMember("Queries", allQueries, _allocator);

}

void ot::Plot1DCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	WidgetViewBase::setFromJsonObject(_object);

	m_projectName = json::getString(_object, "ProjectName");
	m_type = this->stringToPlotType(json::getString(_object, "Type"));
	m_axisQuantity = this->stringToAxisQuantity(json::getString(_object, "AxisQuantity"));

	m_gridVisible = json::getBool(_object, "GridVisible");
	m_gridColor.setFromJsonObject(json::getObject(_object, "GridColor"));
	m_gridWidth = json::getDouble(_object, "GridWidth");

	m_isHidden = json::getBool(_object, "Hidden");
	m_legendVisible = json::getBool(_object, "LegendVisible");

	m_treeIcons.setFromJsonObject(json::getObject(_object, "OldTreeIcons"));

	m_xAxis.setFromJsonObject(json::getObject(_object, "XAxis"));
	m_yAxis.setFromJsonObject(json::getObject(_object, "YAxis"));

	m_xAxisParameter = ot::json::getString(_object, "XAxisParameter");

	const auto& allQueries = ot::json::getArray(_object, "Queries");
	for (uint32_t i = 0; i < allQueries.Size(); i++)
	{
		const auto& query = ot::json::getObject(allQueries, i);
		ValueComparisionDefinition valueDescription;
		valueDescription.setFromJsonObject(query);
		m_queries.push_back(valueDescription);
	}

	m_useLimit = ot::json::getBool(_object,"UseLimitOfCurves");
	m_curveLimit = ot::json::getInt(_object, "NbCurveLimit");
}

bool ot::Plot1DCfg::operator==(const Plot1DCfg& _other) const {
	return (WidgetViewBase::operator==(_other)) &&
		(m_projectName == _other.m_projectName) &&
		(m_type == _other.m_type) &&
		(m_axisQuantity == _other.m_axisQuantity) &&

		(m_gridVisible == _other.m_gridVisible) &&
		(m_gridColor == _other.m_gridColor) &&
		(m_gridWidth == _other.m_gridWidth) &&

		(m_isHidden == _other.m_isHidden) &&
		(m_legendVisible == _other.m_legendVisible) &&

		(m_treeIcons == _other.m_treeIcons) &&

		(m_xAxis == _other.m_xAxis) &&
		(m_yAxis == _other.m_yAxis);
}

bool ot::Plot1DCfg::operator!=(const Plot1DCfg& _other) const {
	return !Plot1DCfg::operator==(_other);
}


void ot::Plot1DCfg::setQueries(std::list<ValueComparisionDefinition>& _queries)
{
	m_queries = _queries;
}
