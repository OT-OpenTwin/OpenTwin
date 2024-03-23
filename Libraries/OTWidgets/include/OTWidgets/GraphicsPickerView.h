//! @file GraphicsPickerView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/GraphicsPicker.h"

#define OT_WIDGETTYPE_GraphicsPicker "GraphicsPicker"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsPickerView : public GraphicsPicker, public WidgetView {
	public:
		GraphicsPickerView();
		virtual ~GraphicsPickerView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		virtual bool setupFromConfig(WidgetViewCfg* _config) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:

	};

}
