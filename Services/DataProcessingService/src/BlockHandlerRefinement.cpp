#include "BlockHandlerRefinement.h"
#include "Application.h"
#include "OTServiceFoundation/TransactionData/TransactionEntryBuilder.h"

BlockHandlerRefinement::BlockHandlerRefinement(EntityBlockRefinement* _zoneBlockEntity, const HandlerMap& _handlerMap)
	:BlockHandler(_zoneBlockEntity, _handlerMap)
{
	m_selectedZone = _zoneBlockEntity->getSelectedZone();
	m_outputConnector = _zoneBlockEntity->getOutputConnectorName();
}

bool BlockHandlerRefinement::executeSpecialized()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_GetEntitiesOfZone,doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Text, ot::JsonString(m_selectedZone, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SETTINGS_ValueMax, true, doc.GetAllocator());
	std::string response;
	Application::instance()->getModelComponent()->sendMessage(false, doc, response);

	ot::JsonDocument responsDoc; 
	responsDoc.fromJson(response);
	auto matchingEntitiesInfos = ot::json::getArray(responsDoc, OT_ACTION_PARAM_MODEL_EntityInfo);
	// These entities have been tagged with a maximum of the selected zone
	
	std::list<ot::EntityInformation> entityInfos;
	for (int i = 0; i < matchingEntitiesInfos.Size(); i++)
	{
		auto entry = ot::json::getObject(matchingEntitiesInfos, i);
		ot::EntityInformation info;
		info.setFromJsonObject(entry);
		entityInfos.push_back(info);
	}

	// Now we check if they need refinement or if there is already a refinement connection comming from them
	ot::TransactionCollectionAccess access(Application::instance()->getCollectionName());
	std::list<ot::EntityInformation> entitiesForRefinement;
	for (const ot::EntityInformation& info : entityInfos)
	{
		std::list<ot::TransactionEntry> transactions =	access.searchEntry(ot::TransactionType::Refinement, ot::EntityIdentifier(info.getEntityID(), info.getEntityVersion()), ot::TransactionCollectionAccess::EdgeDirection::FromStartVertex);
		if (transactions.size() == 0)
		{
			entitiesForRefinement.push_back(info);
		}
	}

	if (entitiesForRefinement.size() > 0)
	{
		// Currently we handle only one per run so that the transactions describe a single refinement. 

		const auto& firstEntity= entitiesForRefinement.begin();
		
		
		
		
		ot::TransactionEntryBuilder::INSTANCE().addFromEntity(ot::EntityIdentifier(firstEntity->getEntityID(), firstEntity->getEntityVersion()));
		ot::TransactionEntryBuilder::INSTANCE().setTransactionType(ot::TransactionType::Refinement);
		
		ot::JsonDocument description;
		description.AddMember("Operation", ot::JsonString("Raw -> Refined", description.GetAllocator()), description.GetAllocator());
		ot::TransactionEntryBuilder::INSTANCE().setTransactionDescription(description.toJson());
		ot::JsonDocument temp;
		std::list<std::string> names = { firstEntity->getEntityName() };
		temp.AddMember("EntityNames", ot::JsonArray(names, temp.GetAllocator()), temp.GetAllocator());

		m_output.setData(std::move(temp["EntityNames"]));

		m_dataPerPort[m_outputConnector] = &m_output;
		return true;
	}
	else
	{
		OT_USER_LOG_I("No data found of the selected zone that need refinement");
		return false;
	}
	
}

std::string BlockHandlerRefinement::getBlockType() const
{
	return "Datalake-Zone Access Block";
}
