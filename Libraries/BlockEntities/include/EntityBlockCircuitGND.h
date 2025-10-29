// @otlicense

#pragma once
#include "EntityBlockCircuitElement.h"

class __declspec(dllexport) EntityBlockCircuitGND : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitGND() : EntityBlockCircuitGND(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockCircuitGND(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockCircuitGND"; }
	virtual std::string getClassName(void) const override { return EntityBlockCircuitGND::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) override;
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	virtual ot::GraphicsItemCfg* createBlockCfg() override;


	double getRotation();
	std::string getFlip();
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	virtual bool updateFromProperties(void) override;
private:

	ot::Connector m_LeftConnector;



	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};