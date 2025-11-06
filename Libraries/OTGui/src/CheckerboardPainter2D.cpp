// @otlicense
// File: CheckerboardPainter2D.cpp
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
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/CheckerboardPainter2D.h"

static ot::Painter2DFactoryRegistrar<ot::CheckerboardPainter2D> fillCfgRegistrar(OT_FactoryKey_CheckerboardPainter2D);

ot::CheckerboardPainter2D::CheckerboardPainter2D() 
	: m_cellSize(10, 10)
{
	m_primary = new FillPainter2D(ot::LightGray);
	m_secondary = new FillPainter2D(ot::Transparent);
}

ot::CheckerboardPainter2D::CheckerboardPainter2D(ot::DefaultColor _primaryColor, ot::DefaultColor _secondaryColor, const Size2D& _cellSize)
	: m_cellSize(_cellSize)
{
	m_primary = new FillPainter2D(_primaryColor);
	m_secondary = new FillPainter2D(_secondaryColor);
}

ot::CheckerboardPainter2D::CheckerboardPainter2D(const ot::Color& _primaryColor, const ot::Color& _secondaryColor, const Size2D& _cellSize)
	: m_cellSize(_cellSize)
{
	m_primary = new FillPainter2D(_primaryColor);
	m_secondary = new FillPainter2D(_secondaryColor);
}

ot::CheckerboardPainter2D::CheckerboardPainter2D(Painter2D* _primaryPainter, Painter2D* _secondaryPainter, const Size2D& _cellSize)
	: m_primary(_primaryPainter), m_secondary(_secondaryPainter), m_cellSize(_cellSize)
{}

ot::CheckerboardPainter2D::~CheckerboardPainter2D() {
	if (m_primary) {
		delete m_primary;
		m_primary = nullptr;
	}
	if (m_secondary) {
		delete m_secondary;
		m_secondary = nullptr;
	}
}

void ot::CheckerboardPainter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::Painter2D::addToJsonObject(_object, _allocator);

	JsonObject primaryObj;
	m_primary->addToJsonObject(primaryObj, _allocator);
	_object.AddMember("Primary", primaryObj, _allocator);

	JsonObject secondaryObj;
	m_secondary->addToJsonObject(secondaryObj, _allocator);
	_object.AddMember("Secondary", secondaryObj, _allocator);

	JsonObject sizeObj;
	m_cellSize.addToJsonObject(sizeObj, _allocator);
	_object.AddMember("CellSize", sizeObj, _allocator);
}

void ot::CheckerboardPainter2D::setFromJsonObject(const ConstJsonObject& _object) {
	ot::Painter2D::setFromJsonObject(_object);

	Painter2D* newPrimary = Painter2DFactory::instance().createFromJSON(json::getObject(_object, "Primary"), OT_JSON_MEMBER_Painter2DType);
	if (newPrimary) this->setPrimaryPainter(newPrimary);
	
	Painter2D* newSecondary = Painter2DFactory::instance().createFromJSON(json::getObject(_object, "Secondary"), OT_JSON_MEMBER_Painter2DType);
	if (newSecondary) this->setSecondaryPainter(newSecondary);

	m_cellSize.setFromJsonObject(json::getObject(_object, "CellSize"));
}

std::string ot::CheckerboardPainter2D::generateQss(void) const {
	OT_LOG_W("Checkboard can not be exported to qss. Returning primary painter qss.");
	return m_primary->generateQss();
}

std::string ot::CheckerboardPainter2D::generateSvgColorString(const std::string& _id) const {
	OT_LOG_W("Checkboard can not be exported to SVG. Returning primary painter svg color string.");
	return m_primary->generateSvgColorString(_id);
}

ot::Color ot::CheckerboardPainter2D::getDefaultColor(void) const {
	return m_primary->getDefaultColor();
}

bool ot::CheckerboardPainter2D::isEqualTo(const Painter2D* _other) const {
	const CheckerboardPainter2D* otherPainter = dynamic_cast<const CheckerboardPainter2D*>(_other);
	if (!otherPainter) return false;

	if (!m_primary->isEqualTo(otherPainter->getPrimaryPainter())) return false;
	if (!m_secondary->isEqualTo(otherPainter->getSecondaryPainter())) return false;
	
	return m_cellSize == otherPainter->getCellSize();
}

void ot::CheckerboardPainter2D::setPrimaryPainter(Painter2D* _painter) {
	if (_painter == m_primary || _painter == m_secondary) return;
	OTAssertNullptr(_painter);
	if (m_primary) delete m_primary;
	m_primary = _painter;
}

void ot::CheckerboardPainter2D::setSecondaryPainter(Painter2D* _painter) {
	if (_painter == m_primary || _painter == m_secondary) return;
	OTAssertNullptr(_painter);
	if (m_secondary) delete m_secondary;
	m_secondary = _painter;
}

void ot::CheckerboardPainter2D::setCellSize(int _size) {
	m_cellSize = Size2D(_size, _size);
}

void ot::CheckerboardPainter2D::setCellSize(int _width, int _height) {
	m_cellSize = Size2D(_width, _height);
}
