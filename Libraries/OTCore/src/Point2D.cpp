// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/Point2D.h"

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