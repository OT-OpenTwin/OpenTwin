//! @file Plot1DAxisCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Plot1DAxisCfg.h"

ot::Plot1DAxisCfg::Plot1DAxisCfg(const Plot1DAxisCfg& _other) {
	*this = _other;
}

ot::Plot1DAxisCfg::~Plot1DAxisCfg() {}

ot::Plot1DAxisCfg& ot::Plot1DAxisCfg::operator=(const Plot1DAxisCfg& _other) {
	if (this == &_other) return *this;

	m_min = _other.m_min;
	m_max = _other.m_max;
	m_isLogScale = _other.m_isLogScale;
	m_isAutoScale = _other.m_isAutoScale;

	return *this;
}

bool ot::Plot1DAxisCfg::operator==(const Plot1DAxisCfg& _other) const {
	return m_min == _other.m_min &&
		m_max == _other.m_max &&
		m_isLogScale == _other.m_isLogScale &&
		m_isAutoScale == _other.m_isAutoScale;
}

bool ot::Plot1DAxisCfg::operator!=(const Plot1DAxisCfg& _other) const {
	return !(*this == _other);
}

void ot::Plot1DAxisCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Min", m_min, _allocator);
	_object.AddMember("Max", m_max, _allocator);
	_object.AddMember("LogScale", m_isLogScale, _allocator);
	_object.AddMember("AutoScale", m_isAutoScale, _allocator);
}

void ot::Plot1DAxisCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_min = json::getDouble(_object, "Min");
	m_max = json::getDouble(_object, "Max");
	m_isLogScale = json::getBool(_object, "LogScale");
	m_isAutoScale = json::getBool(_object, "AutoScale");
}
