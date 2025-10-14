#include "EntityBlockCircuitConnector.h"
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/BasicGraphicsIntersectionItem.h"
#include "OTCore/LogDispatcher.h"

static EntityFactoryRegistrar<EntityBlockCircuitConnector> registrar("EntityBlockCircuitConnector");

EntityBlockCircuitConnector::EntityBlockCircuitConnector(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, owner)
{
	m_navigationOldTreeIconName = "Diod2";
	m_navigationOldTreeIconNameHidden = "Diod2";
	m_blockTitle = "Connector";
}



#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitConnector::CreateBlockCfg() {
#if TEST_ITEM_LOADER==true
	ot::BasicGraphicsIntersectionItem* newConfig = new ot::BasicGraphicsIntersectionItem;
	newConfig->setUid(this->getEntityID());
	newConfig->setName(this->getName());

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