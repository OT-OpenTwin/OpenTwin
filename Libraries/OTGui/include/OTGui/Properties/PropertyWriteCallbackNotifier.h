// @otlicense
// File: PropertyWriteCallbackNotifier.h
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

// OT header
#include "OTGui/GuiTypes.h"

// std header
#include <string>
#include <optional>
#include <functional>

namespace ot {

	class Property;

	//! @class PropertyWriteCallbackNotifierBase
	//! @brief Base class to receive and forward a write callback for property to the set callback method.
	//! @ref PropertyManager
	class PropertyWriteCallbackNotifierBase {
	public:
		PropertyWriteCallbackNotifierBase(const std::string& _propertyPath) :
			m_propertyPath(_propertyPath)
		{}

		//! @brief Interface to call the callback method.
		virtual void call(const ot::Property* _property) = 0;

		const std::string& getPropertyPath(void) const { return m_propertyPath; };

	private:
		std::string m_propertyPath;
	};

	//! @class PropertyWriteCallbackNotifier
	//! @brief Base class to receive and forward a write callback for property to the set callback method.
	//! @tparam T Actual property type used to cast property.
	template <class T> class PropertyWriteCallbackNotifier : public PropertyWriteCallbackNotifierBase {
		OT_DECL_NODEFAULT(PropertyWriteCallbackNotifier)
	public:
		using ManagerGetType = std::function<PropertyManager* (void)>;
		using CallbackType = std::function<void(const T*)>;

		PropertyWriteCallbackNotifier(ManagerGetType _getManager, const std::string& m_propertyPath, std::optional<CallbackType> _method);
		virtual ~PropertyWriteCallbackNotifier();

		virtual void call(const ot::Property* _property) override;
		void callType(const T* _property);

	private:
		std::optional<CallbackType> m_method;
		PropertyManager* m_manager;
	};

}

template <class T>
ot::PropertyWriteCallbackNotifier<T>::PropertyWriteCallbackNotifier(ManagerGetType _getManager, const std::string& _propertyName, std::optional<CallbackType> _method) :
	PropertyWriteCallbackNotifierBase(_propertyName), m_method(_method)
{
	m_manager = _getManager();
	OTAssertNullptr(m_manager);
	m_manager->addWriteCallbackNotifier(this);
}

template <class T>
ot::PropertyWriteCallbackNotifier<T>::~PropertyWriteCallbackNotifier() {
	if (m_manager) {
		m_manager->removeWriteCallbackNotifier(this);
	}
}

template<class T>
void ot::PropertyWriteCallbackNotifier<T>::call(const ot::Property* _property) {
	const T* prop = static_cast<const T*>(_property);
	OTAssertNullptr(prop);
	this->callType(prop);
}

template <class T>
void ot::PropertyWriteCallbackNotifier<T>::callType(const T* _property) {
	if (m_method.has_value()) {
		m_method.value()(_property);
	}
}
