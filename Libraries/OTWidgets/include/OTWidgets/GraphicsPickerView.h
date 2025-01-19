//! @file GraphicsPickerView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/BasicWidgetView.h"

namespace ot {

	class GraphicsPicker;

	class OT_WIDGETS_API_EXPORT GraphicsPickerView : public WidgetView {
	public:
		GraphicsPickerView(GraphicsPicker* _graphicsPicker = (GraphicsPicker*)nullptr);
		virtual ~GraphicsPickerView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		GraphicsPicker* getGraphicsPicker(void) const { return m_graphicsPicker; };

	private:
		GraphicsPicker* m_graphicsPicker;

	};

}
