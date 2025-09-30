// Project header
#include "Application.h"
#include "CategorisationFolderNames.h"
#include "SelectionCategorisationColours.h"
#include "RangeSelectionVisualisationHandler.h"

// OpenTwin header
#include "OTCore/EntityName.h"
#include "OTCore/RuntimeTests.h"
#include "OTGui/TableIndexSchemata.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"

// std header
#include <map>

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_RANGESELECTIONVISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_RANGESELECTIONVISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_RANGESELECTIONVISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_RANGESELECTIONVISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED==true
#define OT_TEST_RANGESELECTIONVISUALIZATIONHANDLER_Interval(___testText) OT_TEST_Interval(ot_intern_rangeselectionvisualizationhandler_lcl_performancetest, "RangeSelectionVisualization", ___testText)
#else
#define OT_TEST_RANGESELECTIONVISUALIZATIONHANDLER_Interval(___testText)
#endif

void RangeSelectionVisualisationHandler::selectRange(const ot::UIDList& _selectedEntityIDs)
{
	OT_TEST_RANGESELECTIONVISUALIZATIONHANDLER_Interval("Select range: Total");

	std::list<ot::EntityInformation> selectedEntitiesInfo;
	ot::ModelServiceAPI::getSelectedEntityInformation(selectedEntitiesInfo);

	std::list<std::shared_ptr<EntityTableSelectedRanges>> selectionEntities = extractSelectionRanges(_selectedEntityIDs);
	std::list<std::shared_ptr<EntityTableSelectedRanges>> selectionsThatNeedVisualisation = findSelectionsThatNeedVisualisation(selectionEntities);
	
	this->bufferSelectionEntities(selectionEntities);

	std::map<std::string, std::map<uint32_t, std::list<ot::TableRange>>> rangesByColourIDByTableNames;
	
	// Determine which selected entities are visualizing the tables
	std::map<std::string, ot::UIDList> visualizingEntitiesByTableName;
	for (const auto& entity : selectionEntities) {
		const std::string tableName = entity->getTableName();
		ot::UIDList& visualizingEntities = visualizingEntitiesByTableName[tableName];
		visualizingEntities.push_back(entity->getEntityID());

		for (auto& it : selectedEntitiesInfo) {
			if (it.getEntityName() != entity->getName()) {
				if (it.getEntityName() == tableName) {
					// Add table as visualizing entity if selected
					if (std::find(visualizingEntities.begin(), visualizingEntities.end(), it.getEntityID()) == visualizingEntities.end()) {
						visualizingEntities.push_back(it.getEntityID());
					}
				}
			}
		}
	}

	if (!selectionsThatNeedVisualisation.empty())
	{
		{
			OT_TEST_RANGESELECTIONVISUALIZATIONHANDLER_Interval("Select range: Gather ranges");

			for (auto& selectionEntity : selectionsThatNeedVisualisation) {
				//First we get the selected range
				ot::TableRange userRange = selectionEntity->getSelectedRange();
				ot::TableRange selectionRange = ot::TableIndexSchemata::userRangeToSelectionRange(userRange);

				//Now we determine the colour for the range
				const std::string tableName = selectionEntity->getTableName();
				uint32_t colourID;
				std::string name = selectionEntity->getName();
				std::string::difference_type n = std::count(name.begin(), name.end(), '/');
				if (n == 2) //First topology level: RMD
				{
					colourID = 0;
				}
				else if (n == 3) //Second topology level: MSMD files
				{
					colourID = 1;
				}
				else if (n == 4) //Third topology level: Parameter and Quantities
				{
					if (name.find(CategorisationFolderNames::getParameterFolderName()) != std::string::npos) {
						colourID = 2;
					}
					else {
						colourID = 3;
					}
				}
				else {
					assert(0);
				}

				//Now we store the range for its colour and table 
				rangesByColourIDByTableNames[tableName][colourID].push_back(selectionRange);
			}
		}

		const bool clearSelection = true;

		{
			OT_TEST_RANGESELECTIONVISUALIZATIONHANDLER_Interval("Select range: Request coloring");

			for (const auto& rangesByColourIDByTableName : rangesByColourIDByTableNames) {
				const std::string tableName = rangesByColourIDByTableName.first;
				auto& rangesByColourIDs = rangesByColourIDByTableName.second;
				bool tableExisting = requestToOpenTable(tableName, visualizingEntitiesByTableName[tableName]);
				if (!tableExisting) {
					OT_LOG_E("Selection has a not existing table listed.");
				}
				else {
					for (const auto& rangesByColourID : rangesByColourIDs) {
						uint32_t colourID = rangesByColourID.first;
						ot::Color typeColour;
						if (colourID == 0) {
							typeColour = SelectionCategorisationColours::getRMDColour();
						}
						else if (colourID == 1) {
							typeColour = SelectionCategorisationColours::getSMDColour();
						}
						else if (colourID == 2) {
							typeColour = SelectionCategorisationColours::getParameterColour();
						}
						else {
							assert(colourID == 3);
							typeColour = SelectionCategorisationColours::getQuantityColour();
						}

						const auto& ranges = rangesByColourID.second;
						requestColouringRanges(clearSelection, tableName, typeColour, ranges);
					}
				}
			}
		}
	}
}

void RangeSelectionVisualisationHandler::bufferSelectionEntities(const std::list<std::shared_ptr<EntityTableSelectedRanges>>& _selectedEntities)
{
	m_bufferedSelectionRanges.clear();
	for (auto& selectionEntity : _selectedEntities)
	{
		m_bufferedSelectionRanges.push_back(selectionEntity->getEntityID());
	}
}

std::list<std::shared_ptr<EntityTableSelectedRanges>> RangeSelectionVisualisationHandler::extractSelectionRanges(const ot::UIDList& _selectedEntityIDs)
{
	auto modelComponent = Application::instance()->modelComponent();
	if (modelComponent == nullptr)
	{
		assert(0);
		throw std::exception("Model is not connected");
	}
	std::list<ot::EntityInformation> selectedEntityInfo;
	ot::ModelServiceAPI::getEntityInformation(_selectedEntityIDs, selectedEntityInfo);

	//First we do a preselection on the base of the name. 
	ot::UIDList relevantIDs, relevantVersions;
	for (auto& entityInfo : selectedEntityInfo)
	{
		std::string name = entityInfo.getEntityName();
		if (name.find(CategorisationFolderNames::getRootFolderName()) != std::string::npos)
		{
			relevantIDs.push_back(entityInfo.getEntityID());
			relevantVersions.push_back(entityInfo.getEntityVersion());
		}
	}
	Application::instance()->prefetchDocumentsFromStorage(relevantIDs);
	auto version = relevantVersions.begin();

	std::list<std::shared_ptr<EntityTableSelectedRanges>> selectionEntities;
	for (const ot::UID& uid : relevantIDs)
	{
		EntityBase* entBase = ot::EntityAPI::readEntityFromEntityIDandVersion(uid, *version, Application::instance()->getClassFactory());
		EntityTableSelectedRanges* selectionRange = dynamic_cast<EntityTableSelectedRanges*>(entBase);
		if (selectionRange != nullptr)
		{
			selectionEntities.push_back(std::shared_ptr<EntityTableSelectedRanges>(selectionRange));
		}
		else
		{
			delete entBase;
			entBase = nullptr;
		}
		version++;
	}
	return selectionEntities;
}

const std::list<std::shared_ptr<EntityTableSelectedRanges>> RangeSelectionVisualisationHandler::findSelectionsThatNeedVisualisation(const std::list<std::shared_ptr<EntityTableSelectedRanges>>& _selectionEntities)
{
	std::list<std::shared_ptr<EntityTableSelectedRanges>> selectionThatNeedVisualisation;
	for (auto& selectedRange : _selectionEntities)
	{
		const ot::UID entityID = selectedRange->getEntityID();
		auto entry = std::find(m_bufferedSelectionRanges.begin(), m_bufferedSelectionRanges.end(), entityID);
		if (entry == m_bufferedSelectionRanges.end())
		{
			selectionThatNeedVisualisation.push_back(selectedRange);
		}
	}
	return selectionThatNeedVisualisation;
}

bool RangeSelectionVisualisationHandler::requestToOpenTable(const std::string& _tableName, const ot::UIDList& _visualizingEntities)
{
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(_tableName, entityInfo);
	EntityBase* entityBase = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
	IVisualisationTable* table = dynamic_cast<IVisualisationTable*>(entityBase);
	if (table == nullptr)
	{
		return false;
	}

	ot::JsonDocument document;
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);
	info.addToJsonObject(document, document.GetAllocator()); 
	document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, document.GetAllocator());
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setAsActiveView(true);
	visualisationCfg.setOverrideViewerContent(false);
	visualisationCfg.setVisualisingEntities(_visualizingEntities);
	ot::JsonObject visualisationCfgJSon;
	visualisationCfg.addToJsonObject(visualisationCfgJSon, document.GetAllocator());
	document.AddMember(visualisationCfgJSon, OT_ACTION_PARAM_Visualisation_Config, document.GetAllocator());

	document.AddMember(OT_ACTION_PARAM_KeepCurrentEntitySelection, true, document.GetAllocator());
	
	const bool loadContent = true;
	ot::TableCfg tableCfg = table->getTableConfig(true);;
	ot::JsonObject cfgObj;
	tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

	document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

	std::string answer;
	Application::instance()->uiComponent()->sendMessage(false, document, answer);
	return true;
}

void RangeSelectionVisualisationHandler::requestColouringRanges(bool _clearSelection, const std::string& _tableName, const ot::Color& _colour, const std::list<ot::TableRange>& ranges)
{
	ot::JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TABLE_SetCurrentSelectionBackground, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(_tableName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(Application::instance()->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_ClearSelection, _clearSelection, doc.GetAllocator());

	ot::JsonObject obj;
	_colour.addToJsonObject(obj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Color, obj, doc.GetAllocator());

	ot::JsonArray vectOfRanges;
	for (auto range : ranges)
	{
		ot::JsonObject temp;
		range.addToJsonObject(temp, doc.GetAllocator());
		vectOfRanges.PushBack(temp, doc.GetAllocator());
	}
	doc.AddMember(OT_ACTION_PARAM_Ranges, vectOfRanges, doc.GetAllocator());

	auto uiComponent =	Application::instance()->uiComponent();
	std::string answer;
	uiComponent->sendMessage(true, doc, answer);
}
