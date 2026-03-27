// @otlicense

// OpenTwin header
#include "OTCore/JSON/JSONVectoriser.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTModelEntities/EntityDatasetInfo.h"

static EntityFactoryRegistrar<ot::EntityDatasetInfo> registrar(ot::EntityDatasetInfo::className());

ot::EntityDatasetInfo::EntityDatasetInfo(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms)
	: EntityBase(_ID, _parent, _mdl, _ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/Hint");
	treeItem.setHiddenIcon("Tree/Hint");
	this->setDefaultTreeItem(treeItem);

	ot::VisualisationTypes visTypes = this->getVisualizationTypes();
	visTypes.addTableVisualisation();
	this->setDefaultVisualizationTypes(visTypes);
}

void ot::EntityDatasetInfo::addVisualizationNodes()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

bool ot::EntityDatasetInfo::updateFromProperties()
{
	assert(getProperties().anyPropertyNeedsUpdate());
	setModified();
	getProperties().forceResetUpdateForAllProperties();

	return false; 
}

void ot::EntityDatasetInfo::createProperties(const std::string& _initialProjectName)
{
	EntityPropertiesProjectList* projectListProp = EntityPropertiesProjectList::createProperty("General", "Project", _initialProjectName, "", getProperties());
	
	getProperties().forceResetUpdateForAllProperties();
}

ot::GenericDataStructMatrix ot::EntityDatasetInfo::getTable()
{
	auto obs = getObserver();
	if (!obs)
	{
		OT_LOG_W("Can not get metadata campaign. No observer for EntityDatasetInfo { \"Entity\": \"" + getName() + "\" }");
		return GenericDataStructMatrix();
	}

	std::string collectionName;
	auto campaign = obs->getMetadataCampaign(getProjectName(), collectionName);
	if (!campaign)
	{
		OT_LOG_W("No metadata campaign for project \"" + getProjectName() + "\"");
		return GenericDataStructMatrix();
	}

	std::list<std::string> rows;

	size_t colCount = 0;

	for (const auto& seriesMetadata : campaign->getSeriesMetadata())
	{
		std::list<std::string> columnHeader;
		JSONVectoriser::vectorise(seriesMetadata.getMetadata(), columnHeader, "");
		rows.splice(rows.end(), std::move(columnHeader));
	}

	GenericDataStructMatrix matrix(static_cast<uint32_t>(rows.size()), 1);
	uint32_t rowIndex = 0;
	for (const std::string& row : rows)
	{
		matrix.setValue({ 0, rowIndex }, ot::Variable(row));
		rowIndex++;
	}
	
	return matrix;
}

void ot::EntityDatasetInfo::setTable(const ot::GenericDataStructMatrix& _table)
{
	OT_LOG_E("EntityDatasetInfo can not be set from config");
}

ot::TableCfg ot::EntityDatasetInfo::getTableConfig(bool _includeData)
{
	const GenericDataStructMatrix matrix = getTable();
	ot::TableCfg cfg(matrix, getHeaderMode());

	cfg.setEntityInformation(getBasicEntityInformation());
	cfg.setTitle(getBasicEntityInformation().getEntityName());
	cfg.setReadOnly(true);

	return cfg;
}

char ot::EntityDatasetInfo::getDecimalDelimiter()
{
	return '.';
}

bool ot::EntityDatasetInfo::visualiseTable()
{
	return true;
}

ot::TableCfg::TableHeaderMode ot::EntityDatasetInfo::getHeaderMode()
{
	return ot::TableCfg::TableHeaderMode::Horizontal;
}

void ot::EntityDatasetInfo::setProjectName(const std::string& _projectName)
{
	PropertyHelper::setProjectPropertyValue(_projectName, this, "Project", "General");
}

std::string ot::EntityDatasetInfo::getProjectName() const
{
	return PropertyHelper::getProjectPropertyValue(this, "Project", "General");
}
