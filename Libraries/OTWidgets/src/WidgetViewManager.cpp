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

// Qt header
#include <QtWidgets/qmenu.h>

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
		m_dockManager->setConfigFlag(ads::CDockManager::DisableTabTextEliding, true);
		m_dockManager->setStyleSheet("");
	}

	m_dockToggleRoot = new QAction("Windows");
	QMenu* newMenu = new QMenu;
	m_dockToggleRoot->setMenu(newMenu);

	this->connect(m_dockManager, &ads::CDockManager::focusedDockWidgetChanged, this, &WidgetViewManager::slotViewFocused);
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
			WidgetView* parentView = this->findView(_viewConfiguration->parentViewName());
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

ot::WidgetView* ot::WidgetViewManager::findView(const std::string& _viewName) const {
	const auto it = m_viewNameMap.find(_viewName);
	if (it == m_viewNameMap.end()) return nullptr;
	else return it->second.second;
}

ot::WidgetView* ot::WidgetViewManager::viewFromDockWidget(ads::CDockWidget* _dock) {
	for (const auto& it : m_viewNameMap) {
		if (it.second.second->getViewDockWidget() == _dock) return it.second.second;
	}
	return nullptr;
}

void ot::WidgetViewManager::closeView(const std::string& _viewName) {
	WidgetView* view = this->findView(_viewName);
	if (view == nullptr) return;

	if (view->viewIsProtected()) return;

	// Set the view as deleted by manager so it wont remove itself and remove it from the maps
	view->m_isDeletedByManager = true;
	this->forgetView(_viewName);

	// Remove the toggle dock action
	m_dockToggleRoot->menu()->removeAction(view->getViewDockWidget()->toggleViewAction());

	// Remove the dock widget itself
	m_dockManager->removeDockWidget(view->getViewDockWidget());
}

void ot::WidgetViewManager::closeViews(const BasicServiceInformation& _owner) {
	std::list<std::string>* lst = this->findViewNameList(_owner);
	if (lst) {
		std::list<std::string> tmp = *lst;
		for (const std::string& i : tmp) {
			this->closeView(i);
		}
	}
}

void ot::WidgetViewManager::closeViews(void) {
	std::list<std::string> tmp;
	for (const auto& it : m_viewNameMap) {
		tmp.push_back(it.first);
	}
	for (const std::string& i : tmp) {
		this->closeView(i);
	}
}

void ot::WidgetViewManager::forgetView(WidgetView* _view) {
	OTAssertNullptr(_view);
	this->forgetView(_view->viewData().name());
}

ot::WidgetView* ot::WidgetViewManager::forgetView(const std::string& _viewName) {
	// Find view and owner
	auto nameIt = m_viewNameMap.find(_viewName);
	if (nameIt == m_viewNameMap.end()) return nullptr;

	// Get view
	ot::WidgetView* ret = nameIt->second.second;

	// Disconnect signals
	this->disconnect(ret->getViewDockWidget(), &ads::CDockWidget::closeRequested, this, &WidgetViewManager::slotViewCloseRequested);

	// If the view is the current central, set current central to 0
	if (nameIt->second.second == m_focusInfo.last) m_focusInfo.last = nullptr;
	if (nameIt->second.second == m_focusInfo.lastSide) m_focusInfo.lastSide = nullptr;
	if (nameIt->second.second == m_focusInfo.lastTool) m_focusInfo.lastTool = nullptr;
	if (nameIt->second.second == m_focusInfo.lastCentral) m_focusInfo.lastCentral = nullptr;

	// Find name list from owner and erase the view entry
	std::list<std::string>* lst = this->findViewNameList(nameIt->second.first);
	auto lstIt = std::find(lst->begin(), lst->end(), _viewName);
	if (lstIt != lst->end()) lst->erase(lstIt);
	if (lst->empty()) m_viewOwnerMap.erase(nameIt->second.first);

	// Now remove the entry from the view name map
	m_viewNameMap.erase(_viewName);

	return ret;
}

// ###########################################################################################################################################################################################################################################################################################################################

// View manipulation

void ot::WidgetViewManager::setCurrentView(const std::string& _viewName) {
	WidgetView* view = this->findView(_viewName);
	if (!view) return;

	view->getViewDockWidget()->setAsCurrentTab();
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

bool ot::WidgetViewManager::viewExists(const std::string& _viewName) const {
	return this->findView(_viewName);
}

bool ot::WidgetViewManager::viewTitleExists(const QString& _title) const {
	for (const auto& it : m_viewNameMap) {
		if (QString::fromStdString(it.second.second->viewData().title()) == _title || it.second.second->currentViewTitle() == _title) return true;
	}
	return false;
}

// ###########################################################################################################################################################################################################################################################################################################################

void ot::WidgetViewManager::slotViewFocused(ads::CDockWidget* _oldFocus, ads::CDockWidget* _newFocus) {
	WidgetView* o = this->viewFromDockWidget(_oldFocus);
	WidgetView* n = this->viewFromDockWidget(_newFocus);

	if (o) {
		Q_EMIT viewFocusLost(o);
	}

	if (n) {
		m_focusInfo.last = n;
		if (n->viewData().flags() & WidgetViewBase::ViewIsCentral) m_focusInfo.lastCentral = n;
		if (n->viewData().flags() & WidgetViewBase::ViewIsSide) m_focusInfo.lastSide = n;
		if (n->viewData().flags() & WidgetViewBase::ViewIsTool) m_focusInfo.lastTool = n;
		Q_EMIT viewFocused(n);
	}
}

void ot::WidgetViewManager::slotViewCloseRequested(void) {
	WidgetView* view = this->viewFromDockWidget(dynamic_cast<ads::CDockWidget*>(sender()));
	if (!view) {
		OT_LOG_E("View not found");
		return;
	}

	if (view->viewData().flags() & WidgetViewBase::ViewDefaultCloseHandling) {
		view->getViewDockWidget()->toggleView(view->getViewDockWidget()->isClosed());
	}
	else {
		Q_EMIT viewCloseRequested(view);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

ot::WidgetViewManager::WidgetViewManager()
	: m_dockManager(nullptr), m_dockToggleRoot(nullptr)
{
	m_focusInfo.last = nullptr;
	m_focusInfo.lastSide = nullptr;
	m_focusInfo.lastTool = nullptr;
	m_focusInfo.lastCentral = nullptr;
}

ot::WidgetViewManager::~WidgetViewManager() {
	this->deleteLater();
}

bool ot::WidgetViewManager::addViewImpl(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _area) {
	OTAssertNullptr(_view);
	// Ensure view does not exist
	if (this->viewExists(_view->viewData().name())) {
		OT_LOG_W("A widget view with the name \"" + _view->viewData().name() + "\" already exists");
		return false;
	}

	// Get view name list for given owner
	auto lst = this->findOrCreateViewNameList(_owner);
	auto lstIt = std::find(lst->begin(), lst->end(), _view->viewData().name());
	if (lstIt != lst->end()) {
		OT_LOG_E("Invalid entry");
		return false;
	}

	// Add view
	if (!_area) _area = this->determineBestParentArea(_view);

	if (_area) {
		m_dockManager->addDockWidget(intern::convertDockArea(_view->viewData().dockLocation()), _view->getViewDockWidget(), _area);
	}
	else {
		m_dockManager->addDockWidgetTab(intern::convertDockArea(_view->viewData().dockLocation()), _view->getViewDockWidget());
	}

	// Add view toggle (if view is closeable)
	if (_view->getViewDockWidget()->features() & ads::CDockWidget::DockWidgetClosable) {
		m_dockToggleRoot->menu()->addAction(_view->getViewDockWidget()->toggleViewAction());
	}

	// Store information
	lst->push_back(_view->viewData().name());
	m_viewNameMap.insert_or_assign(_view->viewData().name(), std::pair<BasicServiceInformation, WidgetView*>(_owner, _view));

	// Update focus information
	this->slotViewFocused((m_focusInfo.last ? m_focusInfo.last->getViewDockWidget() : nullptr), _view->getViewDockWidget());

	// Connect signals
	this->connect(_view->getViewDockWidget(), &ads::CDockWidget::closeRequested, this, &WidgetViewManager::slotViewCloseRequested);

	return true;
}

ads::CDockAreaWidget* ot::WidgetViewManager::determineBestParentArea(WidgetView* _newView) const {
	if (_newView->viewData().flags() & WidgetViewBase::ViewIsSide) {
		if (m_focusInfo.lastCentral) {
			return m_focusInfo.lastCentral->getViewDockWidget()->dockAreaWidget();
		}
		else if (m_focusInfo.lastSide) {
			return m_focusInfo.lastSide->getViewDockWidget()->dockAreaWidget();
		}
	}
	if (m_focusInfo.lastCentral) {
		return m_focusInfo.lastCentral->getViewDockWidget()->dockAreaWidget();
	}
	else if (m_focusInfo.lastSide) {
		return m_focusInfo.lastSide->getViewDockWidget()->dockAreaWidget();
	}
	else if (m_focusInfo.lastTool) {
		return m_focusInfo.lastTool->getViewDockWidget()->dockAreaWidget();
	}
	else {
		return nullptr;
	}
}

std::list<std::string>* ot::WidgetViewManager::findViewNameList(const BasicServiceInformation& _owner) {
	auto it = m_viewOwnerMap.find(_owner);
	if (it == m_viewOwnerMap.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

std::list<std::string>* ot::WidgetViewManager::findOrCreateViewNameList(const BasicServiceInformation& _owner) {
	std::list<std::string>* ret = this->findViewNameList(_owner);
	if (!ret) {
		ret = new std::list<std::string>;
		m_viewOwnerMap.insert_or_assign(_owner, ret);
	}
	return ret;
}