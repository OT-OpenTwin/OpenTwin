//! @file ColorStyle.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/ColorStyle.h"

ot::ColorStyle::ColorStyle() {

}

ot::ColorStyle::ColorStyle(const ColorStyle& _other)
	: m_styleSheet(_other.m_styleSheet), m_values(_other.m_values)
{
	
}

ot::ColorStyle::~ColorStyle() {

}

ot::ColorStyle& ot::ColorStyle::operator = (const ColorStyle& _other) {
	m_styleSheet = _other.m_styleSheet;
	m_values = _other.m_values;

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::ColorStyle::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Sheet", JsonString(m_styleSheet.toStdString(), _allocator), _allocator);
	JsonArray valueArr;
	for (const auto& it : m_values) {
		JsonObject valueObj;
		it.second.addToJsonObject(valueObj, _allocator);
		valueArr.PushBack(valueObj, _allocator);
	}
	_object.AddMember("Values", valueArr, _allocator);
}

void ot::ColorStyle::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_styleSheet = QString::fromStdString(json::getString(_object, "Sheet"));

	m_values.clear();
	ConstJsonArray valueArr = json::getArray(_object, "Values");
	for (JsonSizeType i = 0; i < valueArr.Size(); i++) {
		ConstJsonObject valueObj = json::getObject(valueArr, i);
		ColorStyleValue newValue;
		newValue.setFromJsonObject(valueObj);
		m_values.insert_or_assign(newValue.name(), newValue);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::ColorStyle::addValue(const ColorStyleValue& _value) {
	if (this->hasValue(_value.name())) {
		OT_LOG_W("A value with the name \"" + _value.name() + "\" already exists");
	}
}

bool ot::ColorStyle::hasValue(const std::string& _name) const {
	return m_values.find(_name) != m_values.end();
}

const ot::ColorStyleValue& ot::ColorStyle::getValue(const std::string& _name, const ColorStyleValue& _default) const {
	const auto& it = m_values.find(_name);
	if (it == m_values.end()) return _default;
	else return it->second;
}
