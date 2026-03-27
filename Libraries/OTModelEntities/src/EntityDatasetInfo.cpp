// @otlicense

// OpenTwin header
#include "OTCore/JSON/JSONVectoriser.h"
#include "OTCore/DataStruct/GenericDataStructMatrixMerger.h"
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

	JSONToVariableConverter jsonVar;

	std::list<GenericDataStructMatrix> matrices;

	for (const auto& seriesMetadata : campaign->getSeriesMetadata())
	{
		const JsonDocument& metadata = seriesMetadata.getMetadata();
		std::string dooc = json::toJson(metadata);
		std::list<std::string> columnHeader;
		JSONVectoriser::vectorise(metadata, columnHeader, "", JSONVectoriser::AddLeavesOnly);
		
		GenericDataStructMatrix matrix(2, static_cast<uint32_t>(columnHeader.size()));
		uint32_t columnIndex = 0;
		for (const std::string& col : columnHeader)
		{
			matrix.setValue(MatrixEntryPointer(0, columnIndex), ot::Variable(col));
			const JsonValue& val = JSONVectoriser::getValue(metadata, col);
			const std::string dbg = json::toJson(val);
			matrix.setValue(MatrixEntryPointer(1, columnIndex), jsonVar(val));

			columnIndex++;
		}

		matrices.push_back(std::move(matrix));
	}

	return GenericDataStructMatrixMerger::tableMerge(matrices, getHeaderMode() == ot::TableCfg::TableHeaderMode::Horizontal);
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
