// @otlicense

// OpenTwin header
#include "OTCore/EntityName.h"
#include "OTCore/JSON/JSONVectoriser.h"
#include "OTCore/DataStruct/GenericDataStructMatrixMerger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
#include "OTModelEntities/EntityDatasetInfo.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTCore/PythonHeader/PythonHeaderAnalyser.h"
#include "OTCore/PythonHeader/PythonHeaderEventBuilder.h"
static EntityFactoryRegistrar<ot::EntityDatasetInfo> registrar(ot::EntityDatasetInfo::className());

ot::EntityDatasetInfo::EntityDatasetInfo(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms)
	: EntityBase(_ID, _parent, _mdl, _ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/Hint");
	treeItem.setHiddenIcon("Tree/Hint");
	treeItem.setIsEditable(true);
	this->setDefaultTreeItem(treeItem);

	this->setDeletable(true);

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
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	// No changes
	return false; 
}

void ot::EntityDatasetInfo::createProperties(const std::string& _initialProjectName, ot::UID _scriptFolder, ot::UID _manifestFolder)
{
	EntityPropertiesProjectList* projectListProp = EntityPropertiesProjectList::createProperty("General", "Project", _initialProjectName, "", getProperties());
	m_propertyBundleEventHandling.setFolderIDManifests(_manifestFolder);
	m_propertyBundleEventHandling.setFolderIDScripts(_scriptFolder);
	m_propertyBundleEventHandling.setProperties(this);
	getProperties().forceResetUpdateForAllProperties();
}

ot::GenericDataStructMatrix ot::EntityDatasetInfo::getTable()
{
	OT_LOG_E("getTable is not supported for EntityDatasetInfo");
	return GenericDataStructMatrix();
}

void ot::EntityDatasetInfo::setTable(const ot::GenericDataStructMatrix& _table)
{
	OT_LOG_E("EntityDatasetInfo can not be set from config");
}

ot::TableCfg ot::EntityDatasetInfo::getTableConfig(bool _includeData)
{
	auto obs = getObserver();
	if (!obs)
	{
		OT_LOG_W("Can not get metadata campaign. No observer for EntityDatasetInfo { \"Entity\": \"" + getName() + "\" }");
		return TableCfg();
	}

	std::string collectionName;
	auto campaign = obs->getMetadataCampaign(getProjectName(), collectionName);
	if (!campaign)
	{
		OT_LOG_W("No metadata campaign for project \"" + getProjectName() + "\"");
		return TableCfg();
	}

	JSONToVariableConverter jsonVar;

	std::list<GenericDataStructMatrix> matrices;

	std::list<std::string> rowHeaders;

	for (const auto& seriesMetadata : campaign->getSeriesMetadata())
	{
		rowHeaders.push_back(seriesMetadata.getName());

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

	const GenericDataStructMatrix matrix = GenericDataStructMatrixMerger::tableMerge(matrices, getHeaderMode() == ot::TableCfg::TableHeaderMode::Horizontal);

	ot::TableCfg cfg(matrix, getHeaderMode());

	cfg.setEntityInformation(getBasicEntityInformation());
	cfg.setTitle(getBasicEntityInformation().getEntityName());
	cfg.setReadOnly(true);
	cfg.setColumnSortingEnabled(true);
	for (uint32_t i = 0; i < matrix.getNumberOfColumns(); i++)
	{
		cfg.setColumnHeaderFilterBehavior(i, ot::TableHeaderItemCfg::FilterBehavior::UseText);
	}

	rowHeaders = ot::EntityName::removeMatchingParentPaths(rowHeaders);

	auto rowHeaderIt = rowHeaders.begin();
	if (rowHeaders.size() == cfg.getRowCount())
	{
		for (int i = 0; i < cfg.getRowCount(); i++)
		{
			OTAssert(rowHeaderIt != rowHeaders.end(), "Iterator out of range");
			cfg.setRowHeader(i, *rowHeaderIt);
			++rowHeaderIt;
		}
	}
	else
	{
		OT_LOG_E("Row header size mismatch for { \"Entity\": \"" + getName() + "\", \"ExpectedRowHeaderSize\": " + std::to_string(cfg.getRowCount()) + ", \"ActualRowHeaderSize\": " + std::to_string(rowHeaders.size()) + " }");
	}

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
	setModified();
}

std::string ot::EntityDatasetInfo::getProjectName() const
{
	return PropertyHelper::getProjectPropertyValue(this, "Project", "General");
}

std::string ot::EntityDatasetInfo::getScriptName()
{
	std::string scriptName;
	return scriptName;
}

std::string ot::EntityDatasetInfo::getEnvironmentName()
{
	std::string environmentName;
	return environmentName;
}

std::optional<std::string> ot::EntityDatasetInfo::getEventHandlingFunction(PythonEventType _type, std::map<ot::UID, EntityBase*>& _entityMap)
{
	auto idAndName = m_propertyBundleEventHandling.getSelectedScript(this);
	
	EntityBase* base =	readEntityFromEntityID(nullptr, idAndName.first, _entityMap);
	if (base != nullptr)
	{
		EntityFileText* script = dynamic_cast<EntityFileText*>(base);
		if (script != nullptr)
		{
			const std::string scriptContent = script->getText();
			PythonHeaderEventBuilder eventBuilder;
			bool success = eventBuilder.interpret(scriptContent);
			if (success)
			{
				return eventBuilder.getFunctionName(_type);
			}
			else
			{
				const std::string report = eventBuilder.getReport();
				OT_USER_LOG_E("Failed to anaylse given python script for event handling. Errors: " + report);
			}
		}
	}
	
	return std::nullopt;
	
}
