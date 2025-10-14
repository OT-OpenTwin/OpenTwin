#include "EntityBlockCircuitCapacitor.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsItemFileCfg.h"

static EntityFactoryRegistrar<EntityBlockCircuitCapacitor> registrar("EntityBlockCircuitCapacitor");

EntityBlockCircuitCapacitor::EntityBlockCircuitCapacitor(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlockCircuitElement(ID, parent, obs, ms, owner)
{
	m_navigationOldTreeIconName = "Capacitor";
	m_navigationOldTreeIconNameHidden = "Capacitor";
	m_blockTitle = "Capacitor";

	const std::string connectorNameLeft = "Left7";
	m_LeftConnector = { ot::ConnectorType::Out,connectorNameLeft,connectorNameLeft };
	m_connectorsByName[connectorNameLeft] = m_LeftConnector;

	const std::string connectorNameRight = "Right7";
	m_RightConnector = { ot::ConnectorType::In,connectorNameRight,connectorNameRight };
	m_connectorsByName[connectorNameRight] = m_RightConnector;
}

void EntityBlockCircuitCapacitor::createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID)
{
	EntityBlockCircuitElement::createProperties(_circuitModelFolderName + "/Capacitor/", _circuitModelFolderID);
	EntityPropertiesString::createProperty("Element Property", "Capacity", "10uF", "default", getProperties());
}

const double EntityBlockCircuitCapacitor::getRotation() {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

const std::string EntityBlockCircuitCapacitor::getFlip() {
	auto propertyBase = getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
	return value;
}

bool EntityBlockCircuitCapacitor::updateFromProperties(void)
{
	bool refresh = false;
	refresh = EntityBlockCircuitElement::updateFromProperties();
	

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();
	}

	return refresh;
}

std::string EntityBlockCircuitCapacitor::getElementType()
{
	auto propertyBase = getProperties().getProperty("Capacity");
	auto elementType = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(elementType != nullptr);

	return elementType->getValue();
}

std::string EntityBlockCircuitCapacitor::getTypeAbbreviation() {
	return "C";
}

std::string EntityBlockCircuitCapacitor::getFolderName() {
	return "Capacitor";
}

#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitCapacitor::CreateBlockCfg()
{
#if TEST_ITEM_LOADER==true
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitCapacitor");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/Capacitor.ot.json");

	std::string name = getNameOnly();
	
	newConfig->addStringMapEntry("Name", name);
	
	

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
	ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();

	myStack->setName("EntityBlock");
	myStack->setTitle("EntityBlockCircuitCapacitor");
	myStack->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);

	//Second I create an Image
	ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
	image->setImagePath("CircuitElementImages/Capacitor.png");
	image->setSizePolicy(ot::SizePolicy::Dynamic);
	image->setMaintainAspectRatio(true);

	myStack->addItemBottom(image, false, true);

	//Then I create a layout
	ot::GraphicsHBoxLayoutItemCfg* myLayout = new ot::GraphicsHBoxLayoutItemCfg();

	myLayout->setMinimumSize(ot::Size2DD(150.0, 150.0));
	myStack->addItemTop(myLayout, true, false);

	// Now i want connections on the item for this i need rectangle items
	ot::GraphicsEllipseItemCfg* connection1 = new ot::GraphicsEllipseItemCfg();
	connection1->setName("Left7");
	ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection1->setOutline(ot::PenFCfg(1., ot::Color(ot::Black)));
	connection1->setBackgroundPainer(painter1);
	connection1->setAlignment(ot::AlignCenter);
	connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));

	ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
	connection2->setName("Right7");
	ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection2->setOutline(ot::PenFCfg(1., ot::Color(ot::Black)));
	connection2->setBackgroundPainer(painter2);
	connection2->setAlignment(ot::AlignCenter);
	connection2->setMaximumSize(ot::Size2DD(10.0, 10.0));

	connection1->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	connection2->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);


	//Here i add them to the Layout
	myLayout->addChildItem(connection1);
	myLayout->addStrech(1);
	myLayout->addChildItem(connection2);

	return myStack;
}

void EntityBlockCircuitCapacitor::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitCapacitor::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}