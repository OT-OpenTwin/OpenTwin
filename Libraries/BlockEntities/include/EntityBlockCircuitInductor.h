#pragma once
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitInductor : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitInductor() : EntityBlockCircuitInductor(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockCircuitInductor(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockCircuitInductor"; }
	virtual std::string getClassName(void) override { return EntityBlockCircuitInductor::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) override;
	std::string getElementType();
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	virtual ot::GraphicsItemCfg* createBlockCfg() override;

	const double getRotation();
	const std::string getFlip();
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	const ot::Connector getRightConnector() const { return m_RightConnector; }
	virtual bool updateFromProperties(void) override;
private:

	ot::Connector m_LeftConnector;
	ot::Connector m_RightConnector;


	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};
