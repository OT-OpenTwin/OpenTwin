#include "EntityBlockCircuitVoltageSource.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsItemFileCfg.h"

EntityBlockCircuitVoltageSource::EntityBlockCircuitVoltageSource(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlockCircuitElement(ID, parent, obs, ms, factory, owner) {
	_navigationOldTreeIconName = "VoltageSource";
	_navigationOldTreeIconNameHidden = "VoltageSource";
	_blockTitle = "Voltage Source";

	

	const std::string connectorPositive = "positivePole";
	m_positiveConnector = { ot::ConnectorType::In,connectorPositive,connectorPositive };
	_connectorsByName[connectorPositive] = m_positiveConnector;

	const std::string connectorNegative = "negativePole";
	m_negativeConnector = { ot::ConnectorType::Out,connectorNegative,connectorNegative };
	_connectorsByName[connectorNegative] = m_negativeConnector;
	

}

void EntityBlockCircuitVoltageSource::createProperties() {
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

void EntityBlockCircuitVoltageSource::createACProperties() {
	createAmplitudeProperties();
}

void EntityBlockCircuitVoltageSource::createDCProperties() {
	EntityPropertiesString::createProperty("DC-Properties", "DC-Voltage", "100", "default", getProperties());
}

void EntityBlockCircuitVoltageSource::createPULSEProperties() {
	EntityPropertiesString::createProperty("Pulse Properties", "Pulse Initial value", "0", "default", getProperties());
	EntityPropertiesString::createProperty("Pulse Properties", "Pulse Pulsed value", "0", "default", getProperties());
	EntityPropertiesString::createProperty("Pulse Properties", "Delay time", "0.0", "default", getProperties());
	EntityPropertiesString::createProperty("Pulse Properties", "Rise time", "0.0", "default", getProperties());
	EntityPropertiesString::createProperty("Pulse Properties", "Fall time", "0.0", "default", getProperties());
	EntityPropertiesString::createProperty("Pulse Properties", "Pulse width", "0.0", "default", getProperties());
	EntityPropertiesString::createProperty("Pulse Properties", "Period", "0.0", "default", getProperties());
	EntityPropertiesString::createProperty("Pulse Properties", "Number of Pulses", "0", "default", getProperties());
}

double EntityBlockCircuitVoltageSource::getRotation() {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

std::string EntityBlockCircuitVoltageSource::getFlip() {
	auto propertyBase = getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
	return value;
}



std::vector<std::string> EntityBlockCircuitVoltageSource::getPulseParameters() {

	std::vector<std::string> parameters;

	auto propertyBase = getProperties().getProperty("Pulse Initial value");
	auto property1 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property1->getValue());

	propertyBase = getProperties().getProperty("Pulse Pulsed value");
	auto property2 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property2->getValue());

	propertyBase = getProperties().getProperty("Delay time");
	auto property3 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property3->getValue());

	propertyBase = getProperties().getProperty("Rise time");
	auto property4 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property4->getValue());

	propertyBase = getProperties().getProperty("Fall time");
	auto property5 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property5->getValue());

	propertyBase = getProperties().getProperty("Pulse width");
	auto property6 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property6->getValue());

	propertyBase = getProperties().getProperty("Period");
	auto property7 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property7->getValue());

	propertyBase = getProperties().getProperty("Number of Pulses");
	auto property8 = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	parameters.push_back(property8->getValue());

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
	EntityPropertiesInteger::createProperty("Sin Properties", "Offset", 0, "default", getProperties());
	EntityPropertiesInteger::createProperty("Sin Properties", "Amplitude", 0, "default", getProperties());
	EntityPropertiesInteger::createProperty("Sin Properties", "Frequency", 0, "default", getProperties());
	EntityPropertiesDouble::createProperty("Sin Properties", "Delay", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("Sin Properties", "Damping factor", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("Sin Properties", "Phase", 0.0, "default", getProperties());
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
	EntityPropertiesInteger::createProperty("Exponential Properties", "Exponential Initial Value", 0, "default", getProperties());
	EntityPropertiesInteger::createProperty("Exponential Properties", "Exponential Pulsed Value", 0, "default", getProperties());
	EntityPropertiesDouble::createProperty("Exponential Properties", "Rise delay time", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("Exponential Properties", "Rise time constant", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("Exponential Properties", "Fall delay time", 0.0, "default", getProperties());
	EntityPropertiesDouble::createProperty("Exponential Properties", "Fall time constant", 0.0, "default", getProperties());
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
	CreateBlockItem();
	return refresh;
}


std::string EntityBlockCircuitVoltageSource::getVoltage()
{
	auto propertyBase = getProperties().getProperty("DC-Voltage");
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
	auto propertyBase = getProperties().getProperty("TRAN");
	auto function = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(function != nullptr);

	return function->getValue();
}



#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitVoltageSource::CreateBlockCfg()
{
#if TEST_ITEM_LOADER==true
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitVoltageSource");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/VoltageSource.ot.json");

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
	connection1->setOutline(ot::PenFCfg(1., ot::Color(ot::Black)));
	connection1->setBackgroundPainer(painter1);
	connection1->setAlignment(ot::AlignCenter);
	connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));
	//connection1->setMargins(10.0, 0.0, 0.0, 0.0);

	ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
	connection2->setName("Right3");
	ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection2->setOutline(ot::PenFCfg(1., ot::Color(ot::Black)));
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


