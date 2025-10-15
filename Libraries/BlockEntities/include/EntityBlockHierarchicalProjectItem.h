//! @file EntityBlockHierarchicalProjectItem.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockHierarchicalProjectItem : public EntityBlock {
public:
	EntityBlockHierarchicalProjectItem() : EntityBlockHierarchicalProjectItem(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockHierarchicalProjectItem(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockHierarchicalProjectItem"; }
	virtual std::string getClassName(void) override { return EntityBlockHierarchicalProjectItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

private:
};

