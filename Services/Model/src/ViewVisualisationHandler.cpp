#include <stdafx.h>
#include "ViewVisualisationHandler.h"
#include "Application.h"
#include "Model.h"
#include "IVisualisationText.h"

void ViewVisualisationHandler::handleVisualisationRequest(ot::UID _entityID)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);
	IVisualisationText* textEntity = dynamic_cast<IVisualisationText*>(baseEntity);
	assert(textEntity != nullptr);

	ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL);

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
