// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ExternalServicesComponent.h"
#include "ContextMenuManager.h"
#include "OTGui/Event/NavigationContextRequestData.h"
#include "OTGuiAPI/ContextMenuHandler.h"

ContextMenuManager::ContextMenuManager(AppBase* _app)
	: m_app(_app)
{
}

ContextMenuManager::~ContextMenuManager()
{
	m_app = nullptr;
}

bool ContextMenuManager::navigationItemContextRequest(const ot::BasicEntityInformation& _hoveredEntity, ot::MenuCfg& _resultMenu)
{
	// Prepare request data
	if (_hoveredEntity.getEntityName().empty())
	{
		return false;
	}

	std::unique_ptr<ot::NavigationContextRequestData> requestData(new ot::NavigationContextRequestData(_hoveredEntity));

	ot::JsonDocument requestDoc = ot::ContextMenuHandler::createContextMenuRequestedDocument(requestData.release());

	// Send request and handle response
	auto ext = m_app->getExternalServicesComponent();

	std::list<std::string> responsesJson;
	ext->broadcastToViewNotifiers(requestDoc.toJson(), responsesJson);

	for (const std::string& responseJson : responsesJson)
	{
		ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseJson);
		if (!response.isOk() || response.getWhat().empty())
		{
			return false;
		}

		ot::JsonDocument doc;
		if (!doc.fromJson(response.getWhat()))
		{
			return false;
		}

		//! @todo here we need to merge the menu cfgs
		_resultMenu.setFromJsonObject(doc.getConstObject());
	}

	return true;
}
