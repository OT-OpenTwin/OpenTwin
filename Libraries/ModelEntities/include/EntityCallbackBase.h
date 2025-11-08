// @otlicense
// File: EntityCallbackBase.h
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
#include "ModelEntitiesAPIExport.h"

// std header
#include <map>
#include <list>

namespace ot {

	class OT_MODELENTITIES_API_EXPORT EntityCallbackBase {
		OT_DECL_DEFCOPY(EntityCallbackBase)
		OT_DECL_DEFMOVE(EntityCallbackBase)
	public:
		//! @brief Callback flags for entity changes.
		//! @warning The order of the enum values must not be changed as they are stored as bit flags.
		//! Every entry value must be double the previous one.
		enum class Callback : int64_t {
			None = 0 << 0, //! @brief No callback.

			// !!! Do not change the order of the values !!!

			Properties = 1 << 0, //! @brief Property changes.
			Selection  = 1 << 1, //! @brief Selection changes.
			DataNotify = 1 << 2, //! @brief Data changes will notify the service.
			DataHandle = 1 << 3, //! @brief Data changes will be handled by the service (Overrides DataNotify).

			// !!! Do not forget to update CallbackIteratorLast when adding new values !!!

			CallbackIteratorFirst = Properties, //! @brief First value for iteration.
			CallbackIteratorLast = DataHandle //! @brief Last value for iteration.
		};
		typedef ot::Flags<Callback, int64_t> CallbackFlags;

		EntityCallbackBase() = default;
		virtual ~EntityCallbackBase() = default;

		//! @brief Clears all registered callbacks and their services.
		//! @param _supressChangedNotification If true the callbackDataChanged() method will not be called.
		void clearCallbacks(bool _supressChangedNotification = false);

		//! @brief Adds the provided service for the provided callback.
		//! @param _callback Callback to register.
		//! @param _serviceName Name of the service registering for the callback.
		//! @param _supressChangedNotification If true the callbackDataChanged() method will not be called.
		void registerCallback(Callback _callback, const std::string& _serviceName, bool _supressChangedNotification = false);

		//! @brief Adds the provided services for the provided callbacks.
		//! @param _callbacks Callbacks to register.
		//! @param _serviceName Name of the service registering for the callbacks.
		//! @param _supressChangedNotification If true the callbackDataChanged() method will not be called.
		void registerCallbacks(const CallbackFlags& _callbacks, const std::string& _serviceName, bool _supressChangedNotification = false);

		//! @brief Returns the registered callbacks for the provided service.
		//! @param _serviceName Service name.
		//! @return All callbacks registered for the service.
		CallbackFlags getServiceCallbacks(const std::string& _serviceName) const;

		//! @brief Returns the service registered for the provided callback.
		//! @param _callback Callback.
		//! @return Name of the service registered for the callback. Empty string if no service is registered.
		std::list<std::string> getServicesForCallback(Callback _callback) const;

		//! @brief Sets all registered callbacks and their services.
		//! @param _callbackServices Map of callbacks and their services.
		//! @param _supressChangedNotification If true the callbackDataChanged() method will not be called.
		void setCallbackData(const std::map<Callback, std::list<std::string>>& _callbackServices, bool _supressChangedNotification = false);

		//! @brief Sets all registered callbacks and their services.
		//! @param _callbackServices Map of callbacks and their services.
		//! @param _supressChangedNotification If true the callbackDataChanged() method will not be called.
		void setCallbackData(std::map<Callback, std::list<std::string>>&& _callbackServices, bool _supressChangedNotification = false);

		//! @brief Sets all registered callbacks and their services from another EntityCallbackBase instance.
		//! @param _other Other callback base object to get the data from.
		//! @param _supressChangedNotification If true the callbackDataChanged() method will not be called.
		void setCallbackData(const EntityCallbackBase& _other, bool _supressChangedNotification = false);

		//! @brief Returns all registered callbacks and their services.
		const std::map<Callback, std::list<std::string>>& getCallbackData() const { return m_callbackServices; };

	protected:
		virtual void callbackDataChanged() {};

	private:
		std::map<Callback, std::list<std::string>> m_callbackServices;

	};

}