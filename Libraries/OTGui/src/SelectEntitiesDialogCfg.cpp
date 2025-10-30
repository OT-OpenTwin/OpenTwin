// @otlicense

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTGui/SelectEntitiesDialogCfg.h"

ot::SelectEntitiesDialogCfg::SelectEntitiesDialogCfg() {

}

ot::SelectEntitiesDialogCfg::~SelectEntitiesDialogCfg() {

}

void ot::SelectEntitiesDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	NavigationTreePackage::addToJsonObject(_object, _allocator);
}

void ot::SelectEntitiesDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_title = json::getString(_object, "Title");
	NavigationTreePackage::setFromJsonObject(_object);
}