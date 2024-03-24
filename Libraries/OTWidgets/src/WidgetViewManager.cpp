//! @file WidgetViewManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTGui/WidgetViewCfg.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/WidgetViewFactory.h"

// ADS header
#include <ads/DockManager.h>
#include <ads/DockAreaWidget.h>

namespace ot {
	namespace intern {

		ads::DockWidgetArea convertDockArea(ot::WidgetViewCfg::ViewDockLocation _dockLocation) {
			switch (_dockLocation)
			{
			case ot::WidgetViewCfg::Default: return ads::CenterDockWidgetArea;
			case ot::WidgetViewCfg::Left: return ads::LeftDockWidgetArea;
			case ot::WidgetViewCfg::Top: return ads::TopDockWidgetArea;
			case ot::WidgetViewCfg::Right: return ads::RightDockWidgetArea;
			case ot::WidgetViewCfg::Bottom: return ads::BottomDockWidgetArea;
			default:
				OT_LOG_E("Unknown dock location (" + std::to_string((int)_dockLocation) + ")");
				return ads::CenterDockWidgetArea;
			}
		}

	}
}

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
		m_dockManager->setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton, false);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// View Management

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
				it->second->m_isDeletedByManager = true;

				m_dockManager->removeDockWidget(it->second->getViewDockWidget());
				
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

void ot::WidgetViewManager::closeViews(void) {
	std::list<BasicServiceInformation> tmp;
	for (const auto& it : m_views) {
		tmp.push_back(it.first);
	}
	for (const BasicServiceInformation& i : tmp) {
		this->closeViews(i);
	}
}

void ot::WidgetViewManager::forgetView(WidgetView* _view) {
	for (const auto& v : m_views) {
		for (const auto& e : *v.second) {
			if (e.second == _view) {
				v.second->erase(e.first);

				if (v.second->empty()) {
					m_views.erase(v.first);
				}
				return;
			}
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// View manipulation

void ot::WidgetViewManager::setCurrentView(const std::string& _viewName) {

}

std::string ot::WidgetViewManager::saveState(int _version) const {
	QByteArray tmp = m_dockManager->saveState(_version);
	if (tmp.isEmpty()) return std::string();

	std::string ret;
	ret.reserve((tmp.size() * 3) - 1);

	for (char c : tmp) {
		if (!ret.empty()) ret.push_back(';');
		ret.append(std::to_string((int)c));
	}

	return ret;
}

bool ot::WidgetViewManager::restoreState(std::string _state, int _version) {
	if (_state.empty()) return false;
	QByteArray tmp;

	size_t ix = _state.find(';');
	bool convertFail = false;
	while (ix != std::string::npos) {
		tmp.append((char)ot::stringToNumber<int>(_state.substr(0, ix), convertFail));
		if (convertFail) {
			OT_LOG_E("State contains invalid characters");
			return false;
		}
		_state.erase(0, ix + 1);
		ix = _state.find(';');
	}

	if (!_state.empty()) {
		tmp.append((char)ot::stringToNumber<int>(_state, convertFail));
	}

	return m_dockManager->restoreState(tmp, _version);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Information gathering

QString ot::WidgetViewManager::getCurrentViewTitle(void) const {
	return QString();
}

bool ot::WidgetViewManager::viewExists(const std::string& _viewName) const {
	return false;
}

bool ot::WidgetViewManager::viewTitleExists(const QString& _title) const {
	return false;
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

		if (_area) {
			m_dockManager->addDockWidget(intern::convertDockArea(_view->initialDockLocation()), _view->getViewDockWidget(), _area);
		}
		else {
			m_dockManager->addDockWidgetTab(intern::convertDockArea(_view->initialDockLocation()), _view->getViewDockWidget());
		}
		
		return true;
	}
	else if (it->second != _view) {
		OT_LOG_W("A different view with the same name and owner was already added before. Skipping add. (View name: " + _view->name() + "; View title: " + _view->viewTitle().toStdString() + ")");
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