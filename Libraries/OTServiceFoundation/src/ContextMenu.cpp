#include "OTCore/OTAssert.h"
#include "OTServiceFoundation/ContextMenu.h"
#include "OTServiceFoundation/TypeConverter.h"

ot::ContextMenuItemRole::ContextMenuItemRole() : m_role(NoRole) {}

ot::ContextMenuItemRole::ContextMenuItemRole(itemRole _role, const std::string& _roleText)
	: m_role(_role), m_roleText(_roleText) {}

ot::ContextMenuItemRole::ContextMenuItemRole(const ContextMenuItemRole& _other)
	: m_role(_other.m_role), m_roleText(_other.m_roleText) {}

ot::ContextMenuItemRole& ot::ContextMenuItemRole::operator = (const ContextMenuItemRole& _other) {
	m_role = _other.m_role;
	m_roleText = _other.m_roleText;
	return *this;
}

ot::ContextMenuItemRole::~ContextMenuItemRole() {}

std::string ot::ContextMenuItemRole::roleToString(void) const {
	return roleToString(m_role);
}

std::string ot::ContextMenuItemRole::roleToString(itemRole _role) {
	switch (_role)
	{
	case ot::ContextMenuItemRole::NoRole: return "NoRole";
	case ot::ContextMenuItemRole::SettingsReference: return "SettingsReference";
	case ot::ContextMenuItemRole::Clear: return "Clear";
	case ot::ContextMenuItemRole::Custom: return "Custom";
	default:
		OTAssert(0, "Invalid role");
		return "NoRole";
	}
}

ot::ContextMenuItemRole::itemRole ot::ContextMenuItemRole::stringToRole(const std::string& _role) {
	if (roleToString(NoRole) == _role) { return NoRole; }
	else if (roleToString(SettingsReference) == _role) { return SettingsReference; }
	else if (roleToString(Clear) == _role) { return Clear; }
	else if (roleToString(Custom) == _role) { return Custom; }
	else { OTAssert(0, "Invalid role"); return NoRole; }
}

void ot::ContextMenuItemRole::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Role", JsonString(roleToString(m_role), _allocator), _allocator);
	_object.AddMember("RoleText", JsonString(m_roleText, _allocator), _allocator);
}

void ot::ContextMenuItemRole::setFromJsonObject(const ConstJsonObject& _object) {
	m_role = stringToRole(json::getString(_object, "Role"));
	m_roleText = json::getString(_object, "RoleText");
}

// #########################################################################################################################################

// #########################################################################################################################################

// #########################################################################################################################################

ot::AbstractContextMenuItem::AbstractContextMenuItem(itemType _type) : m_type(_type) {}

ot::AbstractContextMenuItem::AbstractContextMenuItem(const AbstractContextMenuItem& _other)
	: m_type(_other.m_type) {}

ot::AbstractContextMenuItem& ot::AbstractContextMenuItem::operator = (const AbstractContextMenuItem& _other) {
	m_type = _other.m_type;
	return *this;
}

ot::AbstractContextMenuItem::~AbstractContextMenuItem() {}

std::string ot::AbstractContextMenuItem::typeToString(void) const {
	return typeToString(m_type);
}

std::string ot::AbstractContextMenuItem::typeToString(itemType _type) {
	switch (_type)
	{
	case ot::AbstractContextMenuItem::Seperator: return "Seperator";
	case ot::AbstractContextMenuItem::Item: return "Item";
	default:
		OTAssert(0, "Unknown item type");
		return "Seperator";
	}
}

ot::AbstractContextMenuItem::itemType ot::AbstractContextMenuItem::stringToType(const std::string& _type) {
	if (typeToString(Seperator) == _type) {
		return Seperator;
	}
	else if (typeToString(Item) == _type) {
		return Item;
	}
	else {
		OTAssert(0, "Invalid item type");
		return Seperator;
	}
}

void ot::AbstractContextMenuItem::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Type", JsonString(typeToString(m_type), _allocator), _allocator);
	this->addInternalToJsonObject(_object, _allocator);
}

ot::AbstractContextMenuItem * ot::AbstractContextMenuItem::buildItemFromJsonObject(const ConstJsonObject& _object) {
	if (!_object.HasMember("Type")) {
		OTAssert(0, "Type is missing");
		return nullptr;
	}
	if (!_object["Type"].IsString()) {
		OTAssert(0, "Type is not a string");
		return nullptr;
	}
	itemType type = stringToType(_object["Type"].GetString());
	switch (type)
	{
	case ot::AbstractContextMenuItem::Seperator:
	{
		ot::ContextMenuSeperator * newItem = new ot::ContextMenuSeperator();
		newItem->setFromJsonObject(_object);
		return newItem;
	}
	break;
	case ot::AbstractContextMenuItem::Item:
	{
		ot::ContextMenuItem * newItem = new ot::ContextMenuItem("", "");
		newItem->setFromJsonObject(_object);
		return newItem;
	}
	break;
	default:
		OTAssert(0, "Unknown item type");
		return nullptr;
	}
}

void ot::AbstractContextMenuItem::setFromJsonObject(const ConstJsonObject& _object) {
	m_type = stringToType(json::getString(_object, "Type"));
	setInternalFromJsonObject(_object);
}

// #########################################################################################################################################

// #########################################################################################################################################

// #########################################################################################################################################

ot::ContextMenuSeperator::ContextMenuSeperator() 
	: AbstractContextMenuItem(Seperator) {}

ot::ContextMenuSeperator::ContextMenuSeperator(const ContextMenuSeperator& _other) 
	: AbstractContextMenuItem(Seperator) {}

ot::ContextMenuSeperator& ot::ContextMenuSeperator::operator = (const ContextMenuSeperator& _other) {
	m_type = Seperator;
	return *this;
}

ot::ContextMenuSeperator::~ContextMenuSeperator() {}

ot::AbstractContextMenuItem * ot::ContextMenuSeperator::createCopy(void) const {
	return new ContextMenuSeperator;
}

void ot::ContextMenuSeperator::addInternalToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {}

void ot::ContextMenuSeperator::setInternalFromJsonObject(const ConstJsonObject& _object) {}

// #########################################################################################################################################

// #########################################################################################################################################

// #########################################################################################################################################

ot::ContextMenuItem::ContextMenuItem(const std::string& _name, const std::string& _text, const std::string& _icon, const ContextMenuItemRole& _role) 
	: AbstractContextMenuItem(Item), m_name(_name), m_text(_text), m_icon(_icon), m_role(_role) {}

ot::ContextMenuItem::ContextMenuItem(const ContextMenuItem& _other) 
	: AbstractContextMenuItem(Item), m_name(_other.m_name), m_text(_other.m_text), m_icon(_other.m_icon), m_role(_other.m_role) {}

ot::ContextMenuItem& ot::ContextMenuItem::operator = (const ContextMenuItem& _other) {
	m_type = Item;
	m_name = _other.m_name;
	m_text = _other.m_text;
	m_icon = _other.m_icon;
	m_role = _other.m_role;
	return *this;
}

ot::ContextMenuItem::~ContextMenuItem() {}

// #######################################################################################################

// Getter

bool ot::ContextMenuItem::isChecked(void) const {
	return m_checkState.flagIsSet(Checked);
}

bool ot::ContextMenuItem::isCheckable(void) const {
	return m_flags.flagIsSet(ItemIsCheckable);
}

ot::AbstractContextMenuItem * ot::ContextMenuItem::createCopy(void) const {
	return new ContextMenuItem(m_name, m_text, m_icon, m_role);
}

// #######################################################################################################

// Setter

void ot::ContextMenuItem::setChecked(bool _isChecked) {
	if (_isChecked) { m_checkState = Checked; }
	else { m_checkState = Unchecked; }
}

void ot::ContextMenuItem::setCheckable(bool _isCheckable) {
	if (_isCheckable) { m_flags.setFlag(ItemIsCheckable); }
	else { m_flags.removeFlag(ItemIsCheckable); }
}

// #######################################################################################################

// Operators

ot::ContextMenuItem& ot::ContextMenuItem::operator + (ItemFlag _flag) {
	m_flags.setFlag(_flag);
	return *this;
}

ot::ContextMenuItem& ot::ContextMenuItem::operator - (ItemFlag _flag) {
	m_flags.removeFlag(_flag);
	return *this;
}

ot::ContextMenuItem& ot::ContextMenuItem::operator + (ItemCheckedState _checkedState) {
	m_checkState = _checkedState;
	return *this;
}

// #######################################################################################################

// Protected functions

void ot::ContextMenuItem::addInternalToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Icon", JsonString(m_icon, _allocator), _allocator);
	_object.AddMember("Flags", JsonArray(convert::toString(m_flags), _allocator), _allocator);
	_object.AddMember("CheckedState", JsonString(convert::toString(m_checkState.data()), _allocator), _allocator);
	
	JsonObject roleObj;
	m_role.addToJsonObject(roleObj, _allocator);
	_object.AddMember("Role", roleObj, _allocator);
}

void ot::ContextMenuItem::setInternalFromJsonObject(const ConstJsonObject& _object) {
	// Check object
	if (!_object.HasMember("Name")) { OTAssert(0, "Member Name is missing"); return; }
	if (!_object["Name"].IsString()) { OTAssert(0, "Member Name is not a string"); return; }
	if (!_object.HasMember("Text")) { OTAssert(0, "Member Text is missing"); return; }
	if (!_object["Text"].IsString()) { OTAssert(0, "Member Text is not a string"); return; }
	if (!_object.HasMember("Icon")) { OTAssert(0, "Member Icon is missing"); return; }
	if (!_object["Icon"].IsString()) { OTAssert(0, "Member Icon is not a string"); return; }
	if (!_object.HasMember("Role")) { OTAssert(0, "Member Role is missing"); return; }
	if (!_object["Role"].IsObject()) { OTAssert(0, "Member Role is not a object"); return; }
	if (!_object.HasMember("Flags")) { OTAssert(0, "Member Role is missing"); return; }
	if (!_object["Flags"].IsArray()) { OTAssert(0, "Member Role is not a object"); return; }
	if (!_object.HasMember("CheckedState")) { OTAssert(0, "Member Role is missing"); return; }
	if (!_object["CheckedState"].IsString()) { OTAssert(0, "Member Role is not a object"); return; }

	// Store data
	m_name = json::getString(_object, "Name");
	m_text = json::getString(_object, "Text");
	m_icon = json::getString(_object, "Icon");
	m_role.setFromJsonObject(json::getObject(_object, "Role"));
	m_checkState = convert::toItemCheckedState(json::getString(_object, "CheckedState"));
	m_flags = convert::toItemFlags(json::getStringList(_object, "Flags"));
}

// #########################################################################################################################################

// #########################################################################################################################################

// #########################################################################################################################################

ot::ContextMenu::ContextMenu(const std::string& _name) : m_name(_name) {}

ot::ContextMenu::ContextMenu(const ContextMenu& _other) 
	: m_name(_other.m_name)
{
	for (auto itm : _other.m_items) {
		m_items.push_back(itm->createCopy());
	}
}

ot::ContextMenu& ot::ContextMenu::operator = (const ContextMenu& _other) {
	for (auto itm : m_items) { delete itm; }
	m_items.clear();

	m_name = _other.m_name;
	for (auto itm : _other.m_items) { m_items.push_back(itm->createCopy()); }

	return *this;
}

ot::ContextMenu::~ContextMenu() {}

// #######################################################################################################

// Getter

bool ot::ContextMenu::hasItems(void) const { return !m_items.empty(); }

void ot::ContextMenu::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	JsonArray itemArr;
	for (auto itm : m_items) {
		JsonObject itemObj;
		itm->addToJsonObject(itemObj, _allocator);
		itemArr.PushBack(itemObj, _allocator);
	}
	_object.AddMember("Items", itemArr, _allocator);
}

// #######################################################################################################

// Setter

void ot::ContextMenu::setFromJsonObject(const ConstJsonObject& _object) {
	if (!_object.HasMember("Name")) { OTAssert(0, "Member Name is missing"); return; }
	if (!_object["Name"].IsString()) { OTAssert(0, "Member Name is not a string"); return; }
	if (!_object.HasMember("Items")) { OTAssert(0, "Member Items is missing"); return; }
	if (!_object["Items"].IsArray()) { OTAssert(0, "Member Items is not a array"); return; }
	
	m_name = json::getString(_object, "Name");
	std::list<ConstJsonObject> itemsArr = json::getObjectList(_object, "Items");
	for (auto itemObj : itemsArr) {
		AbstractContextMenuItem * itm = AbstractContextMenuItem::buildItemFromJsonObject(itemObj);
		if (itm) { m_items.push_back(itm); }
	}
}

ot::ContextMenu& ot::ContextMenu::addItem(const ContextMenuSeperator& _item) {
	m_items.push_back(new ContextMenuSeperator(_item));
	return *this;
}

ot::ContextMenu& ot::ContextMenu::addItem(const ContextMenuItem& _item) {
	m_items.push_back(new ContextMenuItem(_item));
	return *this;
}

ot::ContextMenu& ot::ContextMenu::operator << (const ContextMenuSeperator& _item) {
	m_items.push_back(new ContextMenuSeperator(_item));
	return *this;
}

ot::ContextMenu& ot::ContextMenu::operator << (const ContextMenuItem& _item) {
	m_items.push_back(new ContextMenuItem(_item));
	return *this;
}
