/*
 * ControlsManager.cpp
 *
 *  Created on: February 20, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#include "ControlsManager.h"
#include "AppBase.h"

// OT header
#include "OTCore/ServiceBase.h"

// AK header
#include <akAPI/uiAPI.h>

ControlsManager::~ControlsManager() {
	for (auto itm : m_creatorMap) { delete itm.second; }
}

void ControlsManager::uiElementCreated(ot::ServiceBase * _creator, ak::UID _controlUid, bool _elementHasEvents) {
	
	// Store ui info
	if (_elementHasEvents) {
		auto oldItm = m_uiToCreatorMap.find(_controlUid);
		if (oldItm != m_uiToCreatorMap.end()) {
			assert(0);
			throw std::exception("Item was already registered");
		}
		m_uiToCreatorMap.insert_or_assign(_controlUid, _creator);
	}
	
	// Store creator info
	auto creatorList{ m_creatorMap.find(_creator) };
	if (creatorList == m_creatorMap.end()) {
		// Creators first element
		std::vector<ak::UID> * newVector{ new std::vector<ak::UID> };
		newVector->push_back(_controlUid);
		m_creatorMap.insert_or_assign(_creator, newVector);
	}
	else { creatorList->second->push_back(_controlUid); }
}

bool ControlsManager::destroyUiControl(ak::UID _controlUid) {
	// Check if object exists
	if (!ak::uiAPI::object::exists(_controlUid)) { return false; }

	// First destroy the actual UI element
	ak::uiAPI::object::destroy(_controlUid, true);

	// Check if object was destroyed
	if (ak::uiAPI::object::exists(_controlUid)) { return false; }

	// Call the clean-up
	uiControlWasDestroyed(_controlUid);

	return true;
}

void ControlsManager::destroyUiControls(const std::vector<ak::UID> & _controlUids) {
	bool destroyed = true;
	while (destroyed)
	{
		destroyed = false;
		for (auto itm : _controlUids) {
			if (destroyUiControl(itm)) {
				destroyed = true;
			}
		}
	}
}

void ControlsManager::uiControlWasDestroyed(ak::UID _controlUid) {
	// Clean up the reference list
	m_uiToCreatorMap.erase(_controlUid);

	for (auto creatorList : m_creatorMap) {
		auto it = std::find(creatorList.second->begin(), creatorList.second->end(), _controlUid);
		while (it != creatorList.second->end()) {
			creatorList.second->erase(it);
			it = std::find(creatorList.second->begin(), creatorList.second->end(), _controlUid);
		}
	}
}

void ControlsManager::serviceDisconnected(ot::ServiceBase * _service) {
	// Get the controls created by this service
	auto controlsList = m_creatorMap.find(_service);
	if (controlsList != m_creatorMap.end()) {
		destroyUiControls(*controlsList->second);
		delete controlsList->second;
		// Finally remove the creator information from the creator map
		m_creatorMap.erase(_service);
	}
}

ot::ServiceBase * ControlsManager::objectCreator(ak::UID _itemUid) {
	auto itm{ m_uiToCreatorMap.find(_itemUid) };
	if (itm == m_uiToCreatorMap.end()) {
		return nullptr;
	}
	return itm->second;
}

// #######################################################################################################################

// #######################################################################################################################

// #######################################################################################################################

LockManager::~LockManager() {
	
}

void LockManager::uiElementCreated(ot::ServiceBase * _service, ak::UID _uid, const ot::Flags<ot::ui::lockType> & _typeFlags) {
	// Check for duplicate
	auto oldItm{ m_uiElements.find(_uid) };
	assert(oldItm == m_uiElements.end());	// Element information already stored

	// Create new entrys
	LockManagerElement * uiElement = new LockManagerElement{ _uid, _typeFlags };
	m_uiElements.insert_or_assign(_uid, uiElement);

	// Initialize enabled list
	auto lst{ serviceEnabledLevel(_service) };
	lst->insert_or_assign(_uid, 0);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				uiElement->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::uiElementDestroyed(ak::UID _uid) {
	
	// Clean up UI -> lock map
	auto oldItm{ m_uiElements.find(_uid) };
	if (oldItm != m_uiElements.end()) {
		delete oldItm->second;
		m_uiElements.erase(_uid);
	}
	
	// Clean up service -> UI -> enabled map
	for (auto senderItem : m_serviceToUiEnabledLevel) {
		senderItem.second->erase(_uid);
	}
	
}

void LockManager::lock(ot::ServiceBase * _service, ot::ui::lockType _type) {
	auto service = serviceLockLevel(_service);

	// Get old value
	auto oldLevel = service->find(_type);
	int v = 0;
	if (oldLevel != service->end()) {
		v = oldLevel->second;
	}

	// Store new data
	service->insert_or_assign(_type, v + 1);

	for (auto itm : m_uiElements) {
		itm.second->lock(1, _type);
	}

	if (_type == ot::ui::tlAll) { m_owner->setWaitingAnimationVisible(true); }
}

void LockManager::lock(ot::ServiceBase * _service, const ot::Flags<ot::ui::lockType> & _typeFlags) {
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlAll)) {
		lock(_service, ot::ui::lockType::tlAll);
	} 
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlModelWrite)) {
		lock(_service, ot::ui::lockType::tlModelWrite);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlModelRead)) {
		lock(_service, ot::ui::lockType::tlModelRead);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlNavigationAll)) {
		lock(_service, ot::ui::lockType::tlNavigationAll);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlNavigationWrite)) {
		lock(_service, ot::ui::lockType::tlNavigationWrite);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlProperties)) {
		lock(_service, ot::ui::lockType::tlProperties);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlViewRead)) {
		lock(_service, ot::ui::lockType::tlViewRead);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlViewWrite)) {
		lock(_service, ot::ui::lockType::tlViewWrite);
	}
}

void LockManager::unlock(ot::ServiceBase * _service) {
	auto service = serviceLockLevel(_service);

	for (auto lockVal : *service) {
		for (auto itm : m_uiElements) {
			itm.second->unlock(lockVal.second, lockVal.first);
		}
		service->insert_or_assign(lockVal.first, 0);
	}

	if (lockLevel(ot::ui::tlAll) <= 0) { m_owner->setWaitingAnimationVisible(false); }
}

void LockManager::unlock(ot::ServiceBase * _service, ot::ui::lockType _type) {
	auto service = serviceLockLevel(_service);

	// Get old value
	auto oldLevel = service->find(_type);
	int v = 0;
	if (oldLevel != service->end()) {
		v = oldLevel->second;
	}

	// Store data
	if (v > 0) {
		service->insert_or_assign(_type, v - 1);
		for (auto itm : m_uiElements) {
			itm.second->unlock(1, _type);
		}
	}
	else {
		service->insert_or_assign(_type, v);
	}

	if (_type == ot::ui::tlAll) {
		if (lockLevel(ot::ui::tlAll) <= 0) { m_owner->setWaitingAnimationVisible(false); }
	}
}

void LockManager::unlock(ot::ServiceBase * _service, const ot::Flags<ot::ui::lockType> & _typeFlags) {
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlAll)) {
		unlock(_service, ot::ui::lockType::tlAll);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlModelWrite)) {
		unlock(_service, ot::ui::lockType::tlModelWrite);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlModelRead)) {
		unlock(_service, ot::ui::lockType::tlModelRead);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlNavigationAll)) {
		unlock(_service, ot::ui::lockType::tlNavigationAll);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlNavigationWrite)) {
		unlock(_service, ot::ui::lockType::tlNavigationWrite);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlProperties)) {
		unlock(_service, ot::ui::lockType::tlProperties);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlViewRead)) {
		unlock(_service, ot::ui::lockType::tlViewRead);
	}
	if (_typeFlags.flagIsSet(ot::ui::lockType::tlViewWrite)) {
		unlock(_service, ot::ui::lockType::tlViewWrite);
	}
}

void LockManager::disable(ot::ServiceBase * _service, ak::UID _element) {
	auto service = serviceEnabledLevel(_service);
	auto serviceEnabledVal = service->find(_element);
	if (serviceEnabledVal == service->end()) {
		service->insert_or_assign(_element, 1);
	}
	else {
		service->insert_or_assign(_element, serviceEnabledVal->second + 1);
	}
	uiElement(_element)->disable(1);
}

void LockManager::enable(ot::ServiceBase * _service, ak::UID _element, bool _resetCounter) {
	auto service = serviceEnabledLevel(_service);
	auto serviceEnabledVal = service->find(_element);
	if (serviceEnabledVal == service->end()) {
		service->insert_or_assign(_element, 0);
	}
	else {
		int oldValue = serviceEnabledVal->second;
		if (oldValue > 0) {
			if (_resetCounter) {
				service->insert_or_assign(_element, 0);
				uiElement(_element)->enable(oldValue);
			}
			else {
				service->insert_or_assign(_element, oldValue -1);
				uiElement(_element)->enable(1);
			}
		}
	}
}

void LockManager::cleanService(ot::ServiceBase * _service, bool _reenableElement, bool _eraseUiElements) {
	auto serviceE = serviceEnabledLevel(_service);
	auto serviceL = serviceLockLevel(_service);

	for (auto e : *serviceE) {
		if (e.second > 0 && _reenableElement) {
			uiElement(e.first)->enable(e.second);
		}
		if (_eraseUiElements) {
			auto uiItm = m_uiElements.find(e.first);
			if (uiItm != m_uiElements.end()) {
				delete uiItm->second;
				m_uiElements.erase(e.first);
			}
		}
	}

	for (auto l : *serviceL) {
		if (l.second > 0 && _reenableElement) {
			for (auto itm : m_uiElements) {
				itm.second->unlock(l.second, l.first);
			}
		}
	}

	delete serviceE;
	delete serviceL;
	m_serviceToUiEnabledLevel.erase(_service);
	m_serviceToUiLockLevel.erase(_service);

	if (lockLevel(ot::ui::tlAll) <= 0) { m_owner->setWaitingAnimationVisible(false); }
}

int LockManager::lockLevel(ot::ui::lockType _type) {
	int ret{ 0 };
	for (auto info : m_serviceToUiLockLevel) {
		for (auto lockInfo : *info.second) {
			if (lockInfo.first == _type) { ret += lockInfo.second; }
		}
	}
	return ret;
}

// #######################################################################################################################

// Private functions

std::map<ot::ui::lockType, int> * LockManager::generateDefaultLockMap(void) const {
	// Create new entry
	std::map<ot::ui::lockType, int> * newMap{ new std::map<ot::ui::lockType, int> };

	// Insert initial data
	newMap->insert_or_assign(ot::ui::lockType::tlAll, 0);
	newMap->insert_or_assign(ot::ui::lockType::tlModelWrite, 0);
	newMap->insert_or_assign(ot::ui::lockType::tlModelRead, 0);
	newMap->insert_or_assign(ot::ui::lockType::tlNavigationAll, 0);
	newMap->insert_or_assign(ot::ui::lockType::tlNavigationWrite, 0);
	newMap->insert_or_assign(ot::ui::lockType::tlProperties, 0);
	newMap->insert_or_assign(ot::ui::lockType::tlViewRead, 0);
	newMap->insert_or_assign(ot::ui::lockType::tlViewWrite, 0);
	return newMap;
}

LockManagerElement * LockManager::uiElement(ak::UID _uid) {
	auto itm{ m_uiElements.find(_uid) };
	assert(itm != m_uiElements.end());	// Item does not exist
	return itm->second;
}

std::map<ot::ui::lockType, int> * LockManager::serviceLockLevel(ot::ServiceBase * _service) {
	auto itm = m_serviceToUiLockLevel.find(_service);
	if (itm == m_serviceToUiLockLevel.end()) {
		auto newMap = new std::map<ot::ui::lockType, int>;
		m_serviceToUiLockLevel.insert_or_assign(_service, newMap);
		return newMap;
	}
	else { return itm->second; }
}

std::map<ak::UID, int> * LockManager::serviceEnabledLevel(ot::ServiceBase * _service) {
	auto itm = m_serviceToUiEnabledLevel.find(_service);
	if (itm == m_serviceToUiEnabledLevel.end()) {
		auto newMap = new std::map<ak::UID, int>;
		m_serviceToUiEnabledLevel.insert_or_assign(_service, newMap);
		return newMap;
	}
	else { return itm->second; }
}

// #######################################################################################################################

// #######################################################################################################################

// #######################################################################################################################

LockManagerElement::LockManagerElement(ak::UID _uid, const ot::Flags<ot::ui::lockType> & _flags)
	: m_disabledCount{ 0 }, m_lockCount{ 0 }, m_lockTypes{ _flags }, m_uid{ _uid }
{}

void LockManagerElement::enable(int _value) {
	m_disabledCount -= _value;
	if (m_disabledCount < 0)
	{
		assert(0); // This control is enabled without being disabled before
		m_disabledCount = 0;
	}

	if (m_disabledCount == 0 && m_lockCount == 0) {
		ak::uiAPI::object::setEnabled(m_uid, true);
	}
}

void LockManagerElement::disable(int _value) {
	m_disabledCount += _value;

	if (m_disabledCount > 0) {
		ak::uiAPI::object::setEnabled(m_uid, false);
	}
}

void LockManagerElement::lock(int _value, ot::ui::lockType _lockType) {
	if (m_lockTypes.flagIsSet(_lockType)) {
		bool isTreeItem = m_lockTypes.flagIsSet(ot::ui::lockType::tlNavigationWrite) || m_lockTypes.flagIsSet(ot::ui::lockType::tlNavigationAll);

		m_lockCount += _value;

		if (m_lockCount > 0) {
			if (isTreeItem)
			{
				if (_lockType == ot::ui::lockType::tlNavigationWrite || _lockType == ot::ui::lockType::tlNavigationAll) {
					ak::uiAPI::tree::setIsReadOnly(m_uid, true);
				}
			}
			else {
				ak::uiAPI::object::setEnabled(m_uid, false);
			}
		}
	}
}

void LockManagerElement::unlock(int _value, ot::ui::lockType _lockType) {
	if (m_lockTypes.flagIsSet(_lockType)) {
		bool isTreeItem = m_lockTypes.flagIsSet(ot::ui::lockType::tlNavigationWrite) || m_lockTypes.flagIsSet(ot::ui::lockType::tlNavigationAll);

		m_lockCount -= _value;

		if (m_lockCount < 0)
		{
			assert(0); // This control is unlocked without being locked before
			m_lockCount = 0;
		}

		if (m_lockCount == 0 && m_disabledCount == 0) {
			if (isTreeItem)
			{
				if (_lockType == ot::ui::lockType::tlNavigationWrite || _lockType == ot::ui::lockType::tlNavigationAll) {
					ak::uiAPI::tree::setIsReadOnly(m_uid, false);
				}
			}
			else {
				ak::uiAPI::object::setEnabled(m_uid, true);
			}
		}
	}
}
