/*
 *  UiComponent.cpp
 *
 *  Created on: 10/02/2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021, OpenTwin
 */

#include "OTCore/OTAssert.h"
#include "OTCore/ThisService.h"

#include "OTCommunication/actionTypes.h"

#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/UiPluginComponent.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/SettingsData.h"

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
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuPage, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

		m_uiElements.push_back(_pageName);
	}
}

void ot::components::UiComponent::addMenuGroup(
	const std::string &			_pageName,
	const std::string &			_groupName
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuGroupVisible(_pageName, _groupName))
	{
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuGroup, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

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
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuSubgroup, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, JsonString(_subgroupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _subgroupName);
	}
}

void ot::components::UiComponent::addMenuButton(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_buttonName,
	const std::string &			_text,
	const LockTypeFlags&		_lockTypes,
	const std::string &			_iconName,
	const std::string &			_iconFolder,
	const std::string &			_keySequence,
	const ContextMenu&			_contextMenu
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _buttonName))
	{
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuButton, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_buttonName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_text, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_IconName, JsonString(_iconName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_IconFolder, JsonString(_iconFolder, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		if (!_keySequence.empty()) {
			cmdDoc.AddMember(OT_ACTION_PARAM_UI_KeySequence, JsonString(_keySequence, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		}

		JsonObject contextMenuData;
		_contextMenu.addToJsonObject(contextMenuData, cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenu, contextMenuData, cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _buttonName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _buttonName] = true;
	}
}

void ot::components::UiComponent::addMenuButton(MenuButtonDescription& _menuButtonDescription,  const LockTypeFlags& _lockTypes, const std::string & _iconName, const std::string & _iconFolder, const std::string & _keySequence, const ContextMenu & _contextMenu)
{
	addMenuButton(_menuButtonDescription.GetPageName(), _menuButtonDescription.GetGroupName(), _menuButtonDescription.GetSubgroupName(), _menuButtonDescription.GetButtonName(), _menuButtonDescription.GetButtonText(), _lockTypes, _iconName, _iconFolder, _keySequence, _contextMenu);
}

void ot::components::UiComponent::addMenuButton(
	const std::string &			_pageName,
	const std::string &			_groupName,
	const std::string &			_subgroupName,
	const std::string &			_buttonName,
	const std::string &			_text,
	const LockTypeFlags&		_lockTypes,
	const std::string &			_iconName,
	const std::string &			_iconFolder,
	const std::string &			_keySequence,
	const ContextMenu&			_contextMenu
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _buttonName))
	{
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuButton, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, JsonString(_subgroupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_buttonName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_text, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_IconName, JsonString(_iconName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_IconFolder, JsonString(_iconFolder, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		if (!_keySequence.empty()) {
			cmdDoc.AddMember(OT_ACTION_PARAM_UI_KeySequence, JsonString(_keySequence, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		}

		JsonObject contextMenuData;
		_contextMenu.addToJsonObject(contextMenuData, cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContextMenu, contextMenuData, cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

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
	const LockTypeFlags&		_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _checkboxName))
	{
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuCheckbox, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_checkboxName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_checkboxText, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_CheckedState, _isChecked, cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

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
	const LockTypeFlags&		_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _checkboxName))
	{
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuCheckbox, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, JsonString(_subgroupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_checkboxName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_checkboxText, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_CheckedState, _isChecked, cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

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
	const LockTypeFlags&		_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _lineEditName))
	{
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuLineEdit, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_lineEditName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_initialText, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText, JsonString(_labelText, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

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
	const LockTypeFlags&		_lockTypes
) {
	if (TemplateDefaultManager::getTemplateDefaultManager()->isUIMenuActionVisible(_pageName, _groupName, _lineEditName))
	{
		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddMenuLineEdit, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_GroupName, JsonString(_groupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName, JsonString(_subgroupName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_lineEditName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_initialText, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText, JsonString(_labelText, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		std::string response;
		m_application->sendMessage(true, m_serviceName, cmdDoc, response);

		m_uiElements.push_back(_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _lineEditName);
		m_uiControlStateInUI[_pageName + ":" + _groupName + ":" + _subgroupName + ":" + _lineEditName] = true;
	}
}

void ot::components::UiComponent::activateMenuPage(
	const std::string &			_pageName
) {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_ActivateToolbarTab, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_TabName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::addShortcut(
	const std::string &			_keySequence
) {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddShortcut, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_KeySequence, JsonString(_keySequence, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::createRubberband(
	const std::string &			_note,
	const std::string &			_configurationJson
) {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_VIEW_CreateRubberband, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_Note, JsonString(_note, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_Document, JsonString(_configurationJson, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
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

	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_OBJ_SetToolTip, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_controlName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_toolTipText, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
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
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_EnableDisableControls, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_EnabledControlsList, JsonArray(_enabledControls, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_DisabledControlsList, JsonArray(_disabledControls, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::setLineEditValues(
	const std::string &				_controlName,
	const std::string &				_text,
	bool							_errorState
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SetLineEditValues, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_controlName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, JsonString(_text, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ErrorState, _errorState, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::setCheckboxValues(
	const std::string &				_controlName,
	bool							_checkedState
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SetCheckboxValues, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, JsonString(_controlName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, _checkedState, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::lockUI(
	const LockTypeFlags&		_lockTypes
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_Lock, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::unlockUI(
	const LockTypeFlags&		_lockTypes
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_Unlock, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_ElementLockTypes, JsonArray(toStringList(_lockTypes), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::switchMenuTab(
	const std::string &				_pageName
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SwitchMenuTab, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_PageName, JsonString(_pageName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::registerForModelEvents(void) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_RegisterForModelEvents, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::deregisterForModelEvents(void) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_DeregisterForModelEvents, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::addIconSearchPath(const std::string& _iconPath) {
#ifdef _DEBUG
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddIconSearchPath, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_IconFolder, JsonString(_iconPath, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_application->serviceURL(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, JsonString(m_application->serviceName(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, JsonString(m_application->serviceType(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
#endif // _DEBUG
}

void ot::components::UiComponent::notifyUiSetupCompleted(void) {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_ServiceSetupCompleted, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	ot::ThisService::addIdToJsonDocument(cmdDoc);

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

// #####################################################################################################################

// Information IO

void ot::components::UiComponent::displayMessage(
	const std::string &				_message
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_DisplayMessage, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(_message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::displayDebugMessage(
	const std::string &				_message
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_DisplayDebugMessage, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(_message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::displayErrorPrompt(
	const std::string &				_message
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_ReportError, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(_message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::displayWarningPrompt(
	const std::string &				_message
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_ReportWarning, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(_message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::displayInformationPrompt(
	const std::string &				_message
) const {
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_ReportInformation, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(_message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	
	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

// #####################################################################################################################

// Progress bar control

void ot::components::UiComponent::setProgressInformation(std::string message, bool continuous)
{
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SetProgressVisibility, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString(message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_VisibleState, true, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContinuousState, continuous, cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::setProgress(int percentage)
{
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SetProgressbarValue, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_PERCENT, percentage, cmdDoc.GetAllocator());
	
	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::closeProgressInformation(void)
{
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SetProgressVisibility, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, JsonString("", cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_VisibleState, false, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContinuousState, false, cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
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
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_RemoveElements, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectNames, JsonArray(_itemList, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);

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
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_RemoveElements, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectNames, JsonArray(m_uiElements, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
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

	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_VIEW_EnterEntitySelectionMode, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_REPLYTO, _serviceToReplyTo, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_AllowMultipleSelection, _allowMultipleSelection, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Filter, JsonString(_filter, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Message, JsonString(_message, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames, JsonArray(optionNames, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues, JsonArray(optionValues, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());

	switch (_selectionType)
	{
	case ot::components::UiComponent::FACE: cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SelectionType, JsonString("FACE", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::EDGE: cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SelectionType, JsonString("EDGE", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::SHAPE: cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SelectionType, JsonString("SHAPE", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::TRANSFORM: cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_SelectionType, JsonString("TRANSFORM", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	default: assert(0); throw std::exception("Invalid selection type");
	}

	switch (_actionType)
	{
	case ot::components::UiComponent::PORT: cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Action, JsonString("PORT", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::REMOVE_FACE: cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Action, JsonString("REMOVE_FACE", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::BOOLEAN_ADD: cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Action, JsonString("BOOLEAN_ADD", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::BOOLEAN_SUBTRACT: cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Action, JsonString("BOOLEAN_SUBTRACT", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::BOOLEAN_INTERSECT: cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Action, JsonString("BOOLEAN_INTERSECT", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	case ot::components::UiComponent::TRANSFORM_SHAPES: cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_Action, JsonString("TRANSFORM", cmdDoc.GetAllocator()), cmdDoc.GetAllocator()); break;
	default: assert(0); throw std::exception("Invalid action type");
	}

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::freeze3DView(UID _visualizationModelUID, bool _flag)
{
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_Freeze3DView, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID, cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_CONTROL_BOOLEAN_STATE, _flag, cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::refreshSelection(UID _visualizationModelUID)
{
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_VIEW_RefreshSelection, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID, cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::refreshAllViews(UID _visualizationModelUID)
{
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_VIEW_Refresh, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModelUID, cmdDoc.GetAllocator());

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
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

bool ot::components::UiComponent::sendMessage(bool _queue, JsonDocument& _doc, std::string& _response) {
	return m_application->sendMessage(true, m_serviceName, _doc, _response);
}

// #####################################################################################################################

// Plugin

void ot::components::UiComponent::requestUiPlugin(const std::string& _pluginName, const std::string& _filename) {
	if (m_application->pluginExists(_pluginName)) {
		OT_LOG_EAS("A plugin with the name \"" + _pluginName + "\" is already connected");
		return;
	}
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_RequestPlugin, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_application->serviceURL(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, JsonString(m_application->serviceName(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, JsonString(m_application->serviceType(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_NAME, JsonString(_pluginName, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_PATH, JsonString(_filename, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	
	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

void ot::components::UiComponent::addPluginSearchPath(const std::string& _pluginPath) {
#ifdef _DEBUG
	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddPluginSearchPath, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, JsonString(m_application->serviceURL(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, JsonString(m_application->serviceName(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, JsonString(m_application->serviceType(), cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_UI_PLUGIN_PATH, JsonString(_pluginPath, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	
	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
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
		OTAssert(0, "Nullptr provided @addSettingsData");
		return;
	}

	JsonDocument cmdDoc;
	cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_AddSettingsData, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
	cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());
	_settingsData->addToJsonDocument(cmdDoc);

	std::string response;
	m_application->sendMessage(true, m_serviceName, cmdDoc, response);
}

