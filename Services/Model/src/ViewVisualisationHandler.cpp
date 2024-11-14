#include <stdafx.h>
#include "ViewVisualisationHandler.h"
#include "Application.h"
#include "Model.h"
#include "IVisualisationText.h"
#include "IVisualisationTable.h"

void ViewVisualisationHandler::handleVisualisationRequest(ot::UID _entityID)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);
	
	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);
	
	IVisualisationText* textEntity = dynamic_cast<IVisualisationText*>(baseEntity);
	if (textEntity != nullptr && textEntity->visualiseText())
	{
		ot::JsonDocument document;
		info.addToJsonObject(document, document.GetAllocator());
		document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, document.GetAllocator());
		ot::TextEditorCfg configuration = textEntity->createConfig();
		ot::JsonObject cfgObj;
		configuration.addToJsonObject(cfgObj, document.GetAllocator());

		document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
		std::string response;
		Application::instance()->queuedRequestToFrontend(document);
	}
	
	IVisualisationTable* tableEntity = dynamic_cast<IVisualisationTable*>(baseEntity);
	if (tableEntity != nullptr && tableEntity->visualiseTable())
	{
		ot::JsonDocument document;
		info.addToJsonObject(document, document.GetAllocator());
		document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, document.GetAllocator());
		ot::TableCfg tableCfg =	tableEntity->getTableConfig();
		ot::JsonObject cfgObj;
		tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

		document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());
		std::string response;
		Application::instance()->queuedRequestToFrontend(document);
	}
}
