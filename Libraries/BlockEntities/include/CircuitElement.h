#pragma once
#include "EntityBlock.h"
class __declspec(dllexport) CircuitElement : public EntityBlock
{
	public:
		CircuitElement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
		virtual std::string getClassName(void) override { return "CircuitElement"; };
		virtual entityType getEntityType(void) override { return TOPOLOGY; };
		//void createProperties();



	private:
	
		ot::Connector m_LeftConnector;
		ot::Connector m_RightConnector;
		//std::list<Connection> listOfConnections;

		virtual ot::GraphicsItemCfg* CreateBlockCfg() override;

		void AddStorageData(bsoncxx::builder::basic::document& storage) override;
		void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};