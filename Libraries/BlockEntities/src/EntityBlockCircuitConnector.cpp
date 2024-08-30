#include "EntityBlockCircuitConnector.h"
#include "OTGui/GraphicsItemFileCfg.h"

EntityBlockCircuitConnector::EntityBlockCircuitConnector(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "Diod2";
	_navigationTreeIconNameHidden = "Diod2";
	_blockTitle = "Connector";
	const std::string connectorNameLeft = "Connector";
	m_LeftConnector = { ot::ConnectorType::Out,connectorNameLeft,connectorNameLeft };
	_connectorsByName[connectorNameLeft] = m_LeftConnector;
}

#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitConnector::CreateBlockCfg() {
#if TEST_ITEM_LOADER==true
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitConnector");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGrid | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState);
	newConfig->setFile("Circuit/Connector.ot.json");


	return newConfig;
#endif
}

void EntityBlockCircuitConnector::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitConnector::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}