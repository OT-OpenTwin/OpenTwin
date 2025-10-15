#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockCircuitConnector : public EntityBlock
{
public:
	EntityBlockCircuitConnector() : EntityBlockCircuitConnector(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockCircuitConnector(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	static std::string className() { return "EntityBlockCircuitConnector"; }
	virtual std::string getClassName(void) override { return EntityBlockCircuitConnector::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	
	
	
private:

	ot::Connector m_LeftConnector;


	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};