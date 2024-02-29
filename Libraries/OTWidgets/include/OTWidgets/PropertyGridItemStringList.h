//! @file PropertyGridItemStringList.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PropertyGridItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT PropertyGridItemStringList : public PropertyGridItem {
	public:
		PropertyGridItemStringList();
		virtual ~PropertyGridItemStringList();

		virtual bool setupFromConfig(Property* _config) override;

	private:

	};

}
