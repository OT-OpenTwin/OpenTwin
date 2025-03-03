//! @file PropertyManagerWriteCallbackNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTCore/OTAssert.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>
#include <optional>
#include <functional>

namespace ot {

	class Property;

	//! @class PropertyManagerWriteCallbackNotifierBase
	//! @brief Base class to receive and forward a write callback for property to the set callback method.
	//! @ref PropertyManager
	class PropertyManagerWriteCallbackNotifierBase {
	public:
		PropertyManagerWriteCallbackNotifierBase(const std::string& _propertyPath) :
			m_propertyPath(_propertyPath)
		{}

		//! @brief Interface to call the callback method.
		virtual void call(const ot::Property* _property) = 0;

		const std::string& getPropertyPath(void) const { return m_propertyPath; };

	private:
		std::string m_propertyPath;
	};

	//! @class PropertyManagerWriteCallbackNotifier
	//! @brief Base class to receive and forward a write callback for property to the set callback method.
	//! @tparam T Actual property type used to cast property.
	template <class T> class PropertyManagerWriteCallbackNotifier : public PropertyManagerWriteCallbackNotifierBase {
		OT_DECL_NODEFAULT(PropertyManagerWriteCallbackNotifier)
	public:
		using ManagerGetType = std::function<PropertyManager* (void)>;
		using CallbackType = std::function<void(const T*)>;

		PropertyManagerWriteCallbackNotifier(ManagerGetType _getManager, const std::string& m_propertyPath, std::optional<CallbackType> _method);
		virtual ~PropertyManagerWriteCallbackNotifier();

		virtual void call(const ot::Property* _property) override;
		void callType(const T* _property);

	private:
		std::optional<CallbackType> m_method;
		PropertyManager* m_manager;
	};

}

template <class T>
ot::PropertyManagerWriteCallbackNotifier<T>::PropertyManagerWriteCallbackNotifier(ManagerGetType _getManager, const std::string& _propertyName, std::optional<CallbackType> _method) :
	PropertyManagerWriteCallbackNotifierBase(_propertyName), m_method(_method)
{
	m_manager = _getManager();
	OTAssertNullptr(m_manager);
	m_manager->addWriteCallbackNotifier(this);
}

template <class T>
ot::PropertyManagerWriteCallbackNotifier<T>::~PropertyManagerWriteCallbackNotifier() {
	if (m_manager) {
		m_manager->removeWriteCallbackNotifier(this);
	}
}

template<class T>
void ot::PropertyManagerWriteCallbackNotifier<T>::call(const ot::Property* _property) {
	const T* prop = static_cast<const T*>(_property);
	OTAssertNullptr(prop);
	this->callType(prop);
}

template <class T>
void ot::PropertyManagerWriteCallbackNotifier<T>::callType(const T* _property) {
	if (m_method.has_value()) {
		m_method.value()(_property);
	}
}
