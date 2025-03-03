//! @file ManagedWidgetPropertyObject.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/ManagedPropertyObjectBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT ManagedWidgetPropertyObject : public ManagedPropertyObject {
		OT_DECL_NOCOPY(ManagedWidgetPropertyObject)
	public:
		//! @brief Constructor.
		//! @param _propertyManager Property manager to use. If null a new WidgetPropertyManager will be created.
		ManagedWidgetPropertyObject();

		//! @brief Move constructor.
		//! @param _other Other object to move data from.
		ManagedWidgetPropertyObject(ManagedWidgetPropertyObject&& _other) noexcept;

		//! @brief Destructor.
		virtual ~ManagedWidgetPropertyObject() {};

		//! @brief Assignment move operator.
		//! @param _other Other object to move data from.
		//! @return Reference to this object.
		ManagedWidgetPropertyObject& operator = (ManagedWidgetPropertyObject&& _other) noexcept;

		virtual void propertyValueHasChanged(const ot::Property* _property) {};

	};

}