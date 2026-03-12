// @otlicense

// OpenTwin header
#include <stdafx.h>
#include "Model.h"
#include "Application.h"
#include "ContextActionHandler.h"
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/ContextMenuRequestEvent.h"
#include "OTGui/Event/NavigationContextRequestData.h"

using namespace ot;

class ContextActionHandler::PrivateData
{
public:
	PrivateData();
};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ContextActionHandler::ContextActionHandler() 
{
	m_data = new PrivateData;
}

ContextActionHandler::~ContextActionHandler()
{
	OTAssertNullptr(m_data);
	delete m_data;
}

MenuCfg ContextActionHandler::contextMenuRequested(const ContextMenuRequestEvent& _event)
{
	auto data = _event.getRequestData();
	if (!data) {
		return MenuCfg();
	}
	
	if (data->getClassName() == NavigationContextRequestData::className()) {
		return createNavigationContextMenu(static_cast<const NavigationContextRequestData*>(data));
	}
	else
	{
		OT_LOG_W("Unsupported context menu request type: \"" + data->getClassName() + "\"");
		return MenuCfg();
	}
}

MenuCfg ContextActionHandler::createNavigationContextMenu(const NavigationContextRequestData* _requestData) const
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

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ContextActionHandler::PrivateData::PrivateData()
{}

