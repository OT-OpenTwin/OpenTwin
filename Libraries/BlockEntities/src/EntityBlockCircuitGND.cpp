#include "EntityBlockCircuitGND.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/GraphicsGroupItemCfg.h"
#include "OTGui/StyleRefPainter2D.h"

static EntityFactoryRegistrar<EntityBlockCircuitGND> registrar("EntityBlockCircuitGND");

EntityBlockCircuitGND::EntityBlockCircuitGND(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlockCircuitElement(ID, parent, obs, ms, owner) {
	m_navigationOldTreeIconName = "GND";
	m_navigationOldTreeIconNameHidden = "GND";
	m_blockTitle = "GND";

	const std::string connectorNameLeft = "GND";
	m_LeftConnector = { ot::ConnectorType::Out,connectorNameLeft,connectorNameLeft };
	m_connectorsByName[connectorNameLeft] = m_LeftConnector;

}

void EntityBlockCircuitGND::createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) {
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesSelection::createProperty("Transform-Properties", "Flip", { "NoFlip" , "FlipVertically" , "FlipHorizontally" }, "NoFlip", "default", getProperties());
}

std::string EntityBlockCircuitGND::getTypeAbbreviation() {
	return "GND";
}

std::string EntityBlockCircuitGND::getFolderName() {
	return "GND";
}




double EntityBlockCircuitGND::getRotation() {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

std::string EntityBlockCircuitGND::getFlip() {
	auto propertyBase = getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
	return value;
}


#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitGND::CreateBlockCfg() {
#if TEST_ITEM_LOADER==true
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitGND");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/GND.ot.json");

	
	//Map of String to Enum
	std::map<std::string, ot::Transform::FlipState> stringFlipMap;
	stringFlipMap.insert_or_assign("NoFlip", ot::Transform::NoFlip);
	stringFlipMap.insert_or_assign("FlipVertically", ot::Transform::FlipVertically);
	stringFlipMap.insert_or_assign("FlipHorizontally", ot::Transform::FlipHorizontally);



	double rotation = getRotation();
	std::string flip = getFlip();
	ot::Transform::FlipState flipState(stringFlipMap[flip]);


	ot::Transform transform;
	transform.setRotation(rotation);
	transform.setFlipState(flipState);
	newConfig->setTransform(transform);

	return newConfig;
#endif


	
}

bool EntityBlockCircuitGND::updateFromProperties(void)
{
	bool refresh = false;
	refresh = EntityBlockCircuitElement::updateFromProperties();

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();

	}
	
	return refresh;
}

void EntityBlockCircuitGND::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitGND::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}
