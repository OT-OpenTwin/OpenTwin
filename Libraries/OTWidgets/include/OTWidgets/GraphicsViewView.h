//! @file GraphicsViewView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

namespace ot {

	class GraphicsView;

	class OT_WIDGETS_API_EXPORT GraphicsViewView : public WidgetView {
	public:
		GraphicsViewView(GraphicsView* _graphicsView = (GraphicsView*)nullptr);
		virtual ~GraphicsViewView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QWidget* getViewWidget() override;

		virtual void viewRenamed() override;

		// ###########################################################################################################################################################################################################################################################################################################################

		GraphicsView* getGraphicsView(void) const { return m_graphicsView; };

	private:
		GraphicsView* m_graphicsView;
	};

}
