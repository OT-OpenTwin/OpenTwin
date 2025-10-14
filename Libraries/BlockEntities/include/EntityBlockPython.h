#pragma once

#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockPython : public EntityBlock
{
public:
	EntityBlockPython() : EntityBlockPython(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockPython"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	void createProperties(const std::string& scriptFolder, ot::UID scriptFolderID);
	std::string getSelectedScript();
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	virtual bool updateFromProperties() override;

	static const std::string getIconName() { return "Script.svg"; }
private:
	const std::string _propertyNameScripts = "Scripts";

	void updateBlockAccordingToScriptHeader();
	void resetBlockRelatedAttributes();
};
