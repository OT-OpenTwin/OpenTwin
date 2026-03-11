// @otlicense

// OpenTwin header
#include <stdafx.h>
#include "Model.h"
#include "Application.h"
#include "ContextActionHandler.h"

ContextActionHandler::ContextActionHandler(Application* _application) 
	: m_app(_application)
{}

ot::MenuCfg ContextActionHandler::contextMenuRequested(const ot::ContextMenuRequestEvent& _event)
{
	auto data = _event.getRequestData();
	if (!data) {
		return ot::MenuCfg();
	}
	
	if (data->getClassName() == ot::NavigationContextRequestData::className()) {
		return createNavigationContextMenu(*static_cast<const ot::NavigationContextRequestData*>(data));
	}
	else
	{
		OT_LOG_W("Unsupported context menu request type: \"" + data->getClassName() + "\"");
		return ot::MenuCfg();
	}

}

ot::MenuCfg ContextActionHandler::createNavigationContextMenu(const ot::NavigationContextRequestData& _requestData) const
{
	ot::MenuCfg menu;

	auto model = m_app->getModel();
	if (!model) {
		OT_LOG_E("Model is not available.");
		return menu;
	}

	EntityBase* entity = model->getEntityByID(_requestData.getEntityID());
	if (!entity) {
		OT_LOG_E("Entity with ID " + std::to_string(_requestData.getEntityID()) + " is not available in the model.");
		return menu;
	}

	menu.addButton("NavigateTo", "Navigate to \"" + _requestData.getEntityName() + "\"");
	menu.addSeparator();
	menu.addButton("EntityID", "{ \"EntityID\": " + std::to_string(_requestData.getEntityID()) + " }");

	if (entity->getDeletable())
	{
		menu.addSeparator();
		menu.addButton("DeleteEntity", "Delete", "ContextMenu/Remove.png");
	}

	return menu;
}
