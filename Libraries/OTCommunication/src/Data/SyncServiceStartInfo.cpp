// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Data/SyncServiceStartInfo.h"

void ot::SyncServiceStartInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember(OT_ACTION_PARAM_Config, JsonObject(m_seviceInfo, _allocator), _allocator);
}

void ot::SyncServiceStartInfo::setFromJsonObject(const ConstJsonObject & _jsonObject)
{
	m_seviceInfo.setFromJsonObject(json::getObject(_jsonObject, OT_ACTION_PARAM_Config));
}

bool ot::SyncServiceStartInfo::isValid() const
{
	return !m_seviceInfo.getServiceName().empty()
		&& !m_seviceInfo.getServiceType().empty()
		&& !m_seviceInfo.getServiceURL().empty()
		&& m_seviceInfo.getServiceID() != ot::invalidServiceID;
}
