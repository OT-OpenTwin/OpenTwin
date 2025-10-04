//! @file GDSDebugInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/GDSDebugInfo.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::GDSDebugInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Url", JsonString(m_url, _allocator), _allocator);
	_jsonObject.AddMember("GSSUrl", JsonString(m_gssUrl, _allocator), _allocator);
	_jsonObject.AddMember("StartupWorkerRunning", m_startupWorkerRunning, _allocator);
	_jsonObject.AddMember("StartupWorkerStopping", m_startupWorkerStopping, _allocator);

	JsonArray requestedArray;
	this->addServicesToJson(m_requestedServices, requestedArray, _allocator);
	_jsonObject.AddMember("RequestedServices", requestedArray, _allocator);

	JsonArray ldsArray;
	for (const LDSInfo& lds : m_localDirectoryServices) {
		JsonObject ldsObject;
		ldsObject.AddMember("Url", JsonString(lds.url, _allocator), _allocator);
		ldsObject.AddMember("ServiceID", lds.serviceID, _allocator);
		ldsObject.AddMember("SupportedServices", JsonArray(lds.supportedServices, _allocator), _allocator);

		JsonArray servicesArray;
		this->addServicesToJson(lds.services, servicesArray, _allocator);
		ldsObject.AddMember("Services", servicesArray, _allocator);
		ldsArray.PushBack(ldsObject, _allocator);
	}
	_jsonObject.AddMember("LDSList", ldsArray, _allocator);
}

void ot::GDSDebugInfo::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_url = ot::json::getString(_jsonObject, "Url");
	m_gssUrl = ot::json::getString(_jsonObject, "GSSUrl");
	m_startupWorkerRunning = ot::json::getBool(_jsonObject, "StartupWorkerRunning");
	m_startupWorkerStopping = ot::json::getBool(_jsonObject, "StartupWorkerStopping");

	m_requestedServices = this->servicesFromJson(ot::json::getObjectList(_jsonObject, "RequestedServices"));

	m_localDirectoryServices.clear();
	for (const ConstJsonObject& ldsObject : ot::json::getObjectList(_jsonObject, "LDSList")) {
		LDSInfo lds;
		lds.url = ot::json::getString(ldsObject, "Url");
		lds.serviceID = static_cast<serviceID_t>(ot::json::getUInt(ldsObject, "ServiceID"));
		lds.supportedServices = ot::json::getStringList(ldsObject, "SupportedServices");
		lds.services = this->servicesFromJson(ot::json::getObjectList(ldsObject, "Services"));
		m_localDirectoryServices.push_back(lds);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

ot::GDSDebugInfo::ServiceInfo ot::GDSDebugInfo::infoFromInitData(const ServiceInitData& _data) {
	ServiceInfo info;
	info.serviceID = _data.getServiceID();
	info.serviceName = _data.getServiceName();
	info.serviceType = _data.getServiceType();
	info.sessionID = _data.getSessionID();
	info.lssUrl = _data.getSessionServiceURL();
	return info;
}

void ot::GDSDebugInfo::addServiceInfoToJson(const ServiceInfo& _info, ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("Name", JsonString(_info.serviceName, _allocator), _allocator);
	_jsonObject.AddMember("Type", JsonString(_info.serviceType, _allocator), _allocator);
	_jsonObject.AddMember("ID", _info.serviceID, _allocator);
	_jsonObject.AddMember("SessionID", JsonString(_info.sessionID, _allocator), _allocator);
	_jsonObject.AddMember("LSSUrl", JsonString(_info.lssUrl, _allocator), _allocator);
}

ot::GDSDebugInfo::ServiceInfo ot::GDSDebugInfo::serviceInfoFromJson(const ot::ConstJsonObject& _jsonObject) const {
	ServiceInfo info;
	
	info.serviceName = ot::json::getString(_jsonObject, "Name");
	info.serviceType = ot::json::getString(_jsonObject, "Type");
	info.serviceID = static_cast<serviceID_t>(ot::json::getUInt(_jsonObject, "ID"));
	info.sessionID = ot::json::getString(_jsonObject, "SessionID");
	info.lssUrl = ot::json::getString(_jsonObject, "LSSUrl");

	return info;
}

void ot::GDSDebugInfo::addServicesToJson(const std::list<ServiceInfo>& _services, ot::JsonArray& _jsonArray, ot::JsonAllocator& _allocator) const {
	for (const auto& service : _services) {
		JsonObject serviceObject;
		this->addServiceInfoToJson(service, serviceObject, _allocator);
		_jsonArray.PushBack(serviceObject, _allocator);
	}
}

std::list<ot::GDSDebugInfo::ServiceInfo> ot::GDSDebugInfo::servicesFromJson(const std::list<ot::ConstJsonObject>& _jsonArray) const {
	std::list<ServiceInfo> lst;

	for (const auto& item : _jsonArray) {
		lst.push_back(this->serviceInfoFromJson(item));
	}

	return lst;
}
