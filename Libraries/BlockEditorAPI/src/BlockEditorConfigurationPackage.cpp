//! @file BlockEditorConfigurationPackage.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorConfigurationPackage.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::BlockEditorConfigurationPackage::BlockEditorConfigurationPackage(const std::string& _editorName, const std::string& _editorTitle)
	: m_editorName(_editorName), m_editorTitle(_editorTitle) {}

ot::BlockEditorConfigurationPackage::~BlockEditorConfigurationPackage() {
	this->memClear();
}

void ot::BlockEditorConfigurationPackage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueArray(categoriesArr);
	for (auto c : m_topLevelBlockCategories) {
		OT_rJSON_createValueObject(categoryObj);
		c->addToJsonObject(_document, categoryObj);
		categoriesArr.PushBack(categoryObj, _document.GetAllocator());
	}

	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_BLOCKEDITOR_EditorName, m_editorName);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_BLOCKEDITOR_EditorTitle, m_editorTitle);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_BLOCKEDITOR_Categories, categoriesArr);
}

void ot::BlockEditorConfigurationPackage::setFromJsonObject(OT_rJSON_val& _object) {
	this->memClear();

	OT_rJSON_checkMember(_object, OT_ACTION_PARAM_BLOCKEDITOR_EditorName, String);
	OT_rJSON_checkMember(_object, OT_ACTION_PARAM_BLOCKEDITOR_EditorTitle, String);
	OT_rJSON_checkMember(_object, OT_ACTION_PARAM_BLOCKEDITOR_Categories, Array);

	m_editorName = _object[OT_ACTION_PARAM_BLOCKEDITOR_EditorName].GetString();
	m_editorTitle = _object[OT_ACTION_PARAM_BLOCKEDITOR_EditorTitle].GetString();
	OT_rJSON_val categoriesArr = _object[OT_ACTION_PARAM_BLOCKEDITOR_Categories].GetArray();
	for (rapidjson::SizeType i = 0; i < categoriesArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(categoriesArr, i, Object);
		OT_rJSON_val categoryObj = categoriesArr[i].GetObject();
		ot::BlockCategoryConfiguration* newCategory = new ot::BlockCategoryConfiguration;
		newCategory->setFromJsonObject(categoryObj);
		m_topLevelBlockCategories.push_back(newCategory);
	}
}

void ot::BlockEditorConfigurationPackage::setTopLevelBlockCategories(const std::list<ot::BlockCategoryConfiguration*>& _categories) {
	this->memClear();
	m_topLevelBlockCategories = _categories;
}

void ot::BlockEditorConfigurationPackage::addTopLevelBlockCategory(ot::BlockCategoryConfiguration* _category) {
	m_topLevelBlockCategories.push_back(_category);
}

void ot::BlockEditorConfigurationPackage::addTopLevelBlockCategories(const std::list<ot::BlockCategoryConfiguration*>& _categories) {
	for (auto c : _categories) m_topLevelBlockCategories.push_back(c);
}

void ot::BlockEditorConfigurationPackage::memClear(void) {
	for (auto c : m_topLevelBlockCategories) delete c;
	m_topLevelBlockCategories.clear();
}