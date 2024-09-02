#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockCircuitConnector : public EntityBlock
{
public:
	EntityBlockCircuitConnector(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockCircuitConnector"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	
	
	
private:

	ot::Connector m_LeftConnector;


	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};