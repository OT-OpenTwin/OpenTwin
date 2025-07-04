#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitCurrentMeter : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitCurrentMeter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockCircuitCurrentMeter"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	void createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) override;
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	const double getRotation();
	const std::string getFlip();
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	virtual bool updateFromProperties(void) override;
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	const ot::Connector getRightConnector() const { return m_RightConnector; }

private:

	ot::Connector m_LeftConnector;
	ot::Connector m_RightConnector;
	//std::list<Connection> listOfConnections

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};