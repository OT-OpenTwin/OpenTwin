// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Data/GatewayConnectionInfo.h"

void ot::GatewayConnectionInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_gatewayServiceUrl, _allocator), _allocator);
}

void ot::GatewayConnectionInfo::setFromJsonObject(const ConstJsonObject & _jsonObject)
{
	m_gatewayServiceUrl = json::getString(_jsonObject, OT_ACTION_PARAM_SERVICE_URL);
}

bool ot::GatewayConnectionInfo::isValid() const
{
	return !m_gatewayServiceUrl.empty();
}
