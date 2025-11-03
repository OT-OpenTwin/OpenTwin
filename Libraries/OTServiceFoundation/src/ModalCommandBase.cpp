// @otlicense
// File: ModalCommandBase.cpp
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

#include "OTServiceFoundation/ModalCommandBase.h"
#include "OTServiceFoundation/ApplicationBase.h"

ot::ModalCommandBase::ModalCommandBase(ot::ApplicationBase *app, const std::string &actionMnuID, const std::string &actionBtnID) 
	: application(app),
	  actionMenuID(actionMnuID),
	  actionButtonID(actionBtnID)
{
	application->addModalCommand(this);
}

ot::ModalCommandBase::~ModalCommandBase()
{
	application->removeModalCommand(this);
	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), true);

	// Remove all UI elements from the modal command tab
	std::list<std::string> objectNameList;
	for (const auto& item : uiActionMap) objectNameList.push_back(item.first);
	for (const auto& item : uiSubGroupMap) objectNameList.push_back(item.first);
	for (const auto& item : uiGroupMap) objectNameList.push_back(item.first);
	for (const auto& item : uiMenuMap) objectNameList.push_back(item.first);
	application->getUiComponent()->removeUIElements(objectNameList);

	// Enable the action button
	application->getUiComponent()->setControlState(actionButtonID, true);

	// Update the menu states and send everything
	application->modelSelectionChanged();
	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), false);

	// Activate the previous tab from which the command was originally started
	application->getUiComponent()->activateMenuPage(actionMenuID);
}

void ot::ModalCommandBase::setupUI(void)
{
	// Add the specific commands
	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), true);

	LockTypes modelRead;
	modelRead.set(LockType::ModelRead);

	LockTypes modelWrite;
	modelWrite.set(LockType::ModelWrite);

	std::string mainTab = initializeAndCreateUI(modelRead, modelWrite);

	application->getUiComponent()->setControlState(actionButtonID, false);
	application->getUiComponent()->activateMenuPage(mainTab);

	application->modelSelectionChanged();
	
	application->enableMessageQueuing(application->getUiComponent()->getServiceName(), false);
}

void ot::ModalCommandBase::addMenuPage(const std::string &menu)
{
	application->getUiComponent()->addMenuPage(menu);
	uiMenuMap[menu] = true;
}

void ot::ModalCommandBase::addMenuGroup(const std::string &menu, const std::string &group)
{
	application->getUiComponent()->addMenuGroup(menu, group);
	uiGroupMap[menu + ":" + group] = true;
}

void ot::ModalCommandBase::addMenuSubgroup(const std::string &menu, const std::string &group, const std::string &subgroup)
{
	application->getUiComponent()->addMenuSubGroup(menu, group, subgroup);
	uiSubGroupMap[menu + ":" + group + ":" + subgroup] = true;
}

void ot::ModalCommandBase::addMenuAction(const std::string &menu, const std::string &group, const std::string &buttonName, const std::string &text, const LockTypes& flags, const std::string &iconName, const std::string &iconFolder)
{
	application->getUiComponent()->addMenuButton(menu, group, buttonName, text, flags, iconName, iconFolder);
	uiActionMap[menu + ":" + group + ":" + buttonName] = true;
}

void ot::ModalCommandBase::addMenuAction(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &buttonName, const std::string &text, const LockTypes& flags, const std::string &iconName, const std::string &iconFolder)
{
	application->getUiComponent()->addMenuButton(menu, group, subgroup, buttonName, text, flags, iconName, iconFolder);
	uiActionMap[menu + ":" + group + ":" + subgroup + ":" + buttonName] = true;
}

void ot::ModalCommandBase::addMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, const std::string &boxText, bool checked, const LockTypes& flags)
{
	application->getUiComponent()->addMenuCheckbox(menu, group, subgroup, boxName, boxText, checked, flags);
	uiActionMap[menu + ":" + group + ":" + subgroup + ":" + boxName] = true;
}

void ot::ModalCommandBase::addMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, const std::string &editLabel, const LockTypes& flags)
{
	application->getUiComponent()->addMenuLineEdit(menu, group, subgroup, editName, editLabel, editText, flags);
	uiActionMap[menu + ":" + group + ":" + subgroup + ":" + editName] = true;
}

void ot::ModalCommandBase::setMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, bool checked)
{
	application->getUiComponent()->setCheckboxValues(menu + ":" + group + ":" + subgroup + ":"+ boxName, checked);
}

void ot::ModalCommandBase::setMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, bool error)
{
	application->getUiComponent()->setLineEditValues(menu + ":" + group + ":" + subgroup + ":"+ editName, editText, error);
}
