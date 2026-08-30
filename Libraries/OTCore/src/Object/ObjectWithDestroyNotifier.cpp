// @otlicense

// OpenTwin header
#include "OTCore/Object/ObjectDestroyNotifier.h"
#include "OTCore/Object/ObjectWithDestroyNotifier.h"

ot::ObjectWithDestroyNotifier::ObjectWithDestroyNotifier()
{}

ot::ObjectWithDestroyNotifier::~ObjectWithDestroyNotifier()
{
	std::list<ObjectDestroyNotifier*> notifiers = std::move(m_destroyNotifiers);
	m_destroyNotifiers.clear();
	for (auto notifier : notifiers)
	{
		notifier->onObjectDestroyed(this);
	}
}

void ot::ObjectWithDestroyNotifier::addDestroyNotifier(ObjectDestroyNotifier * _notifier)
{
	OTAssertNullptr(_notifier);
	OTAssert(std::find(m_destroyNotifiers.begin(), m_destroyNotifiers.end(), _notifier) == m_destroyNotifiers.end(), "Notifier already registered");
	m_destroyNotifiers.push_back(_notifier);
	_notifier->addRegisteredObject(this);
}

void ot::ObjectWithDestroyNotifier::removeDestroyNotifier(ObjectDestroyNotifier * _notifier)
{
	OTAssertNullptr(_notifier);
	OTAssert(std::find(m_destroyNotifiers.begin(), m_destroyNotifiers.end(), _notifier) != m_destroyNotifiers.end(), "Notifier not registered");
	m_destroyNotifiers.remove(_notifier);
	_notifier->removeRegisteredObject(this);
}

void ot::ObjectWithDestroyNotifier::notifyPreDestroy()
{
	for (auto notifier : m_destroyNotifiers)
	{
		notifier->onObjectPreDestroy(this);
	}
}

void ot::ObjectWithDestroyNotifier::removeAllDestroyNotifiers()
{
	std::list<ObjectDestroyNotifier*> notifiers = std::move(m_destroyNotifiers);
	m_destroyNotifiers.clear();
	for (auto notifier : notifiers)
	{
		notifier->removeRegisteredObject(this);
	}
}
