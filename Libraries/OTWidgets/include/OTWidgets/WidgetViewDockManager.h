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
	class WidgetViewDockComponentsFactory;

	class OT_WIDGETS_API_EXPORT WidgetViewDockManager : public ads::CDockManager {
		OT_DECL_NOCOPY(WidgetViewDockManager)
	public:
		WidgetViewDockManager();
		virtual ~WidgetViewDockManager();

		void addView(WidgetView* _view, ads::CDockAreaWidget* _areaWidget, const WidgetView::InsertFlags& _insertFlags);

		void setComponentsFactory(WidgetViewDockComponentsFactory* _factory) { m_componentsFactory = _factory; };
		WidgetViewDockComponentsFactory* getComponentsFactory(void) const { return m_componentsFactory; };

	private:
		ads::DockWidgetArea getDockWidgetArea(const WidgetView* _view) const;

		WidgetViewDockComponentsFactory* m_componentsFactory;
	};

}