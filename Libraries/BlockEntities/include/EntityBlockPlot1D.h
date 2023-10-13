#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockPlot1D : public EntityBlock
{
public:
	EntityBlockPlot1D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockPlot1D"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	virtual void addVisualizationNodes(void) override;
	void createProperties();

	std::string getXLabel();
	std::string getYLabel();

	std::string getXUnit();
	std::string getYUnit();

private:
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
};
