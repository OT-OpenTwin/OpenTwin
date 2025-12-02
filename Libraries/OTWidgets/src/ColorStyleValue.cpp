// @otlicense
// File: ColorStyleValue.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/ColorStyleValue.h"

ot::ColorStyleValue::ColorStyleValue() :
	m_painter(nullptr), m_entryKey(ColorStyleValueEntry::WidgetBackground)
{
	m_painter = new FillPainter2D;
}

ot::ColorStyleValue::ColorStyleValue(const ColorStyleValue& _other) :
	m_entryKey(_other.m_entryKey), m_painter(nullptr)
{
	m_painter = _other.m_painter->createCopy();
}

ot::ColorStyleValue::~ColorStyleValue() {
	delete m_painter;
}

ot::ColorStyleValue& ot::ColorStyleValue::operator = (const ColorStyleValue& _other) {
	m_entryKey = _other.m_entryKey;
	m_painter = _other.m_painter->createCopy();
	
	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::ColorStyleValue::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(toString(m_entryKey), _allocator), _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::ColorStyleValue::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_entryKey = stringToColorStyleValueEntry(json::getString(_object, "Name"));

	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* newPainter = Painter2DFactory::create(painterObj);
	if (!newPainter) {
		OT_LOG_E("Failed to create painter");
		return;
	}
	newPainter->setFromJsonObject(painterObj);
	this->setPainter(newPainter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

QString ot::ColorStyleValue::toQss() const {
	OTAssertNullptr(m_painter);
	if (m_painter) return QString::fromStdString(m_painter->generateQss());
	else return "";
}

QByteArray ot::ColorStyleValue::toSvgColorString(const std::string& _id) const {
	OTAssertNullptr(m_painter);
	if (m_painter) return QByteArray::fromStdString(m_painter->generateSvgColorString(_id));
	else return QByteArray();
}

QColor ot::ColorStyleValue::toColor() const {
	OTAssertNullptr(m_painter);
	if (m_painter) return QtFactory::toQColor(m_painter->getDefaultColor());
	else return QColor();
}

QBrush ot::ColorStyleValue::toBrush() const {
	return QtFactory::toQBrush(m_painter);
}

void ot::ColorStyleValue::setPainter(Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (m_painter == _painter) {
		return;
	}
	if (m_painter) {
		delete m_painter;
	}
	m_painter = _painter;
}

void ot::ColorStyleValue::setPainter(const Painter2D* _painter) {
	this->setPainter(_painter->createCopy());
}

void ot::ColorStyleValue::setColor(Color _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::ColorStyleValue::setColor(DefaultColor _color) {
	this->setPainter(new FillPainter2D(_color));
}