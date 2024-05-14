#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockCircuitInductor : public EntityBlock
{
public:
	EntityBlockCircuitInductor(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockCircuitInductor"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	void createProperties();
	std::string getElementType();

	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	const ot::Connector getRightConnector() const { return m_RightConnector; }

private:

	ot::Connector m_LeftConnector;
	ot::Connector m_RightConnector;


	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};
