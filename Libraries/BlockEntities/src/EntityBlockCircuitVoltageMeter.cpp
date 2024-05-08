#include "EntityBlockCircuitVoltageMeter.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"

EntityBlockCircuitVoltageMeter::EntityBlockCircuitVoltageMeter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "VoltMeter";
	_navigationTreeIconNameHidden = "VoltMeter";
	_blockTitle = "Voltage Meter";

	const std::string connectorNameLeft = "Left4";
	m_LeftConnector = { ot::ConnectorType::Out,connectorNameLeft,connectorNameLeft };
	_connectorsByName[connectorNameLeft] = m_LeftConnector;

	const std::string connectorNameRight = "Right4";
	m_RightConnector = { ot::ConnectorType::In,connectorNameRight,connectorNameRight };
	_connectorsByName[connectorNameRight] = m_RightConnector;
}

ot::GraphicsItemCfg* EntityBlockCircuitVoltageMeter::CreateBlockCfg()
{
	ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();
	myStack->setName("EntityBlock");
	myStack->setTitle("EntityBlockCircuitVoltageMeter");
	myStack->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);

	//Second I create an Image
	ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
	image->setImagePath("CircuitElementImages/VoltMeter.png");
	image->setSizePolicy(ot::SizePolicy::Dynamic);
	image->setMaintainAspectRatio(true);

	myStack->addItemBottom(image, false, true);

	//Then I create a layout
	ot::GraphicsHBoxLayoutItemCfg* myLayout = new ot::GraphicsHBoxLayoutItemCfg();

	myLayout->setMinimumSize(ot::Size2DD(150.0, 150.0));
	myStack->addItemTop(myLayout, true, false);

	// Now i want connections on the item for this i need rectangle items
	ot::GraphicsEllipseItemCfg* connection1 = new ot::GraphicsEllipseItemCfg();
	connection1->setName("Left4");
	ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection1->setOutline(ot::OutlineF(1., ot::Color(ot::Black)));
	connection1->setBackgroundPainer(painter1);
	connection1->setAlignment(ot::AlignCenter);
	connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));

	ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
	connection2->setName("Right4");
	ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection2->setOutline(ot::OutlineF(1., ot::Color(ot::Black)));
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

void EntityBlockCircuitVoltageMeter::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitVoltageMeter::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}