// @otlicense
// File: Rect.cpp
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
#include "OTCore/Rect.h"

ot::Rect::Rect() 
    : m_topLeft(0, 0), m_bottomRight(0, 0)
{}

ot::Rect::Rect(const Point2D& _topLeft, const Point2D& _bottomRight) 
    : m_topLeft(_topLeft), m_bottomRight(_bottomRight)
{}

ot::Rect::Rect(const Point2D& _topLeft, const Size2D& _size)
    : m_topLeft(_topLeft), m_bottomRight(_topLeft.x() + _size.width(), _topLeft.y() + _size.height())
{}

ot::Rect::Rect(const Rect& _other)
    : m_topLeft(_other.m_topLeft), m_bottomRight(_other.m_bottomRight)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class methods

void ot::Rect::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject topLeftObj;
	m_topLeft.addToJsonObject(topLeftObj, _allocator);

	JsonObject bottomRightObj;
	m_bottomRight.addToJsonObject(bottomRightObj, _allocator);

	_object.AddMember("TopLeft", topLeftObj, _allocator);
	_object.AddMember("BottomRight", bottomRightObj, _allocator);
}

void ot::Rect::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_topLeft.setFromJsonObject(json::getObject(_object, "TopLeft"));
	m_bottomRight.setFromJsonObject(json::getObject(_object, "BottomRight"));
}

ot::Rect ot::Rect::unionWith(const Rect& _other) const {
    Point2D newTopLeft(
        std::min(m_topLeft.x(), _other.getTopLeft().x()),
        std::min(m_topLeft.y(), _other.getTopLeft().y())
    );
    Point2D newBottomRight(
        std::max(m_bottomRight.x(), _other.getBottomRight().x()),
        std::max(m_bottomRight.y(), _other.getBottomRight().y())
    );
    return Rect(newTopLeft, newBottomRight);
}

ot::Rect ot::Rect::intersectsWith(const Rect& _other) const {
    Point2D newTopLeft(
        std::max(m_topLeft.x(), _other.getTopLeft().x()),
        std::max(m_topLeft.y(), _other.getTopLeft().y())
    );
    Point2D newBottomRight(
        std::min(m_bottomRight.x(), _other.getBottomRight().x()),
        std::min(m_bottomRight.y(), _other.getBottomRight().y())
    );

    // Check if there is no intersection
    if (newTopLeft.x() > newBottomRight.x() || newTopLeft.y() > newBottomRight.y()) {
        return Rect(Point2D(0, 0), Point2D(0, 0));
    }
    return Rect(newTopLeft, newBottomRight);
}

void ot::Rect::moveTo(const Point2D& _topLeft) {
    this->moveBy(_topLeft - m_topLeft);
}

void ot::Rect::moveBy(const Point2D& _distance) {
    m_topLeft += _distance;
    m_bottomRight += _distance;
}

ot::RectF ot::Rect::toRectF(void) const {
    return RectF(m_topLeft.toPoint2DF(), m_bottomRight.toPoint2DF());
}

ot::RectD ot::Rect::toRectD(void) const {
    return RectD(m_topLeft.toPoint2DD(), m_bottomRight.toPoint2DD());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::RectF::RectF() 
    : m_topLeft(0.f, 0.f), m_bottomRight(0.f, 0.f)
{}

ot::RectF::RectF(const Point2DF& _topLeft, const Point2DF& _bottomRight) 
    : m_topLeft(_topLeft), m_bottomRight(_bottomRight)
{}

ot::RectF::RectF(const Point2DF& _topLeft, const Size2DF& _size)
    : m_topLeft(_topLeft), m_bottomRight(_topLeft.x() + _size.width(), _topLeft.y() + _size.height())
{}

ot::RectF::RectF(const RectF& _other)
    : m_topLeft(_other.m_topLeft), m_bottomRight(_other.m_bottomRight)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class methods

void ot::RectF::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
    JsonObject topLeftObj;
    m_topLeft.addToJsonObject(topLeftObj, _allocator);

    JsonObject bottomRightObj;
    m_bottomRight.addToJsonObject(bottomRightObj, _allocator);

    _object.AddMember("TopLeft", topLeftObj, _allocator);
    _object.AddMember("BottomRight", bottomRightObj, _allocator);
}

void ot::RectF::setFromJsonObject(const ot::ConstJsonObject& _object) {
    m_topLeft.setFromJsonObject(json::getObject(_object, "TopLeft"));
    m_bottomRight.setFromJsonObject(json::getObject(_object, "BottomRight"));
}

ot::RectF ot::RectF::unionWith(const RectF& _other) const {
    Point2DF newTopLeft(
        std::min(m_topLeft.x(), _other.getTopLeft().x()),
        std::min(m_topLeft.y(), _other.getTopLeft().y())
    );
    Point2DF newBottomRight(
        std::max(m_bottomRight.x(), _other.getBottomRight().x()),
        std::max(m_bottomRight.y(), _other.getBottomRight().y())
    );
    return RectF(newTopLeft, newBottomRight);
}

ot::RectF ot::RectF::intersectsWith(const RectF& _other) const {
    Point2DF newTopLeft(
        std::max(m_topLeft.x(), _other.getTopLeft().x()),
        std::max(m_topLeft.y(), _other.getTopLeft().y())
    );
    Point2DF newBottomRight(
        std::min(m_bottomRight.x(), _other.getBottomRight().x()),
        std::min(m_bottomRight.y(), _other.getBottomRight().y())
    );

    // Check if there is no intersection
    if (newTopLeft.x() > newBottomRight.x() || newTopLeft.y() > newBottomRight.y()) {
        return RectF(Point2DF(0.f, 0.f), Point2DF(0.f, 0.f));
    }
    return RectF(newTopLeft, newBottomRight);
}

void ot::RectF::moveTo(const Point2DF& _topLeft) {
    this->moveBy(_topLeft - m_topLeft);
}

void ot::RectF::moveBy(const Point2DF& _distance) {
    m_topLeft += _distance;
    m_bottomRight += _distance;
}

ot::Rect ot::RectF::toRect(void) const {
    return Rect(m_topLeft.toPoint2D(), m_bottomRight.toPoint2D());
}

ot::RectD ot::RectF::toRectD(void) const {
    return RectD(m_topLeft.toPoint2DD(), m_bottomRight.toPoint2DD());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::RectD::RectD() 
    : m_topLeft(0., 0.), m_bottomRight(0., 0.)
{}

ot::RectD::RectD(const Point2DD& _topLeft, const Point2DD& _bottomRight)
    : m_topLeft(_topLeft), m_bottomRight(_bottomRight)
{}

ot::RectD::RectD(const Point2DD& _topLeft, const Size2DD& _size)
    : m_topLeft(_topLeft), m_bottomRight(_topLeft.x() + _size.width(), _topLeft.y() + _size.height())
{}

ot::RectD::RectD(const RectD& _other)
    : m_topLeft(_other.m_topLeft), m_bottomRight(_other.m_bottomRight)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class methods

void ot::RectD::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
    JsonObject topLeftObj;
    m_topLeft.addToJsonObject(topLeftObj, _allocator);

    JsonObject bottomRightObj;
    m_bottomRight.addToJsonObject(bottomRightObj, _allocator);

    _object.AddMember("TopLeft", topLeftObj, _allocator);
    _object.AddMember("BottomRight", bottomRightObj, _allocator);
}

void ot::RectD::setFromJsonObject(const ot::ConstJsonObject& _object) {
    m_topLeft.setFromJsonObject(json::getObject(_object, "TopLeft"));
    m_bottomRight.setFromJsonObject(json::getObject(_object, "BottomRight"));
}

ot::RectD ot::RectD::unionWith(const RectD& _other) const {
    Point2DD newTopLeft(
        std::min(m_topLeft.x(), _other.getTopLeft().x()),
        std::min(m_topLeft.y(), _other.getTopLeft().y())
    );
    Point2DD newBottomRight(
        std::max(m_bottomRight.x(), _other.getBottomRight().x()),
        std::max(m_bottomRight.y(), _other.getBottomRight().y())
    );
    return RectD(newTopLeft, newBottomRight);
}

ot::RectD ot::RectD::intersectsWith(const RectD& _other) const {
    Point2DD newTopLeft(
        std::max(m_topLeft.x(), _other.getTopLeft().x()),
        std::max(m_topLeft.y(), _other.getTopLeft().y())
    );
    Point2DD newBottomRight(
        std::min(m_bottomRight.x(), _other.getBottomRight().x()),
        std::min(m_bottomRight.y(), _other.getBottomRight().y())
    );

    // Check if there is no intersection
    if (newTopLeft.x() > newBottomRight.x() || newTopLeft.y() > newBottomRight.y()) {
        return RectD(Point2DD(0., 0.), Point2DD(0., 0.));
    }
    return RectD(newTopLeft, newBottomRight);
}

void ot::RectD::moveTo(const Point2DD& _topLeft) {
    this->moveBy(_topLeft - m_topLeft);
}

void ot::RectD::moveBy(const Point2DD& _distance) {
    m_topLeft += _distance;
    m_bottomRight += _distance;
}

ot::Rect ot::RectD::toRect(void) const {
    return Rect(m_topLeft.toPoint2D(), m_bottomRight.toPoint2D());
}

ot::RectF ot::RectD::toRectF(void) const {
    return RectF(m_topLeft.toPoint2DF(), m_bottomRight.toPoint2DF());
}
