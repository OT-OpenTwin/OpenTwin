//! @file WidgetViewDockManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/WidgetViewDockManager.h"
#include "OTWidgets/WidgetViewDockComponentsFactory.h"

// Qt ADS header
#include <ads/DockAreaWidget.h>

ot::WidgetViewDockManager::WidgetViewDockManager() : m_componentsFactory(nullptr) {

}

ot::WidgetViewDockManager::~WidgetViewDockManager() {
	if (m_componentsFactory) {
		m_componentsFactory->forgetDockManager();
		ads::CDockComponentsFactory::resetDefaultFactory();
	}
}

void ot::WidgetViewDockManager::addView(WidgetView* _view, ads::CDockAreaWidget* _areaWidget, const WidgetView::InsertFlags& _insertFlags) {
	ads::DockWidgetArea area = this->getDockWidgetArea(_view);

	if (!_areaWidget) {
		_areaWidget = this->lastAddedDockAreaWidget(area);
	}
	
	if (_areaWidget) {
		this->addDockWidget(ads::CenterDockWidgetArea, _view->getViewDockWidget(), _areaWidget);
	}
	else {
		this->addDockWidget(area, _view->getViewDockWidget(), nullptr);
	}

}

ads::DockWidgetArea ot::WidgetViewDockManager::getDockWidgetArea(const WidgetView* _view) const {
	switch (_view->getViewData().getDockLocation()) {
	case ot::WidgetViewBase::Default: return ads::CenterDockWidgetArea;
	case ot::WidgetViewBase::Left: return ads::LeftDockWidgetArea;
	case ot::WidgetViewBase::Top: return ads::TopDockWidgetArea;
	case ot::WidgetViewBase::Right: return ads::RightDockWidgetArea;
	case ot::WidgetViewBase::Bottom: return ads::BottomDockWidgetArea;
	default:
		OT_LOG_EAS("Unknown dock location (" + std::to_string((int)_view->getViewData().getDockLocation()) + ")");
		return ads::CenterDockWidgetArea;
	}
}