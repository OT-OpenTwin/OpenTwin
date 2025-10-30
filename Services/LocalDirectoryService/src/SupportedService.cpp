// @otlicense

// LDS header
#include "SupportedService.h"

SupportedService::SupportedService() : m_maxCrashRestarts(0), m_maxStartupRestarts(0) {}

SupportedService::SupportedService(const std::string& _name, const std::string& _type) :
	m_name(_name), m_type(_type), m_maxCrashRestarts(0), m_maxStartupRestarts(0)
{}

void SupportedService::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);
	_object.AddMember("MaxCrashRestarts", m_maxCrashRestarts, _allocator);
	_object.AddMember("MaxStartupRestarts", m_maxStartupRestarts, _allocator);
}