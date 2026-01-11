// @otlicense
// File: GraphicsDecorationCfg.cpp
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
#include "OTGui/Graphics/GraphicsDecorationCfg.h"
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTGui/Painter/StyleRefPainter2D.h"

// Static functions

std::string ot::GraphicsDecorationCfg::symbolToString(Symbol _symbol) {
	switch (_symbol) {
	case ot::GraphicsDecorationCfg::NoSymbol: return "No Symbol";
	case ot::GraphicsDecorationCfg::Triangle: return "Triangle";
	case ot::GraphicsDecorationCfg::FilledTriange: return "Filled Triangle";
	case ot::GraphicsDecorationCfg::Arrow: return "Arrow";
	case ot::GraphicsDecorationCfg::VArrow: return "V Arrow";
	case ot::GraphicsDecorationCfg::Circle: return "Circle";
	case ot::GraphicsDecorationCfg::FilledCircle: return "Filled Circle";
	case ot::GraphicsDecorationCfg::Square: return "Square";
	case ot::GraphicsDecorationCfg::FilledSquare: return "Filled Square";
	case ot::GraphicsDecorationCfg::Diamond: return "Diamond";
	case ot::GraphicsDecorationCfg::FilledDiamond: return "Filled Diamond";
	case ot::GraphicsDecorationCfg::Cross: return "Cross";
	default:
		OT_LOG_E("Unknown symbol (" + std::to_string(static_cast<int32_t>(_symbol)) + ")");
		return "No Symbol";
	}
}

ot::GraphicsDecorationCfg::Symbol ot::GraphicsDecorationCfg::stringToSymbol(const std::string& _symbol) {
	if (_symbol == symbolToString(Symbol::NoSymbol)) return Symbol::NoSymbol;
	else if (_symbol == symbolToString(Symbol::Triangle)) return Symbol::Triangle;
	else if (_symbol == symbolToString(Symbol::FilledTriange)) return Symbol::FilledTriange;
	else if (_symbol == symbolToString(Symbol::Arrow)) return Symbol::Arrow;
	else if (_symbol == symbolToString(Symbol::VArrow)) return Symbol::VArrow;
	else if (_symbol == symbolToString(Symbol::Circle)) return Symbol::Circle;
	else if (_symbol == symbolToString(Symbol::FilledCircle)) return Symbol::FilledCircle;
	else if (_symbol == symbolToString(Symbol::Square)) return Symbol::Square;
	else if (_symbol == symbolToString(Symbol::FilledSquare)) return Symbol::FilledSquare;
	else if (_symbol == symbolToString(Symbol::Diamond)) return Symbol::Diamond;
	else if (_symbol == symbolToString(Symbol::FilledDiamond)) return Symbol::FilledDiamond;
	else if (_symbol == symbolToString(Symbol::Cross)) return Symbol::Cross;
	else {
		OT_LOG_E("Unknown symbol \"" + _symbol + "\"");
		return Symbol::NoSymbol;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructors / Destructor / Assignment

ot::GraphicsDecorationCfg::GraphicsDecorationCfg(Symbol _symbol, bool _reverse)
	: m_reverse(_reverse), m_symbol(_symbol), m_size(5., 5.), m_fillPainter(nullptr)
{
	m_fillPainter = new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBackground);
	m_outlinePen.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemBorder));
}

ot::GraphicsDecorationCfg::GraphicsDecorationCfg(const GraphicsDecorationCfg& _other) 
	: m_reverse(_other.m_reverse), m_symbol(_other.m_symbol), m_size(_other.m_size), m_outlinePen(_other.m_outlinePen), m_fillPainter(nullptr)
{
	if (_other.m_fillPainter) {
		m_fillPainter = _other.m_fillPainter->createCopy();
	}
}

ot::GraphicsDecorationCfg::GraphicsDecorationCfg(GraphicsDecorationCfg&& _other) noexcept 
	: m_reverse(_other.m_reverse), m_symbol(_other.m_symbol), m_size(std::move(_other.m_size)), m_outlinePen(std::move(_other.m_outlinePen)), m_fillPainter(_other.m_fillPainter)
{
	_other.m_fillPainter = nullptr;
}

ot::GraphicsDecorationCfg::GraphicsDecorationCfg(const ConstJsonObject& _jsonObject)
	: m_reverse(false), m_symbol(Symbol::NoSymbol), m_size(5., 5.), m_fillPainter(nullptr)
{
	setFromJsonObject(_jsonObject);
}

ot::GraphicsDecorationCfg::~GraphicsDecorationCfg() {
	if (m_fillPainter) {
		delete m_fillPainter;
		m_fillPainter = nullptr;
	}
}

ot::GraphicsDecorationCfg& ot::GraphicsDecorationCfg::operator=(const GraphicsDecorationCfg& _other) {
	if (this != &_other) {
		m_reverse = _other.m_reverse;
		m_symbol = _other.m_symbol;
		m_size = _other.m_size;
		m_outlinePen = _other.m_outlinePen;

		if (m_fillPainter) {
			delete m_fillPainter;
			m_fillPainter = nullptr;
		}
		if (_other.m_fillPainter) {
			m_fillPainter = _other.m_fillPainter->createCopy();
		}
	}

	return *this;
}

ot::GraphicsDecorationCfg& ot::GraphicsDecorationCfg::operator=(GraphicsDecorationCfg&& _other) noexcept {
	if (this != &_other) {
		m_reverse = _other.m_reverse;
		m_symbol = _other.m_symbol;
		m_size = std::move(_other.m_size);
		m_outlinePen = std::move(_other.m_outlinePen);

		if (m_fillPainter) {
			delete m_fillPainter;
		}
		m_fillPainter = _other.m_fillPainter;
		_other.m_fillPainter = nullptr;
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::GraphicsDecorationCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Symbol", static_cast<int32_t>(m_symbol), _allocator);
	_object.AddMember("Reverse", m_reverse, _allocator);
	_object.AddMember("Size", JsonObject(m_size, _allocator), _allocator);
	_object.AddMember("OutlinePen", JsonObject(m_outlinePen, _allocator), _allocator);

	if (m_fillPainter) {
		_object.AddMember("FillPainter", JsonObject(m_fillPainter, _allocator), _allocator);
	}
	else {
		_object.AddMember("FillPainter", JsonNullValue(), _allocator);
	}
}

void ot::GraphicsDecorationCfg::setFromJsonObject(const ConstJsonObject& _object) {
	m_symbol = static_cast<Symbol>(json::getInt(_object, "Symbol"));
	m_reverse = json::getBool(_object, "Reverse");
	m_size.setFromJsonObject(json::getObject(_object, "Size"));
	m_outlinePen.setFromJsonObject(json::getObject(_object, "OutlinePen"));

	Painter2D* newPainter = nullptr;
	if (json::isObject(_object, "FillPainter")) {
		newPainter = Painter2DFactory::create(json::getObject(_object, "FillPainter"));

	}

	setFillPainter(newPainter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsDecorationCfg::setFillPainter(Painter2D* _painter) {
	if (m_fillPainter == _painter) {
		return;
	}

	if (m_fillPainter) {
		delete m_fillPainter;
		m_fillPainter = nullptr;
	}

	m_fillPainter = _painter;
}
