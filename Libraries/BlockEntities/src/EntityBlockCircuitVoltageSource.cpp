#include "EntityBlockCircuitVoltageSource.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"


EntityBlockCircuitVoltageSource::EntityBlockCircuitVoltageSource(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "VoltageSource";
	_navigationTreeIconNameHidden = "VoltageSource";
	_blockTitle = "Voltage Source";

	const std::string connectorNameLeft = "Left";
	m_LeftConnector = { ot::ConnectorType::In,connectorNameLeft,connectorNameLeft };
	_connectorsByName[connectorNameLeft] = m_LeftConnector;

	const std::string connectorNameRight = "Right";
	m_RightConnector = { ot::ConnectorType::Out,connectorNameRight,connectorNameRight };
	_connectorsByName[connectorNameRight] = m_RightConnector;

}

void EntityBlockCircuitVoltageSource::createProperties()
{
	EntityPropertiesString::createProperty("Element Property", "ElementType", "100", "default", getProperties());
	EntityPropertiesSelection::createProperty("Element Property", "Type", { "dc","ac" }, "dc", "default", getProperties());

	createACProperties();
	SetVisibleACProperties(false);
	
}

std::string EntityBlockCircuitVoltageSource::getElementType()
{
	auto propertyBase = getProperties().getProperty("ElementType");
	auto elementType = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(elementType != nullptr);

	return elementType->getValue();
}

std::string EntityBlockCircuitVoltageSource::getType()
{
	auto propertyBase = getProperties().getProperty("Type");
	auto elementType = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(elementType != nullptr);

	return elementType->getValue();
}

std::string EntityBlockCircuitVoltageSource::getFunction()
{
	auto propertyBase = getProperties().getProperty("Function");
	auto function = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(function != nullptr);

	return function->getValue();
}



ot::GraphicsItemCfg* EntityBlockCircuitVoltageSource::CreateBlockCfg()
{
	ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();
	myStack->setName("EntityBlockCircuitVoltageSource");
	myStack->setTitle("EntityBlockCircuitVoltageSource");
	myStack->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);

	ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
	image->setImagePath("CircuitElementImages/VoltageSource.png");
	image->setSizePolicy(ot::SizePolicy::Dynamic);
	image->setMaintainAspectRatio(true);

	myStack->addItemBottom(image, false, true);

	ot::GraphicsHBoxLayoutItemCfg* myLayout = new ot::GraphicsHBoxLayoutItemCfg();
	myLayout->setMinimumSize(ot::Size2DD(150.0, 150.0));

	myStack->addItemTop(myLayout, true, false);

	ot::GraphicsEllipseItemCfg* connection1 = new ot::GraphicsEllipseItemCfg();
	connection1->setName("Left");
	ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
	connection1->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
	connection1->setBackgroundPainer(painter1);
	connection1->setAlignment(ot::AlignCenter);
	connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));
	//connection1->setMargins(10.0, 0.0, 0.0, 0.0);

	ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
	connection2->setName("Right");
	ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Color::DefaultColor::Blue));
	connection2->setBorder(ot::Border(ot::Color(ot::Color::Black), 1));
	connection2->setBackgroundPainer(painter2);
	connection2->setAlignment(ot::AlignCenter);
	connection2->setMaximumSize(ot::Size2DD(10.0, 10.0));

	connection1->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	connection2->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);

	myLayout->addChildItem(connection1);
	myLayout->addStrech(1);
	myLayout->addChildItem(connection2);


	return myStack;
}

bool EntityBlockCircuitVoltageSource::updateFromProperties(void)
{
	bool refresh = false;
	auto baseProperty = getProperties().getProperty("Type");
	auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(baseProperty);
	if (selectionProperty->getValue() == "ac")
	{
		refresh = SetVisibleACProperties(true);
	}
	else
	{
		refresh = SetVisibleACProperties(false);
	}

	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}

	return refresh;

}

void EntityBlockCircuitVoltageSource::createACProperties()
{
	EntityPropertiesSelection::createProperty("AC-Properties", "Function", { "10 sin(0 1 1k)" }, "10 sin(0 1 1k)", "default", getProperties());
}

bool EntityBlockCircuitVoltageSource::SetVisibleACProperties(bool visible)
{
	const bool isVisible = getProperties().getProperty("Function")->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty("Function")->setVisible(visible);
		this->setModified();
	}
	return refresh;
}

void EntityBlockCircuitVoltageSource::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitVoltageSource::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}


