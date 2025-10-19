//! @file EntityBlockHierarchicalContainerItem.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "EntityBlock.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalContainerItem : public EntityBlock {
public:
	EntityBlockHierarchicalContainerItem() : EntityBlockHierarchicalContainerItem(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockHierarchicalContainerItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string className() { return "EntityBlockHierarchicalContainerItem"; }
	virtual std::string getClassName(void) override { return EntityBlockHierarchicalContainerItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;

};

