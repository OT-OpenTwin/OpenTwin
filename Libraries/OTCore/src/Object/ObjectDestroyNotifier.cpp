// @otlicense

// OpenTwin header
#include "OTCore/Object/ObjectDestroyNotifier.h"
#include "OTCore/Object/ObjectWithDestroyNotifier.h"

ot::ObjectDestroyNotifier::ObjectDestroyNotifier()
{}

ot::ObjectDestroyNotifier::~ObjectDestroyNotifier()
{}

void ot::ObjectDestroyNotifier::addRegisteredObject(ObjectWithDestroyNotifier * _object)
{
	OTAssertNullptr(_object);
	OTAssert(std::find(m_registeredObjects.begin(), m_registeredObjects.end(), _object) == m_registeredObjects.end(), "Object already registered");
	m_registeredObjects.push_back(_object);
}

void ot::ObjectDestroyNotifier::removeRegisteredObject(ObjectWithDestroyNotifier * _object)
{
	OTAssertNullptr(_object);
	OTAssert(std::find(m_registeredObjects.begin(), m_registeredObjects.end(), _object) != m_registeredObjects.end(), "Object not registered");
	m_registeredObjects.remove(_object);
}
