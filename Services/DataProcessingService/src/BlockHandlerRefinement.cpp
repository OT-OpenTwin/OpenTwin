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
	
	if (matchingEntitiesInfos.Size() > 0)
	{
		// Currently we handle only one per run (temporary)
		std::list<ot::EntityInformation> entityInfos;
		std::list<std::string> names;
		const auto& entry = matchingEntitiesInfos.begin();
		ot::EntityInformation info;
		info.setFromJsonObject(entry->GetObject());
		names.push_back(info.getEntityName());
		entityInfos.push_back(info);
		
		ot::TransactionEntryBuilder::INSTANCE().addFromEntity(ot::EntityIdentifier(info.getEntityID(), info.getEntityVersion()));
		ot::TransactionEntryBuilder::INSTANCE().setTransactionType(ot::TransactionType::Refinement);
		
		ot::JsonDocument description;
		description.AddMember("Operation", ot::JsonString("Raw -> Refined", description.GetAllocator()), description.GetAllocator());
		ot::TransactionEntryBuilder::INSTANCE().setTransactionDescription(description.toJson());
		ot::JsonDocument temp;
		temp.AddMember("EntityNames", ot::JsonArray(names, temp.GetAllocator()),temp.GetAllocator());

		m_output.setData(std::move(temp["EntityNames"]));

		m_dataPerPort[m_outputConnector] = &m_output;
		return true;
	}
	else
	{
		return false;
	}
	
}

std::string BlockHandlerRefinement::getBlockType() const
{
	return "Datalake-Zone Access Block";
}
