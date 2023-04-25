/*
 *  UiComponent.cpp
 *
 *  Created on: 10/02/2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021, OpenTwin
 */

#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/otAssert.h"

#include "OpenTwinCommunication/actionTypes.h"

#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/UiPluginComponent.h"
#include "OpenTwinFoundation/ApplicationBase.h"
#include "OpenTwinFoundation/SettingsData.h"

#include "TemplateDefaultManager.h"

#include <iostream>

ot::components::UiComponent::UiComponent(
	const std::string &			_name,
	const std::string &			_type,
	const std::string &			_url,
	serviceID_t					_id,
	ApplicationBase *			_application
)
	: ServiceBase(_name, _type, _url, _id), m_application(_application)
{
	assert(m_application != nullptr);	// No application provided
}

ot::components::UiComponent::~UiComponent() {}

// #####################################################################################################################

// Ribbon function

void ot::components::UiComponent::addMenuPage(
	const std::string &			_pageName
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuPageVisible(_pageName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuPage);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);

		m_application->sendMessage(true, m_serviceName, cmdDoc);

		m_uiElements.push_back(_pageName);
	}
}

void ot::components::UiComponent::addMenuGroup(
	const std::string &			_pageName,
	const std::string &			_groupName
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuGroupVisible(_pageName, _groupName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuGroup);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);

		m_application->sendMessage(true, m_serviceName, cmdDoc);

		m_uiElements.push_back(_pageName + ":" + _groupName);
	}
}

void ot::components::UiComponent::addMenuSubGroup(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_subgroupName
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuGroupVisible(_pageName, _groupName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuSubgroup);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName, _subgroupName);

		m_application->sendMessage(true, m_serviceName, cmdDoc);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _subgroupName);
	}
}

void ot::components::UiComponent::addMenuButton(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_buttonName,
	const std::string &			_text,
	const Flags<ui::lockType> &	_lockTypes,
	const std::string &			_iconName,
	const std::string &			_iconFolder,
	const std::string &			_keySequence,
	const ContextMenu&			_contextMenu
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _buttonName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuButton);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _buttonName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _text);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_IconName, _iconName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_IconFolder, _iconFolder);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ui::toList(_lockTypes));
		if (!_keySequence.empty()) {
			ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_KeySequence, _keySequence);
		}
		OT_rJSON_createValueObject(contextMenuData);
		_contextMenu.addToJsonObject(cmdDoc, contextMenuData);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ContextMenu, contextMenuData);

		m_application->sendMessage(true, m_serviceName, cmdDoc);
		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _buttonName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _buttonName] = true;
	}
}

void ot::components::UiComponent::addMenuButton(MenuButtonDescription& _menuButtonDescription,  const Flags<ui::lockType>& _lockTypes, const std::string & _iconName, const std::string & _iconFolder, const std::string & _keySequence, const ContextMenu & _contextMenu)
{
	addMenuButton(_menuButtonDescription.GetPageName(), _menuButtonDescription.GetGroupName(), _menuButtonDescription.GetButtonName(), _menuButtonDescription.GetButtonText(), _lockTypes, _iconName, _iconFolder, _keySequence, _contextMenu);
}

void ot::components::UiComponent::addMenuButton(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_subgroupName,
	const std::string &			_buttonName,
	const std::string &			_text,
	const Flags<ui::lockType> &	_lockTypes,
	const std::string &			_iconName,
	const std::string &			_iconFolder,
	const std::string &			_keySequence,
	const ContextMenu&			_contextMenu
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _buttonName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuButton);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName, _subgroupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _buttonName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _text);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_IconName, _iconName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_IconFolder, _iconFolder);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ui::toList(_lockTypes));
		if (!_keySequence.empty()) {
			ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_KeySequence, _keySequence);
		}
		OT_rJSON_createValueObject(contextMenuData);
		_contextMenu.addToJsonObject(cmdDoc, contextMenuData);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ContextMenu, contextMenuData);

		m_application->sendMessage(true, m_serviceName, cmdDoc);
		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _buttonName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _buttonName] = true;
	}
}

void ot::components::UiComponent::addMenuCheckbox(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_checkboxName,
	const std::string &			_checkboxText,
	bool						_isChecked,
	const Flags<ui::lockType> &	_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _checkboxName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuCheckbox);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _checkboxName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _checkboxText);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_CheckedState, _isChecked);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ui::toList(_lockTypes));

		m_application->sendMessage(true, m_serviceName, cmdDoc);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _checkboxName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _checkboxName] = true;
	}
}

void ot::components::UiComponent::addMenuCheckbox(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_subgroupName,
	const std::string &			_checkboxName,
	const std::string &			_checkboxText,
	bool						_isChecked,
	const Flags<ui::lockType> &	_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _checkboxName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuCheckbox);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName, _subgroupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _checkboxName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _checkboxText);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_CheckedState, _isChecked);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ui::toList(_lockTypes));

		m_application->sendMessage(true, m_serviceName, cmdDoc);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _checkboxName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _checkboxName] = true;
	}
}

void ot::components::UiComponent::addMenuLineEdit(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_lineEditName,
	const std::string &			_labelText,
	const std::string &			_initialText,
	const Flags<ui::lockType> &	_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _lineEditName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuLineEdit);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _lineEditName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _initialText);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText, _labelText);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ui::toList(_lockTypes));

		m_application->sendMessage(true, m_serviceName, cmdDoc);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _lineEditName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _lineEditName] = true;
	}
}

void ot::components::UiComponent::addMenuLineEdit(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_subgroupName,
	const std::string &			_lineEditName,
	const std::string &			_labelText,
	const std::string &			_initialText,
	const Flags<ui::lockType> &	_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _lineEditName))
	{
		OT_rJSON_createDOC(cmdDoc);
		ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddMenuLineEdit);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_GroupName, _groupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_SubgroupName, _subgroupName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _lineEditName);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _initialText);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText, _labelText);
		ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ui::toList(_lockTypes));

		m_application->sendMessage(true, m_serviceName, cmdDoc);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _lineEditName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _lineEditName] = true;
	}
}

void ot::components::UiComponent::activateMenuPage(
	const std::string &			_pageName
) {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_ActivateToolbarTab);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_TabName, _pageName);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::addShortcut(
	const std::string &			_keySequence
) {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddShortcut);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_KeySequence, _keySequence);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::createRubberband(
	const std::string &			_note,
	const std::string &			_configurationJson
) {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_CreateRubberband);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_VIEW_RUBBERBAND_Note, _note);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_VIEW_RUBBERBAND_Document, _configurationJson);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

//void ot::components::UiComponent::createBlockEditor(const std::string& _name, const std::string& _title, const std::list<BlockCategoryConfiguration*>& _configuration) {
//	OT_rJSON_createDOC(cmdDoc);
//	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_BLOCKEDITOR_CreateBlockEditor);
//	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_BLOCKEDITOR_EditorName, _name);
//	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_BLOCKEDITOR_EditorTitle, _title);
//
//	OT_rJSON_createValueArray(categoriesArr);
//	for (auto c : _configuration) {
//		OT_rJSON_createValueObject(categoryObj);
//		c->addToJsonObject(cmdDoc, categoryObj);
//		categoriesArr.PushBack(categoryObj, cmdDoc.GetAllocator());
//	}
//	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_BLOCKEDITOR_Categories, categoriesArr);
//
//	m_application->sendMessage(true, m_serviceName, cmdDoc);
//}

// #####################################################################################################################

// Control manipulation

void ot::components::UiComponent::setControlToolTip(
	const std::string &			_controlName,
	const std::string &			_toolTipText
) const {
#ifdef _DEBUG
	{
		// In debug mode, ensure that the control name is valid
		assert(controlNameExists(_controlName));
	}
#endif // _DEBUG

	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_OBJ_SetToolTip);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _controlName);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _toolTipText);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::setControlToolTip(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_controlName,
	const std::string &			_toolTipText
) const { setControlToolTip(_pageName + ":" + _groupName + ":" + _controlName, _toolTipText); }

void ot::components::UiComponent::setControlToolTip(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_subgroupName,
	const std::string &			_controlName,
	const std::string &			_toolTipText
) const { setControlToolTip(_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _controlName, _toolTipText); }

void ot::components::UiComponent::setControlsEnabledState(
	const std::list<std::string> &	_enabledControls,
	const std::list<std::string> &	_disabledControls
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_EnableDisableControls);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_EnabledControlsList, _enabledControls);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_DisabledControlsList, _disabledControls);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::setLineEditValues(
	const std::string &				_controlName,
	const std::string &				_text,
	bool							_errorState
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetLineEditValues);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, _controlName);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectText, _text);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ErrorState, _errorState);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::setCheckboxValues(
	const std::string &				_controlName,
	bool							_checkedState
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetCheckboxValues);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectName,   _controlName);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_CheckedState, _checkedState);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::lockUI(
	const Flags<ui::lockType> &		_lockTypes
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_Lock);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ot::ui::toList(_lockTypes));

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::unlockUI(
	const Flags<ui::lockType> &		_lockTypes
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_Unlock);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_ElementLockTypes, ot::ui::toList(_lockTypes));

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::switchMenuTab(
	const std::string &				_pageName
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SwitchMenuTab);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_PageName, _pageName);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::registerForModelEvents(void) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_RegisterForModelEvents);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::deregisterForModelEvents(void) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_DeregisterForModelEvents);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::addIconSearchPath(const std::string& _iconPath) {
#ifdef _DEBUG
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddIconSearchPath);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_URL, m_application->serviceURL());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, m_application->serviceName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, m_application->serviceType());
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_IconFolder, _iconPath);
	m_application->sendMessage(true, m_serviceName, doc);
#endif // _DEBUG	
}

// #####################################################################################################################

// Information IO

void ot::components::UiComponent::displayMessage(
	const std::string &				_message
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_DisplayMessage);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, _message);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::displayDebugMessage(
	const std::string &				_message
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_DisplayDebugMessage);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, _message);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::displayErrorPrompt(
	const std::string &				_message
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_ReportError);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, _message);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::displayWarningPrompt(
	const std::string &				_message
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_ReportWarning);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, _message);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::displayInformationPrompt(
	const std::string &				_message
) const {
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_ReportInformation);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, _message);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

// #####################################################################################################################

// Progress bar control

void ot::components::UiComponent::setProgressInformation(std::string message, bool continuous)
{
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetProgressVisibility);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, message);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_VisibleState, true);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ContinuousState, continuous);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::setProgress(int percentage)
{
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetProgressbarValue);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_PERCENT, percentage);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::closeProgressInformation(void)
{
	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetProgressVisibility);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_MESSAGE, "");
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_VisibleState, false);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_UI_CONTROL_ContinuousState, false);

	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

void ot::components::UiComponent::addControlNameToList(
	std::list<std::string> &		_list,
	const std::string &				_pageName,
	const std::string &				_groupName,
	const std::string &				_controlName
) {
#ifdef _DEBUG
	{
		// In debug mode, ensure that the control name is valid
		assert(controlNameExists(_pageName + ":" + _groupName + ":" + _controlName));
	}
#endif // _DEBUG
	_list.push_back(_pageName + ":" + _groupName + ":" + _controlName);
}

void ot::components::UiComponent::addControlNameToList(
	std::list<std::string> &		_list,
	const std::string &				_pageName,
	const std::string &				_groupName,
	const std::string &				_subgroupName,
	const std::string &				_controlName
) {
#ifdef _DEBUG
	{
		// In debug mode, ensure that the control name is valid
		assert(controlNameExists(_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _controlName));
	}
#endif // _DEBUG
	_list.push_back(_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _controlName);
}

// #####################################################################################################################

// Element deletion

void ot::components::UiComponent::removeUIElements(const std::list<std::string> & _itemList)
{
	OT_rJSON_createDOC(sessionDoc);
	ot::rJSON::add(sessionDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_RemoveElements);
	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectNames, _itemList);

	m_application->sendMessage(true, m_serviceName, sessionDoc);

	// Clean up entries
	for (auto itm : _itemList) {
		auto findIter = std::find(m_uiElements.begin(), m_uiElements.end(), itm);
		if (findIter != m_uiElements.end()) {
			m_uiElements.erase(findIter);
		}
	}
}

void ot::components::UiComponent::removeAllUIElements(void)
{
	OT_rJSON_createDOC(sessionDoc);
	ot::rJSON::add(sessionDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_RemoveElements);
	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());

	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_UI_CONTROL_ObjectNames, m_uiElements);

	m_application->sendMessage(true, m_serviceName, sessionDoc);
	m_uiElements.clear();
}

// #####################################################################################################################

// View manipulation

void ot::components::UiComponent::enterEntitySelectionMode(
	UID											_visualizationModelUID,
	entitySlectionType							_selectionType,
	bool										_allowMultipleSelection,
	const std::string &							_filter,
	entitySelectionAction						_actionType,
	const std::string &							_message,
	const std::map<std::string, std::string> &	_options,
	ot::serviceID_t								_serviceToReplyTo
) const {
	std::list<std::string> optionNames;
	std::list<std::string> optionValues;
	for (auto itm : _options) {
		optionNames.push_back(itm.first);
		optionValues.push_back(itm.second);
	}

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_EnterEntitySelectionMode);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_REPLYTO, _serviceToReplyTo);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_AllowMultipleSelection, _allowMultipleSelection);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Filter, _filter);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Message, _message);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames, optionNames);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues, optionValues);

	switch (_selectionType)
	{
	case ot::components::UiComponent::FACE: ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_SelectionType, "FACE"); break;
	case ot::components::UiComponent::EDGE: ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_SelectionType, "EDGE"); break;
	case ot::components::UiComponent::SHAPE: ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_SelectionType, "SHAPE"); break;
	case ot::components::UiComponent::TRANSFORM: ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_SelectionType, "TRANSFORM"); break;
	default: assert(0); throw std::exception("Invalid selection type");
	}

	switch (_actionType)
	{
	case ot::components::UiComponent::PORT: ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Action, "PORT"); break;
	case ot::components::UiComponent::REMOVE_FACE: ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Action, "REMOVE_FACE"); break;
	case ot::components::UiComponent::BOOLEAN_ADD: ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Action, "BOOLEAN_ADD"); break;
	case ot::components::UiComponent::BOOLEAN_SUBTRACT: ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Action, "BOOLEAN_SUBTRACT"); break;
	case ot::components::UiComponent::BOOLEAN_INTERSECT: ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Action, "BOOLEAN_INTERSECT"); break;
	case ot::components::UiComponent::TRANSFORM_SHAPES: ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_Action, "TRANSFORM"); break;
	default: assert(0); throw std::exception("Invalid action type");
	}

	m_application->sendMessage(true, m_serviceName, doc);
}

void ot::components::UiComponent::freeze3DView(UID _visualizationModelUID, bool _flag)
{
	OT_rJSON_createDOC(sessionDoc);
	ot::rJSON::add(sessionDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_Freeze3DView);
	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID);
	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_UI_CONTROL_BOOLEAN_STATE, _flag);

	m_application->sendMessage(true, m_serviceName, sessionDoc);
}

void ot::components::UiComponent::refreshSelection(UID _visualizationModelUID)
{
	OT_rJSON_createDOC(sessionDoc);
	ot::rJSON::add(sessionDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_RefreshSelection);
	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID);

	m_application->sendMessage(true, m_serviceName, sessionDoc);
}

void ot::components::UiComponent::refreshAllViews(UID _visualizationModelUID)
{
	OT_rJSON_createDOC(sessionDoc);
	ot::rJSON::add(sessionDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_Refresh);
	ot::rJSON::add(sessionDoc, OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID);

	m_application->sendMessage(true, m_serviceName, sessionDoc);
}

void ot::components::UiComponent::sendUpdatedControlState(void)
{
	// Determine which state needs to be changed
	std::list<std::string> enabledControls, disabledControls;

	for (auto item : m_uiControlState)
	{
		if (m_uiControlStateInUI.count(item.first) == 0)
		{
			m_uiControlStateInUI[item.first] = true;  // Items are enabled by default
		}

		if (m_uiControlStateInUI[item.first] != item.second)
		{
			// The state of this item is changing
			if (item.second)
			{
				// The item is now enabled
				enabledControls.push_back(item.first);
			}
			else
			{
				// The item is now disabled
				disabledControls.push_back(item.first);
			}
		}
	}

	// Send the state
	setControlsEnabledState(enabledControls, disabledControls);

	// Set the current state as the ui state
	m_uiControlStateInUI = m_uiControlState;
}

void ot::components::UiComponent::setControlState(const std::string controlName, bool enabled)
{
	m_uiControlState[controlName] = enabled;
}

std::string  ot::components::UiComponent::sendMessage(bool _queue, OT_rJSON_doc & _doc)
{
	return m_application->sendMessage(_queue, m_serviceName, _doc);
}

// #####################################################################################################################

// Plugin

void ot::components::UiComponent::requestUiPlugin(const std::string& _pluginName, const std::string& _filename) {
	if (m_application->pluginExists(_pluginName)) {
		std::cout << "A plugin with the specified name (" << _pluginName << ") is already connected" << std::endl;
		otAssert(0, "A plugin with the specified name is already connected");
		return;
	}
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_RequestPlugin);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_URL, m_application->serviceURL());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, m_application->serviceName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, m_application->serviceType());
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_PLUGIN_NAME, _pluginName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_PLUGIN_PATH, _filename);
	m_application->sendMessage(true, m_serviceName, doc);
}

void ot::components::UiComponent::addPluginSearchPath(const std::string& _pluginPath) {
#ifdef _DEBUG
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddPluginSearchPath);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_URL, m_application->serviceURL());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, m_application->serviceName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, m_application->serviceType());
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_PLUGIN_PATH, _pluginPath);
	m_application->sendMessage(true, m_serviceName, doc);
#endif // _DEBUG

}

// #####################################################################################################################

// Private functions

bool ot::components::UiComponent::controlNameExists(const std::string & _name) const {
	return std::find(m_uiElements.begin(), m_uiElements.end(), _name) != m_uiElements.end();
}

void ot::components::UiComponent::sendSettingsData(
	SettingsData *				_settingsData
) {
	if (_settingsData == nullptr) {
		otAssert(0, "Nullptr provided @addSettingsData");
		return;
	}

	OT_rJSON_createDOC(cmdDoc);
	ot::rJSON::add(cmdDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_AddSettingsData);
	ot::rJSON::add(cmdDoc, OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID());
	_settingsData->addToJsonDocument(cmdDoc);
	m_application->sendMessage(true, m_serviceName, cmdDoc);
}

