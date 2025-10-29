// @otlicense

// AK header
#include <akCore/aObject.h>
#include <akCore/aException.h>

ak::aObject::aObject(objectType _type, UID _uid)//, int _references)
	: m_uid(_uid), m_references(0), m_objectType(_type), m_parentObject(nullptr), m_owner(nullptr)
{}

ak::aObject::aObject(
	const ak::aObject &			_other
) {
	m_uid = _other.uid();
	m_objectType = _other.type();
}

ak::aObject & ak::aObject::operator = (
	const ak::aObject &			_other
) {
	m_uid = _other.uid();
	m_objectType = _other.type();
	return *this;
}

ak::aObject::~aObject() {}

// ################################################################################

void ak::aObject::removeChildObject(
	aObject *								_child
) {
	assert(_child != nullptr); // Nullptr provided
	assert(m_childObjects.find(_child->uid()) != m_childObjects.end()); // Child is not registered
	m_childObjects.erase(_child->uid());
}

void ak::aObject::addChildObject(
	aObject *								_child
) {
	assert(_child != nullptr);
	assert(m_childObjects.find(_child->uid()) == m_childObjects.end()); // Child with the specified UID already exists
	m_childObjects.insert_or_assign(_child->uid(), _child);
}

// ################################################################################

void ak::aObject::setParentObject(
	aObject *								_parentObject
) { m_parentObject = _parentObject; }

ak::aObject * ak::aObject::parentObject(void) const { return m_parentObject; }

int ak::aObject::childObjectCount(void) const { return m_childObjects.size(); }

ak::aObject * ak::aObject::childObject(
	UID									_childUID
) {
	auto itm = m_childObjects.find(_childUID);
	assert(itm != m_childObjects.end()); // Invalid object UID
	assert(itm->second != nullptr);	// Nullptr stored
	return itm->second;
}

void ak::aObject::setOwner(
	aObject *								_object
) { m_owner = _object; }

ak::aObject * ak::aObject::owner(void) const { return m_owner; }

// ################################################################################

void ak::aObject::setUid(UID _UID) { m_uid = _UID; }

ak::UID ak::aObject::uid(void) const { return m_uid; }

ak::objectType ak::aObject::type(void) const { return m_objectType; }

bool ak::aObject::isWidgetType(void) const { return false; }