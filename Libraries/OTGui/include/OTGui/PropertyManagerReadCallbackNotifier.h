//! @file PropertyManagerReadCallbackNotifier.h
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

	class OT_GUI_API_EXPORT PropertyManagerReadCallbackNotifier {
		OT_DECL_NODEFAULT(PropertyManagerReadCallbackNotifier)
	public:
		using ManagerGetType = std::function<PropertyManager*(void)>;
		using CallbackType = std::function<void(const std::string&, const std::string&)>;

		PropertyManagerReadCallbackNotifier(ManagerGetType _getManager, const std::string& _propertyPath, std::optional<CallbackType> _method);
		virtual ~PropertyManagerReadCallbackNotifier();

		void call(const std::string& _propertyGroupName, const std::string& _propertyName);

		const std::string& getPropertyPath(void) const { return m_propertyPath; };

	private:
		std::optional<CallbackType> m_method;
		std::string m_propertyPath;
		PropertyManager* m_manager;
	};

}