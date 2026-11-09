// @otlicense
// File: ObjectDestroyNotifier.h
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
	class ObjectWithDestroyNotifier;

	class OT_CORE_API_EXPORT ObjectDestroyNotifier
	{
		OT_DECL_NOCOPY(ObjectDestroyNotifier)
		OT_DECL_NOMOVE(ObjectDestroyNotifier)
	public:
		explicit ObjectDestroyNotifier();
		virtual ~ObjectDestroyNotifier();

	protected:
		//! @brief This method is called when an object that was registered to this notifier is about to be destroyed.
		//! The object is still valid when this method is called, so it can be used to access the object.
		virtual void onObjectPreDestroy(ObjectWithDestroyNotifier* _object) {};

		//! @brief This method is called when an object that was registered to this notifier is destroyed.
		//! @note The object is already destroyed when this method is called, so it should not be used to access the object.
		virtual void onObjectDestroyed(const ObjectWithDestroyNotifier* _object) {};

	private:
		friend class ObjectWithDestroyNotifier;
		//! @brief Registers an object to be notified when it is destroyed.
		//! This method should be called from the object itself when the notifier was successfully registered to it.
		void addRegisteredObject(ObjectWithDestroyNotifier* _object);

		//! @brief Unregisters an object from the notifier.
		//! This method should be called from the object itself when the notifier was successfully unregistered from it.
		void removeRegisteredObject(ObjectWithDestroyNotifier* _object);

		std::list<ObjectWithDestroyNotifier*> m_registeredObjects;

	};

}