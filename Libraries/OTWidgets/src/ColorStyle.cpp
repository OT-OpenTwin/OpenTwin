//! @file ColorStyle.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/ColorStyleTypes.h"

ot::ColorStyle::ColorStyle() {

}

ot::ColorStyle::ColorStyle(const ColorStyle& _other)
	: m_styleSheet(_other.m_styleSheet), m_values(_other.m_values), m_name(_other.m_name)
{
	
}

ot::ColorStyle::~ColorStyle() {

}

ot::ColorStyle& ot::ColorStyle::operator = (const ColorStyle& _other) {
	m_styleSheet = _other.m_styleSheet;
	m_values = _other.m_values;
	m_name = _other.m_name;

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

bool ot::ColorStyle::setupFromFile(QByteArray _data) {
	if (_data.isEmpty()) return false;
	qsizetype ix = _data.indexOf('\n');
	if (ix == -1) {
		OT_LOG_E("ColorStyle file broken");
		return false;
	}
	while (ix != -1) {
		qsizetype ix2 = _data.indexOf(':');
		if (ix2 > ix) {
			OT_LOG_E("ColorStyle file key broken");
			return false;
		}
		QByteArray k = _data.mid(0, ix2 + 1);
		QByteArray n = _data.mid(0, ix);
		n.remove(0, ix2 + 1);
		_data.remove(0, ix + 1);
		ix = _data.indexOf('\n');

		if (k == OT_COLORSTYLE_FILE_KEY_Name) {
			this->setColorStyleName(n.toStdString());
		}
		else if (k == OT_COLORSTYLE_FILE_KEY_Values) {
			JsonDocument doc;
			if (!doc.fromJson(n.toStdString())) {
				OT_LOG_E("Failed to parse document");
				return false;
			}
			for (JsonSizeType i = 0; i < doc.Size(); i++) {
				ConstJsonObject vObj = json::getObject(doc, i);
				ColorStyleValue newValue;
				newValue.setFromJsonObject(vObj);
				this->addValue(newValue);
			}
		}
		else if (k == OT_COLORSTYLE_FILE_KEY_StyleSheet) {
			std::string tmp = _data.toStdString();
			this->setStyleSheet(QString::fromStdString(tmp));
			return true;
		}
		else {
			OT_LOG_E("Unknown color style file key \"" + k.toStdString() + "\"");
			return false;
		}
	}
	return false;
}