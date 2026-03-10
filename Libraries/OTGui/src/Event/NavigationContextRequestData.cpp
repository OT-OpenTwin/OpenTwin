// @otlicense

// OpenTwin header
#include "OTGui/Event/NavigationContextRequestData.h"

static ot::ContextRequestData::Registrar<ot::NavigationContextRequestData> registrar(ot::NavigationContextRequestData::className());

void ot::NavigationContextRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	ContextRequestData::addToJsonObject(_object, _allocator);
	_object.AddMember("NavigationItemPath", JsonString(m_navigationItemPath, _allocator), _allocator);
}

void ot::NavigationContextRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	ContextRequestData::setFromJsonObject(_object);
	m_navigationItemPath = json::getString(_object, "NavigationItemPath");
}
