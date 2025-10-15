//! @file EntityBlockHierarchicalProjectItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "EntityBlockHierarchicalProjectItem.h"

static EntityFactoryRegistrar<EntityBlockHierarchicalProjectItem> registrar(EntityBlockHierarchicalProjectItem::className());

EntityBlockHierarchicalProjectItem::EntityBlockHierarchicalProjectItem(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	: EntityBlock(ID, parent, obs, ms, owner)
{

}

ot::GraphicsItemCfg* EntityBlockHierarchicalProjectItem::createBlockCfg() {
	return nullptr;
}

bool EntityBlockHierarchicalProjectItem::updateFromProperties() {
	return false;
}

void EntityBlockHierarchicalProjectItem::createProperties() {
	
}

void EntityBlockHierarchicalProjectItem::setProjectInformation(const ot::ProjectInformation& info) {
}

ot::ProjectInformation EntityBlockHierarchicalProjectItem::getProjectInformation() const {
	return ot::ProjectInformation();
}
