//! @file ControlsManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2020
// ###########################################################################################################################################################################################################################################################################################################################

#include "ControlsManager.h"
#include "AppBase.h"

// OT header
#include "OTCore/ServiceBase.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/GraphicsView.h"

// AK header
#include <akAPI/uiAPI.h>
#include <akWidgets/aTreeWidget.h>

ControlsManager::~ControlsManager() {
	for (auto itm : m_creatorMap) { delete itm.second; }
}

void ControlsManager::uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::UID _controlUid, bool _elementHasEvents) {
	
	// Store ui info
	if (_elementHasEvents) {
		auto oldItm = m_uiToCreatorMap.find(_controlUid);
		if (oldItm != m_uiToCreatorMap.end()) {
			assert(0);
			throw std::exception("Item was already registered");
		}
		m_uiToCreatorMap.insert_or_assign(_controlUid, _serviceInfo);
	}
	
	// Store creator info
	auto creatorList{ m_creatorMap.find(_serviceInfo) };
	if (creatorList == m_creatorMap.end()) {
		// Creators first element
		std::vector<ot::UID> * newVector{ new std::vector<ot::UID> };
		newVector->push_back(_controlUid);
		m_creatorMap.insert_or_assign(_serviceInfo, newVector);
	}
	else { creatorList->second->push_back(_controlUid); }
}

bool ControlsManager::destroyUiControl(ot::UID _controlUid) {
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

void ControlsManager::destroyUiControls(const std::vector<ot::UID> & _controlUids) {
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

void ControlsManager::uiControlWasDestroyed(ot::UID _controlUid) {
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

void ControlsManager::serviceDisconnected(const ot::BasicServiceInformation& _serviceInfo) {
	// Get the controls created by this service
	auto controlsList = m_creatorMap.find(_serviceInfo);
	if (controlsList != m_creatorMap.end()) {
		destroyUiControls(*controlsList->second);
		delete controlsList->second;
		// Finally remove the creator information from the creator map
		m_creatorMap.erase(_serviceInfo);
	}
}

ot::BasicServiceInformation ControlsManager::objectCreator(ot::UID _itemUid) {
	auto itm{ m_uiToCreatorMap.find(_itemUid) };
	if (itm == m_uiToCreatorMap.end()) {
		OT_LOG_E("Creator not found");
		return ot::BasicServiceInformation();
	}
	return itm->second;
}

// #######################################################################################################################

// #######################################################################################################################

// #######################################################################################################################

LockManager::LockManager(AppBase* _owner)
	: m_owner(_owner)
{}

LockManager::~LockManager() {
	
}

void LockManager::uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::UID _uid, const ot::LockTypeFlags & _typeFlags) {
	// Check for duplicate
	auto oldItm{ m_uiElements.find(_uid) };
	assert(oldItm == m_uiElements.end());	// Element information already stored

	// Create new entrys
	LockManagerElement * uiElement = new LockManagerElement{ _uid, _typeFlags };
	m_uiElements.insert_or_assign(_uid, uiElement);

	// Initialize enabled list
	auto lst = serviceEnabledLevel(_serviceInfo);
	lst->insert_or_assign(_uid, 0);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				OT_LOG_D("Log applied on UID uiElementCreated");

				uiElement->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ak::aTreeWidget* _tree, const ot::LockTypeFlags& _typeFlags) {
	// Create new entrys
	LockManagerElement* newTree = new LockManagerElement(_tree, _typeFlags);
	m_specials.push_back(newTree);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				OT_LOG_D("Log applied on TeeWidget uiElementCreated");

				newTree->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::PropertyGrid* _propertyGrid, const ot::LockTypeFlags& _typeFlags) {
	// Create new entrys
	LockManagerElement* newProp = new LockManagerElement(_propertyGrid, _typeFlags);
	m_specials.push_back(newProp);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				OT_LOG_D("Log applied on propGrid uiElementCreated");
				newProp->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::GraphicsView* _graphicsView, const ot::LockTypeFlags& _typeFlags) {
	// Create new entrys
	LockManagerElement* newGraphics = new LockManagerElement(_graphicsView, _typeFlags);
	m_specials.push_back(newGraphics);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				OT_LOG_D("Log applied on graphicsView uiElementCreated");
				newGraphics->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::TextEditor* _text, const ot::LockTypeFlags& _typeFlags) {
	// Create new entrys
	LockManagerElement* newText = new LockManagerElement(_text, _typeFlags);
	m_specials.push_back(newText);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				OT_LOG_D("Log applied on textEditor uiElementCreated");
				newText->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::uiViewCreated(const ot::BasicServiceInformation& _serviceInfo, ot::WidgetView* _view, const ot::LockTypeFlags& _typeFlags) {
	// Create new entrys
	LockManagerElement* newView = new LockManagerElement(_view, _typeFlags);
	m_specials.push_back(newView);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				OT_LOG_D("Log applied on View created");

				newView->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::registerLockable(const ot::BasicServiceInformation& _serviceInfo, LockableWidget* _lockable, const ot::LockTypeFlags& _typeFlags) {
	// Create new entrys
	LockManagerElement* newLockable = new LockManagerElement(_lockable, _typeFlags);
	m_specials.push_back(newLockable);

	// Check existing locks
	for (auto itm : m_serviceToUiLockLevel) {
		for (auto lockLevel : *itm.second) {
			if (_typeFlags.flagIsSet(lockLevel.first) && lockLevel.second > 0) {
				OT_LOG_D("Log applied on register lockable");

				newLockable->lock(lockLevel.second, lockLevel.first);
			}
		}
	}
}

void LockManager::uiElementDestroyed(ot::UID _uid) {
	
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

void LockManager::uiElementDestroyed(ak::aTreeWidget* _tree) {
	for (LockManagerElement* element : m_specials) {
		if (element->getTree() == _tree) {
			this->removeSpeciaElement(element);
			return;
		}
	}
}

void LockManager::uiElementDestroyed(ot::PropertyGrid* _propertyGrid) {
	for (LockManagerElement* element : m_specials) {
		if (element->getProp() == _propertyGrid) {
			this->removeSpeciaElement(element);
			return;
		}
	}
}

void LockManager::uiElementDestroyed(ot::GraphicsView* _graphicsView) {
	for (LockManagerElement* element : m_specials) {
		if (element->getGraphics() == _graphicsView) {
			this->removeSpeciaElement(element);
			return;
		}
	}
}

void LockManager::uiElementDestroyed(ot::TextEditor* _text) {
	for (LockManagerElement* element : m_specials) {
		if (element->getText() == _text) {
			this->removeSpeciaElement(element);
			return;
		}
	}
}

void LockManager::uiViewDestroyed(ot::WidgetView* _view) {
	for (LockManagerElement* element : m_specials) {
		if (element->getView() == _view) {
			this->removeSpeciaElement(element);
			return;
		}
	}
}

void LockManager::deregisterLockable(LockableWidget* _lockable) {
	for (LockManagerElement* element : m_specials) {
		if (element->getLockable() == _lockable) {
			this->removeSpeciaElement(element);
			return;
		}
	}
}

void LockManager::lock(const ot::BasicServiceInformation& _serviceInfo, ot::LockTypeFlag _type) {
	OT_LOG_D("Lock applied on request from: " + _serviceInfo.serviceName() + " LogFlag:" + ot::toString(_type));
	auto service = serviceLockLevel(_serviceInfo);

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

	for (LockManagerElement* specialElement : m_specials) {
		specialElement->lock(1, _type);
	}

	if (_type & ot::LockAll) 
	{ 
		m_owner->setWaitingAnimationVisible(true); 
	}
}

void LockManager::lock(const ot::BasicServiceInformation& _serviceInfo, const ot::LockTypeFlags & _typeFlags) {

	
	if (_typeFlags.flagIsSet(ot::LockAll)) {
		lock(_serviceInfo, ot::LockAll);
	} 
	if (_typeFlags.flagIsSet(ot::LockModelWrite)) {
		lock(_serviceInfo, ot::LockModelWrite);
	}
	if (_typeFlags.flagIsSet(ot::LockModelRead)) {
		lock(_serviceInfo, ot::LockModelRead);
	}
	if (_typeFlags.flagIsSet(ot::LockNavigationAll)) {
		lock(_serviceInfo, ot::LockNavigationAll);
	}
	if (_typeFlags.flagIsSet(ot::LockNavigationWrite)) {
		lock(_serviceInfo, ot::LockNavigationWrite);
	}
	if (_typeFlags.flagIsSet(ot::LockProperties)) {
		lock(_serviceInfo, ot::LockProperties);
	}
	if (_typeFlags.flagIsSet(ot::LockViewRead)) {
		lock(_serviceInfo, ot::LockViewRead);
	}
	if (_typeFlags.flagIsSet(ot::LockViewWrite)) {
		lock(_serviceInfo, ot::LockViewWrite);
	}
}

void LockManager::unlock(const ot::BasicServiceInformation& _serviceInfo) {
	auto service = serviceLockLevel(_serviceInfo);
	OT_LOG_D("Unlock applied on request from: " + _serviceInfo.serviceName());

	for (auto lockVal : *service) {
		for (auto itm : m_uiElements) {
			itm.second->unlock(lockVal.second, lockVal.first);
		}

		for (LockManagerElement* specialElement : m_specials) {
			specialElement->unlock(lockVal.second, lockVal.first);
		}

		service->insert_or_assign(lockVal.first, 0);
	}

	if (lockLevel(ot::LockAll) <= 0) { m_owner->setWaitingAnimationVisible(false); }

	const std::string state = printLockState();
	OT_LOG_D("Log state after unlock:\n" + state);
}

void LockManager::unlock(const ot::BasicServiceInformation& _serviceInfo, ot::LockTypeFlag _type) {
	auto service = serviceLockLevel(_serviceInfo);
	OT_LOG_D("Unlock applied on request from: " + _serviceInfo.serviceName() + " LogFlag:" + ot::toString(_type));
	std::string state = printLockState();
	OT_LOG_D("Log state before unlock:\n" + state);
	// Get old value
	auto oldLevel = service->find(_type);
	int lockFlagCount = 0;
	if (oldLevel != service->end()) {
		lockFlagCount = oldLevel->second;
	}
	OT_LOG_D("Unlock old value found");
	// Store data
	if (lockFlagCount > 0) {
		OT_LOG_D("Unlocking. Lock count larger then 0");
		service->insert_or_assign(_type, lockFlagCount - 1);
		for (auto itm : m_uiElements) {
			itm.second->unlock(1, _type);
		}

		for (LockManagerElement* specialElement : m_specials) {
			specialElement->unlock(1, _type);
		}
	}
	else {
		OT_LOG_D("Unlocking. Lock count is 0");
		service->insert_or_assign(_type, lockFlagCount);
	}

	if (_type & ot::LockAll) {
		if (lockLevel(ot::LockAll) <= 0) { m_owner->setWaitingAnimationVisible(false); }
	}
	state = printLockState();
	OT_LOG_D("Log state after unlock:\n" + state);
}

void LockManager::unlock(const ot::BasicServiceInformation& _serviceInfo, const ot::LockTypeFlags & _typeFlags) {
	if (_typeFlags.flagIsSet(ot::LockAll)) {
		unlock(_serviceInfo, ot::LockAll);
	}
	if (_typeFlags.flagIsSet(ot::LockModelWrite)) {
		unlock(_serviceInfo, ot::LockModelWrite);
	}
	if (_typeFlags.flagIsSet(ot::LockModelRead)) {
		unlock(_serviceInfo, ot::LockModelRead);
	}
	if (_typeFlags.flagIsSet(ot::LockNavigationAll)) {
		unlock(_serviceInfo, ot::LockNavigationAll);
	}
	if (_typeFlags.flagIsSet(ot::LockNavigationWrite)) {
		unlock(_serviceInfo, ot::LockNavigationWrite);
	}
	if (_typeFlags.flagIsSet(ot::LockProperties)) {
		unlock(_serviceInfo, ot::LockProperties);
	}
	if (_typeFlags.flagIsSet(ot::LockViewRead)) {
		unlock(_serviceInfo, ot::LockViewRead);
	}
	if (_typeFlags.flagIsSet(ot::LockViewWrite)) {
		unlock(_serviceInfo, ot::LockViewWrite);
	}
}

void LockManager::disable(const ot::BasicServiceInformation& _serviceInfo, ot::UID _element) {
	auto service = serviceEnabledLevel(_serviceInfo);
	auto serviceEnabledVal = service->find(_element);
	if (serviceEnabledVal == service->end()) {
		service->insert_or_assign(_element, 1);
	}
	else {
		service->insert_or_assign(_element, serviceEnabledVal->second + 1);
	}
	LockManagerElement* element = this->uiElement(_element);
	if (element) {
		element->disable(1);
	}
}

void LockManager::enable(const ot::BasicServiceInformation& _serviceInfo, ot::UID _element, bool _resetCounter) {
	auto service = serviceEnabledLevel(_serviceInfo);
	auto serviceEnabledVal = service->find(_element);
	if (serviceEnabledVal == service->end()) {
		service->insert_or_assign(_element, 0);
	}
	else {
		int oldValue = serviceEnabledVal->second;
		if (oldValue > 0) {
			if (_resetCounter) {
				service->insert_or_assign(_element, 0);
				LockManagerElement* element = this->uiElement(_element);
				if (element) {
					element->enable(oldValue);
				}
			}
			else {
				service->insert_or_assign(_element, oldValue - 1);
				LockManagerElement* element = this->uiElement(_element);
				if (element) {
					element->enable(1);
				}
			}
		}
	}
}

void LockManager::cleanService(const ot::BasicServiceInformation& _serviceInfo, bool _reenableElement, bool _eraseUiElements) {
	auto serviceE = serviceEnabledLevel(_serviceInfo);
	auto serviceL = serviceLockLevel(_serviceInfo);
	OT_LOG_D("clean service due to: " + _serviceInfo.serviceName(), " reenableElement: " + std::to_string(_reenableElement) + " eraseUIElements: " + std::to_string(_eraseUiElements));
	for (auto e : *serviceE) {
		if (e.second > 0 && _reenableElement) {
			LockManagerElement* element = this->uiElement(e.first);
			if (element) {
				element->enable(e.second);
			}
		}
		if (_eraseUiElements) {
			auto uiItm = m_uiElements.find(e.first);
			if (uiItm != m_uiElements.end()) {
				delete uiItm->second;
				m_uiElements.erase(e.first);
			}
		}
	}
	OT_LOG_D("Clean service unlocks");
	for (auto l : *serviceL) {
		if (l.second > 0 && _reenableElement) {
			for (auto itm : m_uiElements) {
				itm.second->unlock(l.second, l.first);
			}
			
			for (LockManagerElement* specialElement : m_specials) {
				specialElement->unlock(l.second, l.first);
			}
		}
	}
	const std::string state =	printLockState();
	OT_LOG_D(state);

	delete serviceE;
	delete serviceL;
	m_serviceToUiEnabledLevel.erase(_serviceInfo);
	m_serviceToUiLockLevel.erase(_serviceInfo);

	if (lockLevel(ot::LockAll) <= 0) { 
		m_owner->setWaitingAnimationVisible(false);
	}
}

int LockManager::lockLevel(ot::LockTypeFlag _type) {
	int ret{ 0 };
	for (auto info : m_serviceToUiLockLevel) {
		for (auto lockInfo : *info.second) {
			if (lockInfo.first == _type) { ret += lockInfo.second; }
		}
	}
	return ret;
}

std::string LockManager::printLockState()
{
	std::string state("");
	for (auto flagCountByLogFlagByService : m_serviceToUiLockLevel)
	{
		state += "Service: " + flagCountByLogFlagByService.first.serviceName() + "\n";
		auto flagCountsByLogFlags = flagCountByLogFlagByService.second;
		for (auto& flagCountsByLogFlag : *flagCountsByLogFlags)
		{
			state += "	Flag: " + ot::toString(flagCountsByLogFlag.first) + " lock count: " + std::to_string(flagCountsByLogFlag.second) + "\n";
		}
		state += "\n";
	}

	return state;
}

// #######################################################################################################################

// Private functions

std::map<ot::LockTypeFlag, int> * LockManager::generateDefaultLockMap(void) const {
	// Create new entry
	std::map<ot::LockTypeFlag, int> * newMap{ new std::map<ot::LockTypeFlag, int> };

	// Insert initial data
	newMap->insert_or_assign(ot::LockAll, 0);
	newMap->insert_or_assign(ot::LockModelWrite, 0);
	newMap->insert_or_assign(ot::LockModelRead, 0);
	newMap->insert_or_assign(ot::LockNavigationAll, 0);
	newMap->insert_or_assign(ot::LockNavigationWrite, 0);
	newMap->insert_or_assign(ot::LockProperties, 0);
	newMap->insert_or_assign(ot::LockViewRead, 0);
	newMap->insert_or_assign(ot::LockViewWrite, 0);
	return newMap;
}

LockManagerElement * LockManager::uiElement(ot::UID _uid) {
	auto itm = m_uiElements.find(_uid);
	if (itm == m_uiElements.end()) {
		return nullptr;
	}
	else {
		return itm->second;
	}
}

std::map<ot::LockTypeFlag, int> * LockManager::serviceLockLevel(const ot::BasicServiceInformation& _serviceInfo) {
	auto itm = m_serviceToUiLockLevel.find(_serviceInfo);
	if (itm == m_serviceToUiLockLevel.end()) {
		auto newMap = new std::map<ot::LockTypeFlag, int>;
		m_serviceToUiLockLevel.insert_or_assign(_serviceInfo, newMap);
		return newMap;
	}
	else { return itm->second; }
}

std::map<ot::UID, int> * LockManager::serviceEnabledLevel(const ot::BasicServiceInformation& _serviceInfo) {
	auto itm = m_serviceToUiEnabledLevel.find(_serviceInfo);
	if (itm == m_serviceToUiEnabledLevel.end()) {
		auto newMap = new std::map<ot::UID, int>;
		m_serviceToUiEnabledLevel.insert_or_assign(_serviceInfo, newMap);
		return newMap;
	}
	else { return itm->second; }
}

void LockManager::removeSpeciaElement(LockManagerElement* _element) {
	auto it = std::find(m_specials.begin(), m_specials.end(), _element);
	if (it != m_specials.end()) {
		m_specials.erase(it);
	}
	else {
		OT_LOG_E("Unknown lock manager element");
	}
}

// #######################################################################################################################

// #######################################################################################################################

// #######################################################################################################################

ScopedLockManagerLock::ScopedLockManagerLock(LockManager* _manager, const ot::BasicServiceInformation& _serviceInfo, const ot::LockTypeFlags& _typeFlags)
	: m_skipUnlock(false), m_manager(_manager), m_service(_serviceInfo), m_flags(_typeFlags)
{
	OTAssertNullptr(m_manager);
	m_manager->lock(m_service, m_flags);
}

ScopedLockManagerLock::~ScopedLockManagerLock() {
	if (!m_skipUnlock) {
		OTAssertNullptr(m_manager);
		m_manager->unlock(m_service, m_flags);
	}
}

void ScopedLockManagerLock::setNoUnlock(void) {
	m_skipUnlock = true;
}

// #######################################################################################################################

// #######################################################################################################################

// #######################################################################################################################

LockManagerElement::LockManagerElement(ot::UID _uid, const ot::LockTypeFlags & _flags)
	: m_disabledCount(0), m_lockCount(0), m_lockTypes(_flags), m_uid(_uid), m_tree(nullptr), m_prop(nullptr), m_graphics(nullptr), m_text(nullptr), m_view(nullptr), m_lockable(nullptr)
{}

LockManagerElement::LockManagerElement(ak::aTreeWidget* _tree, const ot::LockTypeFlags& _flags) 
	: m_disabledCount(0), m_lockCount(0), m_lockTypes(_flags), m_uid(ak::invalidUID), m_tree(_tree), m_prop(nullptr), m_graphics(nullptr), m_text(nullptr), m_view(nullptr), m_lockable(nullptr)
{}

LockManagerElement::LockManagerElement(ot::PropertyGrid* _prop, const ot::LockTypeFlags& _flags)
	: m_disabledCount(0), m_lockCount(0), m_lockTypes(_flags), m_uid(ak::invalidUID), m_tree(nullptr), m_prop(_prop), m_graphics(nullptr), m_text(nullptr), m_view(nullptr), m_lockable(nullptr)
{}

LockManagerElement::LockManagerElement(ot::GraphicsView * _graphics, const ot::LockTypeFlags & _flags)
	: m_disabledCount(0), m_lockCount(0), m_lockTypes(_flags), m_uid(ak::invalidUID), m_tree(nullptr), m_prop(nullptr), m_graphics(_graphics), m_text(nullptr), m_view(nullptr), m_lockable(nullptr)
{}

LockManagerElement::LockManagerElement(ot::TextEditor* _text, const ot::LockTypeFlags& _flags)
	: m_disabledCount(0), m_lockCount(0), m_lockTypes(_flags), m_uid(ak::invalidUID), m_tree(nullptr), m_prop(nullptr), m_graphics(nullptr), m_text(_text), m_view(nullptr), m_lockable(nullptr)
{}

LockManagerElement::LockManagerElement(ot::WidgetView* _view, const ot::LockTypeFlags& _flags)
	: m_disabledCount(0), m_lockCount(0), m_lockTypes(_flags), m_uid(ak::invalidUID), m_tree(nullptr), m_prop(nullptr), m_graphics(nullptr), m_text(nullptr), m_view(_view), m_lockable(nullptr)
{}

LockManagerElement::LockManagerElement(LockableWidget * _lockable, const ot::LockTypeFlags & _flags) 
	: m_disabledCount(0), m_lockCount(0), m_lockTypes(_flags), m_uid(ak::invalidUID), m_tree(nullptr), m_prop(nullptr), m_graphics(nullptr), m_text(nullptr), m_view(nullptr), m_lockable(_lockable)
{}

void LockManagerElement::enable(int _value) {
	m_disabledCount -= _value;
	if (m_disabledCount < 0)
	{
		assert(0); // This control is enabled without being disabled before
		m_disabledCount = 0;
	}

	if (m_disabledCount == 0 && m_lockCount == 0) {
		if (m_tree) {
			m_tree->setEnabled(true);
		}
		else if (m_prop) {
			m_prop->getTreeWidget()->setEnabled(true);
		}
		else if (m_graphics) {
			m_graphics->setReadOnly(false);
		}
		else if (m_text) {
			m_text->setReadOnly(false);
		}
		else if (m_view) {
			m_view->getViewWidget()->setEnabled(true);
		}
		else if (m_lockable) {
			m_lockable->setWidgetLocked(false);
		}
		else {
			if (ak::uiAPI::object::exists(m_uid)) {
				OT_LOG_D("Enabling: " + std::to_string(m_uid));
				ak::uiAPI::object::setEnabled(m_uid, true);
			}
		}
	}
}

void LockManagerElement::disable(int _value) {
	m_disabledCount += _value;

	if (m_disabledCount > 0) {
		if (m_tree) {
			m_tree->setEnabled(false);
		}
		else if (m_prop) {
			m_prop->getTreeWidget()->setEnabled(false);
		}
		else if (m_graphics) {
			m_graphics->setReadOnly(true);
		}
		else if (m_text) {
			m_text->setReadOnly(true);
		}
		else if (m_view) {
			m_view->getViewWidget()->setEnabled(false);
		}
		else if (m_lockable) {
			m_lockable->setWidgetLocked(true);
		}
		else {
			OT_LOG_D("Disabling: " + std::to_string(m_uid));
			ak::uiAPI::object::setEnabled(m_uid, false);
		}
	}
}

void LockManagerElement::lock(int _value, ot::LockTypeFlag _lockType) {
	if (m_lockTypes.flagIsSet(_lockType)) {
		m_lockCount += _value;

		if (m_lockCount > 0) {
			if (m_tree) {
				if ((_lockType & ot::LockNavigationWrite) || (_lockType & ot::LockNavigationAll)) {
					m_tree->setIsReadOnly(true);
				}
			}
			else if (m_prop) {
				m_prop->getTreeWidget()->setEnabled(false);
			}
			else if (m_graphics) {
				m_graphics->setReadOnly(true);
			}
			else if (m_text) {
				m_text->setReadOnly(true);
			}
			else if (m_view) {
				m_view->getViewWidget()->setEnabled(false);
			}
			else if (m_lockable) {
				m_lockable->setWidgetLocked(true);
			}
			else {
				OT_LOG_D("Locking: " + std::to_string(m_uid));
				ak::uiAPI::object::setEnabled(m_uid, false);
			}
		}
	}
}

void LockManagerElement::unlock(int _value, ot::LockTypeFlag _lockType) {
	if (m_lockTypes.flagIsSet(_lockType)) {
		m_lockCount -= _value;

		if (m_lockCount < 0)
		{
			assert(0); // This control is unlocked without being locked before
			m_lockCount = 0;
		}

		if (m_lockCount == 0 && m_disabledCount == 0) {
			if (m_tree) {
				if ((_lockType & ot::LockNavigationWrite) || (_lockType & ot::LockNavigationAll)) {
					m_tree->setIsReadOnly(false);
				}
			}
			else if (m_prop) {
				m_prop->getTreeWidget()->setEnabled(true);
			}
			else if (m_graphics) {
				m_graphics->setReadOnly(false);
			}
			else if (m_text) {
				m_text->setReadOnly(false);
			}
			else if (m_view) {
				m_view->getViewWidget()->setEnabled(true);
			}
			else if (m_lockable) {
				m_lockable->setWidgetLocked(false);
			}
			else {
				OT_LOG_D("Unlocking: " + std::to_string(m_uid));
				ak::uiAPI::object::setEnabled(m_uid, true);
			}
		}
	}
}

