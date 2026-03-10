// @otlicense

// OpenTwin header
#include <stdafx.h>
#include "ContextActionHandler.h"

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

	menu.addButton("NavigateTo", "Navigate to \"" + _requestData.getNavigationItemPath() + "\"");
	menu.addSeparator();
	menu.addButton("EntityID", "{ \"EntityID\": " + std::to_string(_requestData.getEntityID()) + " }");
	menu.addButton("EntityName", "{ \"EntityName\": \"" + _requestData.getEntityName() + "\" }");

	return menu;
}
