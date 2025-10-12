//! @file PropertyWriteCallbackNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
