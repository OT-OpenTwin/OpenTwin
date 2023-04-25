#include "ContextMenuManager.h"
#include "AppBase.h"
#include "ViewerComponent.h"
#include "ExternalServicesComponent.h"
#include "UserSettings.h"

#include "OpenTwinCore/ServiceBase.h"
#include "OpenTwinCore/otAssert.h"

#include <akAPI/uiAPI.h>
#include <akGui/aToolButtonCustomContextMenu.h>
#include <akGui/aContextMenuItem.h>

using namespace ak;

#define CONTEXTMENUMANAGER_ROLE_ID_Custom "Custom"
#define CONTEXTMENUMANAGER_ROLE_ID_SettingsRef "SettingsRef"
#define CONTEXTMENUMANAGER_ROLE_ID_Clear "Clear"

ContextMenuEntry::ContextMenuEntry(const std::string& _name, ID _id, const std::string& _text, const std::string& _role, const std::string& _roleText, const std::string& _icon) 
	: m_name(_name), m_text(_text), m_role(_role), m_roleText(_roleText), m_icon(_icon), m_id(_id)
{}

ContextMenuEntry::ContextMenuEntry(const ContextMenuEntry& _other) 
	: m_name(_other.m_name), m_id(_other.m_id), m_text(_other.m_text), m_role(_other.m_role), m_icon(_other.m_icon)
{}

ContextMenuEntry& ContextMenuEntry::operator = (const ContextMenuEntry& _other) {
	m_name = _other.m_name;
	m_id = _other.m_id;
	m_text = _other.m_text;
	m_role = _other.m_role;
	m_icon = _other.m_icon;
	return *this;
}

ContextMenuEntry::~ContextMenuEntry() {}

// ######################################################################################################################################

// ######################################################################################################################################

// ######################################################################################################################################

ContextMenuItem::ContextMenuItem(ot::ServiceBase * _service, ak::UID _creatorUid, const std::string& _name, UID _uid)
	: m_service(_service), m_creatorUid(_creatorUid), m_name(_name), m_uid(_uid), m_isViewerContext(false) {}

ContextMenuItem::ContextMenuItem(const ContextMenuItem& _other)
	: m_service(_other.m_service), m_creatorUid(_other.m_creatorUid), m_name(_other.m_name), m_uid(_other.m_uid), m_isViewerContext(_other.m_isViewerContext) {}

ContextMenuItem& ContextMenuItem::operator = (const ContextMenuItem& _other) {
	m_isViewerContext = _other.m_isViewerContext;
	m_creatorUid = _other.m_creatorUid;
	m_service = _other.m_service;
	m_name = _other.m_name;
	m_uid = _other.m_uid;
	return *this;
}

ContextMenuItem::~ContextMenuItem() {
	for (auto itm : m_items) {
		delete itm;
	}
	m_items.clear();
	uiAPI::object::destroy(m_uid);
}

ContextMenuEntry * ContextMenuItem::addEntry(ContextMenuEntry * _entry) {
	for (auto itm : m_items) {
		if (itm->name() == _entry->name()) {
			otAssert(0, "Item with the same name already exists");
			AppBase::instance()->appendInfoMessage("[WARNING] Failed to add context menu entry: Entry with the same name already exists");
			return nullptr;
		}
	}
	aToolButtonCustomContextMenu * obj = uiAPI::object::get<aToolButtonCustomContextMenu>(m_uid);
	if (obj == nullptr) { otAssert(0, "Menu cast failed"); return nullptr; }
	aContextMenuItem * itm;
	contextMenuRole role = cmrNone;
	
	if (_entry->role() == "Clear") {
		role = cmrClear;
	}

	if (_entry->icon().empty()) {
		itm = new aContextMenuItem(_entry->text().c_str(), role);
	}
	else {
		itm = new aContextMenuItem(uiAPI::getIcon(_entry->icon().c_str(), "ContextMenu"), _entry->text().c_str(), role);
	}
	_entry->setId(obj->addMenuItem(itm));
	m_items.push_back(_entry);
	return _entry;
}

ContextMenuEntry * ContextMenuItem::addEntry(ot::ContextMenuItem * _item) {
	for (auto itm : m_items) {
		if (itm->name() == _item->name()) {
			otAssert(0, "Item with the same name already exists");
			AppBase::instance()->appendInfoMessage("[WARNING] Failed to add context menu entry: Entry with the same name already exists");
			return nullptr;
		}
	}
	aToolButtonCustomContextMenu * obj = uiAPI::object::get<aToolButtonCustomContextMenu>(m_uid);
	if (obj == nullptr) { otAssert(0, "Menu cast failed"); return nullptr; }
	aContextMenuItem * itm;
	std::string roleId;
std::string roleText;
contextMenuRole role = cmrNone;
if (_item->role().role() == ot::ContextMenuItemRole::NoRole) {}
else if (_item->role().role() == ot::ContextMenuItemRole::Clear) {
	role = cmrClear;
	roleId = CONTEXTMENUMANAGER_ROLE_ID_Clear;
}
else if (_item->role().role() == ot::ContextMenuItemRole::Custom) {
	roleText = _item->role().roleText();
	roleId = CONTEXTMENUMANAGER_ROLE_ID_Custom;
}
else if (_item->role().role() == ot::ContextMenuItemRole::SettingsReference) {
	roleText = _item->role().roleText();
	roleId = CONTEXTMENUMANAGER_ROLE_ID_SettingsRef;
}

if (_item->icon().empty()) {
	itm = new aContextMenuItem(_item->text().c_str(), role);
}
else {
	itm = new aContextMenuItem(uiAPI::getIcon(_item->icon().c_str(), "ContextMenu"), _item->text().c_str(), role);
}
itm->setCheckable(_item->isCheckable());
if (_item->isCheckable()) {
	itm->setChecked(_item->isChecked());
}
ContextMenuEntry * newEntry = new ContextMenuEntry(_item->name(), obj->addMenuItem(itm), _item->text(), roleId, roleText, _item->icon());
m_items.push_back(newEntry);
return newEntry;
}

ContextMenuEntry * ContextMenuItem::addEntry(const std::string& _name, const std::string& _text, const std::string& _role, const std::string& _icon) {
	for (auto itm : m_items) {
		if (itm->name() == _name) {
			otAssert(0, "Item with the same name already exists");
			AppBase::instance()->appendInfoMessage("[WARNING] Failed to add context menu entry: Entry with the same name already exists");
			return nullptr;
		}
	}
	aToolButtonCustomContextMenu * obj = uiAPI::object::get<aToolButtonCustomContextMenu>(m_uid);
	aContextMenuItem * itm;

	contextMenuRole role = cmrNone;
	if (_role == "Clear") {
		role = cmrClear;
	}

	if (_icon.empty()) {
		itm = new aContextMenuItem(_text.c_str(), role);
	}
	else {
		itm = new aContextMenuItem(uiAPI::getIcon(_icon.c_str(), "ContextMenu"), _text.c_str(), role);
	}

	ContextMenuEntry * entry = new ContextMenuEntry(_name, obj->addMenuItem(itm), _text, _role, _icon);
	m_items.push_back(entry);
	return entry;
}

void ContextMenuItem::addSeperator(void) {
	aToolButtonCustomContextMenu * obj = uiAPI::object::get<aToolButtonCustomContextMenu>(m_uid);
	if (obj == nullptr) { otAssert(0, "Menu cast failed"); return; }
	obj->addMenuSeperator();
}

const ContextMenuEntry * ContextMenuItem::findItemById(ak::ID _id) {
	for (auto itm : m_items) {
		if (itm->id() == _id) { return itm; }
	}
	otAssert(0, "Item not found");
	return nullptr;
}

// ######################################################################################################################################

// ######################################################################################################################################

// ######################################################################################################################################

ContextMenuManager::ContextMenuManager() {}

ContextMenuManager::~ContextMenuManager() {
	for (auto entry : m_serviceData) {
		for (auto itm : *entry.second) {
			delete itm;
		}
		delete entry.second;
	}
	m_serviceData.clear();
	m_uiToDataMap.clear();
}

void ContextMenuManager::notify(UID _sender, eventType _event, int _info1, int _info2) {
	ContextMenuItem * item = findItemByUid(_sender);
	if (item) {
		if (_event == etContextMenuItemClicked) {
			const ContextMenuEntry * entry = item->findItemById(_info1);
			if (entry) {
				if (entry->role().empty()) {
					if (item->service()) {
						if (AppBase::instance()->getExternalServicesComponent()) {
							AppBase::instance()->getExternalServicesComponent()->contextMenuItemClicked(item->service(), item->name(), entry->name());
						} else {
							AppBase::instance()->showErrorPrompt("Fatal:\n\nExternal services component not found for context menu event", "Fatal error");
						}
					}
					else {
						if (item->isViewerContext()) {
							if (AppBase::instance()->getViewerComponent()) {
								AppBase::instance()->getViewerComponent()->contextMenuItemClicked(item->name(), entry->name());
							}
							else {
								AppBase::instance()->showErrorPrompt("Fatal:\n\nViewer component not found for context menu event", "Fatal error");
							}
						}
						else {
							// uiService context item
						}
					}
				}
				else {
					if (entry->role() == CONTEXTMENUMANAGER_ROLE_ID_SettingsRef) {
						QString group = entry->roleText().c_str();
						UserSettings::instance()->showDialog(group);
					}
					else if (entry->role() == CONTEXTMENUMANAGER_ROLE_ID_Custom) {

					}
				}
			}
			else {
				otAssert(0, "Failed to find context menu entry by id");
				printIError("Failed to find context menu entry by id");
			}
		}
		else if (_event == etContextMenuItemCheckedChanged) {
			const ContextMenuEntry * entry = item->findItemById(_info1);
			if (entry) {
				if (entry->role().empty()) {
					if (item->service()) {
						if (AppBase::instance()->getExternalServicesComponent()) {
							AppBase::instance()->getExternalServicesComponent()->contextMenuItemCheckedChanged(item->service(), item->name(), entry->name(), _info2);
						}
						else {
							AppBase::instance()->showErrorPrompt("Fatal:\n\nExternal services component not found for context menu event", "Fatal error");
						}
					}
					else {
						if (item->isViewerContext()) {
							if (AppBase::instance()->getViewerComponent()) {
								AppBase::instance()->getViewerComponent()->contextMenuItemCheckedChanged(item->name(), entry->name(), _info2);
							}
							else {
								AppBase::instance()->showErrorPrompt("Fatal:\n\nExternal services component not found for context menu event", "Fatal error");
							}
						}
						else {
							// uiService context item
						}
					}
				}
				else {
					if (entry->role() == CONTEXTMENUMANAGER_ROLE_ID_SettingsRef) {
						QString group = entry->roleText().c_str();
						UserSettings::instance()->showDialog(group);
					}
					else if (entry->role() == CONTEXTMENUMANAGER_ROLE_ID_Custom) {

					}
				}
			}
			else {
				otAssert(0, "Failed to find context menu entry by id");
				printIError("Failed to find context menu entry by id");
			}
		}
	}
}

ContextMenuItem * ContextMenuManager::createItem(ot::ServiceBase * _creator, ak::UID _creatorUid, ak::UID _controlUid, const ot::ContextMenu& _menu) {
	std::list<ContextMenuItem *> * entries = getOrCreateDataList(_creator);
	for (auto itm : *entries) {
		if (itm->name() == _menu.name()) {
			otAssert(0, "An item with the given name was already created");
			return nullptr;
		}
	}
	ak::UID newUid = uiAPI::createToolButtonCustomContextMenu(_creatorUid, _controlUid);
	ContextMenuItem * newItem = new ContextMenuItem(_creator, _controlUid, _menu.name(), newUid);
	entries->push_back(newItem);
	m_uiToDataMap.insert_or_assign(newUid, newItem);

	for (auto itm : _menu.items()) {
		if (itm->type() == ot::AbstractContextMenuItem::Item) {
			ot::ContextMenuItem * actualItem = dynamic_cast<ot::ContextMenuItem *>(itm);
			if (actualItem) {
				newItem->addEntry(actualItem);
			}
			else {
				otAssert(0, "Context menu item cast failed");
				printSError(_creator, "Context menu item cast failed");
			}
		}
		else if (itm->type() == ot::AbstractContextMenuItem::Seperator) {
			ot::ContextMenuSeperator * actualItem = dynamic_cast<ot::ContextMenuSeperator *>(itm);
			if (actualItem) { newItem->addSeperator(); }
			else {
				otAssert(0, "Context menu seperator cast failed");
				printSError(_creator, "Context menu seperator cast failed");
			}
		}
		else {
			otAssert(0, "Unknown context menu item type");
			printSError(_creator, "Unknown context menu item type");
		}
	}
	uiAPI::registerUidNotifier(newUid, this);
	return newItem;
}

ContextMenuItem * ContextMenuManager::createItem(ot::ServiceBase * _creator, ak::UID _creatorUid, const std::string& _name, ak::UID _controlUid, const std::list<ContextMenuEntry *>& _entries) {
	std::list<ContextMenuItem *> * entries = getOrCreateDataList(_creator);
	for (auto itm : *entries) {
		if (itm->name() == _name) {
			otAssert(0, "An item with the given name was already created");
			return nullptr;
		}
	}
	ak::UID newUid = uiAPI::createToolButtonCustomContextMenu(_creatorUid, _controlUid);
	ContextMenuItem * newItem = new ContextMenuItem(_creator, _controlUid, _name, newUid);
	entries->push_back(newItem);
	m_uiToDataMap.insert_or_assign(newUid, newItem);
	for (auto itm : _entries) { newItem->addEntry(itm); }
	uiAPI::registerUidNotifier(newUid, this);
	return newItem;
}

void ContextMenuManager::serviceDisconnected(ot::ServiceBase * _service) {
	if (!hasDataListEntry(_service)) { return; }
	std::list<ContextMenuItem *> * serviceData = getOrCreateDataList(_service);
	for (auto itm : *serviceData) {
		m_uiToDataMap.erase(itm->uid());
		delete itm;
	}
	delete serviceData;
	m_serviceData.erase(_service);
}

void ContextMenuManager::uiItemDestroyed(ak::UID _itemUid) {
	for (auto service : m_serviceData) {
		for (auto item : *service.second) {
			if (item->creatorUid() == _itemUid || item->uid() == _itemUid) {
				service.second->erase(std::find(service.second->begin(), service.second->end(), item));
				m_uiToDataMap.erase(item->uid());
				if (uiAPI::object::exists(_itemUid)) { uiAPI::object::destroy(_itemUid); }
				delete item;
				uiItemDestroyed(_itemUid);
				return;
			}
		}
	}
}

std::list<ContextMenuItem *> * ContextMenuManager::getOrCreateDataList(ot::ServiceBase * _service) {
	auto entry = m_serviceData.find(_service);
	if (entry == m_serviceData.end()) {
		std::list<ContextMenuItem *> * newList = new std::list<ContextMenuItem *>;
		m_serviceData.insert_or_assign(_service, newList);
		return newList;
	}
	else {
		return entry->second;
	}
}

bool ContextMenuManager::hasDataListEntry(ot::ServiceBase * _service) {
	auto entry = m_serviceData.find(_service);
	return entry != m_serviceData.end();
}

ContextMenuItem * ContextMenuManager::findItemByUid(ak::UID _uid) {
	auto entry = m_uiToDataMap.find(_uid);
	if (entry == m_uiToDataMap.end()) {
		otAssert(0, "No entry found");
		return nullptr;
	}
	return entry->second;
}