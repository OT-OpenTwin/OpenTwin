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

ot::WidgetViewManager& ot::WidgetViewManager::instance(void) {
	static WidgetViewManager g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################


bool ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetView* _view) {
	OTAssertNullptr(_view);
	auto map = this->findOrCreateViewMap(_owner);
	auto it = map->find(_view->name());
	if (it == map->end()) {
		map->insert_or_assign(_view->name(), _view);
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

ot::WidgetView* ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetViewCfg* _viewConfiguration) {
	WidgetView* newView = WidgetViewFactory::createView(_viewConfiguration);
	if (newView) {
		if (!this->addView(_owner, newView)) {
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
			delete it->second;
			map->erase(_viewName);
		}
		if (map->empty()) {
			this->clear(_owner);
		}
	}
}

void ot::WidgetViewManager::closeViews(const BasicServiceInformation& _owner) {
	auto map = this->findViewMap(_owner);
	for (const auto& it : *map) {
		delete it.second;
	}
	map->clear();
	this->clear(_owner);
}

ot::WidgetView* ot::WidgetViewManager::findOrCreateDefaultView(DefaultWidgetViewType _viewType) {
	auto it = m_defaultViews.find(_viewType);
	if (it != m_defaultViews.end()) {
		return it->second;
	}

	switch (_viewType)
	{
	case ot::WidgetViewManager::ProjectNavigationViewType:

		return nullptr;
		break;

	case ot::WidgetViewManager::PropertyGridViewType:

		return nullptr;
		break;

	case ot::WidgetViewManager::OutputWindowViewType:

		return nullptr;
		break;

	case ot::WidgetViewManager::GraphicsPickerViewType:

		return nullptr;
		break;

	default:
		OT_LOG_EAS("Unknown default widget view type (" + std::to_string((int)_viewType) + ")");
		return nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

ot::WidgetViewManager::WidgetViewManager() {

}

ot::WidgetViewManager::~WidgetViewManager() {

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