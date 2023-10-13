#pragma once

#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockPython : public EntityBlock
{
public:
	EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockPython"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	virtual void addVisualizationNodes(void) override;
	void createProperties(const std::string& scriptFolder, ot::UID scriptFolderID);
	std::string getSelectedScript();

private:
	
	ot::Color _colourTitle;
	ot::Color _colourBackground;


	std::string _blockTitle = "";
	std::string _blockName = "";
	std::string _imageName = "";

	ot::Point2DD _position;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
};
