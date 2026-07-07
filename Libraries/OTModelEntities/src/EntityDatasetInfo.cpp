// @otlicense

// OpenTwin header
#include "OTCore/EntityName.h"
#include "OTCore/JSON/JSONVectoriser.h"
#include "OTCore/DataStruct/GenericDataStructMatrixMerger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
#include "OTModelEntities/EntityDatasetInfo.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTCore/Python/PythonHeaderAnalyser.h"
#include "OTCore/Python/PythonHeaderEventBuilder.h"
#include "OTCore/FolderNames.h"
#include "OTModelEntities/Lms/EntityPythonScript.h"
#include "OTModelEntities/EntityPythonManifest.h"
#include "OTCore/ReturnMessage.h"

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

	// Check if LoadFromLibrary was selected
	auto basePropertyModel = getProperties().getProperty("Script");
	auto modelProperty = dynamic_cast<EntityPropertiesExtendedEntityList*>(basePropertyModel);
	if (modelProperty == nullptr)
	{
		OT_LOG_E("Model selection property cast failed");
		return false;
	}

	auto basePropertyManifest = getProperties().getProperty("Environment");
	auto manifestProperty = dynamic_cast<EntityPropertiesExtendedEntityList*>(basePropertyManifest);
	if (manifestProperty == nullptr)
	{
		OT_LOG_E("Manifest selection property cast failed");
		return false;
	}

	// #########################################################

	// Legacy support (before introcuding value action handling)

	if (modelProperty->getValueName() == "< Load from Library >" && modelProperty->getCurrentValueHandlingType() == ot::PropertyBase::ValueHandlingType::Value)
	{
		requestScriptLoadFromLibrary();
	}
	else if (manifestProperty->getValueName() == "< Load from Library >" && manifestProperty->getCurrentValueHandlingType() == ot::PropertyBase::ValueHandlingType::Value)
	{
		requestManifestLoadFromLibrary();
	}

	// Legacy support (before introcuding value action handling)
	
	// #########################################################

	bool dataUpdate = PropertyHelper::getEntityProjectListProperty(this, "Project", "General")->needsUpdate();
	if (dataUpdate)
	{
		ot::VisualisationCfg visualisationCfg;
		visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TABLE_Setup);
		visualisationCfg.setOverrideViewerContent(dataUpdate);
		visualisationCfg.setAsActiveView(true);
		getObserver()->requestVisualisation(getEntityID(), visualisationCfg);

	}

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
		rowHeaders.push_back(seriesMetadata.getLabel());

		const JsonDocument& metadata = seriesMetadata.getMetadata();
		std::string dooc = json::toJson(metadata);
		std::list<std::string> columnHeader;
		JSONVectoriser::vectorise(metadata, columnHeader, "", JSONVectoriser::AddLeavesOnly);

		GenericDataStructMatrix matrix(2, static_cast<uint32_t>(columnHeader.size()));
		uint32_t columnIndex = 0;
		for (const std::string& col : columnHeader)
		{
			matrix.setValue(MatrixEntryPointer(0, columnIndex), ot::Variable(col));
			auto val = JSONVectoriser::getValue(metadata, col);
			if (val.has_value())
			{
				const std::string dbg = json::toJson(val.value());
				matrix.setValue(MatrixEntryPointer(1, columnIndex), jsonVar(val.value()));
			}
			else
			{
				matrix.setValue(MatrixEntryPointer(1, columnIndex), "");

			}
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

	for (const ValueComparisonDescription& filter : m_activeFilters)
	{
		auto headerItem = cfg.getColumnHeaderByText(filter.getName());
		if (headerItem)
		{
			headerItem->addActiveFilter(filter);
		}
		else
		{
			OT_LOG_W("Could not find column header for active filter \"" + filter.getName() + "\" in EntityDatasetInfo { \"Entity\": \"" + getName() + "\" }");
		}
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

void ot::EntityDatasetInfo::setActiveFilters(const std::list<ValueComparisonDescription>& _filters)
{
	if (m_activeFilters != _filters)
	{
		m_activeFilters = _filters;
		setModified();
	}
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
	std::string scriptName = m_propertyBundleEventHandling.getSelectedScript(this).second;
	return scriptName;
}

std::string ot::EntityDatasetInfo::getEnvironmentName()
{
	std::string environmentName = m_propertyBundleEventHandling.getSelectedManifest(this).second;
	return environmentName;
}

std::optional<std::string> ot::EntityDatasetInfo::getEventHandlingFunction(PythonEventType _type, std::map<ot::UID, EntityBase*>& _entityMap)
{
	auto idAndName = m_propertyBundleEventHandling.getSelectedScript(this);

	EntityBase* base = readEntityFromEntityID(nullptr, idAndName.first, _entityMap);
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

std::list<ot::LibraryElement> ot::EntityDatasetInfo::libraryElementWasSet(const ot::LibraryElement& _libraryElement, EntityBase* _entity, ot::NewModelStateInfo& _newStateInfo)
{

	std::list<ot::LibraryElement> resultList;

	std::string dependencyID = _libraryElement.getAdditionalInfoValue("DependencyID");
	std::string dependencyCollection = _libraryElement.getAdditionalInfoValue("DependencyCollection");
	if (!dependencyID.empty() && dependencyID != std::to_string(ot::invalidUID))
	{
		// Create the LibraryElementRequest configuration
		ot::LibraryElementRequest request;
		request.setRequestingEntityID(_libraryElement.getRequestingEntityID());
		request.setCollectionName(dependencyCollection);
		request.setCallBackAction(OT_ACTION_CMD_LMS_LibraryElementRequest);
		request.setEntityType(EntityPythonManifest::className());
		request.setNewEntityFolder(ot::FolderNames::PythonManifestFolder);
		request.setPropertyName("Environment");
		request.setCallBackService(_libraryElement.getCallBackService());
		request.setValue(dependencyID);

		// Send the document to the observer (Model Service)
		std::string answer = getObserver()->requestLibraryElement(request);

		// Process the answer
		ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(answer);
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(rMsg.getWhat());

		ot::LibraryElement returnedElement;
		returnedElement.setFromJsonObject(ot::json::getObject(responseDoc, OT_ACTION_PARAM_Config));

		resultList.push_back(returnedElement);


		return resultList;
	}
	return resultList;
}

void ot::EntityDatasetInfo::nonValuePropertyValueSelected(const EntityPropertiesBase* _property)
{
	if (_property->getName() == "Script")
	{
		const EntityPropertiesExtendedEntityList* actualProperty = dynamic_cast<const EntityPropertiesExtendedEntityList*>(_property);
		if (!actualProperty)
		{
			OT_LOG_E("Property cast failed for property \"Script\" in EntityDatasetInfo { \"Entity\": \"" + getName() + "\" }");
			return;
		}

		const std::string selectedValue = actualProperty->getValueName();
		if (selectedValue == "< Load from Library >")
		{
			requestScriptLoadFromLibrary();
		}
		else
		{
			OT_LOG_E("Unexpected value selected for property \"Script\" in EntityDatasetInfo { \"Entity\": \"" + getName() + "\", \"SelectedValue\": \"" + selectedValue + "\" }");
		}
	}
	else if (_property->getName() == "Environment")
	{
		const EntityPropertiesExtendedEntityList* actualProperty = dynamic_cast<const EntityPropertiesExtendedEntityList*>(_property);
		if (!actualProperty)
		{
			OT_LOG_E("Property cast failed for property \"Environment\" in EntityDatasetInfo { \"Entity\": \"" + getName() + "\" }");
			return;
		}
		const std::string selectedValue = actualProperty->getValueName();
		if (selectedValue == "< Load from Library >")
		{
			requestManifestLoadFromLibrary();
		}
		else
		{
			OT_LOG_E("Unexpected value selected for property \"Environment\" in EntityDatasetInfo { \"Entity\": \"" + getName() + "\", \"SelectedValue\": \"" + selectedValue + "\" }");
		}
	}
	else
	{
		OT_LOG_E("Unexpected non value property change for property \"" + _property->getName() + "\" in EntityDatasetInfo { \"Entity\": \"" + getName() + "\" }");
	}
}

void ot::EntityDatasetInfo::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBase::addStorageData(_storage);

	JsonDocument filtersArr(rapidjson::kArrayType);

	for (const auto& filter : m_activeFilters)
	{
		JsonObject filterObj;
		filter.addToJsonObject(filterObj, filtersArr.GetAllocator());
		filtersArr.PushBack(filterObj, filtersArr.GetAllocator());
	}

	_storage.append(bsoncxx::builder::basic::kvp("ActiveFilters", filtersArr.toJson()));
}

void ot::EntityDatasetInfo::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBase::readSpecificDataFromDataBase(_docView, _entityMap);

	m_activeFilters.clear();
	auto docIt = _docView.find("ActiveFilters");
	if (docIt != _docView.end())
	{
		std::string filterJsonStr(docIt->get_string().value);

		if (!filterJsonStr.empty())
		{
			JsonDocument filtersArr;
			if (!filtersArr.fromJson(std::string(docIt->get_string().value)))
			{
				OT_LOG_E("Failed to parse active filters for EntityDatasetInfo { \"Entity\": \"" + getName() + "\" }");
			}
			else
			{
				for (JsonSizeType i = 0; i < filtersArr.Size(); i++)
				{
					const ConstJsonObject filterObj = json::getObject(filtersArr, i);
					ValueComparisonDescription filter;
					filter.setFromJsonObject(filterObj);
					m_activeFilters.push_back(std::move(filter));
				}
			}
		}
	}
}

void ot::EntityDatasetInfo::requestScriptLoadFromLibrary()
{
	ot::LibraryElementSelectionCfg config;
	config.setRequestingEntityID(this->getEntityID());
	config.setCollectionName("PythonScripts");
	config.setCallBackAction(OT_ACTION_CMD_LMS_CreateConfig);
	config.setEntityType(EntityPythonScript::className());
	config.setNewEntityFolder(ot::FolderNames::PythonScriptFolder);
	config.setPropertyName("Script");

	// if it was selected use observer to send message to LMS
	getObserver()->requestConfigForModelDialog(config);
}

void ot::EntityDatasetInfo::requestManifestLoadFromLibrary()
{
	ot::LibraryElementSelectionCfg config;
	config.setRequestingEntityID(this->getEntityID());
	config.setCollectionName("PythonEnvironments");
	config.setCallBackAction(OT_ACTION_CMD_LMS_CreateConfig);
	config.setEntityType(EntityPythonManifest::className());
	config.setNewEntityFolder(ot::FolderNames::PythonManifestFolder);
	config.setPropertyName("Environment");
	// if it was selected use observer to send message to LMS
	getObserver()->requestConfigForModelDialog(config);
}
