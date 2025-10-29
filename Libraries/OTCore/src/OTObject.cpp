// @otlicense

//! @file OTObject.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTObject.h"

ot::OTObject::OTObject(OTObject* _parentObject) : m_parentObject(_parentObject) {

}

ot::OTObject::~OTObject() {
	// Parent
	if (m_parentObject) {
		m_parentObject->forgetObject(this);
	}
	m_parentObject = nullptr;

	// Delete notifier
	for (OTObject* obj : m_deleteNotifier) {
		obj->objectWasDestroyed(this);
	}
	m_deleteNotifier.clear();

	// Watched
	for (OTObject* obj : m_watchedObjects) {
		obj->forgetObject(this);
	}
	m_watchedObjects.clear();

	// Childs
	for (OTObject* obj : m_childObjects) {
		obj->m_parentObject = nullptr;
		delete obj;
	}
	m_childObjects.clear();
}

void ot::OTObject::setParentOTObject(OTObject* _newParent) {
	if (_newParent == m_parentObject) {
		return;
	}

	if (m_parentObject) {
		m_parentObject->forgetChildObject(this);
	}

	m_parentObject = _newParent;

	if (m_parentObject) {
		m_parentObject->registerChildObject(this);
	}
}

void ot::OTObject::registerDeleteNotifier(OTObject* _notifier) {
	auto deleteIt = std::find(m_deleteNotifier.begin(), m_deleteNotifier.end(), _notifier);
	if (deleteIt == m_deleteNotifier.end()) {
		m_deleteNotifier.push_back(_notifier);
	}
}

void ot::OTObject::removeDeleteNotifier(OTObject* _notifier) {
	auto deleteIt = std::find(m_deleteNotifier.begin(), m_deleteNotifier.end(), _notifier);
	if (deleteIt != m_deleteNotifier.end()) {
		m_deleteNotifier.erase(deleteIt);
	}
}

ot::OTObject* ot::OTObject::findChildOTObject(const std::string& _objectName) {
	OTObject* result = nullptr;
	for (OTObject* obj : m_childObjects) {
		if (obj->getOTObjectName() == _objectName) {
			result = obj;
			break;
		}
		else {
			result = obj->findChildOTObject(_objectName);
			if (result) {
				break;
			}
		}
	}

	return result;
}

const ot::OTObject* ot::OTObject::findChildOTObject(const std::string& _objectName) const {
	const OTObject* result = nullptr;
	for (const OTObject* obj : m_childObjects) {
		if (obj->getOTObjectName() == _objectName) {
			result = obj;
			break;
		}
		else {
			result = obj->findChildOTObject(_objectName);
			if (result) {
				break;
			}
		}
	}

	return result;
}

void ot::OTObject::registerChildObject(OTObject* _object) {
	auto childIt = std::find(m_childObjects.begin(), m_childObjects.end(), _object);
	if (childIt == m_childObjects.end()) {
		m_childObjects.push_back(_object);
	}
}

void ot::OTObject::forgetChildObject(OTObject* _object) {
	auto childIt = std::find(m_childObjects.begin(), m_childObjects.end(), _object);
	if (childIt != m_childObjects.end()) {
		m_childObjects.erase(childIt);
	}
}

void ot::OTObject::forgetObject(OTObject* _object) {
	this->forgetChildObject(_object);
	this->removeDeleteNotifier(_object);

	auto watchedIt = std::find(m_watchedObjects.begin(), m_watchedObjects.end(), _object);
	if (watchedIt != m_watchedObjects.end()) {
		m_watchedObjects.erase(watchedIt);
	}
}
