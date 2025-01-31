//! @file CopyEntityInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/CopyEntityInformation.h"

ot::CopyEntityInformation::CopyEntityInformation()
	: m_uid(0) {

}

ot::CopyEntityInformation::CopyEntityInformation(const ConstJsonObject& _jsonObject) {
	this->setFromJsonObject(_jsonObject);
}

ot::CopyEntityInformation::CopyEntityInformation(UID _uid, const std::string& _name, const std::string& _rawData)
	: m_uid(_uid), m_name(_name), m_rawData(_rawData) {

}

void ot::CopyEntityInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("UID", m_uid, _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Data", JsonString(m_rawData, _allocator), _allocator);
}

void ot::CopyEntityInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_uid = json::getUInt64(_object, "UID");
	m_name = json::getString(_object, "Name");
	m_rawData = json::getString(_object, "Data");
}