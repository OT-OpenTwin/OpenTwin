#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitVoltageMeter : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitVoltageMeter() : EntityBlockCircuitVoltageMeter(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockCircuitVoltageMeter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockCircuitVoltageMeter"; }
	virtual std::string getClassName(void) const override { return "EntityBlockCircuitVoltageMeter"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) override;
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	double getRotation();
	std::string getFlip();
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties(void) override;
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	const ot::Connector getRightConnector() const { return m_RightConnector; }

private:

	ot::Connector m_LeftConnector;
	ot::Connector m_RightConnector;
	//std::list<Connection> listOfConnections

	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};