//! @file PropertyGridItemInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyGridItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT PropertyGridItemInt : public PropertyGridItem {
	public:
		PropertyGridItemInt();
		virtual ~PropertyGridItemInt();

		virtual bool setupFromConfig(Property* _config) override;

	private:

	};

}