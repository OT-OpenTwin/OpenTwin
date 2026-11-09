// @otlicense
// File: ObjectWithDestroyNotifier.h
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

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <list>

namespace ot
{
	class ObjectDestroyNotifier;

	class OT_CORE_API_EXPORT ObjectWithDestroyNotifier
	{
		OT_DECL_NOCOPY(ObjectWithDestroyNotifier)
		OT_DECL_NOMOVE(ObjectWithDestroyNotifier)
	public:
		ObjectWithDestroyNotifier();
		virtual ~ObjectWithDestroyNotifier();

		//! @brief Registers a notifier to be notified when this object is destroyed.
		//! The notifier will be notified when this object is destroyed.
		//! The notifier will be removed from the list of notifiers when it is destroyed.
		//! The caller keeps the ownership of the notifier.
		void addDestroyNotifier(ObjectDestroyNotifier* _notifier);

		//! @brief Unregisters a notifier from the list of notifiers.
		//! The object will deregister from the notifier.
		void removeDestroyNotifier(ObjectDestroyNotifier* _notifier);

		//! @brief Notifies all registered notifiers that this object is about to be destroyed.
		//! This should be called before the object is destroyed to allow the notifiers to 
		//! perform any necessary cleanup or actions before the actual object is destroyed.
		void notifyPreDestroy();

		//! @brief Removes all registered notifiers from the list of notifiers.
		//! The object will deregister from the notifier.
		void removeAllDestroyNotifiers();

	private:
		std::list<ObjectDestroyNotifier*> m_destroyNotifiers;
	};

}