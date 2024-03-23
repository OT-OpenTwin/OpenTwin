/*
 * ControlsManager.h
 *
 *  Created on: February 20, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>

// Open twin header
#include "OTCommunication/UiTypes.h"
#include "OTCore/Flags.h"

// C++ header
#include <string>
#include <map>
#include <vector>

class LockManagerElement;
class AppBase;
namespace ot { class ServiceBase; }
namespace ak { class aTreeWidget; }
namespace ak { class aPropertyGridWidget; }

class ControlsManager {
public:
	ControlsManager() {}
	virtual ~ControlsManager();

	//! @brief Will store the control information
	//! @param _creator The creator of the control (must be unique for elements with events)
	//! @param _controlUid The UID of the control
	//! @param _elementHasEvents If false the creator of the element can not be determined further on
	void uiElementCreated(ot::ServiceBase * _creator, ak::UID _controlUid, bool _elementHasEvents = true);

	//! @brief Will destroy an UI control. Return false if failed to destroy (object still has child objects), or the object does not exist
	bool destroyUiControl(ak::UID _controlUid);
	void destroyUiControls(const std::vector<ak::UID> & _controlUids);
	void uiControlWasDestroyed(ak::UID _controlUid);
	void serviceDisconnected(ot::ServiceBase * _service);
	ot::ServiceBase * objectCreator(ak::UID);

private:
	std::map<ot::ServiceBase *, std::vector<ak::UID> * >	m_creatorMap;		//! Map containing: Service -> UI UID's
	std::map<ak::UID, ot::ServiceBase *>					m_uiToCreatorMap;	//! Map containing: UI UID -> Service

	ControlsManager(const ControlsManager &) = delete;
	ControlsManager& operator = (const ControlsManager &) = delete;
};

class LockManager {
public:
	LockManager(AppBase* _owner);
	virtual ~LockManager();

	//! @brief Will create a new entry for this 
	//! This function will be called from the controls manager automatically if the lock manager was set
	void uiElementCreated(ot::ServiceBase * _service, ak::UID _uid, const ot::Flags<ot::ui::lockType> & _typeFlags);
	void uiElementCreated(ot::ServiceBase * _service, ak::aTreeWidget* _tree, const ot::Flags<ot::ui::lockType> & _typeFlags);
	void uiElementCreated(ot::ServiceBase * _service, ak::aPropertyGridWidget* _propertyGrid, const ot::Flags<ot::ui::lockType> & _typeFlags);

	//! @brief Will remove all the stored information about the UI element
	//! This function will be called from the controls manager automatically if the lock manager was set
	void uiElementDestroyed(ak::UID _uid);

	void lock(ot::ServiceBase * _service, ot::ui::lockType _type);
	void lock(ot::ServiceBase * _service, const ot::Flags<ot::ui::lockType> & _typeFlags);

	void unlock(ot::ServiceBase * _service);
	void unlock(ot::ServiceBase * _service, ot::ui::lockType _type);
	void unlock(ot::ServiceBase * _service, const ot::Flags<ot::ui::lockType> & _typeFlags);

	void disable(ot::ServiceBase * _service, ak::UID _element);
	void enable(ot::ServiceBase * _service, ak::UID _element, bool _resetCounter = false);

	void cleanService(ot::ServiceBase * _service, bool _reenableElement = true, bool _eraseUiElement = false);

	int lockLevel(ot::ui::lockType _type);

private:

	std::map<ot::ui::lockType, int> * generateDefaultLockMap(void) const;
	LockManagerElement * uiElement(ak::UID _uid);
	std::map<ot::ui::lockType, int> * serviceLockLevel(ot::ServiceBase * _service);
	std::map<ak::UID, int> * serviceEnabledLevel(ot::ServiceBase * _service);
	
	AppBase *														m_owner;
	LockManagerElement* m_tree;
	LockManagerElement* m_prop;

	std::map<ot::ServiceBase *, std::map<ot::ui::lockType, int> *>	m_serviceToUiLockLevel;		// Contains a overview of sender to UI elements lock levels

	std::map<ot::ServiceBase *, std::map<ak::UID, int> *>	m_serviceToUiEnabledLevel;		// Contains all enbaled levels for every UI element related to a service

	std::map<ak::UID, LockManagerElement *>					m_uiElements;

	LockManager(const LockManager &) = delete;
	LockManager& operator = (const LockManager &) = delete;
};

class LockManagerElement {
public:
	LockManagerElement(ak::UID _uid, const ot::Flags<ot::ui::lockType> & _flags);
	LockManagerElement(ak::aTreeWidget* _tree, const ot::Flags<ot::ui::lockType> & _flags);
	LockManagerElement(ak::aPropertyGridWidget* _prop, const ot::Flags<ot::ui::lockType> & _flags);

	void enable(int _value);
	void disable(int _value);

	void lock(int _value, ot::ui::lockType _lockType);
	void unlock(int _value, ot::ui::lockType _lockType);


private:

	ak::aTreeWidget* m_tree;
	ak::aPropertyGridWidget* m_prop;
	ak::UID							m_uid;
	ot::Flags<ot::ui::lockType>		m_lockTypes;
	int								m_disabledCount;
	int								m_lockCount;

	LockManagerElement() = delete;
	LockManagerElement(const LockManagerElement &) = delete;
	LockManagerElement & operator = (const LockManagerElement &) = delete;
};