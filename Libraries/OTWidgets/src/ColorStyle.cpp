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
{
	*this = _other;
}

ot::ColorStyle::~ColorStyle() {

}

ot::ColorStyle& ot::ColorStyle::operator = (const ColorStyle& _other) {
	m_styleSheet = _other.m_styleSheet;
	m_int = _other.m_int;
	m_double = _other.m_double;
	m_files = _other.m_files;
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

	JsonArray fileArr;
	for (const auto& it : m_files) {
		JsonObject valueObj;
		valueObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Name, JsonString(it.first, _allocator), _allocator);
		valueObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Path, JsonString(it.second.toStdString(), _allocator), _allocator);
		fileArr.PushBack(valueObj, _allocator);
	}
	_object.AddMember("Files", fileArr, _allocator);

	JsonArray intArr;
	for (const auto& it : m_int) {
		JsonObject valueObj;
		valueObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Name, JsonString(it.first, _allocator), _allocator);
		valueObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Value, it.second, _allocator);
		intArr.PushBack(valueObj, _allocator);
	}
	_object.AddMember("Int", intArr, _allocator);

	JsonArray doubleArr;
	for (const auto& it : m_double) {
		JsonObject valueObj;
		valueObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Name, JsonString(it.first, _allocator), _allocator);
		valueObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Value, it.second, _allocator);
		intArr.PushBack(valueObj, _allocator);
	}
	_object.AddMember("Double", doubleArr, _allocator);
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

	m_files.clear();
	ConstJsonArray fileArr = json::getArray(_object, "Files");
	for (JsonSizeType i = 0; i < fileArr.Size(); i++) {
		ConstJsonObject valueObj = json::getObject(fileArr, i);
		std::string n = json::getString(valueObj, OT_COLORSTYLE_FILE_VALUE_Name);
		QString v = QString::fromStdString(json::getString(valueObj, OT_COLORSTYLE_FILE_VALUE_Path));
		m_files.insert_or_assign(n, v);
	}

	m_int.clear();
	ConstJsonArray intArr = json::getArray(_object, "Int");
	for (JsonSizeType i = 0; i < intArr.Size(); i++) {
		ConstJsonObject valueObj = json::getObject(intArr, i);
		std::string n = json::getString(valueObj, OT_COLORSTYLE_FILE_VALUE_Name);
		int v = json::getInt(valueObj, OT_COLORSTYLE_FILE_VALUE_Value);
		m_int.insert_or_assign(n, v);
	}

	m_double.clear();
	ConstJsonArray doubleArr = json::getArray(_object, "Double");
	for (JsonSizeType i = 0; i < doubleArr.Size(); i++) {
		ConstJsonObject valueObj = json::getObject(doubleArr, i);
		std::string n = json::getString(valueObj, OT_COLORSTYLE_FILE_VALUE_Name);
		double v = json::getDouble(valueObj, OT_COLORSTYLE_FILE_VALUE_Value);
		m_double.insert_or_assign(n, v);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::ColorStyle::addValue(const ColorStyleValue& _value, bool _replace) {
	if (this->hasValue(_value.name()) && !_replace) {
		OT_LOG_W("A value with the name \"" + _value.name() + "\" already exists");
		return;
	}
	m_values.insert_or_assign(_value.name(), _value);
}

bool ot::ColorStyle::hasValue(const std::string& _name) const {
	return m_values.find(_name) != m_values.end();
}

const ot::ColorStyleValue& ot::ColorStyle::getValue(const std::string& _name, const ColorStyleValue& _default) const {
	const auto& it = m_values.find(_name);
	if (it == m_values.end()) return _default;
	else return it->second;
}

void ot::ColorStyle::addFile(const std::string& _name, const QString& _path, bool _replace) {
	if (this->hasFile(_name) && !_replace) {
		OT_LOG_W("A file with the name \"" + _name + "\" already exists");
		return;
	}
	m_files.insert_or_assign(_name, _path);
}

bool ot::ColorStyle::hasFile(const std::string& _name) const {
	return m_files.find(_name) != m_files.end();
}

QString ot::ColorStyle::getFile(const std::string& _name) const {
	const auto& it = m_files.find(_name);
	if (it == m_files.end()) return QString();
	else return it->second;
}

void ot::ColorStyle::addInteger(const std::string& _name, int _value, bool _replace) {
	if (this->hasInteger(_name) && !_replace) {
		OT_LOG_W("A integer with the name \"" + _name + "\" already exists");
		return;
	}
	m_int.insert_or_assign(_name, _value);
}

bool ot::ColorStyle::hasInteger(const std::string& _name) const {
	return m_int.find(_name) != m_int.end();
}

int ot::ColorStyle::getInteger(const std::string& _name) const {
	const auto& it = m_int.find(_name);
	if (it == m_int.end()) return 0;
	else return it->second;
}

void ot::ColorStyle::addDouble(const std::string& _name, double _value, bool _replace) {
	if (this->hasDouble(_name) && !_replace) {
		OT_LOG_W("A double with the name \"" + _name + "\" already exists");
		return;
	}
	m_double.insert_or_assign(_name, _value);
}

bool ot::ColorStyle::hasDouble(const std::string& _name) const {
	return m_double.find(_name) != m_double.end();
}

double ot::ColorStyle::getDouble(const std::string& _name) const {
	const auto& it = m_double.find(_name);
	if (it == m_double.end()) return 0.;
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
			std::string nam = n.toStdString();
			while (nam.find('\r') != std::string::npos) {
				nam.erase(nam.find('\r'));
			}
			this->setColorStyleName(nam);
		}
		else if (k == OT_COLORSTYLE_FILE_KEY_Integers) {
			JsonDocument doc;
			if (!doc.fromJson(n.toStdString())) {
				OT_LOG_E("Failed to parse integer document");
				return false;
			}
			for (JsonSizeType i = 0; i < doc.Size(); i++) {
				ConstJsonObject vObj = json::getObject(doc, i);
				std::string n = json::getString(vObj, OT_COLORSTYLE_FILE_VALUE_Name);
				int v = json::getInt(vObj, OT_COLORSTYLE_FILE_VALUE_Value);
				this->addInteger(n, v, false);
			}
		}
		else if (k == OT_COLORSTYLE_FILE_KEY_Doubles) {
			JsonDocument doc;
			if (!doc.fromJson(n.toStdString())) {
				OT_LOG_E("Failed to parse integer document");
				return false;
			}
			for (JsonSizeType i = 0; i < doc.Size(); i++) {
				ConstJsonObject vObj = json::getObject(doc, i);
				std::string n = json::getString(vObj, OT_COLORSTYLE_FILE_VALUE_Name);
				int v = json::getDouble(vObj, OT_COLORSTYLE_FILE_VALUE_Value);
				this->addDouble(n, v, false);
			}
		}
		else if (k == OT_COLORSTYLE_FILE_KEY_Files) {
			JsonDocument doc;
			if (!doc.fromJson(n.toStdString())) {
				OT_LOG_E("Failed to parse files document");
				return false;
			}
			for (JsonSizeType i = 0; i < doc.Size(); i++) {
				ConstJsonObject vObj = json::getObject(doc, i);
				std::string n = json::getString(vObj, OT_COLORSTYLE_FILE_VALUE_Name);
				QString p = QString::fromStdString(json::getString(vObj, OT_COLORSTYLE_FILE_VALUE_Path));
				this->addFile(n, p, false);
			}
		}
		else if (k == OT_COLORSTYLE_FILE_KEY_Values) {
			JsonDocument doc;
			if (!doc.fromJson(n.toStdString())) {
				OT_LOG_E("Failed to parse values document");
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