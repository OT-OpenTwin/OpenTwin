#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockCircuitElement : public EntityBlock
{
public:
	EntityBlockCircuitElement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockCircuitElement"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties(const ot::UID& _circuitModelFolderID);
	virtual std::string getTypeAbbreviation() = 0;
	
	virtual ot::GraphicsItemCfg* CreateBlockCfg() = 0;

	virtual std::string getCircuitModel();

private:




	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};