#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockConnection : public EntityBase
{
public:
	EntityBlockConnection(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockConnection"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	

	

private:

	ot::GraphicsConnectionCfg connection;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};

