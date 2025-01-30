#include "EntityBlockFileWriter.h"
#include "SharedResources.h"

EntityBlockFileWriter::EntityBlockFileWriter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID,parent,obs,ms,factory,owner)
{
	m_inputConnector = { ot::ConnectorType::In, "Input", "Input" };
	_connectorsByName[m_inputConnector.getConnectorName()] = m_inputConnector;
	_navigationOldTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	_navigationOldTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	_blockTitle = "File Writer";
}

void EntityBlockFileWriter::createProperties()
{
	EntityPropertiesString::createProperty("Headline", "Description", "", "default", getProperties());
	//sEntityPropertiesString::createProperty("File Properties", "Path", "", "default", getProperties());
	EntityPropertiesString::createProperty("File Properties", "File Name", "", "default", getProperties());
	
	//EntityPropertiesSelection::createProperty("File Properties", "Mode", { m_fileModeAppend, m_fileModeOverride }, m_fileModeAppend,"default",getProperties());
}

const std::string& EntityBlockFileWriter::getHeadline()
{
	auto descriptionPropertyBase = getProperties().getProperty("Description");
	assert(descriptionPropertyBase != nullptr);
	auto descriptionProperty = dynamic_cast<EntityPropertiesString*>(descriptionPropertyBase);
	assert(descriptionProperty != nullptr);
	return descriptionProperty->getValue();
}


const std::string& EntityBlockFileWriter::getFileName()
{
	auto propertyBase = getProperties().getProperty("File Name");
	assert(propertyBase != nullptr);
	auto propertyString = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyString != nullptr);
	return propertyString->getValue();
}


ot::GraphicsItemCfg* EntityBlockFileWriter::CreateBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->CreateBlockHeadline());

	const ot::Color colourTitle(ot::Yellow);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameDB());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	
	AddConnectors(block);

	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

