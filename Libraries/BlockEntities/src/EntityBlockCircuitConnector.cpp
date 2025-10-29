// @otlicense

#include "EntityBlockCircuitConnector.h"
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/BasicGraphicsIntersectionItem.h"
#include "OTCore/LogDispatcher.h"

static EntityFactoryRegistrar<EntityBlockCircuitConnector> registrar(EntityBlockCircuitConnector::className());

EntityBlockCircuitConnector::EntityBlockCircuitConnector(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = "Diod2";
	icon.hiddenIcon = "Diod2";
	setNavigationTreeIcon(icon);

	setBlockTitle("Connector");

	resetModified();
}

#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitConnector::createBlockCfg() {
#if TEST_ITEM_LOADER==true
	ot::BasicGraphicsIntersectionItem* newConfig = new ot::BasicGraphicsIntersectionItem;
	newConfig->setUid(this->getEntityID());
	newConfig->setName(this->getName());

	return newConfig;
#endif
}

void EntityBlockCircuitConnector::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitConnector::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}