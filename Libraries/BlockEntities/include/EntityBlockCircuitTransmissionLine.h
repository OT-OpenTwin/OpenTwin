#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitTransmissionLine : public EntityBlockCircuitElement {
public:
	EntityBlockCircuitTransmissionLine(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockCircuitTransmissionLine"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) override;
	std::string getImpedance();
	std::string getTransmissionDelay();
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;


	double getRotation();
	std::string getFlip();

	const ot::Connector getLeftConnectorPos1() const { return m_LeftConnectorPos1; }
	const ot::Connector getLeftConnectorPos2() const { return m_LeftConnectorPos2; }
	const ot::Connector getRightConnectorNeg1() const { return m_RightConnectorNeg1; }
	const ot::Connector getRightConnectorNeg2() const { return m_RightConnectorNeg2; }

	virtual bool updateFromProperties(void) override;
private:

	ot::Connector m_LeftConnectorPos1;
	ot::Connector m_LeftConnectorPos2;
	ot::Connector m_RightConnectorNeg1;
	ot::Connector m_RightConnectorNeg2;
	

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};