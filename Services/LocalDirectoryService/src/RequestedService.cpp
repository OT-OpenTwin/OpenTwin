// @otlicense

// LDS header
#include "RequestedService.h"

RequestedService::RequestedService(ot::ServiceInitData&& _initData, ServiceStartupData&& _startupData)
	: m_initData(std::move(_initData)), m_startupData(std::move(_startupData)) 
{}

RequestedService::RequestedService(const ot::ServiceInitData& _initData, const SupportedService& _supportedServiceInfo)
	: m_initData(_initData), m_startupData(_supportedServiceInfo)
{}

void RequestedService::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::JsonObject initObj;
	m_initData.addToJsonObject(initObj, _allocator);
	_object.AddMember("InitData", initObj, _allocator);

	ot::JsonObject startupObj;
	m_startupData.addToJsonObject(startupObj, _allocator);
	_object.AddMember("StartupData", startupObj, _allocator);
}