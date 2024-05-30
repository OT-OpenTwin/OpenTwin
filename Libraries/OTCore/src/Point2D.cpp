//! @file Point2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"

void ot::Point2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("x", m_x, _allocator);
	_object.AddMember("y", m_y, _allocator);
}

void ot::Point2D::setFromJsonObject(const ConstJsonObject& _object) {
	m_x = json::getInt(_object, "x");
	m_y = json::getInt(_object, "y");
}

ot::Point2DF ot::Point2D::toPoint2DF(void) const {
	return ot::Point2DF((float)m_x, (float)m_y);
}

ot::Point2DD ot::Point2D::toPoint2DD(void) const {
	return ot::Point2DD((double)m_x, (double)m_y);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

void ot::Point2DF::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("x", m_x, _allocator);
	_object.AddMember("y", m_y, _allocator);
}

void ot::Point2DF::setFromJsonObject(const ConstJsonObject& _object) {
	m_x = json::getFloat(_object, "x");
	m_y = json::getFloat(_object, "y");
}

ot::Point2D ot::Point2DF::toPoint2D(void) const {
	return ot::Point2D((int)m_x, (int)m_y);
}

ot::Point2DD ot::Point2DF::toPoint2DD(void) const {
	return ot::Point2DD((double)m_x, (double)m_y);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

void ot::Point2DD::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("x", m_x, _allocator);
	_object.AddMember("y", m_y, _allocator);
}

void ot::Point2DD::setFromJsonObject(const ConstJsonObject& _object) {
	m_x = json::getDouble(_object, "x");
	m_y = json::getDouble(_object, "y");
}

ot::Point2D ot::Point2DD::toPoint2D(void) const {
	return ot::Point2D((int)m_x, (int)m_y);
}

ot::Point2DF ot::Point2DD::toPoint2DF(void) const {
	return ot::Point2DF((float)m_x, (float)m_y);
}