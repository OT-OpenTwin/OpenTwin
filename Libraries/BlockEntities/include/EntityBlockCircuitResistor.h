#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitResistor : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitResistor(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockCircuitResistor"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties() override;
	std::string getResistance();

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;


	double getRotation();
	std::string getFlip();
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	const ot::Connector getRightConnector() const { return m_RightConnector; }
	virtual bool updateFromProperties(void) override;
private:

	ot::Connector m_LeftConnector;
	ot::Connector m_RightConnector;
	//std::list<Connection> listOfConnections

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};