// @otlicense
// File: aObjectManager.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// C++ header
#include <string>
#include <Windows.h>

// AK Core header
#include <akCore/akCore.h>
#include <akCore/aMessenger.h>
#include <akCore/aNotifierObjectManager.h>
#include <akCore/aObject.h>
#include <akCore/aUidMangager.h>

// AK GUI header
#include <akGui/aAction.h>
#include <akGui/aObjectManager.h>
#include <akGui/aSignalLinker.h>
#include <akGui/aTimer.h>

// AK Widgets header
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aNiceLineEditWidget.h>
#include <akWidgets/aToolButtonWidget.h>
#include <akWidgets/aTtbGroup.h>
#include <akWidgets/aTtbPage.h>
#include <akWidgets/aTtbSubgroup.h>
#include <akWidgets/aWidget.h>
#include <akWidgets/aWindowManager.h>

#define SETTING_VERSION_WINDOWSTATE "Version-WindowState"
#define SETTING_VERSION_APPLICATION "Version-Application"
#define CONFIG_VERSION_WINDOWSTATE "2.0"

#ifdef _DEBUG
#define DEBUG_OUT(__message)											\
			if (getenv("UICORE_API_DEBUG") != nullptr)					\
			{															\
				if (getenv("UICORE_API_DEBUG")[0] == '1') {				\
						OutputDebugStringA(__message);					\
				}														\
			}															
#else
#define DEBUG_OUT(__message)
#endif // _DEBUG



ak::aObjectManager::aObjectManager(
	aMessenger *										_messenger,
	aUidManager *									_uidManager
)
	: m_messenger(nullptr),
	m_signalLinker(nullptr),
	m_uidManager(nullptr),
	m_notifier(nullptr)
{
	// Check parameter
	assert(_messenger != nullptr); // nullptr provided
	assert(_uidManager != nullptr); // nullptr provided

	// Apply settings
	m_messenger = _messenger;
	m_uidManager = _uidManager;

	// Create signal linker
	m_signalLinker = new aSignalLinker(m_messenger, m_uidManager);

	// Create notifier
	m_notifier = new aNotifierObjectManager(this);
}

ak::aObjectManager::~aObjectManager() {}

// ###############################################################################################################################################

// Object creation

ak::UID ak::aObjectManager::createAction(
	UID												_creatorUid,
	const QString &										_text,
	const QIcon &										_icon,
	QToolButton::ToolButtonPopupMode					_popupMode
) {
	// Create object
	aAction * obj = new aAction(_icon, _text, _popupMode);
	// Set parameter
	obj->setToolTip("");
	obj->setWhatsThis("");
	m_signalLinker->addLink(obj);
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createCheckBox(
	UID												_creatorUid,
	const QString &										_text,
	bool												_checked
) {
	// Create object
	aCheckBoxWidget * obj = new aCheckBoxWidget(_text);
	// Set parameter
	obj->setChecked(_checked);
	m_signalLinker->addLink(obj);
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createLineEdit(
	UID													_creatorUid,
	const QString &											_initialText
) {
	// Create object
	aLineEditWidget * obj = new aLineEditWidget(_initialText);
	// Connect to signal linker
	m_signalLinker->addLink(obj);
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createNiceLineEdit(
	UID												_creatorUid,
	const QString &										_initialText,
	const QString &										_infoLabelText
) {
	// Create object
	aNiceLineEditWidget * obj = new aNiceLineEditWidget(_initialText, _infoLabelText);
	// Set parameter
	m_signalLinker->addLink(obj);
	obj->setUid(m_uidManager->getId());
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createTimer(
	UID												_creatorUid
) {
	// Create object
	aTimer * obj = new aTimer();
	// Set parameter
	m_signalLinker->addLink(obj);
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createToolButton(
	UID												_creatorUid
) {
	// Create object
	aToolButtonWidget * obj = new aToolButtonWidget();
	// Set parameter
	m_signalLinker->addLink(obj);
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createToolButton(
	UID												_creatorUid,
	const QString &										_text
) {
	// Create object
	aToolButtonWidget * obj = new aToolButtonWidget(_text);
	// Set parameter
	m_signalLinker->addLink(obj);
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createToolButton(
	UID												_creatorUid,
	const QString &										_text,
	const QIcon &										_icon
) {
	// Create object
	aToolButtonWidget * obj = new aToolButtonWidget(_icon, _text);
	// Set parameter
	m_signalLinker->addLink(obj);
	// Store data
	m_mapObjects.insert_or_assign(obj->uid(), obj);
	addCreatedUid(_creatorUid, obj->uid());
	return obj->uid();
}

ak::UID ak::aObjectManager::createTabToolBarSubContainer(
	UID												_creatorUid,
	UID												_parentUid,
	const QString &										_text
) {
	// Find parent object
	auto itm = m_mapObjects.find(_parentUid);
	assert(itm != m_mapObjects.end()); // Invalid object UID provided
	// Check object type
	if (itm->second->type() != otTabToolbarPage &&
		itm->second->type() != otTabToolbarGroup &&
		itm->second->type() != otTabToolbarSubgroup
		) {
		// Check if the provided object is located at a main window
		if (itm->second->type() == otMainWindow) {
			return createTabToolBarPage(_creatorUid, _parentUid, _text);
		}
		else {
			assert(0); // Invalid object type
			return invalidUID;
		}
	}
	// Cast container
	aTtbContainer * cont = nullptr;
	cont = dynamic_cast<aTtbContainer *>(itm->second);
	assert(cont != nullptr); // Cast failed

	aTtbContainer * sub = cont->getSubContainer(_text);

	if (sub == nullptr) {
		// Create subcontainer
		sub = cont->createSubContainer(_text);
		assert(sub != nullptr); // Invalid information received
		sub->setUid(m_uidManager->getId());
		// Store data
		m_mapObjects.insert_or_assign(sub->uid(), sub);
		addCreatedUid(_creatorUid, sub->uid());

		sub->setParentObject(cont);
		cont->addChildObject(sub);
	}
	return sub->uid();
}

ak::UID ak::aObjectManager::createTabToolBarPage(
	UID												_creatorUid,
	UID												_uiManagerUid,
	const QString &										_text
) {
	// Find object
	auto itm = m_mapObjects.find(_uiManagerUid);
	assert(itm != m_mapObjects.end()); // Invalid object UID provided
	// Check object type
	assert(itm->second->type() == otMainWindow); // Invalid object type provided
	// Cast ui
	aWindowManager * ui = nullptr;
	ui = dynamic_cast<aWindowManager *>(itm->second);
	assert(ui != nullptr); // Cast failed

	aTtbContainer * cont = ui->getTabToolBarSubContainer(_text);
	if (cont == nullptr) {
		// Create container
		cont = ui->createTabToolbarSubContainer(_text);
		assert(cont != nullptr); // Invalid information received
		cont->setUid(m_uidManager->getId());
		// Store data
		m_mapObjects.insert_or_assign(cont->uid(), cont);
		addCreatedUid(_creatorUid, cont->uid());

		cont->setParentObject(ui);
		ui->addChildObject(cont);
	}
	return cont->uid();
}

ak::UID ak::aObjectManager::createWindow(
	UID												_creatorUid
) {
	// Create new ui manager
	aWindowManager * ui = nullptr;
	ui = new aWindowManager(m_messenger, m_uidManager);
	// Store data
	m_mapObjects.insert_or_assign(ui->uid(), ui);
	addCreatedUid(_creatorUid, ui->uid());
	return ui->uid();
}

// ###############################################################################################################################################

ak::aObject * ak::aObjectManager::object(
	UID												_objectUid
) {
	auto itm = m_mapObjects.find(_objectUid);
	if (itm == m_mapObjects.end()) { assert(0); throw ObjectNotFoundException(); }
	return itm->second;
}

ak::aObject * ak::aObjectManager::object(
	const QString &									_objectUniqueName
) {
	auto itm = m_mapUniqueNames.find(_objectUniqueName);
	if (itm == m_mapUniqueNames.end()) { assert(0); throw ObjectNotFoundException(); }
	return itm->second;
}

// ###############################################################################################################################################

void ak::aObjectManager::creatorDestroyed(
	UID												_creatorUid
) {
	try {
		m_notifier->disable();
		// Get all UIDs created by this creator
		auto itm = m_mapCreators.find(_creatorUid);
		if (itm != m_mapCreators.end()) {

			bool erased{ true };
			while (erased) {
				erased = false;

				// Get and destroy every single object created by this creator
				for (int i = 0; i < itm->second->size() && !erased; i++) {
					destroy(itm->second->at(i), true);
					if (!objectExists(itm->second->at(i))) {
						itm->second->erase(itm->second->begin() + i);
						erased = true;
						break;
					}
				}

			}

			// Delete vector
			std::vector<UID> *	r = itm->second;
			delete r;
			itm->second = nullptr;
			// Remove creator UID
			m_mapCreators.erase(_creatorUid);
		}
		m_notifier->enable();
	}
	catch (const std::exception & e) {
		assert(0);	// Something went wrong
	}
	catch (...) {
		assert(0);	// Something went horribly wrong
	}
}

void ak::aObjectManager::destroy(
	UID												_objectUID,
	bool												_ignoreIfObjectHasChildObjects
) {
	m_notifier->disable();

	auto object = m_mapObjects.find(_objectUID);
	assert(object != m_mapObjects.end());	// Invalid object UID

	if (object != m_mapObjects.end()) // Avoid a UI crash if something goes wrong here
	{
		aObject * actualObject = object->second;

		if (_ignoreIfObjectHasChildObjects) {
			if (actualObject->childObjectCount() != 0) { return; }
			switch (actualObject->type())
			{
			case otTabToolbarPage:
			case otTabToolbarGroup:
			case otTabToolbarSubgroup:
			{
				aTtbContainer * container = nullptr;
				container = dynamic_cast<aTtbContainer *>(actualObject);
				assert(container != nullptr); // Cast failed
				if (container->subContainerCount() != 0) { return; }
			}
			break;
			}
		}

#ifdef _DEBUG
		std::string _debugMessage{ "[UICORE] [DEBUG] [OBJECT MANAGER]: Object destroyed (UID:" };
		_debugMessage.append(std::to_string(_objectUID)).append("; Type: ");
		_debugMessage.append(toQString(m_mapObjects.find(_objectUID)->second->type()).toStdString()).append(")\n");
		DEBUG_OUT(_debugMessage.c_str());
#endif // _DEBUG

		m_mapOwners.erase(_objectUID);
		m_mapObjects.erase(_objectUID);

		// Remove the unique name access to this object
		if (!actualObject->uniqueName().isEmpty()) {
			m_mapUniqueNames.erase(actualObject->uniqueName());
		}

		// Destroy object
		delete actualObject;
	}

	m_notifier->enable();
}

void ak::aObjectManager::destroyAll(void) {
	if (m_signalLinker != nullptr) { delete m_signalLinker; m_signalLinker = nullptr; }
	m_signalLinker = new aSignalLinker(m_messenger, m_uidManager);

	auto itm = m_mapObjects.begin();

	// Destroy all objects that are not a main window
	while (itm != m_mapObjects.end()) {
		ak::aObject * obj = itm->second;
		if (obj->type() != ak::otMainWindow) {
			// Erase item
			m_mapObjects.erase(obj->uid());
			// Destroy all objects that were created by this item
			creatorDestroyed(obj->uid());
			delete obj;
			itm = m_mapObjects.begin();
		}
		else {
			creatorDestroyed(obj->uid()); itm++;
		}
	}
	for (itm = m_mapObjects.begin(); itm != m_mapObjects.end(); itm++) {
		ak::aObject * obj = itm->second;
		if (obj != nullptr) { delete obj; }
	}
	m_mapCreators.clear();
	m_mapObjects.clear();
	m_mapUniqueNames.clear();
}

void ak::aObjectManager::setObjectUniqueName(
	UID												_objectUid,
	const QString &										_uniqueName
) {
	auto actualObject{ object(_objectUid) };
	assert(actualObject != nullptr);	// The object was not created

	auto duplicateEntry{ m_mapUniqueNames.find(_uniqueName) };
	if (duplicateEntry != m_mapUniqueNames.end()) {
		if (duplicateEntry->second == actualObject) { return; }
		assert(0); // Another object with the same unique name does already exist
	}

	// Remove the old unique name
	if (!actualObject->uniqueName().isEmpty()) { m_mapUniqueNames.erase(actualObject->uniqueName()); }
	
	// Store information
	actualObject->setUniqueName(_uniqueName);
	if (!_uniqueName.isEmpty()) {
		m_mapUniqueNames.insert_or_assign(_uniqueName, actualObject);
	}

#ifdef _DEBUG
	{
		std::string _debugMessage{ "[UICORE] [DEBUG] [OBJECT MANAGER]: Object unique name set (Name:" };
		_debugMessage.append(_uniqueName.toStdString());
		_debugMessage.append("; UID: ");
		_debugMessage.append(std::to_string(actualObject->uid()));
		_debugMessage.append(")\n");
		DEBUG_OUT(_debugMessage.c_str());
	}
#endif // _DEBUG
}

/*
void ak::aObjectManager::removeAlias(
	const QString &										_alias
) {
#ifdef _DEBUG
	{
		std::string _debugMessage{ "[UICORE] [DEBUG] [OBJECT MANAGER]: Alias removed (Alias:" };
		_debugMessage.append(_alias.toStdString());
		_debugMessage.append("; UID: ");
		auto _obj = m_mapAliases.find(_alias);
		if (_obj != m_mapAliases.end()) {
			_debugMessage.append(std::to_string(_obj->second));
			auto _aObj = m_mapObjects.find(_obj->second);
		}
		_debugMessage.append(")\n");
		DEBUG_OUT(_debugMessage.c_str());
	}
#endif // _DEBUG

	m_mapAliases.erase(_alias);
}
*/

ak::UID ak::aObjectManager::objectCreator(
	UID												_objectUID
) {
	auto owner = m_mapOwners.find(_objectUID);
	assert(owner != m_mapOwners.end());	// Invalid UID
	return owner->second;
}

bool ak::aObjectManager::objectExists(
	UID												_objectUID
) {
	auto itm{ m_mapObjects.find(_objectUID) };
	return !(itm == m_mapObjects.end());
}

// ###############################################################################################################################################

void ak::aObjectManager::addCreatedUid(
	UID												_creatorUid,
	UID												_createdUid
) {
	auto itm = m_mapCreators.find(_creatorUid);
	if (itm == m_mapCreators.end()) {
		// First object created by this creator
		std::vector<UID> *	v = nullptr;
		v = new std::vector<UID>;

		// Store data
		v->push_back(_createdUid);
		m_mapCreators.insert_or_assign(_creatorUid, v);
	}
	else {
		// Object(s) already created by this creator before

		// Check for duplicate
		for (int i = 0; i < itm->second->size(); i++) {
			assert(itm->second->at(i) != _createdUid); // UID already stored
		}
		// Store data
		itm->second->push_back(_createdUid);
	}

	// Store owner information
	assert(m_mapOwners.find(_createdUid) == m_mapOwners.end());	// Created UID already stored
	m_mapOwners.insert_or_assign(_createdUid, _creatorUid);

#ifdef _DEBUG
	std::string _debugMessage{ "[UICORE] [DEBUG] [OBJECT MANAGER]: Object created (UID:" };
	_debugMessage.append(std::to_string(_createdUid)).append("; Type: ");
	_debugMessage.append(toQString(m_mapObjects.find(_createdUid)->second->type()).toStdString()).append(")\n");
	DEBUG_OUT(_debugMessage.c_str());
#endif // _DEBUG
}

QWidget * ak::aObjectManager::castToWidget(
	UID												_objectUid
) {
	// Get widget
	auto obj = m_mapObjects.find(_objectUid);
	assert(obj != m_mapObjects.end()); // Invalid object UID
	assert(obj->second->isWidgetType()); // Object is not a widget
	ak::aWidget * widget = nullptr;
	widget = dynamic_cast<ak::aWidget *>(obj->second);
	assert(widget != nullptr); // Cast failed
	return widget->widget();
}

ak::ObjectNotFoundException::ObjectNotFoundException() : std::exception("Object not found") {}
