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
#include "OTSystem/Flags.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GuiTypes.h"

// C++ header
#include <string>
#include <map>
#include <vector>

class LockManagerElement;
class AppBase;
namespace ak { class aTreeWidget; }
namespace ot { class PropertyGrid; }
namespace ot { class GraphicsView; }
namespace ot { class WidgetView; }
namespace ot { class TextEditor; }

class ControlsManager {
public:
	ControlsManager() {}
	virtual ~ControlsManager();

	//! @brief Will store the control information
	//! @param _creator The creator of the control (must be unique for elements with events)
	//! @param _controlUid The UID of the control
	//! @param _elementHasEvents If false the creator of the element can not be determined further on
	void uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::UID _controlUid, bool _elementHasEvents = true);

	//! @brief Will destroy an UI control. Return false if failed to destroy (object still has child objects), or the object does not exist
	bool destroyUiControl(ot::UID _controlUid);
	void destroyUiControls(const std::vector<ot::UID> & _controlUids);
	void uiControlWasDestroyed(ot::UID _controlUid);
	void serviceDisconnected(const ot::BasicServiceInformation& _serviceInfo);
	ot::BasicServiceInformation objectCreator(ot::UID);

private:
	std::map<ot::BasicServiceInformation, std::vector<ot::UID> * >	m_creatorMap;		//! Map containing: Service -> UI UID's
	std::map<ot::UID, ot::BasicServiceInformation>					m_uiToCreatorMap;	//! Map containing: UI UID -> Service

	ControlsManager(const ControlsManager &) = delete;
	ControlsManager& operator = (const ControlsManager &) = delete;
};

class LockableWidget {
	OT_DECL_NOCOPY(LockableWidget)
public:
	LockableWidget() {};
	virtual ~LockableWidget() {};

	virtual void setWidgetLocked(bool _isLocked) = 0;
};

class LockManager {
public:
	LockManager(AppBase* _owner);
	virtual ~LockManager();

	//! @brief Will create a new entry for this 
	//! This function will be called from the controls manager automatically if the lock manager was set
	void uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::UID _uid, const ot::LockTypeFlags & _typeFlags);
	void uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ak::aTreeWidget* _tree, const ot::LockTypeFlags & _typeFlags);
	void uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::PropertyGrid* _propertyGrid, const ot::LockTypeFlags & _typeFlags);
	void uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::GraphicsView* _graphicsView, const ot::LockTypeFlags & _typeFlags);
	void uiElementCreated(const ot::BasicServiceInformation& _serviceInfo, ot::TextEditor* _text, const ot::LockTypeFlags & _typeFlags);
	void uiViewCreated(const ot::BasicServiceInformation& _serviceInfo, ot::WidgetView* _view, const ot::LockTypeFlags & _typeFlags);
	void registerLockable(const ot::BasicServiceInformation& _serviceInfo, LockableWidget* _lockable, const ot::LockTypeFlags & _typeFlags);

	//! @brief Will remove all the stored information about the UI element
	//! This function will be called from the controls manager automatically if the lock manager was set
	void uiElementDestroyed(ot::UID _uid);
	void uiElementDestroyed(ak::aTreeWidget* _tree);
	void uiElementDestroyed(ot::PropertyGrid* _propertyGrid);
	void uiElementDestroyed(ot::GraphicsView* _graphicsView);
	void uiElementDestroyed(ot::TextEditor* _text);
	void uiViewDestroyed(ot::WidgetView* _view);
	void deregisterLockable(LockableWidget* _lockable);

	void lock(const ot::BasicServiceInformation& _serviceInfo, ot::LockTypeFlag _type);
	void lock(const ot::BasicServiceInformation& _serviceInfo, const ot::LockTypeFlags & _typeFlags);

	void unlock(const ot::BasicServiceInformation& _serviceInfo);
	void unlock(const ot::BasicServiceInformation& _serviceInfo, ot::LockTypeFlag _type);
	void unlock(const ot::BasicServiceInformation& _serviceInfo, const ot::LockTypeFlags & _typeFlags);

	void disable(const ot::BasicServiceInformation& _serviceInfo, ot::UID _element);
	void enable(const ot::BasicServiceInformation& _serviceInfo, ot::UID _element, bool _resetCounter = false);

	void cleanService(const ot::BasicServiceInformation& _serviceInfo, bool _reenableElement = true, bool _eraseUiElement = false);

	int lockLevel(ot::LockTypeFlag _type);

private:
	std::map<ot::LockTypeFlag, int> * generateDefaultLockMap(void) const;
	LockManagerElement * uiElement(ot::UID _uid);
	std::map<ot::LockTypeFlag, int> * serviceLockLevel(const ot::BasicServiceInformation& _serviceInfo);
	std::map<ot::UID, int> * serviceEnabledLevel(const ot::BasicServiceInformation& _serviceInfo);
	void removeSpeciaElement(LockManagerElement* _element);

	AppBase *														m_owner;
	std::list<LockManagerElement*> m_specials;

	std::map<ot::BasicServiceInformation, std::map<ot::LockTypeFlag, int> *>	m_serviceToUiLockLevel;		// Contains a overview of sender to UI elements lock levels

	std::map<ot::BasicServiceInformation, std::map<ot::UID, int> *>	m_serviceToUiEnabledLevel;		// Contains all enbaled levels for every UI element related to a service

	std::map<ot::UID, LockManagerElement *>					m_uiElements;

	LockManager(const LockManager &) = delete;
	LockManager& operator = (const LockManager &) = delete;
};

class ScopedLockManagerLock {
	OT_DECL_NOCOPY(ScopedLockManagerLock)
	OT_DECL_NODEFAULT(ScopedLockManagerLock)
public:
	ScopedLockManagerLock(LockManager* _manager, const ot::BasicServiceInformation& _serviceInfo, const ot::LockTypeFlags& _typeFlags);
	~ScopedLockManagerLock();

	//! @brief If set the lock flags won't be unset when destroying.
	void setNoUnlock(void);

private:
	bool m_skipUnlock;
	LockManager* m_manager;
	ot::BasicServiceInformation m_service;
	ot::LockTypeFlags m_flags;
};

class LockManagerElement {
public:
	LockManagerElement(ot::UID _uid, const ot::LockTypeFlags & _flags);
	LockManagerElement(ak::aTreeWidget* _tree, const ot::LockTypeFlags & _flags);
	LockManagerElement(ot::PropertyGrid* _prop, const ot::LockTypeFlags & _flags);
	LockManagerElement(ot::GraphicsView* _graphics, const ot::LockTypeFlags & _flags);
	LockManagerElement(ot::TextEditor* _text, const ot::LockTypeFlags & _flags);
	LockManagerElement(ot::WidgetView* _view, const ot::LockTypeFlags & _flags);
	LockManagerElement(LockableWidget* _lockable, const ot::LockTypeFlags & _flags);

	void enable(int _value);
	void disable(int _value);

	void lock(int _value, ot::LockTypeFlag _lockType);
	void unlock(int _value, ot::LockTypeFlag _lockType);

	ak::aTreeWidget* getTree(void) const { return m_tree; };
	ot::PropertyGrid* getProp(void) const { return m_prop; };
	ot::GraphicsView* getGraphics(void) const { return m_graphics; };
	ot::TextEditor* getText(void) const { return m_text; };
	ot::WidgetView* getView(void) const { return m_view; };
	LockableWidget* getLockable(void) const { return m_lockable; };

private:

	ak::aTreeWidget* m_tree;
	ot::PropertyGrid* m_prop;
	ot::GraphicsView* m_graphics;
	ot::TextEditor* m_text;
	ot::WidgetView* m_view;
	LockableWidget* m_lockable;
	ot::UID							m_uid;
	ot::LockTypeFlags		m_lockTypes;
	int								m_disabledCount;
	int								m_lockCount;

	LockManagerElement() = delete;
	LockManagerElement(const LockManagerElement &) = delete;
	LockManagerElement & operator = (const LockManagerElement &) = delete;
};