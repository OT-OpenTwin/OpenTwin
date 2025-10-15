#include "EntityBlockFileWriter.h"
#include "SharedResources.h"
#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntityBlockFileWriter> registrar(EntityBlockFileWriter::className());

EntityBlockFileWriter::EntityBlockFileWriter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID,parent,obs,ms,owner)
{
	m_inputConnector = { ot::ConnectorType::In, "Input", "Input" };
	m_connectorsByName[m_inputConnector.getConnectorName()] = m_inputConnector;
	m_navigationOldTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_navigationOldTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_blockTitle = "File Writer";
}

void EntityBlockFileWriter::createProperties()
{
	EntityPropertiesString::createProperty("Headline", "Description", "", "default", getProperties());
	//sEntityPropertiesString::createProperty("File Properties", "Path", "", "default", getProperties());
	EntityPropertiesString::createProperty("File Properties", "File Name", "", "default", getProperties());
	
	EntityPropertiesSelection::createProperty("File Properties", "File type", { "txt", "json", "csv"}, "txt", "default", getProperties());
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


ot::GraphicsItemCfg* EntityBlockFileWriter::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());

	const ot::Color colourTitle(ot::Yellow);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameDB());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	
	addConnectors(block);

	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

const std::string EntityBlockFileWriter::getFileType()
{
	return PropertyHelper::getSelectionPropertyValue(this, "File type");
}

