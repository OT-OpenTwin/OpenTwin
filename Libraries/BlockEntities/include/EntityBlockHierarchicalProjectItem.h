//! @file EntityBlockHierarchicalProjectItem.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "EntityBlock.h"
#include "OTCore/ProjectInformation.h"

class __declspec(dllexport) EntityBlockHierarchicalProjectItem : public EntityBlock {
public:
	EntityBlockHierarchicalProjectItem() : EntityBlockHierarchicalProjectItem(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockHierarchicalProjectItem(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	static std::string className() { return "EntityBlockHierarchicalProjectItem"; }
	virtual std::string getClassName(void) override { return EntityBlockHierarchicalProjectItem::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties() override;

	virtual void createProperties();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property accessors

	void setProjectInformation(const ot::ProjectInformation& _info);
	ot::ProjectInformation getProjectInformation() const;

	void setUseLatestVersion(bool _flag);
	bool getUseLatestVersion() const;

	void setCustomVersion(const std::string& _version);
	std::string getCustomVersion() const;
};

