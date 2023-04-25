// Project header
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinFoundation/SettingsData.h"

#include "DataBase.h"
#include "Document\DocumentAccess.h"
#include "Connection\ConnectionAPI.h"

#include <bsoncxx/builder/stream/document.hpp>


#include <climits>

ot::SettingsData::SettingsData(const std::string& _settingsDataName, const std::string& _settingsVersion)
	: m_settingsDataName(_settingsDataName), m_settingsVersion(_settingsVersion)
{

}

ot::SettingsData::~SettingsData() {
	for (auto group : m_groups) {
		delete group;
	}
}

ot::SettingsData * ot::SettingsData::parseFromJsonDocument(rapidjson::Document& _document) {
	if (!_document.IsObject()) {
		otAssert(0, "Document is not an object");
		return nullptr;
	}
	return parseFromJsonObject(_document);
}

ot::SettingsData * ot::SettingsData::parseFromJsonObject(rapidjson::Value& _object) {
	// Check member
	if (!_object.HasMember(OT_ACTION_PARAM_SETTINGS_DataName)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_DataName "\" is missing");
		return nullptr;
	}
	if (!_object.HasMember(OT_ACTION_PARAM_SETTINGS_Version)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Version "\" is missing");
		return nullptr;
	}
	if (!_object.HasMember(OT_ACTION_PARAM_SETTINGS_Groups)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Groups "\" is missing");
		return nullptr;
	}

	// Check types
	if (!_object[OT_ACTION_PARAM_SETTINGS_DataName].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_DataName "\" is not a string");
		return nullptr;
	}
	if (!_object[OT_ACTION_PARAM_SETTINGS_Version].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Version "\" is not a string");
		return nullptr;
	}
	if (!_object[OT_ACTION_PARAM_SETTINGS_Groups].IsArray()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Groups "\" is not a array");
		return nullptr;
	}

	std::string name = _object[OT_ACTION_PARAM_SETTINGS_DataName].GetString();
	std::string version = _object[OT_ACTION_PARAM_SETTINGS_Version].GetString();
	rapidjson::Value groupsData = _object[OT_ACTION_PARAM_SETTINGS_Groups].GetArray();

	SettingsData * newData = new SettingsData(name, version);

	for (rapidjson::SizeType i{ 0 }; i < groupsData.Size(); i++) {
		if (groupsData[i].IsObject()) {
			rapidjson::Value groupEntry = groupsData[i].GetObject();
			SettingsGroup * newGroup = parseGroupFromJsonObject(groupEntry);
			if (newGroup) {
				newData->addGroup(newGroup);
			}
		}
		else {
			otAssert(0, "An entry in the groups data is not an object");
		}
	}

	return newData;
}

ot::SettingsGroup * ot::SettingsData::parseGroupFromJsonObject(rapidjson::Value& _object) {
	if (!_object.IsObject()) {
		otAssert(0, "Group is not an object");
		return nullptr;
	}
	// Check member
	if (!_object.HasMember(OT_ACTION_PARAM_SETTINGS_Name)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Name "\" is missing");
		return nullptr;
	}
	if (!_object.HasMember(OT_ACTION_PARAM_SETTINGS_Title)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Title "\" is missing");
		return nullptr;
	}
	if (!_object.HasMember(OT_ACTION_PARAM_SETTINGS_Groups)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Groups "\" is missing");
		return nullptr;
	}
	if (!_object.HasMember(OT_ACTION_PARAM_SETTINGS_Items)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Items "\" is missing");
		return nullptr;
	}

	// Check types
	if (!_object[OT_ACTION_PARAM_SETTINGS_Name].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Name "\" is not a string");
		return nullptr;
	}
	if (!_object[OT_ACTION_PARAM_SETTINGS_Title].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Title "\" is not a string");
		return nullptr;
	}
	if (!_object[OT_ACTION_PARAM_SETTINGS_Groups].IsArray()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Groups "\" is not a array");
		return nullptr;
	}
	if (!_object[OT_ACTION_PARAM_SETTINGS_Items].IsArray()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Items "\" is not a array");
		return nullptr;
	}

	SettingsGroup * newGroup = new SettingsGroup(
		_object[OT_ACTION_PARAM_SETTINGS_Name].GetString(),
		_object[OT_ACTION_PARAM_SETTINGS_Title].GetString()
	);
	rapidjson::Value groupData = _object[OT_ACTION_PARAM_SETTINGS_Groups].GetArray();
	rapidjson::Value itemData = _object[OT_ACTION_PARAM_SETTINGS_Items].GetArray();

	for (rapidjson::SizeType i{ 0 }; i < groupData.Size(); i++) {
		SettingsGroup * itm = parseGroupFromJsonObject(groupData[i]);
		if (itm) { newGroup->addSubgroup(itm); }
	}
	for (rapidjson::SizeType i{ 0 }; i < itemData.Size(); i++) {
		AbstractSettingsItem * itm = SettingsItemFactory::createItem(itemData[i]);
		if (itm) { newGroup->addItem(itm); }
	}

	return newGroup;
}

// ##############################################################

// Getter

void ot::SettingsData::splitString(std::list<std::string>& _list, const std::string& _string, const std::string& _delimiter) {
	std::string buf = _string;
	size_t ix = buf.find(_delimiter);
	while (ix != std::string::npos) {
		_list.push_back(buf.substr(0, ix));
		buf = buf.substr(ix + _delimiter.length());
		ix = buf.rfind(_delimiter);
	}
	_list.push_back(buf);
}

std::string ot::SettingsData::toJson(void) const {
	OT_rJSON_createDOC(doc);
	addToJsonDocument(doc);
	return ot::rJSON::toJSON(doc);
}

void ot::SettingsData::addToJsonDocument(rapidjson::Document& _document) const {
	ot::rJSON::add(_document, OT_ACTION_PARAM_SETTINGS_DataName, m_settingsDataName);
	ot::rJSON::add(_document, OT_ACTION_PARAM_SETTINGS_Version, m_settingsVersion);

	OT_rJSON_createValueArray(groupsData);
	for (auto group : m_groups) {
		group->addToJsonArray(_document, groupsData);
	}
	ot::rJSON::add(_document, OT_ACTION_PARAM_SETTINGS_Groups, groupsData);
}

void findChilds(std::list<ot::AbstractSettingsItem *> & _list, ot::SettingsGroup * _group) {
	if (_group) {
		for (auto itm : _group->items()) { _list.push_back(itm); }
		for (auto group : _group->subgroups()) { findChilds(_list, group); }
	}
}

std::list<ot::AbstractSettingsItem *> ot::SettingsData::items(void) {
	std::list<ot::AbstractSettingsItem *> ret;
	for (auto group : m_groups) { findChilds(ret, group); }
	return ret;
}

std::list<std::string> ot::SettingsData::itemNames(void) {
	std::list<ot::AbstractSettingsItem *> itemList = items();
	std::list<std::string> ret;
	for (auto itm : itemList) { ret.push_back(itm->logicalName()); }
	return ret;
}

ot::SettingsGroup * ot::SettingsData::findGroupByLogicalName(const std::string& _logicalName, const std::string& _delimiter) {
	std::list<std::string> logicalName;
	splitString(logicalName, _logicalName, _delimiter);
	if (logicalName.size() < 1) { return nullptr; }
	for (auto g : m_groups) {
		if (g->name() == logicalName.front()) {
			if (logicalName.size() == 1) {
				return g;
			}
			else {
				logicalName.pop_front();
				return g->findGroupByLogicalName(logicalName);
			}
		}
	}
	return nullptr;
}

ot::AbstractSettingsItem * ot::SettingsData::findItemByLogicalName(const std::string& _logicalName, const std::string& _delimiter) {
	std::list<std::string> logicalName;
	splitString(logicalName, _logicalName, _delimiter);
	if (logicalName.size() < 2) { return nullptr; }
	for (auto g : m_groups) {
		if (g->name() == logicalName.front()) {
			logicalName.pop_front();
			return g->findItemByLogicalName(logicalName);
		}
	}
	return nullptr;
}

// ##############################################################

// Setter

ot::SettingsGroup* ot::SettingsData::addGroup(const std::string& _name, const std::string& _title) {
	for (auto g : m_groups) {
		if (g->name() == _name) { otAssert(0, "A group with tha same name already exists"); return nullptr; }
	}
	ot::SettingsGroup * newGroup = new ot::SettingsGroup{ _name, _title };
	m_groups.push_back(newGroup);
	newGroup->m_isAttached = true;
	return newGroup;
}

void ot::SettingsData::addGroup(SettingsGroup * _group) {
	if (_group == nullptr) { otAssert(0, "NULL as group provided"); return; }
	if (_group->isAttached()) { otAssert(0, "Group is already attached to a dataset"); return; }
	for (auto g : m_groups) {
		if (g->name() == _group->name()) { otAssert(0, "A group with tha same name already exists"); return; }
	}
	m_groups.push_back(_group);
	_group->m_isAttached = true;
}

bool ot::SettingsData::saveToDatabase(const std::string& _databaseURL, const std::string& _siteID, const std::string &_userName, const std::string &_userPassword, const std::string& _userCollection) {

	assert(!_databaseURL.empty());
	assert(!_siteID.empty());

	std::string settingName =   dataName();
	std::string settingString = toJson();

	try
	{
		// Ensure that we are connected to the database server
		DataStorageAPI::ConnectionAPI::establishConnection(_databaseURL, _siteID, _userName, _userPassword);

		// First, open a connection to the user's settings collection
		DataStorageAPI::DocumentAccess docManager("UserSettings", _userCollection);

		// Now we search for the document with the given name
		auto queryDoc = bsoncxx::builder::basic::document{};
		queryDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingName));

		auto filterDoc = bsoncxx::builder::basic::document{};

		auto result = docManager.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));

		if (!result.getSuccess())
		{
			// The setting does not yet exist -> write a new one
			auto newDoc = bsoncxx::builder::basic::document{};
			newDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingName));
			newDoc.append(bsoncxx::builder::basic::kvp("Data", settingString));

			docManager.InsertDocumentToDatabase(newDoc.extract(), false);
		}
		else
		{
			// The setting already exists -> replace the settings
			try
			{
				// Find the entry corresponding to the project in the collection
				auto doc_find = bsoncxx::builder::stream::document{} << "SettingName" << settingName << bsoncxx::builder::stream::finalize;

				auto doc_modify = bsoncxx::builder::stream::document{}
					<< "$set" << bsoncxx::builder::stream::open_document
					<< "Data" << settingString
					<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

				mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("UserSettings", _userCollection);

				bsoncxx::stdx::optional<mongocxx::result::update> result = collection.update_many(doc_find.view(), doc_modify.view());
			}
			catch (std::exception)
			{
				assert(0); // Could not update the documents
				return false;
			}
		}
	}
	catch (std::exception)
	{
		assert(0); // Something went wrong accessing the settings data
		return false;
	}

	return true;  // Successfully stored the settings
}

bool ot::SettingsData::refreshValuesFromDatabase(const std::string& _databaseURL, const std::string& _siteID, const std::string &_userName, const std::string &_userPassword, const std::string& _userCollection) {

	assert(!_databaseURL.empty());
	assert(!_siteID.empty());

	std::string settingName = dataName();
	std::string settingString;

	try
	{
		// Ensure that we are connected to the database server
		DataStorageAPI::ConnectionAPI::establishConnection(_databaseURL, _siteID, _userName, _userPassword);

		// First, open a connection to the user's settings collection
		DataStorageAPI::DocumentAccess docManager("UserSettings", _userCollection);

		// Now we search for the document with the given name
		auto queryDoc = bsoncxx::builder::basic::document{};
		queryDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingName));

		auto filterDoc = bsoncxx::builder::basic::document{};

		auto result = docManager.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));

		if (!result.getSuccess())
		{
			return true;  // We could not find the document, but this is a standard case when the settings have not yet been stored
		}

		// Now we have found some settings, so retrieve the data
		try
		{
			bsoncxx::builder::basic::document doc;
			doc.append(bsoncxx::builder::basic::kvp("Found", result.getBsonResult().value()));

			auto doc_view = doc.view()["Found"].get_document().view();

			settingString = doc_view["Data"].get_utf8().value.data();
		}
		catch (std::exception)
		{
			// Something went wrong with accessing the settings data
			assert(0);
			return false;
		}

		// Create new settings from json string
		OT_rJSON_parseDOC(importedSettings, settingString.c_str());
		SettingsData * restoredSettings = SettingsData::parseFromJsonDocument(importedSettings);

		// Get all current items
		std::list<AbstractSettingsItem *> currentItems = items();

		for (auto itm : currentItems) {
			// Check if the restored settings have the same item
			AbstractSettingsItem * entry = restoredSettings->findItemByLogicalName(itm->logicalName());
			if (entry) {
				if (entry->type() == itm->type()) {
					itm->setDescription(entry->description());
					itm->setTitle(entry->title());
					itm->setVisible(entry->isVisible());
					itm->copyValuesFromOtherItem(entry);
				}
			}
		}
		delete restoredSettings;
	}
	catch (std::exception)
	{
		assert(0); // Something went wrong accessing the settings data
		return false;
	}

	return true;  // Changed from false to true to avoid display of error messages as long as this function is not implemented
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsGroup::SettingsGroup(const std::string& _name, const std::string& _title)
	: m_name(_name), m_title(_title), m_parent(nullptr), m_isAttached(false) {}

ot::SettingsGroup::~SettingsGroup() {
	for (auto subgroup : m_subgroups) {
		delete subgroup;
	}
	for (auto itm : m_items) {
		delete itm;
	}
}

// ##############################################################

// Setter

void ot::SettingsGroup::addSubgroup(SettingsGroup * _group) {
	if (_group == nullptr) { otAssert(0, "NULL as group provided"); return; }
	if (_group->isAttached()) { otAssert(0, "Group is already attached to a group or dataset"); return; }
	for (auto sg : m_subgroups) {
		if (sg->name() == _group->name()) { otAssert(0, "A subgroup with tha same name already exists"); return; }
	}
	m_subgroups.push_back(_group);
	_group->setParent(this);
}

ot::SettingsGroup * ot::SettingsGroup::addSubgroup(const std::string& _name, const std::string& _title) {
	for (auto sg : m_subgroups) { 
		if (sg->name() == _name) { otAssert(0, "A subgroup with tha same name already exists"); return nullptr; }
	}
	SettingsGroup * newGroup = new SettingsGroup(_name, _title);
	addSubgroup(newGroup);
	return newGroup;
}

void ot::SettingsGroup::addItem(AbstractSettingsItem * _item) {
	if (_item == nullptr) { otAssert(0, "NULL as item provided"); return; }
	if (_item->isAttached()) { otAssert(0, "Item is already attached to a group"); return; }
	for (auto itm : m_items) {
		if (itm->name() == _item->name()) { otAssert(0, "A item with tha same name already exists"); return; }
	}
	m_items.push_back(_item);
	_item->setParent(this);
}

// ##############################################################

// Getter 

void ot::SettingsGroup::addToJsonArray(rapidjson::Document& _document, rapidjson::Value& _array) const {
	OT_rJSON_createValueObject(obj);
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Name, m_name);
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Title, m_title);

	OT_rJSON_createValueArray(subgroupsData);
	for (auto subgroup : m_subgroups) {
		subgroup->addToJsonArray(_document, subgroupsData);
	}
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Groups, subgroupsData);

	OT_rJSON_createValueArray(itemsData);
	for (auto item : m_items) {
		item->addToJsonArray(_document, itemsData);
	}
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Items, itemsData);

	_array.PushBack(obj, _document.GetAllocator());
}

std::string ot::SettingsGroup::logicalName(const std::string& _delimiter) {
	std::string ret;
	if (m_parent) { ret = m_parent->logicalName(_delimiter).append(_delimiter); }
	ret.append(m_name);
	return ret;
}

ot::SettingsGroup * ot::SettingsGroup::findGroupByLogicalName(std::list<std::string>& _logicalName) {
	if (_logicalName.empty()) { return nullptr; }
	for (auto g : m_subgroups) {
		if (g->name() == _logicalName.front()) {
			if (_logicalName.size() == 1) {
				return g;
			}
			else {
				_logicalName.pop_front();
				return g->findGroupByLogicalName(_logicalName);
			}
		}
	}
	return nullptr;
}

ot::AbstractSettingsItem * ot::SettingsGroup::findItemByLogicalName(std::list<std::string>& _logicalName) {
	if (_logicalName.empty()) { return nullptr; }
	if (_logicalName.size() > 1) {
		for (auto g : m_subgroups) {
			if (g->name() == _logicalName.front()) {
				_logicalName.pop_front();
				return g->findItemByLogicalName(_logicalName);
			}
		}
	}
	else {
		for (auto itm : m_items) {
			if (itm->name() == _logicalName.front()) {
				return itm;
			}
		}
	}
	return nullptr;
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::AbstractSettingsItem::AbstractSettingsItem(itemType _type, const std::string& _name, const std::string& _title, const std::string& _description)
	: m_type(_type), m_name(_name), m_title(_title), m_description(_description), m_parent(nullptr), m_isVisible(true) {}

ot::AbstractSettingsItem::~AbstractSettingsItem() {}

// ##############################################################

// Getter

std::string ot::AbstractSettingsItem::typeToString(void) const {
	return typeToString(m_type);
}

std::string ot::AbstractSettingsItem::typeToString(itemType _type) {
	switch (_type)
	{
	case ot::AbstractSettingsItem::InfoText: return "InfoText";
	case ot::AbstractSettingsItem::String: return "StringLine";
	case ot::AbstractSettingsItem::Integer: return "Integer";
	case ot::AbstractSettingsItem::Double: return "Double";
	case ot::AbstractSettingsItem::Boolean: return "Boolean";
	case ot::AbstractSettingsItem::Color: return "Color";
	case ot::AbstractSettingsItem::Selection: return "Selection";
	case ot::AbstractSettingsItem::EditableSelection: return "EditableSelection";
	case ot::AbstractSettingsItem::ListSelection: return "ListSelection";
	case ot::AbstractSettingsItem::DirectorySelect: return "DirectorySelect";
	case ot::AbstractSettingsItem::FileSelectSave: return "FileSelectSave";
	case ot::AbstractSettingsItem::FileSelectOpen: return "FileSelectOpen";
	default:
		otAssert(0, "Unknown type");
		return "UNKNOWN";
	}
}

std::list<std::string> ot::AbstractSettingsItem::parentGroupsList(void) const {
	std::list<std::string> parents;
	ot::SettingsGroup * parent = m_parent;
	while (parent) {
		parents.push_front(parent->name());
		parent = parent->parentGroup();
	}
	return parents;
}

std::string ot::AbstractSettingsItem::logicalName(const std::string& _delimiter) {
	std::string ret;
	if (m_parent) { ret = m_parent->logicalName(_delimiter).append(_delimiter); }
	ret.append(m_name);
	return ret;
}

// ##############################################################

// Protected function

void ot::AbstractSettingsItem::addToJsonArray(rapidjson::Document& _document, rapidjson::Value& _array) const {
	OT_rJSON_createValueObject(obj);
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Type, typeToString());
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Name, m_name);
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Title, m_title);
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_IsVisible, m_isVisible);
	ot::rJSON::add(_document, obj, OT_ACTION_PARAM_SETTINGS_Description, m_description);
	addItemDataToJsonObject(_document, obj);
	_array.PushBack(obj, _document.GetAllocator());
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemInfoText::SettingsItemInfoText(const std::string& _name, const std::string& _title, const std::string& _description)
	: AbstractSettingsItem(InfoText, _name, _title, _description) {}

ot::SettingsItemInfoText::~SettingsItemInfoText() {}

ot::AbstractSettingsItem * ot::SettingsItemInfoText::createCopy(void) const {
	return new SettingsItemInfoText(m_name, m_title);
}

void ot::SettingsItemInfoText::copyValuesFromOtherItem(AbstractSettingsItem * _other) {}

void ot::SettingsItemInfoText::setText(const std::string& _text) { m_title = _text; }

// ##############################################################

// Getter

std::string ot::SettingsItemInfoText::value(void) const { return m_title; }

void ot::SettingsItemInfoText::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemBoolean::SettingsItemBoolean(const std::string& _name, const std::string& _title, bool _value, const std::string& _description)
	: AbstractSettingsItem(Boolean, _name, _title, _description), m_value(_value)
{}

ot::SettingsItemBoolean::~SettingsItemBoolean() {}

ot::AbstractSettingsItem * ot::SettingsItemBoolean::createCopy(void) const {
	SettingsItemBoolean * copy = new SettingsItemBoolean(m_name, m_title, m_value);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemBoolean::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemBoolean * actualOther = dynamic_cast<SettingsItemBoolean *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_value = actualOther->value();
}

// ##############################################################

// Getter

void ot::SettingsItemBoolean::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_value);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemString::SettingsItemString(const std::string& _name, const std::string& _title, const std::string& _value, const std::string& _inputHint, const std::string& _description)
	: AbstractSettingsItem(String, _name, _title, _description), m_value(_value), m_inputHint(_inputHint)
{}

ot::SettingsItemString::~SettingsItemString() {}

ot::AbstractSettingsItem * ot::SettingsItemString::createCopy(void) const {
	SettingsItemString * copy = new SettingsItemString(m_name, m_title, m_value);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemString::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemString * actualOther = dynamic_cast<SettingsItemString *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_value = actualOther->value();
	m_inputHint = actualOther->inputHint();
}

// ##############################################################

// Getter

void ot::SettingsItemString::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_value);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_InputHint, m_inputHint);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemInteger::SettingsItemInteger(const std::string& _name, const std::string& _title, int _value, int _minValue, int _maxValue, const std::string& _description)
	: AbstractSettingsItem(Integer, _name, _title, _description), m_value(_value), m_minValue(_minValue), m_maxValue(_maxValue)
{ checkLimits(); }

ot::SettingsItemInteger::~SettingsItemInteger() {}

ot::AbstractSettingsItem * ot::SettingsItemInteger::createCopy(void) const {
	SettingsItemInteger * copy = new SettingsItemInteger(m_name, m_title, m_value, m_minValue, m_maxValue);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemInteger::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemInteger * actualOther = dynamic_cast<SettingsItemInteger *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_value = actualOther->value();
	m_minValue = actualOther->minValue();
	m_maxValue = actualOther->maxValue();
}

void ot::SettingsItemInteger::setValue(int _value) {
	m_value = _value;
	checkLimits();
}

void ot::SettingsItemInteger::setMinValue(int _min) {
	m_minValue = _min;
	checkLimits();
}

void ot::SettingsItemInteger::setMaxValue(int _max) {
	m_maxValue = _max;
	checkLimits();
}

void ot::SettingsItemInteger::setLimits(int _min, int _max) {
	m_minValue = _min;
	m_maxValue = _max;
	checkLimits();
}

void ot::SettingsItemInteger::setMaxLimits(void) {
	m_minValue = INT_MIN;
	m_maxValue = INT_MAX;
	checkLimits();
}

// ##############################################################

// Protected functions

void ot::SettingsItemInteger::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_value);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueMin, m_minValue);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueMax, m_maxValue);
}

// ##############################################################

// Private functions

void ot::SettingsItemInteger::checkLimits(void) {
	if (m_minValue > m_maxValue) {
		otAssert(0, "Min value is smaller than max value");
		m_minValue = m_maxValue;
	}
	if (m_value < m_minValue) {
		otAssert(0, "Value is smaller than min value");
		m_value = m_minValue;
	}
	if (m_value > m_maxValue) {
		otAssert(0, "Value is greater than max value");
		m_value = m_maxValue;
	}
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemDouble::SettingsItemDouble(const std::string& _name, const std::string& _title, double _value, double _minValue, double _maxValue, int _decimals, const std::string& _description)
	: AbstractSettingsItem(Double, _name, _title, _description), m_value(_value), m_minValue(_minValue), m_maxValue(_maxValue), m_decimals(_decimals)
{ checkLimits(); }

ot::SettingsItemDouble::~SettingsItemDouble() {}

ot::AbstractSettingsItem * ot::SettingsItemDouble::createCopy(void) const {
	SettingsItemDouble * copy = new SettingsItemDouble(m_name, m_title, m_value, m_minValue, m_maxValue, m_decimals);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemDouble::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemDouble * actualOther = dynamic_cast<SettingsItemDouble *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_value = actualOther->value();
	m_minValue = actualOther->minValue();
	m_maxValue = actualOther->maxValue();
	m_decimals = actualOther->decimalPlaces();
}

void ot::SettingsItemDouble::setValue(int _value) {
	m_value = _value;
	checkLimits();
}

void ot::SettingsItemDouble::setMinValue(int _min) {
	m_minValue = _min;
	checkLimits();
}

void ot::SettingsItemDouble::setMaxValue(int _max) {
	m_maxValue = _max;
	checkLimits();
}

void ot::SettingsItemDouble::setLimits(int _min, int _max) {
	m_minValue = _min;
	m_maxValue = _max;
	checkLimits();
}

void ot::SettingsItemDouble::setMaxLimits(void) {
	m_minValue = DBL_MIN;
	m_maxValue = DBL_MAX;
	checkLimits();
}

// ##############################################################

// Protected functions

void ot::SettingsItemDouble::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_value);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueMin, m_minValue);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueMax, m_maxValue);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Decimals, m_decimals);
}

// ##############################################################

// Private functions

void ot::SettingsItemDouble::checkLimits(void) {
	if (m_minValue > m_maxValue) {
		otAssert(0, "Min value is smaller than max value");
		m_minValue = m_maxValue;
	}
	if (m_value < m_minValue) {
		otAssert(0, "Value is smaller than min value");
		m_value = m_minValue;
	}
	if (m_value > m_maxValue) {
		otAssert(0, "Value is greater than max value");
		m_value = m_maxValue;
	}
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemSelection::SettingsItemSelection(const std::string& _name, const std::string& _title, const std::list<std::string>& _possibleSelection, const std::string& _selectedValue, const std::string& _description)
	: AbstractSettingsItem(Selection, _name, _title, _description), m_possibleSelection(_possibleSelection), m_selectedValue(_selectedValue)
{}

ot::SettingsItemSelection::~SettingsItemSelection() {}

ot::AbstractSettingsItem * ot::SettingsItemSelection::createCopy(void) const {
	SettingsItemSelection * copy = new SettingsItemSelection(m_name, m_title, m_possibleSelection, m_selectedValue);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemSelection::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemSelection * actualOther = dynamic_cast<SettingsItemSelection *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_possibleSelection = actualOther->possibleSelection();
	m_selectedValue = actualOther->selectedValue();
}

void ot::SettingsItemSelection::addPossibleSelection(const std::string& _possibleSelection) {
	m_possibleSelection.push_back(_possibleSelection);
}

void ot::SettingsItemSelection::addPossibleSelection(const std::list<std::string>&  _possibleSelection) {
	for (auto itm : _possibleSelection) {
		m_possibleSelection.push_back(itm);
	}
}

void ot::SettingsItemSelection::setPossibleSelection(const std::list<std::string>& _possibleSelection) {
	m_possibleSelection = _possibleSelection;
}

void ot::SettingsItemSelection::setSelectedValue(const std::string& _selectedValue) {
	m_selectedValue = _selectedValue;
}

// ##############################################################

// Getter

void ot::SettingsItemSelection::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_PossibleValue, m_possibleSelection);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_selectedValue);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemEditableSelection::SettingsItemEditableSelection(const std::string& _name, const std::string& _title, const std::list<std::string>& _possibleSelection, const std::string& _selectedValue, const std::string& _inputHint, const std::string& _description)
	: AbstractSettingsItem(EditableSelection, _name, _title, _description), m_possibleSelection(_possibleSelection), m_selectedValue(_selectedValue),
	m_inputHint(_inputHint)
{}

ot::SettingsItemEditableSelection::~SettingsItemEditableSelection() {}

ot::AbstractSettingsItem * ot::SettingsItemEditableSelection::createCopy(void) const {
	SettingsItemEditableSelection * copy = new SettingsItemEditableSelection(m_name, m_title, m_possibleSelection, m_selectedValue, m_inputHint);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemEditableSelection::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemEditableSelection * actualOther = dynamic_cast<SettingsItemEditableSelection *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_possibleSelection = actualOther->possibleSelection();
	m_selectedValue = actualOther->selectedValue();
	m_inputHint = actualOther->inputHint();
}

void ot::SettingsItemEditableSelection::addPossibleSelection(const std::string& _possibleSelection) {
	m_possibleSelection.push_back(_possibleSelection);
}

void ot::SettingsItemEditableSelection::addPossibleSelection(const std::list<std::string>&  _possibleSelection) {
	for (auto itm : _possibleSelection) {
		m_possibleSelection.push_back(itm);
	}
}

void ot::SettingsItemEditableSelection::setPossibleSelection(const std::list<std::string>& _possibleSelection) {
	m_possibleSelection = _possibleSelection;
}

void ot::SettingsItemEditableSelection::setSelectedValue(const std::string& _selectedValue) {
	m_selectedValue = _selectedValue;
}

// ##############################################################

// Getter

void ot::SettingsItemEditableSelection::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_PossibleValue, m_possibleSelection);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_selectedValue);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_InputHint, m_inputHint);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemListSelection::SettingsItemListSelection(const std::string& _name, const std::string& _title, const std::list<std::string>& _possibleSelection, const std::list<std::string>& _selectedValues, const std::string& _description)
	: AbstractSettingsItem(ListSelection, _name, _title, _description), m_possibleSelection(_possibleSelection), m_selectedValues(_selectedValues)
{}

ot::SettingsItemListSelection::~SettingsItemListSelection() {}

ot::AbstractSettingsItem * ot::SettingsItemListSelection::createCopy(void) const {
	SettingsItemListSelection * copy = new SettingsItemListSelection(m_name, m_title, m_possibleSelection, m_selectedValues);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemListSelection::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemListSelection * actualOther = dynamic_cast<SettingsItemListSelection *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_possibleSelection = actualOther->possibleSelection();
	m_selectedValues = actualOther->selectedValues();
}

void ot::SettingsItemListSelection::addPossibleSelection(const std::string& _possibleSelection) {
	m_possibleSelection.push_back(_possibleSelection);
}

void ot::SettingsItemListSelection::addPossibleSelection(const std::list<std::string>&  _possibleSelection) {
	for (auto itm : _possibleSelection) {
		m_possibleSelection.push_back(itm);
	}
}

void ot::SettingsItemListSelection::setPossibleSelection(const std::list<std::string>& _possibleSelection) {
	m_possibleSelection = _possibleSelection;
}

void ot::SettingsItemListSelection::addSelectedValue(const std::string& _selectedValue) {
	m_selectedValues.push_back(_selectedValue);
}

void ot::SettingsItemListSelection::addSelectedValues(const std::list<std::string>& _selectedValues) {
	for (auto itm : _selectedValues) { m_selectedValues.push_back(itm); }
}

void ot::SettingsItemListSelection::setSelectedValues(const std::list<std::string>& _selectedValues) {
	m_selectedValues = _selectedValues;
}

// ##############################################################

// Getter

void ot::SettingsItemListSelection::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_PossibleValue, m_possibleSelection);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_selectedValues);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemColor::SettingsItemColor(const std::string& _name, const std::string& _title, const ot::Color& _color, const std::string& _description)
	: AbstractSettingsItem(Color, _name, _title, _description), m_color(_color) {}

ot::SettingsItemColor::~SettingsItemColor() {}

ot::AbstractSettingsItem * ot::SettingsItemColor::createCopy(void) const {
	SettingsItemColor * copy = new SettingsItemColor(m_name, m_title, m_color);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemColor::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemColor * actualOther = dynamic_cast<SettingsItemColor *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_color = actualOther->value();
}

void ot::SettingsItemColor::setValue(int _r, int _g, int _b, int _a) { m_color.set(_r, _g, _b, _a); }

void ot::SettingsItemColor::setValue(float _r, float _g, float _b, float _a) { m_color.set(_r, _g, _b, _a); }

// ##############################################################

// Getter

void ot::SettingsItemColor::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueA, m_color.aInt());
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueR, m_color.rInt());
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueG, m_color.gInt());
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_ValueB, m_color.bInt());
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemDirectorySelect::SettingsItemDirectorySelect(const std::string& _name, const std::string& _title, const std::string& _directory, const std::string& _inputHint, const std::string& _description)
	: AbstractSettingsItem(DirectorySelect, _name, _title, _description), m_directory(_directory), m_inputHint(_inputHint) {}

ot::SettingsItemDirectorySelect::~SettingsItemDirectorySelect() {}

ot::AbstractSettingsItem * ot::SettingsItemDirectorySelect::createCopy(void) const {
	SettingsItemDirectorySelect * copy = new SettingsItemDirectorySelect(m_name, m_title, m_directory, m_inputHint);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemDirectorySelect::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemDirectorySelect * actualOther = dynamic_cast<SettingsItemDirectorySelect *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_directory = actualOther->directory();
	m_inputHint = actualOther->inputHint();
}

// ##############################################################

// Getter

void ot::SettingsItemDirectorySelect::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_directory);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_InputHint, m_inputHint);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemFileSelectOpen::SettingsItemFileSelectOpen(const std::string& _name, const std::string& _title, const std::string& _file, const std::string& _filter, const std::string& _inputHint, const std::string& _description)
	: AbstractSettingsItem(FileSelectOpen, _name, _title, _description), m_file(_file), m_filter(_filter), m_inputHint(_inputHint) {}

ot::SettingsItemFileSelectOpen::~SettingsItemFileSelectOpen() {}

ot::AbstractSettingsItem * ot::SettingsItemFileSelectOpen::createCopy(void) const {
	SettingsItemFileSelectOpen * copy = new SettingsItemFileSelectOpen(m_name, m_title, m_file, m_filter, m_inputHint);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemFileSelectOpen::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemFileSelectOpen * actualOther = dynamic_cast<SettingsItemFileSelectOpen *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_file = actualOther->file();
	m_filter = actualOther->filter();
	m_inputHint = actualOther->inputHint();
}

// ##############################################################

// Protected functions

void ot::SettingsItemFileSelectOpen::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_file);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Filter, m_filter);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_InputHint, m_inputHint);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::SettingsItemFileSelectSave::SettingsItemFileSelectSave(const std::string& _name, const std::string& _title, const std::string& _file, const std::string& _filter, const std::string& _inputHint, const std::string& _description)
	: AbstractSettingsItem(FileSelectSave, _name, _title, _description), m_file(_file), m_filter(_filter), m_inputHint(_inputHint) {}

ot::SettingsItemFileSelectSave::~SettingsItemFileSelectSave() {}

ot::AbstractSettingsItem * ot::SettingsItemFileSelectSave::createCopy(void) const {
	SettingsItemFileSelectSave * copy = new SettingsItemFileSelectSave(m_name, m_title, m_file, m_filter, m_inputHint);
	copy->setVisible(isVisible());
	return copy;
}

void ot::SettingsItemFileSelectSave::copyValuesFromOtherItem(AbstractSettingsItem * _other) {
	SettingsItemFileSelectSave * actualOther = dynamic_cast<SettingsItemFileSelectSave *>(_other);
	if (actualOther == nullptr) {
		otAssert(0, "Item cast failed");
		return;
	}
	m_file = actualOther->file();
	m_filter = actualOther->filter();
	m_inputHint = actualOther->inputHint();
}

// ##############################################################

// Protected functions

void ot::SettingsItemFileSelectSave::addItemDataToJsonObject(rapidjson::Document& _document, rapidjson::Value& _object) const {
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Value, m_file);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_Filter, m_filter);
	ot::rJSON::add(_document, _object, OT_ACTION_PARAM_SETTINGS_InputHint, m_inputHint);
}

// ############################################################################################

// ############################################################################################

// ############################################################################################

ot::AbstractSettingsItem * ot::SettingsItemFactory::createItem(rapidjson::Value& _jsonObject) {
	// Check for missing members
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Type)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Type "\" is missing");
		return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Name)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Name "\" is missing");
		return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Title)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Title "\" is missing");
		return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_IsVisible)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_IsVisible "\" is missing");
		return nullptr;
	}

	// Check member types
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Type].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Type "\" is not a string");
		return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Name].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Name "\" is not a string");
		return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Title].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Title "\" is not a string");
		return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_IsVisible].IsBool()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_IsVisible "\" is not a bool");
		return nullptr;
	}

	// Get type and name
	std::string itmName = _jsonObject[OT_ACTION_PARAM_SETTINGS_Name].GetString();
	std::string itmTitle = _jsonObject[OT_ACTION_PARAM_SETTINGS_Title].GetString();
	std::string itmType = _jsonObject[OT_ACTION_PARAM_SETTINGS_Type].GetString();
	std::string itmDescription;
	bool isVisible = _jsonObject[OT_ACTION_PARAM_SETTINGS_IsVisible].GetBool();
	
	if (_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Description)) {
		if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Description].IsString()) {
			otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Description "\" is not a string");
			return nullptr;
		}
		itmDescription = _jsonObject[OT_ACTION_PARAM_SETTINGS_Description].GetString();
	}

	if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::InfoText)) {
		return createInfoTextItem(itmName, itmTitle, itmDescription, isVisible);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::Boolean)) {
		return createBooleanItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::String)) {
		return createStringItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::Integer)) {
		return createIntegerItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::Double)) {
		return createDoubleItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::Selection)) {
		return createSelectionItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::EditableSelection)) {
		return createEditableSelectionItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::ListSelection)) {
		return createListSelectionItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::Color)) {
		return createColorItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::DirectorySelect)) {
		return createDirectorySelectItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::FileSelectOpen)) {
		return createFileSelectOpenItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else if (itmType == AbstractSettingsItem::typeToString(AbstractSettingsItem::FileSelectSave)) {
		return createFileSelectSaveItem(itmName, itmTitle, itmDescription, isVisible, _jsonObject);
	}
	else {
		otAssert(0, "Unknown settings type");
		return nullptr;
	}
}

ot::SettingsItemInfoText * ot::SettingsItemFactory::createInfoTextItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible) {
	SettingsItemInfoText * newItem = new SettingsItemInfoText(_name, _title, _description);
	newItem->setVisible(_isVisible);
	newItem->setDescription(_description);
	return newItem;
}

ot::SettingsItemBoolean * ot::SettingsItemFactory::createBooleanItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _value) {
	return new SettingsItemBoolean(_name, _title, _value, _description);
}

ot::SettingsItemBoolean * ot::SettingsItemFactory::createBooleanItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsBool()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a boolean"); return nullptr;
	}
	bool itmValue = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetBool();
	SettingsItemBoolean * newItem = createBooleanItem(_name, _title, _description, itmValue);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemString * ot::SettingsItemFactory::createStringItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _value, const std::string& _inputHint) {
	return new SettingsItemString(_name, _title, _value, _inputHint, _description);
}

ot::SettingsItemString * ot::SettingsItemFactory::createStringItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_InputHint)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a string"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is not a string"); return nullptr;
	}
	std::string itmValue = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetString();
	std::string inputHint = _jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].GetString();
	SettingsItemString * newItem = createStringItem(_name, _title, _description, itmValue, inputHint);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemInteger * ot::SettingsItemFactory::createIntegerItem(const std::string& _name, const std::string& _title, const std::string& _description, int _value, int _minValue, int _maxValue) {
	return new SettingsItemInteger(_name, _title, _value, _minValue, _maxValue, _description);
}

ot::SettingsItemInteger * ot::SettingsItemFactory::createIntegerItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueMin)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMin "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueMax)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMax "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a integer"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMin].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMin "\" is not a integer"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMax].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMax "\" is not a integer"); return nullptr;
	}
	int itmValue = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetInt();
	int itmValueMin = _jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMin].GetInt();
	int itmValueMax = _jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMax].GetInt();
	SettingsItemInteger * newItem = createIntegerItem(_name, _title, _description, itmValue, itmValueMin, itmValueMax);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemDouble * ot::SettingsItemFactory::createDoubleItem(const std::string& _name, const std::string& _title, const std::string& _description, double _value, double _minValue, double _maxValue, int _decimals) {
	return new SettingsItemDouble(_name, _title, _value, _minValue, _maxValue, _decimals, _description);
}

ot::SettingsItemDouble * ot::SettingsItemFactory::createDoubleItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueMin)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMin "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueMax)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMax "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Decimals)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Decimals "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsDouble()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a double"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMin].IsDouble()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMin "\" is not a double"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMax].IsDouble()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueMax "\" is not a double"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Decimals].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Decimals "\" is not a integer"); return nullptr;
	}
	double itmValue = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetDouble();
	double itmValueMin = _jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMin].GetDouble();
	double itmValueMax = _jsonObject[OT_ACTION_PARAM_SETTINGS_ValueMax].GetDouble();
	int itmDecimals = _jsonObject[OT_ACTION_PARAM_SETTINGS_Decimals].GetInt();
	SettingsItemDouble * newItem = createDoubleItem(_name, _title, _description, itmValue, itmValueMin, itmValueMax, itmDecimals);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemSelection * ot::SettingsItemFactory::createSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::list<std::string>& _possibleSelection, const std::string& _selectedValue) {
	return new SettingsItemSelection(_name, _title, _possibleSelection, _selectedValue, _description);
}

ot::SettingsItemSelection * ot::SettingsItemFactory::createSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_PossibleValue)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_PossibleValue "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a string"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_PossibleValue].IsArray()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_PossibleValue "\" is not a string"); return nullptr;
	}
	std::string itmValue = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetString();
	std::list<std::string> possibleSelection = ot::rJSON::getStringList(_jsonObject, OT_ACTION_PARAM_SETTINGS_PossibleValue);
	SettingsItemSelection * newItem = createSelectionItem(_name, _title, _description, possibleSelection, itmValue);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemEditableSelection * ot::SettingsItemFactory::createEditableSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::list<std::string>& _possibleSelection, const std::string& _selectedValue, const std::string& _inputHint) {
	return new SettingsItemEditableSelection(_name, _title, _possibleSelection, _selectedValue, _inputHint, _description);
}

ot::SettingsItemEditableSelection * ot::SettingsItemFactory::createEditableSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_PossibleValue)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_PossibleValue "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_InputHint)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a string"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_PossibleValue].IsArray()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_PossibleValue "\" is not a array"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is not a string"); return nullptr;
	}
	std::string itmValue = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetString();
	std::string inputHint = _jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].GetString();
	std::list<std::string> possibleSelection = ot::rJSON::getStringList(_jsonObject, OT_ACTION_PARAM_SETTINGS_PossibleValue);
	SettingsItemEditableSelection * newItem = createEditableSelectionItem(_name, _title, _description, possibleSelection, itmValue, inputHint);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemListSelection * ot::SettingsItemFactory::createListSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::list<std::string>& _possibleSelection, const std::list<std::string>& _selectedValues) {
	return new SettingsItemListSelection(_name, _title, _possibleSelection, _selectedValues, _description);
}

ot::SettingsItemListSelection * ot::SettingsItemFactory::createListSelectionItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_PossibleValue)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_PossibleValue "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsArray()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a string"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_PossibleValue].IsArray()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_PossibleValue "\" is not a string"); return nullptr;
	}
	std::list<std::string> itmValue = ot::rJSON::getStringList(_jsonObject, OT_ACTION_PARAM_SETTINGS_Value);
	std::list<std::string> possibleSelection = ot::rJSON::getStringList(_jsonObject, OT_ACTION_PARAM_SETTINGS_PossibleValue);
	SettingsItemListSelection * newItem = createListSelectionItem(_name, _title, _description, possibleSelection, itmValue);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemColor * ot::SettingsItemFactory::createColorItem(const std::string& _name, const std::string& _title, const std::string& _description, const ot::Color& _color) {
	return new SettingsItemColor(_name, _title, _color, _description);
}

ot::SettingsItemColor * ot::SettingsItemFactory::createColorItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueA)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueA "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueR)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueR "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueG)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueG "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_ValueB)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueB "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueA].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueA "\" is not a integer"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueR].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueR "\" is not a integer"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueG].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueG "\" is not a integer"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueB].IsInt()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_ValueB "\" is not a integer"); return nullptr;
	}
	SettingsItemColor * newItem = new SettingsItemColor(_name, _title,
		ot::Color(_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueR].GetInt(), _jsonObject[OT_ACTION_PARAM_SETTINGS_ValueG].GetInt(),
			_jsonObject[OT_ACTION_PARAM_SETTINGS_ValueB].GetInt(), _jsonObject[OT_ACTION_PARAM_SETTINGS_ValueA].GetInt()), _description);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemDirectorySelect * ot::SettingsItemFactory::createDirectorySelectItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _directory, const std::string& _inputHint) {
	return new SettingsItemDirectorySelect(_name, _title, _directory, _inputHint, _description);
}

ot::SettingsItemDirectorySelect * ot::SettingsItemFactory::createDirectorySelectItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_InputHint)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a string"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is not a string"); return nullptr;
	}
	std::string dir = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetString();
	std::string inputHint = _jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].GetString();
	SettingsItemDirectorySelect * newItem = new SettingsItemDirectorySelect(_name, _title, dir, inputHint, _description);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemFileSelectOpen * ot::SettingsItemFactory::createFileSelectOpenItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _file, const std::string& _inputHint) {
	return new SettingsItemFileSelectOpen(_name, _title, _file, _inputHint, _description);
}

ot::SettingsItemFileSelectOpen * ot::SettingsItemFactory::createFileSelectOpenItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_InputHint)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a string"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is not a string"); return nullptr;
	}
	std::string file = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetString();
	std::string inputHint = _jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].GetString();
	SettingsItemFileSelectOpen * newItem = new SettingsItemFileSelectOpen(_name, _title, file, inputHint, _description);
	newItem->setVisible(_isVisible);
	return newItem;
}

ot::SettingsItemFileSelectSave * ot::SettingsItemFactory::createFileSelectSaveItem(const std::string& _name, const std::string& _title, const std::string& _description, const std::string& _file, const std::string& _inputHint) {
	return new SettingsItemFileSelectSave(_name, _title, _file, _inputHint, _description);
}

ot::SettingsItemFileSelectSave * ot::SettingsItemFactory::createFileSelectSaveItem(const std::string& _name, const std::string& _title, const std::string& _description, bool _isVisible, rapidjson::Value& _jsonObject) {
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_Value)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is missing"); return nullptr;
	}
	if (!_jsonObject.HasMember(OT_ACTION_PARAM_SETTINGS_InputHint)) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is missing"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_Value].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_Value "\" is not a string"); return nullptr;
	}
	if (!_jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].IsString()) {
		otAssert(0, "Settings item member \"" OT_ACTION_PARAM_SETTINGS_InputHint "\" is not a string"); return nullptr;
	}
	std::string file = _jsonObject[OT_ACTION_PARAM_SETTINGS_Value].GetString();
	std::string inputHint = _jsonObject[OT_ACTION_PARAM_SETTINGS_InputHint].GetString();
	SettingsItemFileSelectSave * newItem = new SettingsItemFileSelectSave(_name, _title, file, inputHint, _description);
	newItem->setVisible(_isVisible);
	return newItem;
}
