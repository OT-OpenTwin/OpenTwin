// @otlicense
// File: Point2D.cpp
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
#include "OTCore/Point2D.h"

ot::Point2D::Point2D(const ConstJsonObject& _jsonObject) : Point2D() {
	setFromJsonObject(_jsonObject);
}

void ot::Point2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("x", m_x, _allocator);
	_object.AddMember("y", m_y, _allocator);
}

void ot::Point2D::setFromJsonObject(const ConstJsonObject& _object) {
	m_x = json::getInt(_object, "x");
	m_y = json::getInt(_object, "y");
}

ot::Point2DF ot::Point2D::toPoint2DF(void) const {
	return ot::Point2DF(static_cast<float>(m_x), static_cast<float>(m_y));
}

ot::Point2DD ot::Point2D::toPoint2DD(void) const {
	return ot::Point2DD(static_cast<double>(m_x), static_cast<double>(m_y));
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Point2DF::Point2DF(const ConstJsonObject& _jsonObject) : Point2DF() {
	setFromJsonObject(_jsonObject);
}

void ot::Point2DF::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("x", m_x, _allocator);
	_object.AddMember("y", m_y, _allocator);
}

void ot::Point2DF::setFromJsonObject(const ConstJsonObject& _object) {
	m_x = json::getFloat(_object, "x");
	m_y = json::getFloat(_object, "y");
}

ot::Point2D ot::Point2DF::toPoint2D(void) const {
	return ot::Point2D(static_cast<int>(m_x), static_cast<int>(m_y));
}

ot::Point2DD ot::Point2DF::toPoint2DD(void) const {
	return ot::Point2DD(static_cast<double>(m_x), static_cast<double>(m_y));
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Point2DD::Point2DD(const ConstJsonObject& _jsonObject) : Point2DD() {
	setFromJsonObject(_jsonObject);
}

void ot::Point2DD::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("x", m_x, _allocator);
	_object.AddMember("y", m_y, _allocator);
}

void ot::Point2DD::setFromJsonObject(const ConstJsonObject& _object) {
	m_x = json::getDouble(_object, "x");
	m_y = json::getDouble(_object, "y");
}

ot::Point2D ot::Point2DD::toPoint2D(void) const {
	return ot::Point2D(static_cast<int>(m_x), static_cast<int>(m_y));
}

ot::Point2DF ot::Point2DD::toPoint2DF(void) const {
	return ot::Point2DF(static_cast<float>(m_x), static_cast<float>(m_y));
}