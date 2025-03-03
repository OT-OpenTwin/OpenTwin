//! @file PropertyManagerNotifier.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class Property;
	class PropertyManager;

	class OT_GUI_API_EXPORT PropertyManagerNotifier {
	public:
		PropertyManagerNotifier();
		PropertyManagerNotifier(const PropertyManagerNotifier& _other);
		PropertyManagerNotifier(PropertyManagerNotifier&& _other) noexcept;
		virtual ~PropertyManagerNotifier();

		PropertyManagerNotifier& operator = (const PropertyManagerNotifier& _other);
		PropertyManagerNotifier& operator = (PropertyManagerNotifier&& _other) noexcept;

		virtual void propertyWasCreated(ot::Property* _property) {};
		virtual void propertyWasDestroyed(ot::Property* _property) {};
		virtual void propertyHasChanged(const ot::Property* _property) {};

	private:
		friend class PropertyManager;
		PropertyManager* m_manager;
	};

}
