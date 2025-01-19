//! @file ViewerView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

class Viewer;

namespace ot {

	class __declspec(dllexport) ViewerView : public WidgetView {
	public:
		ViewerView() = delete;
		ViewerView(ot::UID _modelID, ot::UID _viewerID, double _scaleWidth, double _scaleHeight, int _backgroundR, int _backgroundG, int _backgroundB, int _overlayTextR, int _overlayTextG, int _overlayTextB);
		virtual ~ViewerView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		Viewer* getViewer(void) const { return m_viewer; };

	private:
		Viewer* m_viewer;

	};

}
