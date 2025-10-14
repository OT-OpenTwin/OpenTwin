#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockCircuitElement : public EntityBlock
{
public:
	EntityBlockCircuitElement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual void createProperties(const std::string& _circuitModelFolderName,const ot::UID& _circuitModelFolderID);
	virtual std::string getTypeAbbreviation() = 0;
	virtual std::string getFolderName() = 0;
	virtual bool updateFromProperties(void) override;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() = 0;

	virtual std::string getCircuitModel();

protected:
	virtual const std::string getCollectionType() { return "CircuitModels"; }
	virtual const std::string getCircuitModelFolder() { return "Circuit Models/"; }

private:




	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};