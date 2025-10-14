#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitCapacitor : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitCapacitor() : EntityBlockCircuitCapacitor(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockCircuitCapacitor(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockCircuitCapacitor"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) override;
	std::string getElementType();
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

	const double getRotation();
	const std::string getFlip();
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	const ot::Connector getRightConnector() const { return m_RightConnector; }
	virtual bool updateFromProperties(void) override;
private:

	ot::Connector m_LeftConnector;
	ot::Connector m_RightConnector;


	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


}; 
