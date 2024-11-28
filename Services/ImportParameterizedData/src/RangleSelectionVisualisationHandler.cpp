#include "RangleSelectionVisualisationHandler.h"
#include "Application.h"
#include "TableIndexSchemata.h"
#include "SelectionCategorisationColours.h"
#include "CategorisationFolderNames.h"

#include <map>


void RangleSelectionVisualisationHandler::selectRange(ot::UIDList iDs, ot::UIDList versions)
{
	std::map<std::string, std::map<uint32_t, std::list<ot::TableRange>>> rangesByColourIDByTableNames;
	auto versionIt = versions.begin();
	for (auto idIt = iDs.begin(); idIt != iDs.end(); ++idIt)
	{
		auto modelComponent =	Application::instance()->modelComponent();
		auto baseEntity = modelComponent->readEntityFromEntityIDandVersion(*idIt, *versionIt, Application::instance()->getClassFactory());
		versionIt++;

		std::unique_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));

		//First we get the selected range

		ot::TableRange userRange = rangeEntity->getSelectedRange();
		ot::TableRange selectionRange = TableIndexSchemata::userRangeToSelectionRange(userRange);

		//Now we determine the colour for the range
		const std::string tableName = rangeEntity->getTableName();
		uint32_t colourID;
		std::string name = rangeEntity->getName();
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
			if (name.find(CategorisationFolderNames::getParameterFolderName()) != std::string::npos)
			{
				colourID = 2;
			}
			else
			{
				colourID = 3;
			}
		}
		else
		{
			assert(0);
		}

		//Now we store the range for its colour and table 
		rangesByColourIDByTableNames[tableName][colourID].push_back(selectionRange);
	}

	bool clearSelection = true;
	for (const auto& rangesByColourIDByTableName : rangesByColourIDByTableNames)
	{
		const std::string tableName = rangesByColourIDByTableName.first;
		auto& rangesByColourIDs = rangesByColourIDByTableName.second;
		for (const auto& rangesByColourID : rangesByColourIDs)
		{
			uint32_t colourID = rangesByColourID.first;
			ot::Color typeColour;
			if (colourID == 0)
			{
				typeColour = SelectionCategorisationColours::getRMDColour();
			}
			else if (colourID == 1)
			{
				typeColour = SelectionCategorisationColours::getSMDColour();
			}
			else if (colourID == 2)
			{
				typeColour = SelectionCategorisationColours::getParameterColour();
			}
			else
			{
				assert(colourID == 3);
				typeColour = SelectionCategorisationColours::getQuantityColour();
			}

			const auto& ranges = rangesByColourID.second;
			requestToOpenTable(tableName);
			requestColouringRanges(clearSelection, tableName, typeColour, ranges);
			clearSelection = false;
		}
	}

}

void RangleSelectionVisualisationHandler::requestToOpenTable(const std::string& _tableName)
{
	ot::EntityInformation entityInfo;
	Application::instance()->modelComponent()->getEntityInformation(_tableName, entityInfo);
	EntityBase* entityBase = Application::instance()->modelComponent()->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
	IVisualisationTable* table = dynamic_cast<IVisualisationTable*>(entityBase);
	assert(table != nullptr);

	ot::JsonDocument document;
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);
	info.addToJsonObject(document, document.GetAllocator());
	document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, document.GetAllocator());
	document.AddMember(OT_ACTION_PARAM_VIEW_SetActiveView, true, document.GetAllocator());
	document.AddMember(OT_ACTION_CMD_UI_TABLE_OverrideOfCurrentContent, false, document.GetAllocator());

	ot::TableCfg tableCfg = table->getTableConfig();;
	ot::JsonObject cfgObj;
	tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

	document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

	std::string answer;
	Application::instance()->uiComponent()->sendMessage(false, document, answer);
}

void RangleSelectionVisualisationHandler::requestColouringRanges(bool _clearSelection, const std::string& _tableName, const ot::Color& _colour, const std::list<ot::TableRange>& ranges)
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
