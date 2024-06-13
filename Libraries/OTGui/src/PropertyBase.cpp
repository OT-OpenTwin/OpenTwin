//! @file PropertyBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyBase.h"

std::string ot::PropertyBase::toString(PropertyFlag _flag) {
	switch (_flag)
	{
	case ot::PropertyBase::NoFlags: return "<null>";
	case ot::PropertyBase::IsReadOnly: return "IsReadOnly";
	case ot::PropertyBase::IsProtected: return "IsProtected";
	case ot::PropertyBase::IsHidden: return "IsHidden";
	case ot::PropertyBase::HasInputError: return "HasInputError";
	case ot::PropertyBase::HasMultipleValues: return "HasMultipleValues";
	case ot::PropertyBase::IsDeletable: return "IsDeletable";
	case ot::PropertyBase::AllowCustomValues: return "AllowCustomValues";
	case ot::PropertyBase::AllowMultiselection: return "AllowMultiselection";
	default:
		OT_LOG_EA("Unknown PropertyBase Flag");
		return "<null>";
	}
}

ot::PropertyBase::PropertyFlag ot::PropertyBase::stringToFlag(const std::string& _flag) {
	if (_flag == PropertyBase::toString(PropertyBase::NoFlags)) { return PropertyBase::NoFlags; }
	else if (_flag == PropertyBase::toString(PropertyBase::IsReadOnly)) { return PropertyBase::IsReadOnly; }
	else if (_flag == PropertyBase::toString(PropertyBase::IsProtected)) { return PropertyBase::IsProtected; }
	else if (_flag == PropertyBase::toString(PropertyBase::IsHidden)) { return PropertyBase::IsHidden; }
	else if (_flag == PropertyBase::toString(PropertyBase::HasInputError)) { return PropertyBase::HasInputError; }
	else if (_flag == PropertyBase::toString(PropertyBase::HasMultipleValues)) { return PropertyBase::HasMultipleValues; }
	else if (_flag == PropertyBase::toString(PropertyBase::IsDeletable)) { return PropertyBase::IsDeletable; }
	else if (_flag == PropertyBase::toString(PropertyBase::AllowCustomValues)) { return PropertyBase::AllowCustomValues; }
	else if (_flag == PropertyBase::toString(PropertyBase::AllowMultiselection)) { return PropertyBase::AllowMultiselection; }
	else {
		OT_LOG_EAS("Unknown PropertyBase Flag \"" + _flag + "\"");
		return PropertyBase::NoFlags;
	}
}

std::list<std::string> ot::PropertyBase::toStringList(PropertyFlags _flags) {
	std::list<std::string> ret;
	if (_flags & PropertyBase::IsReadOnly) ret.push_back(PropertyBase::toString(PropertyBase::IsReadOnly));
	if (_flags & PropertyBase::IsHidden) ret.push_back(PropertyBase::toString(PropertyBase::IsHidden));
	if (_flags & PropertyBase::IsProtected) ret.push_back(PropertyBase::toString(PropertyBase::IsProtected));
	if (_flags & PropertyBase::HasInputError) ret.push_back(PropertyBase::toString(PropertyBase::HasInputError));
	if (_flags & PropertyBase::HasMultipleValues) ret.push_back(PropertyBase::toString(PropertyBase::HasMultipleValues));
	if (_flags & PropertyBase::IsDeletable) ret.push_back(PropertyBase::toString(PropertyBase::IsDeletable));
	if (_flags & PropertyBase::AllowCustomValues) ret.push_back(PropertyBase::toString(PropertyBase::AllowCustomValues));
	if (_flags & PropertyBase::AllowMultiselection) ret.push_back(PropertyBase::toString(PropertyBase::AllowMultiselection));
	return ret;
}

ot::PropertyBase::PropertyFlags ot::PropertyBase::stringListToFlags(const std::list<std::string>& _flags) {
	PropertyBase::PropertyFlags ret = PropertyBase::NoFlags;
	for (const std::string& f : _flags) {
		ret |= PropertyBase::stringToFlag(f);
	};
	return ret;
}


void ot::PropertyBase::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Tip", JsonString(m_tip, _allocator), _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("SpecialType", JsonString(m_specialType, _allocator), _allocator);

	JsonArray dataArr;
	for (const auto& it : m_data) {
		JsonObject dataObj;
		dataObj.AddMember("Key", JsonString(it.first, _allocator), _allocator);
		dataObj.AddMember("Value", JsonString(it.second, _allocator), _allocator);
		dataArr.PushBack(dataObj, _allocator);
	}

	_object.AddMember("Data", dataArr, _allocator);
	_object.AddMember("Flags", JsonArray(this->toStringList(m_flags), _allocator), _allocator);
}

void ot::PropertyBase::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_tip = json::getString(_object, "Tip");
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_specialType = json::getString(_object, "SpecialType");

	m_data.clear();
	ConstJsonObjectList dataArr = json::getObjectList(_object, "Data");
	for (ConstJsonObject& dataObj : dataArr) {
		std::string key = json::getString(dataObj, "Key");
		std::string value = json::getString(dataObj, "Value");
		m_data.insert_or_assign(key, value);
	}

	m_flags = this->stringListToFlags(json::getStringList(_object, "Flags"));
}

ot::PropertyBase::PropertyBase(PropertyFlags _flags)
	: m_flags(_flags)
{}

ot::PropertyBase::PropertyBase(const std::string& _name, PropertyFlags _flags)
	: m_name(_name), m_flags(_flags)
{}

ot::PropertyBase::PropertyBase(const PropertyBase& _other) {
	*this = _other;
}

ot::PropertyBase& ot::PropertyBase::operator = (const PropertyBase& _other) {
	m_tip = _other.m_tip;
	m_name = _other.m_name;
	m_title = _other.m_title;
	m_specialType = _other.m_specialType;
	m_data = _other.m_data;
	m_flags = _other.m_flags;
	m_flags = _other.m_flags;

	return *this;
}

std::string ot::PropertyBase::getAdditionalPropertyData(const std::string& _key) const {
	const auto& it = m_data.find(_key);
	if (it == m_data.end()) {
		OT_LOG_EA("Data not found");
		return std::string();
	}
	else {
		return it->second;
	}
}
