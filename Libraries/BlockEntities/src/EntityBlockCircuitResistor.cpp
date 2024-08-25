#include "EntityBlockCircuitResistor.h"
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

EntityBlockCircuitResistor::EntityBlockCircuitResistor(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlockCircuitElement(ID, parent, obs, ms, factory, owner) {
	_navigationTreeIconName = "ResistorBG";
	_navigationTreeIconNameHidden = "ResistorBG";
	_blockTitle = "Resistor";

	const std::string connectorNameLeft = "Left1";
	m_LeftConnector = { ot::ConnectorType::Out,connectorNameLeft,connectorNameLeft };
	_connectorsByName[connectorNameLeft] = m_LeftConnector;

	const std::string connectorNameRight = "Right1";
	m_RightConnector = { ot::ConnectorType::In,connectorNameRight,connectorNameRight };
	_connectorsByName[connectorNameRight] = m_RightConnector;

}

void EntityBlockCircuitResistor::createProperties() {
	EntityPropertiesString::createProperty("Element Property", "ElementType", "200", "default", getProperties());
}

double EntityBlockCircuitResistor::getRotation() {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

std::string EntityBlockCircuitResistor::getFlip() {
	auto propertyBase = getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
	return value;
}

std::string EntityBlockCircuitResistor::getElementType() {
	auto propertyBase = getProperties().getProperty("ElementType");
	auto elementType = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(elementType != nullptr);

	return elementType->getValue();
}
#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitResistor::CreateBlockCfg() {
#if TEST_ITEM_LOADER==true
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitResistor");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGrid | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState);
	newConfig->setFile("Circuit/Resistor.ot.json");
	
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


	/*ot::GraphicsGroupItemCfg* group = new ot::GraphicsGroupItemCfg;
	group->setName("EntityBlockCircuitResistor");
	group->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGrid | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState);
	group->addStringMapEntry("Name", "R1");*/

	ot::GraphicsHBoxLayoutItemCfg* group = new ot::GraphicsHBoxLayoutItemCfg();
	group->setMinimumSize(ot::Size2DD(150.0, 50.0));
	group->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemIsMoveable);
	

	ot::GraphicsEllipseItemCfg* ellipse1 = new ot::GraphicsEllipseItemCfg();
	ellipse1->setName("Left1");
	ellipse1->setOutline(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	ellipse1->setBackgroundPainer(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
	ellipse1->setAlignment(ot::AlignCenter);
	ellipse1->setMaximumSize(ot::Size2DD(10.0, 10.0));
	ellipse1->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemIsConnectable);

	ot::GraphicsEllipseItemCfg* ellipse2 = new ot::GraphicsEllipseItemCfg();
	ellipse2->setName("Right1");
	ellipse2->setOutline(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	ellipse2->setBackgroundPainer(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
	ellipse2->setAlignment(ot::AlignCenter);
	ellipse2->setMaximumSize(ot::Size2DD(10.0, 10.0));
	ellipse2->setPosition(ot::Point2DD(20., 0.0));
	ellipse2->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemIsConnectable);

	group->addChildItem(ellipse1);
	group->addChildItem(ellipse2);

	return group;
	
	//ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();
	//myStack->setName("EntityBlockCircuitResistor");
	//myStack->setTitle("EntityBlockCircuitResistor");
	//myStack->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);


	////Second I create an Image
	//ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
	//image->setImagePath("CircuitElementImages/ResistorBG.png");
	//image->setSizePolicy(ot::SizePolicy::Dynamic);
	//image->setMaintainAspectRatio(true);

	//myStack->addItemBottom(image, false, true);

	////Then I create a layout
	//ot::GraphicsHBoxLayoutItemCfg* myLayout = new ot::GraphicsHBoxLayoutItemCfg();

	//myLayout->setMinimumSize(ot::Size2DD(150.0, 50.0));
	//myStack->addItemTop(myLayout, true, false);





	////Now i want connections on the item for this i need rectangle items
	//ot::GraphicsEllipseItemCfg* connection1 = new ot::GraphicsEllipseItemCfg();
	//connection1->setName("Left1");
	//ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Blue));
	//connection1->setOutline(ot::OutlineF(1., ot::Color(ot::Black)));
	//connection1->setBackgroundPainer(painter1);
	//connection1->setAlignment(ot::AlignCenter);
	//connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));

	//ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
	//connection2->setName("Right1");
	//ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Blue));
	//connection2->setOutline(ot::OutlineF(1., ot::Color(ot::Black)));
	//connection2->setBackgroundPainer(painter2);
	//connection2->setAlignment(ot::AlignCenter);
	//connection2->setMaximumSize(ot::Size2DD(10.0, 10.0));

	//connection1->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	//connection2->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);


	////Here i add them to the Layout
	//myLayout->addChildItem(connection1);
	//myLayout->addStrech(1);
	//myLayout->addChildItem(connection2);





	//return myStack;
}

bool EntityBlockCircuitResistor::updateFromProperties(void)
{
	bool refresh = false;

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();

	}
	CreateBlockItem();
	return refresh;
}

void EntityBlockCircuitResistor::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitResistor::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}
