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

	const std::string connectorNameLeft = "Left3";
	m_LeftConnector = { ot::ConnectorType::In,connectorNameLeft,connectorNameLeft };
	_connectorsByName[connectorNameLeft] = m_LeftConnector;

	const std::string connectorNameRight = "Right3";
	m_RightConnector = { ot::ConnectorType::Out,connectorNameRight,connectorNameRight };
	_connectorsByName[connectorNameRight] = m_RightConnector;

}

void EntityBlockCircuitVoltageSource::createProperties()
{
	createDCProperties();
	createTRANProperties();
	createACProperties();




	SetVisiblePULSEProperties(false);
	SetVisibleSINProperties(false);
	SetVisibleEXPProperties(false);
	
	
}


void EntityBlockCircuitVoltageSource::createTRANProperties() {

	EntityPropertiesSelection::createProperty("TRAN-Properties", "TRAN", { "PULSE", "SIN", "EXP"}, "", "default", getProperties());

	createPULSEProperties();
	createSINProperties();
	createEXPProperties();

}

void EntityBlockCircuitVoltageSource::createACProperties()
{
	createAmplitudeProperties();
}

void EntityBlockCircuitVoltageSource::createDCProperties()
{
	EntityPropertiesString::createProperty("DC-Properties", "DC-Voltage", "100", "default", getProperties());
}

void EntityBlockCircuitVoltageSource::createPULSEProperties()
{
	EntityPropertiesInteger::createProperty("", "Pulse Initial value", 0 , "default", getProperties());
	EntityPropertiesInteger::createProperty("", "Pulse Pulsed value", 0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Delay time", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Rise time", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Fall time", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Pulse width", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Period", 0.0, "default", getProperties());
	EntityPropertiesInteger::createProperty("", "Number of Pulses", 0, "default", getProperties());
}

std::vector<std::string> EntityBlockCircuitVoltageSource::getPulseParameters()
{

	std::vector<std::string> parameters;

	auto propertyBase = getProperties().getProperty("Pulse Initial value");
	auto property1 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	int val1 = property1->getValue();
	parameters.push_back(std::to_string(val1));

	propertyBase = getProperties().getProperty("Pulse Pulsed value");
	auto property2 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	int val2 = property2->getValue();
	parameters.push_back(std::to_string(val2));

	propertyBase = getProperties().getProperty("Delay time");
	auto property3 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val3 = property3->getValue();
	parameters.push_back(std::to_string(val3));

	propertyBase = getProperties().getProperty("Rise time");
	auto property4 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val4 = property4->getValue();
	parameters.push_back(std::to_string(val4));

	propertyBase = getProperties().getProperty("Fall time");
	auto property5 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val5 = property5->getValue();
	parameters.push_back(std::to_string(val5));

	propertyBase = getProperties().getProperty("Pulse width");
	auto property6 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val6 = property6->getValue();
	parameters.push_back(std::to_string(val6));

	propertyBase = getProperties().getProperty("Period");
	auto property7 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val7 = property7->getValue();
	parameters.push_back(std::to_string(val7));

	propertyBase = getProperties().getProperty("Number of Pulses");
	auto property8 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	double val8 = property8->getValue();
	parameters.push_back(std::to_string(val8));

	return parameters;

}

bool EntityBlockCircuitVoltageSource::SetVisiblePULSEProperties(bool visible)
{
	const bool isVisible = getProperties().getProperty("Pulse Initial value")->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty("Pulse Initial value")->setVisible(visible);
		getProperties().getProperty("Pulse Pulsed value")->setVisible(visible);
		getProperties().getProperty("Delay time")->setVisible(visible);
		getProperties().getProperty("Rise time")->setVisible(visible);
		getProperties().getProperty("Fall time")->setVisible(visible);
		getProperties().getProperty("Pulse width")->setVisible(visible);
		getProperties().getProperty("Period")->setVisible(visible);
		getProperties().getProperty("Number of Pulses")->setVisible(visible);
		this->setModified();
	}

	return refresh;
}

void EntityBlockCircuitVoltageSource::createSINProperties()
{
	EntityPropertiesInteger::createProperty("", "Offset", 0, "default", getProperties());
	EntityPropertiesInteger::createProperty("", "Amplitude", 0, "default", getProperties());
	EntityPropertiesInteger::createProperty("", "Frequency", 0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Delay", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Damping factor", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Phase", 0.0, "default", getProperties());
}

std::vector<std::string> EntityBlockCircuitVoltageSource::getSinParameters()
{

	std::vector<std::string> parameters;

	auto propertyBase = getProperties().getProperty("Offset");
	auto property1 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	int val1 = property1->getValue();
	parameters.push_back(std::to_string(val1));

	propertyBase = getProperties().getProperty("Amplitude");
	auto property2 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	int val2 = property2->getValue();
	parameters.push_back(std::to_string(val2));

	propertyBase = getProperties().getProperty("Frequency");
	auto property3 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	int val3 = property3->getValue();
	parameters.push_back(std::to_string(val3));

	propertyBase = getProperties().getProperty("Delay");
	auto property4 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val4 = property4->getValue();
	parameters.push_back(std::to_string(val4));

	propertyBase = getProperties().getProperty("Damping factor");
	auto property5 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val5 = property5->getValue();
	parameters.push_back(std::to_string(val5));

	propertyBase = getProperties().getProperty("Phase");
	auto property6 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val6 = property6->getValue();
	parameters.push_back(std::to_string(val6));

	return parameters;

}


bool EntityBlockCircuitVoltageSource::SetVisibleSINProperties(bool visible)
{
	const bool isVisible = getProperties().getProperty("Offset")->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty("Offset")->setVisible(visible);
		getProperties().getProperty("Amplitude")->setVisible(visible);
		getProperties().getProperty("Frequency")->setVisible(visible);
		getProperties().getProperty("Delay")->setVisible(visible);
		getProperties().getProperty("Damping factor")->setVisible(visible);
		getProperties().getProperty("Phase")->setVisible(visible);
		this->setModified();
	}
	return refresh;
}

void EntityBlockCircuitVoltageSource::createEXPProperties()
{
	EntityPropertiesInteger::createProperty("", "Exponential Initial Value", 0, "default", getProperties());
	EntityPropertiesInteger::createProperty("", "Exponential Pulsed Value", 0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Rise delay time", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Rise time constant", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Fall delay time", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("", "Fall time constant", 0.0, "default", getProperties());
}

std::vector<std::string> EntityBlockCircuitVoltageSource::getExpParameters()
{
	std::vector<std::string> parameters;

	auto propertyBase = getProperties().getProperty("Exponential Initial Value");
	auto property1 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	int val1 = property1->getValue();
	parameters.push_back(std::to_string(val1));

	propertyBase = getProperties().getProperty("Exponential Pulsed Value");
	auto property2 = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	int val2 = property2->getValue();
	parameters.push_back(std::to_string(val2));

	propertyBase = getProperties().getProperty("Rise delay time");
	auto property3 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val3 = property3->getValue();
	parameters.push_back(std::to_string(val3));

	propertyBase = getProperties().getProperty("Rise time constant");
	auto property4 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val4 = property4->getValue();
	parameters.push_back(std::to_string(val4));

	propertyBase = getProperties().getProperty("Fall delay time");
	auto property5 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val5 = property5->getValue();
	parameters.push_back(std::to_string(val5));

	propertyBase = getProperties().getProperty("Fall time constant");
	auto property6 = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double val6 = property6->getValue();
	parameters.push_back(std::to_string(val6));

	return parameters;

}


bool EntityBlockCircuitVoltageSource::SetVisibleEXPProperties(bool visible)
{
	const bool isVisible = getProperties().getProperty("Exponential Initial Value")->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty("Exponential Initial Value")->setVisible(visible);
		getProperties().getProperty("Exponential Pulsed Value")->setVisible(visible);
		getProperties().getProperty("Rise delay time")->setVisible(visible);
		getProperties().getProperty("Rise time constant")->setVisible(visible);
		getProperties().getProperty("Fall delay time")->setVisible(visible);
		getProperties().getProperty("Fall time constant")->setVisible(visible);
		this->setModified();
	}
	return refresh;
}


void EntityBlockCircuitVoltageSource::createAmplitudeProperties()
{
	EntityPropertiesInteger::createProperty("AC-Properties", "AC-Amplitude", 0, "default", getProperties());
}

std::string EntityBlockCircuitVoltageSource::getAmplitude()
{
	auto propertyBase = getProperties().getProperty("AC-Amplitude");
	auto property = dynamic_cast<EntityPropertiesInteger*>(propertyBase);
	assert(propertyBase != nullptr);
	return std::to_string(property->getValue());
}

bool EntityBlockCircuitVoltageSource::updateFromProperties(void)
{
	bool refresh = false;

	auto baseProperty = getProperties().getProperty("TRAN");
	auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(baseProperty);

	if (selectionProperty->getValue() == "PULSE") {
		refresh |= SetVisiblePULSEProperties(true);
		refresh |= SetVisibleSINProperties(false);
		refresh |= SetVisibleEXPProperties(false);
	}
	else if (selectionProperty->getValue() == "SIN") {
		refresh |= SetVisiblePULSEProperties(false);
		refresh |= SetVisibleSINProperties(true);
		refresh |= SetVisibleEXPProperties(false);
	}
	else if (selectionProperty->getValue() == "EXP") {
		refresh |= SetVisiblePULSEProperties(false);
		refresh |= SetVisibleSINProperties(false);
		refresh |= SetVisibleEXPProperties(true);
	}
	

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();
	}

	return refresh;
}


std::string EntityBlockCircuitVoltageSource::getVoltage()
{
	auto propertyBase = getProperties().getProperty("Voltage");
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
	myLayout->setMinimumSize(ot::Size2DD(150.0, 75.0));

	myStack->addItemTop(myLayout, true, false);

	ot::GraphicsEllipseItemCfg* connection1 = new ot::GraphicsEllipseItemCfg();
	connection1->setName("Left3");
	ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection1->setOutline(ot::OutlineF(1., ot::Color(ot::Black)));
	connection1->setBackgroundPainer(painter1);
	connection1->setAlignment(ot::AlignCenter);
	connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));
	//connection1->setMargins(10.0, 0.0, 0.0, 0.0);

	ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
	connection2->setName("Right3");
	ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection2->setOutline(ot::OutlineF(1., ot::Color(ot::Black)));
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






void EntityBlockCircuitVoltageSource::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitVoltageSource::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}


