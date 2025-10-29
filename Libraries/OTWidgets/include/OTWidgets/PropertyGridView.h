// @otlicense

//! @file PropertyGridView.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

namespace ot {

	class PropertyGrid;

	class OT_WIDGETS_API_EXPORT PropertyGridView : public WidgetView {
		OT_DECL_NOCOPY(PropertyGridView)
	public:
		PropertyGridView(PropertyGrid* _propertyGrid = (PropertyGrid*)nullptr);
		virtual ~PropertyGridView();

		// ###########################################################################################################################################################################################################################################################################################################################

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		PropertyGrid* getPropertyGrid(void) const { return m_propertyGrid; };

	private:
		PropertyGrid* m_propertyGrid;
	};

}