#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockStorage : public EntityBlock
{
public:
	EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	virtual std::string getClassName(void) override { return "EntityBlockStorage"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	void createProperties();

private:
	ot::Connector _dataInput;
};
