// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Data/GatewayInitData.h"

ot::GatewayInitData::GatewayInitData()
	: ServiceInitData()
{}

ot::GatewayInitData::GatewayInitData(ServiceInitData&& _serviceInitData)
	: ServiceInitData(std::move(_serviceInitData))
{}

ot::GatewayInitData::GatewayInitData(const ConstJsonObject & _jsonObject)
	: GatewayInitData()
{
	setFromJsonObject(_jsonObject);
}

void ot::GatewayInitData::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	ServiceInitData::addToJsonObject(_jsonObject, _allocator);
}

void ot::GatewayInitData::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	ServiceInitData::setFromJsonObject(_jsonObject);
}
