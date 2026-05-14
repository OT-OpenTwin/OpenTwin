// @otlicense
// File: Plot1DCurveCfg.cpp
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
#include "OTCore/Logging/Logger.h"
#include "OTGui/Painter/FillPainter2D.h"
#include "OTGui/Plot/Plot1DCurveCfg.h"
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTGui/Painter/StyleRefPainter2D.h"

std::string ot::Plot1DCurveCfg::toString(Symbol _symbol)
{
	switch (_symbol)
	{
	case ot::Plot1DCurveCfg::NoSymbol: return "None";
	case ot::Plot1DCurveCfg::Circle: return "Circle";
	case ot::Plot1DCurveCfg::Square: return "Square";
	case ot::Plot1DCurveCfg::Diamond: return "Diamond";
	case ot::Plot1DCurveCfg::TriangleUp: return "Triangle Up";
	case ot::Plot1DCurveCfg::TriangleDown: return "Triangle Down";
	case ot::Plot1DCurveCfg::TriangleLeft: return "Triangle Left";
	case ot::Plot1DCurveCfg::TriangleRight: return "Triangle Right";
	case ot::Plot1DCurveCfg::Cross: return "Cross (+)";
	case ot::Plot1DCurveCfg::XCross: return "X Cross (X)";
	case ot::Plot1DCurveCfg::HLine: return "Horizontal Line";
	case ot::Plot1DCurveCfg::VLine: return "Vertical Line";
	case ot::Plot1DCurveCfg::Star6: return "Star 6";
	case ot::Plot1DCurveCfg::Star8: return "Star 8";
	case ot::Plot1DCurveCfg::Hexagon: return "Hexagon";
	default:
		OT_LOG_E("Unknown plot curve symbol (" + std::to_string(static_cast<int>(_symbol)) + ")");
		return "None";
	}
}

ot::Plot1DCurveCfg::Symbol ot::Plot1DCurveCfg::stringToSymbol(const std::string& _symbol)
{
	if (_symbol == toString(NoSymbol)) return NoSymbol;
	else if (_symbol == toString(Circle)) return Circle;
	else if (_symbol == toString(Square)) return Square;
	else if (_symbol == toString(Diamond)) return Diamond;
	else if (_symbol == toString(TriangleUp)) return TriangleUp;
	else if (_symbol == toString(TriangleDown)) return TriangleDown;
	else if (_symbol == toString(TriangleLeft)) return TriangleLeft;
	else if (_symbol == toString(TriangleRight)) return TriangleRight;
	else if (_symbol == toString(Cross)) return Cross;
	else if (_symbol == toString(XCross)) return XCross;
	else if (_symbol == toString(HLine)) return HLine;
	else if (_symbol == toString(VLine)) return VLine;
	else if (_symbol == toString(Star6)) return Star6;
	else if (_symbol == toString(Star8)) return Star8;
	else if (_symbol == toString(Hexagon)) return Hexagon;
	else
	{
		OT_LOG_E("Unknown plot curve symbol \"" + _symbol + "\"");
		return NoSymbol;
	}
}

std::string ot::Plot1DCurveCfg::toString(CurveNamingBehavior _behavior)
{
	switch (_behavior)
	{
	case ot::Plot1DCurveCfg::Custom: return "Custom";
	case ot::Plot1DCurveCfg::Name: return "Name";
	case ot::Plot1DCurveCfg::NameSeries: return "Name, Series";
	case ot::Plot1DCurveCfg::NameSeriesQuantity: return "Name, Series, Quantity";
	case ot::Plot1DCurveCfg::NameQuantity: return "Name, Quantity";
	default:
		OT_LOG_E("Unknown curve naming behavior (" + std::to_string(static_cast<int>(_behavior)) + ")");
		return "Custom";
	}
}

ot::Plot1DCurveCfg::CurveNamingBehavior ot::Plot1DCurveCfg::stringToCurveNamingBehavior(const std::string& _behavior)
{
	if (_behavior == toString(CurveNamingBehavior::Custom)) return CurveNamingBehavior::Custom;
	else if (_behavior == toString(CurveNamingBehavior::Name)) return CurveNamingBehavior::Name;
	else if (_behavior == toString(CurveNamingBehavior::NameSeries)) return CurveNamingBehavior::NameSeries;
	else if (_behavior == toString(CurveNamingBehavior::NameSeriesQuantity)) return CurveNamingBehavior::NameSeriesQuantity;
	else if (_behavior == toString(CurveNamingBehavior::NameQuantity)) return CurveNamingBehavior::NameQuantity;
	else
	{
		OT_LOG_E("Unknown curve naming behavior \"" + _behavior + "\"");
		return CurveNamingBehavior::Custom;
	}
}

std::list<std::string> ot::Plot1DCurveCfg::getAllCurveNamingBehaviorStrings()
{
	return std::list<std::string>{
		toString(CurveNamingBehavior::Custom),
		toString(CurveNamingBehavior::Name),
		toString(CurveNamingBehavior::NameSeries),
		toString(CurveNamingBehavior::NameSeriesQuantity),
		toString(CurveNamingBehavior::NameQuantity)
	};
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

ot::Plot1DCurveCfg::Plot1DCurveCfg() :
	Plot1DCurveCfg(0, 0, std::string())
{
}

ot::Plot1DCurveCfg::Plot1DCurveCfg(UID _id, UID _version, const std::string& _name) :
	BasicEntityInformation(_name, _id, _version), m_navigationId(0), m_visible(true), m_dimmed(false),
	m_linePen(1., new StyleRefPainter2D(ColorStyleValueEntry::PlotCurve)),
	m_pointSize(5), m_pointInterval(1), m_pointSymbol(NoSymbol), m_pointColorFromCurve(false),
	m_matrixIndex(-1),
	m_pointFillPainter(new StyleRefPainter2D(ColorStyleValueEntry::PlotCurveSymbol)),
	m_pointOulinePen(1., new StyleRefPainter2D(ColorStyleValueEntry::PlotCurveSymbol))
{
	m_linePen.setCap(LineCapStyle::RoundCap);
	m_linePen.setJoinStyle(LineJoinStyle::RoundJoin);

	m_pointOulinePen.setCap(LineCapStyle::RoundCap);
	m_pointOulinePen.setJoinStyle(LineJoinStyle::RoundJoin);
}

ot::Plot1DCurveCfg::~Plot1DCurveCfg()
{
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Opearator

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(const BasicEntityInformation& _other)
{
	BasicEntityInformation::operator=(_other);
	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::Plot1DCurveCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	BasicEntityInformation::addToJsonObject(_object, _allocator);

	_object.AddMember("NavID", m_navigationId, _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);

	_object.AddMember("Visible", m_visible, _allocator);
	_object.AddMember("Dimmed", m_dimmed, _allocator);

	_object.AddMember("LinePen", JsonObject(m_linePen, _allocator), _allocator);

	_object.AddMember("PointSymbol", JsonString(Plot1DCurveCfg::toString(m_pointSymbol), _allocator), _allocator);
	_object.AddMember("PointSize", m_pointSize, _allocator);
	_object.AddMember("PointInterval", m_pointInterval, _allocator);
	_object.AddMember("PointColorFromCurve", m_pointColorFromCurve, _allocator);
	_object.AddMember("PointOutlinePen", JsonObject(m_pointOulinePen, _allocator), _allocator);
	_object.AddMember("PointFillPainter", JsonObject(m_pointFillPainter, _allocator), _allocator);

	_object.AddMember("DataAccess", JsonObject(m_dataAccessConfig, _allocator), _allocator);

	_object.AddMember("ToolTip", JsonString(m_toolTip, _allocator), _allocator);
	_object.AddMember("MatrixIndex", m_matrixIndex, _allocator);

	_object.AddMember("NamingBehavior", JsonString(toString(m_curveNamingBehavior), _allocator), _allocator);
	_object.AddMember("DisplayDependencyDiff", m_displayDependencyDifference, _allocator);
}

void ot::Plot1DCurveCfg::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	BasicEntityInformation::setFromJsonObject(_object);

	m_navigationId = json::getUInt64(_object, "NavID");
	m_title = json::getString(_object, "Title");

	m_visible = json::getBool(_object, "Visible");
	m_dimmed = json::getBool(_object, "Dimmed");

	m_linePen.setFromJsonObject(json::getObject(_object, "LinePen"));

	m_pointSymbol = Plot1DCurveCfg::stringToSymbol(json::getString(_object, "PointSymbol"));
	m_pointSize = json::getInt(_object, "PointSize");
	m_pointInterval = json::getInt(_object, "PointInterval");
	m_pointColorFromCurve = json::getBool(_object, "PointColorFromCurve");
	m_pointOulinePen.setFromJsonObject(json::getObject(_object, "PointOutlinePen"));
	m_pointFillPainter.setFromJsonObject(json::getObject(_object, "PointFillPainter"));

	m_dataAccessConfig.setFromJsonObject(json::getObject(_object, "DataAccess"));

	m_toolTip = json::getString(_object, "ToolTip");
	m_matrixIndex = json::getUInt(_object, "MatrixIndex");

	m_curveNamingBehavior = stringToCurveNamingBehavior(json::getString(_object, "NamingBehavior"));
	m_displayDependencyDifference = json::getBool(_object, "DisplayDependencyDiff");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::Plot1DCurveCfg::setPointFillColor(DefaultColor _color)
{
	this->setPointFillPainter(new FillPainter2D(_color));
}

void ot::Plot1DCurveCfg::setPointFillColor(const Color& _color)
{
	this->setPointFillPainter(new FillPainter2D(_color));
}

void ot::Plot1DCurveCfg::setPointFillColor(ColorStyleValueEntry _styleReference)
{
	this->setPointFillPainter(new StyleRefPainter2D(_styleReference));
}

void ot::Plot1DCurveCfg::setPointFillPainter(Painter2D* _painter)
{
	m_pointFillPainter.setPainter(_painter);
}
