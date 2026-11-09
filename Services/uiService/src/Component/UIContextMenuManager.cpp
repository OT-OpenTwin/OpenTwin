// @otlicense
// File: UIContextMenuManager.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "AppBase.h"
#include "ToolBar.h"
#include "NavigationTreeView.h"
#include "Component/UIContextMenuManager.h"
#include "Component/ExternalServicesComponent.h"
#include "OTGui/Event/NavigationMenuRequestData.h"
#include "OTGuiAPI/MenuHandler.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Menu/Menu.h"
#include "OTWidgets/Event/MenuRequestWidgetEvent.h"
#include "akWidgets/aTreeWidget.h"

UIContextMenuManager::UIContextMenuManager()
	: ot::MenuManager(this), m_app(nullptr)
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
	connect(tree, &ak::aTreeWidget::contextMenuRequest, this, &UIContextMenuManager::showMenu);
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

	ot::EntityTreeItem entityInfo = ViewerAPI::getEntityTreeItem(entityId);
	OTAssert(entityId == entityInfo.getEntityID(), "Received entity information with mismatching ID");
	return entityInfo;
}

ot::MenuCfg UIContextMenuManager::getMenuConfiguration(const ot::MenuCallbackBase* _callbackObject, const ot::MenuRequestEvent& _requestEvent) const
{
	ot::MenuCfg resultMenu;

	OTAssertNullptr(m_app);

	ot::JsonDocument requestDoc = ot::MenuHandler::createMenuRequestedDocument(_requestEvent);

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

bool UIContextMenuManager::notifyMenuOwner(const ot::MenuAction* _action) const
{
	return false;
}

bool UIContextMenuManager::triggerToolbarButton(const std::string& _buttonName) const
{
	OTAssertNullptr(m_app);
	ToolBar* toolBar = m_app->getToolBar();
	OTAssertNullptr(toolBar);
	return toolBar->triggerToolBarButton(QString::fromStdString(_buttonName));
}

bool UIContextMenuManager::checkToolBarButtonExists(const std::string& _buttonName) const
{
	OTAssertNullptr(m_app);
	ToolBar* toolBar = m_app->getToolBar();
	OTAssertNullptr(toolBar);
	return toolBar->checkToolBarButtonExists(QString::fromStdString(_buttonName));
}

bool UIContextMenuManager::isToolBarButtonEnabled(const std::string& _buttonName) const
{
	OTAssertNullptr(m_app);
	ToolBar* toolBar = m_app->getToolBar();
	OTAssertNullptr(toolBar);
	return toolBar->isToolBarButtonEnabled(QString::fromStdString(_buttonName));
}

