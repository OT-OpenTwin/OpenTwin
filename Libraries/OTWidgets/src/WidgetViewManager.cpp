//! @file WidgetViewManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/WidgetViewCfg.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/WidgetViewFactory.h"

// ADS header
#include <ads/DockManager.h>

ot::WidgetViewManager& ot::WidgetViewManager::instance(void) {
	static WidgetViewManager g_instance;
	return g_instance;
}

void ot::WidgetViewManager::initialize(ads::CDockManager* _dockManager) {
	if (m_dockManager) {
		OT_LOG_WA("WidgetViewManager already initialized");
		return;
	}

	m_dockManager = _dockManager;

	if (!m_dockManager) {
		m_dockManager = new ads::CDockManager;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################


bool ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetView* _view) {
	return this->addViewImpl(_owner, _view, nullptr);
}

bool ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _area) {
	return this->addViewImpl(_owner, _view, _area);
}

ot::WidgetView* ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetViewCfg* _viewConfiguration) {
	WidgetView* newView = WidgetViewFactory::createView(_viewConfiguration);
	if (newView) {
		ads::CDockAreaWidget* parentArea = nullptr;
		if (!_viewConfiguration->parentViewName().empty()) {
			WidgetView* parentView = this->findView(_owner, _viewConfiguration->parentViewName());
			if (parentView) {
				parentArea = parentView->getViewDockWidget()->dockAreaWidget();
			}
		}
		if (!this->addView(_owner, newView, parentArea)) {
			delete newView;
			newView = nullptr;
		}
	}
	return newView;
}

ot::WidgetView* ot::WidgetViewManager::findView(const BasicServiceInformation& _owner, const std::string& _viewName) {
	auto map = this->findViewMap(_owner);
	if (map) {
		auto it = map->find(_viewName);
		if (it == map->end()) {
			return nullptr;
		}
		else {
			return it->second;
		}
	}
	else {
		return nullptr;
	}
}

void ot::WidgetViewManager::closeView(const BasicServiceInformation& _owner, const std::string& _viewName) {
	auto map = this->findViewMap(_owner);
	if (map) {
		auto it = map->find(_viewName);
		if (it != map->end()) {
			if (!it->second->viewIsProtected()) {
				delete it->second;
				map->erase(_viewName);
			}
		}
		if (map->empty()) {
			this->clear(_owner);
		}
	}
}

void ot::WidgetViewManager::closeViews(const BasicServiceInformation& _owner) {
	auto map = this->findViewMap(_owner);
	if (map) {
		std::list<std::string> tmp;
		for (const auto& it : *map) {
			tmp.push_back(it.second->name());
		}
		for (const std::string& i : tmp) {
			this->closeView(_owner, i);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

ot::WidgetViewManager::WidgetViewManager()
	: m_dockManager(nullptr) 
{}

ot::WidgetViewManager::~WidgetViewManager() {

}

bool ot::WidgetViewManager::addViewImpl(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _area) {
	OTAssertNullptr(_view);
	auto map = this->findOrCreateViewMap(_owner);
	auto it = map->find(_view->name());
	if (it == map->end()) {
		map->insert_or_assign(_view->name(), _view);
		switch (_view->initialDockLocation())
		{
		case ot::WidgetViewCfg::Default:
			m_dockManager->addDockWidget(ads::CenterDockWidgetArea, _view->getViewDockWidget(), _area);
			break;
		case ot::WidgetViewCfg::Left:
			m_dockManager->addDockWidget(ads::LeftDockWidgetArea, _view->getViewDockWidget(), _area);
			break;
		case ot::WidgetViewCfg::Top:
			m_dockManager->addDockWidget(ads::TopDockWidgetArea, _view->getViewDockWidget(), _area);
			break;
		case ot::WidgetViewCfg::Right:
			m_dockManager->addDockWidget(ads::RightDockWidgetArea, _view->getViewDockWidget(), _area);
			break;
		case ot::WidgetViewCfg::Bottom:
			m_dockManager->addDockWidget(ads::BottomDockWidgetArea, _view->getViewDockWidget(), _area);
			break;
		default:
			OT_LOG_E("Unknown dock location");
			m_dockManager->addDockWidget(ads::CenterDockWidgetArea, _view->getViewDockWidget(), _area);
		}
		return true;
	}
	else if (it->second != _view) {
		OT_LOG_W("A different view with the same name and owner was already added before. Skipping add");
		return false;
	}
	else {
		return true;
	}
}

void ot::WidgetViewManager::clear(const BasicServiceInformation& _owner) {
	auto map = this->findViewMap(_owner);
	if (map) delete map;
	m_views.erase(_owner);
}

std::map<std::string, ot::WidgetView*>* ot::WidgetViewManager::findViewMap(const BasicServiceInformation& _owner) {
	auto it = m_views.find(_owner);
	if (it == m_views.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

std::map<std::string, ot::WidgetView*>* ot::WidgetViewManager::findOrCreateViewMap(const BasicServiceInformation& _owner) {
	std::map<std::string, ot::WidgetView*>* ret = this->findViewMap(_owner);
	if (!ret) {
		ret = new std::map<std::string, ot::WidgetView*>;
		m_views.insert_or_assign(_owner, ret);
	}
	return ret;
}