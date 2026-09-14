#include "BlockHandlerRefinement.h"
#include "Application.h"
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
	std::list<ot::EntityInformation> entityInfos;
	std::list<std::string> names;
	for (const auto& entry : matchingEntitiesInfos)
	{
		ot::EntityInformation info;
		info.setFromJsonObject(entry.GetObject());
		names.push_back(info.getEntityName());
		entityInfos.push_back(info);
	}
	
	ot::JsonDocument temp;
	temp.AddMember("EntityNames", ot::JsonArray(names, temp.GetAllocator()),temp.GetAllocator());

	m_output.setData(std::move(temp["EntityNames"]));

	m_dataPerPort[m_outputConnector] = &m_output;
	return true;
}

std::string BlockHandlerRefinement::getBlockType() const
{
	return "Datalake-Zone Access Block";
}
