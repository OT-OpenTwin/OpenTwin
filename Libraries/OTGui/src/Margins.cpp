// @otlicense

#include "OTSystem/OTAssert.h"
#include "OTGui/Margins.h"

ot::Margins& ot::Margins::operator = (const Margins& _other) {
	m_left = _other.m_left;
	m_top = _other.m_top;
	m_right = _other.m_right;
	m_bottom = _other.m_bottom;
	return *this;
}

void ot::Margins::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("l", m_left, _allocator);
	_object.AddMember("t", m_top, _allocator);
	_object.AddMember("r", m_right, _allocator);
	_object.AddMember("b", m_bottom, _allocator);
}

void ot::Margins::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_left = json::getInt(_jsonObject, "l");
	m_top = json::getInt(_jsonObject, "t");
	m_right = json::getInt(_jsonObject, "r");
	m_bottom = json::getInt(_jsonObject, "b");
}

ot::MarginsF& ot::MarginsF::operator = (const MarginsF& _other) {
	m_left = _other.m_left;
	m_top = _other.m_top;
	m_right = _other.m_right;
	m_bottom = _other.m_bottom;
	return *this;
}

void ot::MarginsF::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("l", m_left, _allocator);
	_object.AddMember("t", m_top, _allocator);
	_object.AddMember("r", m_right, _allocator);
	_object.AddMember("b", m_bottom, _allocator);
}

void ot::MarginsF::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_left = json::getFloat(_jsonObject, "l");
	m_top = json::getFloat(_jsonObject, "t");
	m_right = json::getFloat(_jsonObject, "r");
	m_bottom = json::getFloat(_jsonObject, "b");
}

ot::MarginsD& ot::MarginsD::operator = (const MarginsD& _other) {
	m_left = _other.m_left;
	m_top = _other.m_top;
	m_right = _other.m_right;
	m_bottom = _other.m_bottom;
	return *this;
}

void ot::MarginsD::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("l", m_left, _allocator);
	_object.AddMember("t", m_top, _allocator);
	_object.AddMember("r", m_right, _allocator);
	_object.AddMember("b", m_bottom, _allocator);
}

void ot::MarginsD::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_left = json::getDouble(_jsonObject, "l");
	m_top = json::getDouble(_jsonObject, "t");
	m_right = json::getDouble(_jsonObject, "r");
	m_bottom = json::getDouble(_jsonObject, "b");
}