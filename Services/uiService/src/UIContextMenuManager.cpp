// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ToolBar.h"
#include "NavigationTreeView.h"
#include "UIContextMenuManager.h"
#include "ExternalServicesComponent.h"
#include "OTGui/Event/NavigationContextRequestData.h"
#include "OTGuiAPI/ContextMenuHandler.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/ContextMenu/ContextMenu.h"
#include "OTWidgets/Event/ContextRequestWidgetEvent.h"
#include "akWidgets/aTreeWidget.h"

UIContextMenuManager::UIContextMenuManager()
	: ot::ContextMenuManager(this), m_app(nullptr)
{
	
}

UIContextMenuManager::~UIContextMenuManager()
{
	m_app = nullptr;
}

void UIContextMenuManager::initialize(AppBase* _app)
{
	OTAssertNullptr(_app);

	if (m_app)
	{
		OT_LOG_E("Context menu manager already initialized");
		return;
	}

	m_app = _app;

	ot::NavigationTreeView* treeView = m_app->getProjectNavigation();
	OTAssertNullptr(treeView);

	ak::aTreeWidget* tree = treeView->getTree();
	OTAssertNullptr(tree);

	// Connect signals
	connect(tree, &ak::aTreeWidget::contextMenuRequest, this, &UIContextMenuManager::showContextMenu);
}

ot::BasicEntityInformation UIContextMenuManager::getEntityInformationFromName(const std::string& _entityName, bool* _failFlag) const
{
	OTAssertNullptr(m_app);
	ot::UID entityId = ot::invalidUID;

	entityId = ViewerAPI::getEntityID(_entityName);
	if (entityId == ot::invalidUID)
	{
		if (_failFlag)
		{
			*_failFlag = true;
		}
		return ot::BasicEntityInformation();
	}

	return ViewerAPI::getEntityTreeItem(entityId);
}

ot::MenuCfg UIContextMenuManager::getContextMenuConfiguration(const ot::ContextMenuCallbackBase* _callbackObject, const ot::ContextMenuRequestEvent& _requestEvent) const
{
	ot::MenuCfg resultMenu;

	OTAssertNullptr(m_app);

	ot::JsonDocument requestDoc = ot::ContextMenuHandler::createContextMenuRequestedDocument(_requestEvent);

	// Send request and handle response
	auto ext = m_app->getExternalServicesComponent();
	OTAssertNullptr(ext);

	std::list<std::string> responsesJson;
	ext->broadcastToViewNotifiers(requestDoc.toJson(), responsesJson);

	for (const std::string& responseJson : responsesJson)
	{
		ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseJson);
		if (!response.isOk() || response.getWhat().empty())
		{
			return resultMenu;
		}

		ot::JsonDocument doc;
		if (!doc.fromJson(response.getWhat()))
		{
			return resultMenu;
		}

		//! @todo here we need to merge the menu cfgs
		resultMenu.setFromJsonObject(doc.getConstObject());
	}

	return resultMenu;
}

bool UIContextMenuManager::notifyContextOwner(const ot::ContextMenuAction* _action) const
{
	return false;
}

bool UIContextMenuManager::triggerToolbarButton(const std::string& _buttonName) const
{
	OTAssertNullptr(m_app);
	ToolBar* toolBar = m_app->getToolBar();
	OTAssertNullptr(toolBar);
	QString btnName = QString::fromStdString(_buttonName);
	btnName.replace('/', ':'); // Replace slashes with colons to match the expected format
	return toolBar->triggerToolBarButton(btnName);
}

