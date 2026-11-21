// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Data/GatewayConnectionInfo.h"

void ot::GatewayConnectionInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember(OT_ACTION_PARAM_Gateway_Url, JsonString(m_gatewayUrl, _allocator), _allocator);
	_jsonObject.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_gatewayServiceUrl, _allocator), _allocator);
}

void ot::GatewayConnectionInfo::setFromJsonObject(const ConstJsonObject & _jsonObject)
{

}

bool ot::GatewayConnectionInfo::isValid() const
{
	return !m_gatewayServiceUrl.empty() &&
		!m_gatewayUrl.empty();
}
