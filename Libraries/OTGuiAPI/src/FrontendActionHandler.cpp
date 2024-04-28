//! @file FrontendActionHandler.cpp
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGuiAPI/FrontendActionHandler.h"
#include "OTCommunication/ActionDispatcher.h" 
#include "OTCommunication/ActionDispatcher.h"

std::string ot::FrontendActionHandler::handleKeySequenceActivated(JsonDocument& _document) {
	std::string keySequence = json::getString(_document, OT_ACTION_PARAM_UI_KeySequence);
	this->keySequenceActivated(keySequence);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::FrontendActionHandler::handleSettingsItemChanged(JsonDocument& _document) {
	/*
	ot::SettingsData* data = ot::SettingsData::parseFromJsonDocument(_document);
	if (data) {
		std::list<ot::AbstractSettingsItem*> items = data->items();
		if (items.empty()) {
			delete data;
			return OT_ACTION_RETURN_INDICATOR_Error "SettingsData does not contain any items";
		}
		else if (items.size() > 1) {
			delete data;
			return OT_ACTION_RETURN_INDICATOR_Error "SettingsData does contain more than 1 item";
		}
		if (settingChanged(items.front())) {
			ot::SettingsData* newData = createSettings();
			newData->saveToDatabase(m_databaseURL, m_siteID, DataBase::GetDataBase()->getUserName(), DataBase::GetDataBase()->getUserPassword(), m_DBuserCollection);
			if (newData) {
				if (m_uiComponent) {
					m_uiComponent->sendSettingsData(newData);
				}
				else {
					OTAssert(0, "Settings changed received but no UI is conencted");
				}
				delete newData;
			}
		}
		else {
			ot::SettingsData* newData = createSettings();
			if (newData) {
				newData->saveToDatabase(m_databaseURL, m_siteID, DataBase::GetDataBase()->getUserName(), DataBase::GetDataBase()->getUserPassword(), m_DBuserCollection);
				delete newData;
			}
		}
		delete data;
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		return OT_ACTION_RETURN_INDICATOR_Error "Failed to parse SettingsData";
	}
	*/
	return "";
}

std::string ot::FrontendActionHandler::handleContextMenuItemClicked(JsonDocument& _document) {
	/*
	if (m_uiNotifier) {
		std::string menuName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuName);
		std::string itemName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName);
		m_uiNotifier->contextMenuItemClicked(menuName, itemName);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		OTAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
	*/
	return "";
}

std::string ot::FrontendActionHandler::handleContextMenuItemCheckedChanged(JsonDocument& _document) {
	/*
	if (m_uiNotifier) {
		std::string menuName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuName);
		std::string itemName = json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ContextMenuItemName);
		bool isChecked = json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_CheckedState);
		m_uiNotifier->contextMenuItemCheckedChanged(menuName, itemName, isChecked);
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else {
		OTAssert(0, "No UI connected");
		return OT_ACTION_RETURN_INDICATOR_Error "No UI connected";
	}
	*/
	return "";
}
