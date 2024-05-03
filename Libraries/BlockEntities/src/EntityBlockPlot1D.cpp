#include "EntityBlockPlot1D.h"
#include "OTCommunication/ActionTypes.h"

EntityBlockPlot1D::EntityBlockPlot1D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "Plot1DVisible";
	_navigationTreeIconNameHidden = "Plot1DVisible";
	_blockTitle = "Plot 1D";

	const std::string index = "1";
	const std::string connectorNameYAxis = _connectorYAxisNameBase + index;
	const std::string connectorTitleYAxis = _connectorYAxisTitleBase + index;
	_connectorsByName[connectorNameYAxis] = { ot::ConnectorType::In ,connectorNameYAxis,connectorTitleYAxis };

	const std::string connectorNameXAxis = "XAxis";
	const std::string connectorTitleXAxis = "X-Axis";
	_xAxisConnector = { ot::ConnectorType::In ,connectorNameXAxis, connectorTitleXAxis };
	_connectorsByName[connectorNameXAxis] = _xAxisConnector;
}

bool EntityBlockPlot1D::updateFromProperties()
{
	
	int selectedNumberOfCurves = getNumberOfCurves();

	bool refresh = false;

	if (_connectorsByName.size() > selectedNumberOfCurves + _numberOfConnectorsUnrelatedToCurves) //Number of Curve connectors + one connector for the x - axis
	{
		//Remove curves connectors
		for (size_t i = _connectorsByName.size() - _numberOfConnectorsUnrelatedToCurves; i > selectedNumberOfCurves ; i--)
		{
			std::string index = std::to_string(i);
			const std::string connectorName = _connectorYAxisNameBase +index;
			_connectorsByName.erase(connectorName);
			const std::string propertyName = _propertyCurveNameBase + index;
			getProperties().deleteProperty(propertyName);
		}
		refresh = true;
	}
	else if (_connectorsByName.size() < selectedNumberOfCurves + _numberOfConnectorsUnrelatedToCurves)
	{
		//Add curve connectors
		AddDynamicNumberOfCurves(selectedNumberOfCurves);
		refresh = true;
	}
	if(refresh)
	{
		CreateBlockItem();
	}

	getProperties().forceResetUpdateForAllProperties();

	return refresh;
}

void EntityBlockPlot1D::createProperties()
{
	EntityPropertiesString::createProperty("Graph properties", "X-Axis Label", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "Y-Axis Label", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "X-Axis Unit", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "Y-Axis Unit", "", "default", getProperties());

	EntityPropertiesInteger::createProperty("Data properties", "Number of Curves", 1, "default", getProperties());
	EntityPropertiesString::createProperty(_propertyGroupYAxisDefinition,_propertyCurveNameBase + "1", "", "default", getProperties());
}

std::string EntityBlockPlot1D::getXLabel()
{
	auto propertyBase = getProperties().getProperty("X-Axis Label");
	auto xAxisLabel = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(xAxisLabel != nullptr);

	return xAxisLabel->getValue();
}

std::string EntityBlockPlot1D::getYLabel()
{
	auto propertyBase = getProperties().getProperty("Y-Axis Label");
	auto yAxisLabel = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(yAxisLabel != nullptr);

	return yAxisLabel->getValue();
}

std::string EntityBlockPlot1D::getXUnit()
{
	auto propertyBase = getProperties().getProperty("X-Axis Unit");
	auto xAxisUnit = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(xAxisUnit != nullptr);

	return xAxisUnit->getValue();
}

std::string EntityBlockPlot1D::getYUnit()
{
	auto propertyBase = getProperties().getProperty("Y-Axis Unit");
	auto yAxisUnit = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(yAxisUnit != nullptr);

	return yAxisUnit->getValue();
}

void EntityBlockPlot1D::AddDynamicNumberOfCurves(int numberOfCurves)
{
	for (size_t i = _connectorsByName.size() - _numberOfConnectorsUnrelatedToCurves; i <= numberOfCurves; i++)
	{
		const std::string index = std::to_string(i);
		const std::string connectorNameYAxis = _connectorYAxisNameBase + index;
		const std::string connectorTitleYAxis = _connectorYAxisTitleBase + index;
		ot::Connector newCurveConnector = { ot::ConnectorType::In ,connectorNameYAxis,connectorTitleYAxis };
		_connectorsByName[connectorNameYAxis] = newCurveConnector;
		EntityPropertiesString::createProperty(_propertyGroupYAxisDefinition, _propertyCurveNameBase + index, "", "default", getProperties());
	}
}

void EntityBlockPlot1D::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockPlot1D::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}

ot::GraphicsItemCfg* EntityBlockPlot1D::CreateBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->CreateBlockHeadline());
	//block.setBackgroundImageInsertMode(ot::GraphicsFlowItemBuilder::OnStack);

	const ot::Color colourTitle(ot::Yellow);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setBackgroundImagePath("Images/Graph.svg");
	//block.setToolTip("Plots the X and Y values in a 1D plot");
	AddConnectors(block);
	
	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

const std::list<const ot::Connector*> EntityBlockPlot1D::getConnectorsYAxis()
{
	std::list<const ot::Connector*> allYAxisConnectors;
	for (const auto& connector : _connectorsByName)
	{
		if (connector.first.find(_connectorYAxisNameBase) != std::string::npos)
		{
			allYAxisConnectors.push_back(&connector.second);
		}
	}
	return allYAxisConnectors;
}

const std::list<std::string> EntityBlockPlot1D::getCurveNames()
{
	auto allProperties = getProperties().getListOfAllProperties();
	
	std::list<std::string> allCurveNames;
	
	for (auto property : allProperties)
	{
		if (property->getName().find(_propertyCurveNameBase) != std::string::npos)
		{
			EntityPropertiesString* strProperty = dynamic_cast<EntityPropertiesString*>(property);
			assert(strProperty != nullptr);
			allCurveNames.push_back(strProperty->getValue());
		}
	}
	return allCurveNames;
}

const int EntityBlockPlot1D::getNumberOfCurves()
{
	const auto baseProperty = getProperties().getProperty("Number of Curves");
	const auto intProperty = dynamic_cast<EntityPropertiesInteger*>(baseProperty);
	const int selectedNumberOfCurves = intProperty->getValue();
	if(selectedNumberOfCurves < 1) //At least one curve shall be displayed
	{
		return 1;
	}
	else
	{
		return selectedNumberOfCurves;
	}
}
