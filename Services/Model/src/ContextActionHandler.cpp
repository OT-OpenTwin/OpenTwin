// @otlicense

// OpenTwin header
#include <stdafx.h>
#include "Model.h"
#include "Application.h"
#include "ContextActionHandler.h"
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/MenuRequestEvent.h"
#include "OTGui/Event/GraphicsMenuRequestData.h"
#include "OTGui/Event/NavigationMenuRequestData.h"
#include "OTGui/Event/PlotMenuRequestData.h"
#include "OTGui/Event/TableMenuRequestData.h"
#include "OTGui/Event/TextEditorMenuRequestData.h"
#include "OTGui/Event/View3DMenuRequestData.h"

using namespace ot;

MenuCfg ContextActionHandler::menuRequested(const MenuRequestEvent& _event)
{
	auto data = _event.getRequestData();
	if (!data) {
		return MenuCfg();
	}
	
	if (data->getClassName() == NavigationMenuRequestData::className()) {
		return createContextMenu(static_cast<const NavigationMenuRequestData*>(data));
	}
	else if (data->getClassName() == GraphicsMenuRequestData::className()) {
		return createContextMenu(static_cast<const GraphicsMenuRequestData*>(data));
	}
	else if (data->getClassName() == PlotMenuRequestData::className()) {
		return createContextMenu(static_cast<const PlotMenuRequestData*>(data));
	}
	else if (data->getClassName() == TableMenuRequestData::className()) {
		return createContextMenu(static_cast<const TableMenuRequestData*>(data));
	}
	else if (data->getClassName() == TextEditorMenuRequestData::className()) {
		return createContextMenu(static_cast<const TextEditorMenuRequestData*>(data));
	}
	else if (data->getClassName() == View3DMenuRequestData::className()) {
		return createContextMenu(static_cast<const View3DMenuRequestData*>(data));
	}
	else
	{
		OT_LOG_W("Unsupported context menu request type: \"" + data->getClassName() + "\"");
		return MenuCfg();
	}
}

MenuCfg ContextActionHandler::createContextMenu(const NavigationMenuRequestData* _requestData) const
{
	MenuCfg menu;

	// Get model
	Model* model = Application::instance()->getModel();
	if (!model)
	{
		OT_LOG_EA("Model is not available.");
		return menu;
	}

	// Get entity
	EntityBase* entity = model->getEntityByID(_requestData->getEntityID());
	if (!entity)
	{
		OT_LOG_E("Entity not found { \"ID\": " + std::to_string(_requestData->getEntityID()) + " }");
		return menu;
	}

	// Fill menu
	entity->fillContextMenu(_requestData, menu);

	return menu;
}

MenuCfg ContextActionHandler::createContextMenu(const GraphicsMenuRequestData* _requestData) const
{
	return MenuCfg();
}

MenuCfg ContextActionHandler::createContextMenu(const PlotMenuRequestData* _requestData) const
{
	return MenuCfg();
}

MenuCfg ContextActionHandler::createContextMenu(const TableMenuRequestData* _requestData) const
{
	return MenuCfg();
}

MenuCfg ContextActionHandler::createContextMenu(const TextEditorMenuRequestData* _requestData) const
{
	return MenuCfg();
}

MenuCfg ContextActionHandler::createContextMenu(const View3DMenuRequestData* _requestData) const
{
	return MenuCfg();
}
