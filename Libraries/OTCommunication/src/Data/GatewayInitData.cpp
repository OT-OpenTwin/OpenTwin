// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Data/GatewayInitData.h"

void ot::GatewayInitData::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember(OT_ACTION_PARAM_Project, JsonString(m_projectId, _allocator), _allocator);
}

void ot::GatewayInitData::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	m_projectId = ot::json::getString(_jsonObject, OT_ACTION_PARAM_Project);
}
