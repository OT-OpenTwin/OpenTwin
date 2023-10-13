#include "EntityBlockPlot1D.h"
#include "OpenTwinCommunication/ActionTypes.h"

EntityBlockPlot1D::EntityBlockPlot1D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
}

void EntityBlockPlot1D::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = "Plot1DVisible";
		treeIcons.hiddenIcon = "Plot1DVisible";

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

		treeIcons.addToJsonDoc(&doc);

		getObserver()->sendMessageToViewer(doc);
	}
}

void EntityBlockPlot1D::createProperties()
{
	EntityPropertiesString::createProperty("Graph properties", "X-Axis Label", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "Y-Axis Label", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "X-Axis Unit", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "Y-Axis Unit", "", "default", getProperties());
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
	return nullptr;
}
