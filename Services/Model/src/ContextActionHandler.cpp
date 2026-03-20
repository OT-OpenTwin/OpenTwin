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
	const MenuRequestData* data = _event.getRequestData();
	if (!data) {
		return MenuCfg();
	}
	
	MenuCfg menu;

	// Get model
	Model* model = Application::instance()->getModel();
	if (!model)
	{
		OT_LOG_EA("Model is not available.");
		return menu;
	}

	// Get entity
	EntityBase* entity = model->getEntityByID(data->getEntityID());
	if (!entity)
	{
		OT_LOG_E("Entity not found { \"ID\": " + std::to_string(data->getEntityID()) + " }");
		return menu;
	}

	// Fill menu
	entity->fillContextMenu(data, menu);

	return menu;
}
