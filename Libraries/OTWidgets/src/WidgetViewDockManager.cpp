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
	
	std::string areaString;

	switch (area) {
	case ads::NoDockWidgetArea: areaString = "None"; break;
	case ads::LeftDockWidgetArea: areaString = "Left"; break;
	case ads::RightDockWidgetArea: areaString = "Right"; break;
	case ads::TopDockWidgetArea: areaString = "Top"; break;
	case ads::BottomDockWidgetArea: areaString = "Bottom"; break;
	case ads::CenterDockWidgetArea: areaString = "Center"; break;
	case ads::LeftAutoHideArea: areaString = "Left Auto Hide"; break;
	case ads::RightAutoHideArea: areaString = "Right Auto Hide"; break;
	case ads::TopAutoHideArea: areaString = "Top Auto Hide"; break;
	case ads::BottomAutoHideArea: areaString = "Bottom Auto Hide"; break;
	case ads::OuterDockAreas: areaString = "Outer"; break;
	case ads::AutoHideDockAreas: areaString = "Auto Hide"; break;
	case ads::AllDockAreas: areaString = "All"; break;
	default:
		areaString = "Unknown"; break;
		break;
	}

	if (!_areaWidget) {
		OT_LOG_T("Determining last added dock area for area \"" + areaString + "\"");
		_areaWidget = this->lastAddedDockAreaWidget(area);
	}
	
	if (_areaWidget) {
		OT_LOG_T("Adding view \"" + _view->getViewData().getEntityName() + "\" to existing area");
		this->addDockWidget(ads::CenterDockWidgetArea, _view->getViewDockWidget(), _areaWidget);
	}
	else {
		OT_LOG_T("Adding view \"" + _view->getViewData().getEntityName() + "\" to area \"" + areaString + "\"");
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