//! @file PropertyReadCallbackNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTCore/OTClassHelper.h"
#include "OTGui/PropertyManager.h"

// std header
#include <string>
#include <optional>
#include <functional>

namespace ot {
	
	//! @class PropertyReadCallbackNotifier
	//! @brief Receives and forwards a read callback for property to the set callback method.
	//! @ref PropertyManager
	class OT_GUI_API_EXPORT PropertyReadCallbackNotifier {
		OT_DECL_NODEFAULT(PropertyReadCallbackNotifier)
	public:
		using ManagerGetType = std::function<PropertyManager*(void)>;
		using CallbackType = std::function<void(const std::string&, const std::string&)>;

		PropertyReadCallbackNotifier(ManagerGetType _getManager, const std::string& _propertyPath, std::optional<CallbackType> _method);
		virtual ~PropertyReadCallbackNotifier();

		void call(const std::string& _propertyGroupName, const std::string& _propertyName);

		const std::string& getPropertyPath(void) const { return m_propertyPath; };

	private:
		std::optional<CallbackType> m_method;
		std::string m_propertyPath;
		PropertyManager* m_manager;
	};

}