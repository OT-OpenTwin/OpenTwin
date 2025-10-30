// @otlicense
// File: UIControlsHandler.cpp
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

#include "stdafx.h"

#include "UIControlsHandler.h"
#include "OTServiceFoundation/UiComponent.h"
#include "TemplateDefaultManager.h"
#include "OTGui/KeySequence.h"

UIControlsHandler::UIControlsHandler()
{
	if (Application::instance()->isUiConnected())
	{
		// Load the template defaults if any
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults("UI Configuration");
		createPages();
		createGroups();
		setupButtons();
		createButtons();
	}
}

void UIControlsHandler::createPages()
{
	ot::components::UiComponent* uiComponent = Application::instance()->getUiComponent();
	assert(uiComponent != nullptr);
	uiComponent->addMenuPage(m_pageModel);
	uiComponent->addMenuPage("View");
}

void UIControlsHandler::createGroups()
{
	ot::components::UiComponent* uiComponent = Application::instance()->getUiComponent();
	assert(uiComponent != nullptr);

	uiComponent->addMenuGroup(m_pageModel, m_groupEdit);
	uiComponent->addMenuGroup(m_pageModel, m_groupGeometry);
	uiComponent->addMenuGroup(m_pageModel, m_groupImport);
	
	uiComponent->addMenuGroup(m_pageModel, m_groupParameters);
}

void UIControlsHandler::setupButtons()
{
	m_buttonGeometryInfo = ot::ToolBarButtonCfg(m_pageModel, m_groupGeometry, "Info", "Default/Information");
	m_buttonGeometryInfo.setButtonLockFlags(ot::LockType::ModelRead);
	m_buttonGeometryInfo.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Control, ot::BasicKey::I));
	
	m_buttonCreateParameter = ot::ToolBarButtonCfg(m_pageModel, m_groupParameters, "Create Parameter", "Default/CreateParameter");
	m_buttonCreateParameter.setButtonLockFlags(ot::LockType::ModelRead);
	
	m_buttonDelete = ot::ToolBarButtonCfg(m_pageModel, m_groupEdit, "Delete", "Default/Delete");
	m_buttonDelete.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonDelete.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Delete));

	m_buttonRedo = ot::ToolBarButtonCfg(m_pageModel, m_groupEdit, "Redo", "Default/Redo");
	m_buttonRedo.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonRedo.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Control, ot::BasicKey::Y));

	m_buttonUndo = ot::ToolBarButtonCfg(m_pageModel, m_groupEdit, "Undo", "Default/Undo");
	m_buttonUndo.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonUndo.setButtonKeySequence(ot::KeySequence(ot::BasicKey::Control, ot::BasicKey::Z));
}

void UIControlsHandler::createButtons()
{
	ot::components::UiComponent* uiComponent = Application::instance()->getUiComponent();
	assert(uiComponent != nullptr);
	
	uiComponent->addMenuButton(m_buttonGeometryInfo);

	uiComponent->addMenuButton(m_buttonCreateParameter);

	uiComponent->addMenuButton(m_buttonUndo);
	uiComponent->addMenuButton(m_buttonRedo);
	uiComponent->addMenuButton(m_buttonDelete);
}
