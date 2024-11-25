//! @file WidgetViewDockManager.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

// QT ADS header
#include <ads/DockManager.h>

namespace ot {

	class WidgetView;

	class OT_WIDGETS_API_EXPORT WidgetViewDockManager : public ads::CDockManager {
	public:
		WidgetViewDockManager();
		virtual ~WidgetViewDockManager();

		void addView(WidgetView* _view, ads::CDockAreaWidget* _areaWidget, const WidgetView::InsertFlags& _insertFlags);

	private:
		ads::DockWidgetArea getDockWidgetArea(const WidgetView* _view) const;

	};

}