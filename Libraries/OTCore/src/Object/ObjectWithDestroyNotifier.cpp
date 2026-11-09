// @otlicense
// File: ObjectWithDestroyNotifier.cpp
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
