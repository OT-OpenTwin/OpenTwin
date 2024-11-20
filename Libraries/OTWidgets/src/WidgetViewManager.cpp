//! @file WidgetViewManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/WidgetViewManager.h"

// ADS header
#include <ads/DockManager.h>
#include <ads/DockAreaWidget.h>

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qmenu.h>

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
		ads::CDockManager::setConfigFlag(ads::CDockManager::DisableTabTextEliding, true);
		ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
		m_dockManager = new ads::CDockManager;
		m_dockManager->setStyleSheet("");
	}

	m_dockToggleRoot = new QAction("Windows");
	QMenu* newMenu = new QMenu;
	m_dockToggleRoot->setMenu(newMenu);

	this->connect(m_dockManager, &ads::CDockManager::focusedDockWidgetChanged, this, &WidgetViewManager::slotViewFocused);
}

// ###########################################################################################################################################################################################################################################################################################################################

// View Management

bool ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetView* _view, ads::DockWidgetArea _insertArea) {
	return this->addViewImpl(_owner, _view, nullptr, _insertArea);
}

bool ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _parentArea, ads::DockWidgetArea _insertArea) {
	return this->addViewImpl(_owner, _view, _parentArea, _insertArea);
}

ot::WidgetView* ot::WidgetViewManager::findView(const std::string& _entityName, WidgetViewBase::ViewType _type) const {
	ViewNameTypeListEntry entry;
	entry.first = _entityName;
	entry.second = _type;
	return this->findView(entry);
	
}

ot::WidgetView* ot::WidgetViewManager::findViewFromTitle(const std::string& _viewTitle) const {
	for (const ViewEntry& entry : m_views) {
		if (entry.second->getViewData().getTitle() == _viewTitle || entry.second->getCurrentViewTitle().toStdString() == _viewTitle) {
			return entry.second;
		}
	}
	return nullptr;
}

bool ot::WidgetViewManager::findViewAndOwner(const std::string& _entityName, WidgetViewBase::ViewType _type, WidgetView*& _view, BasicServiceInformation& _owner) const {
	for (const ViewEntry& view : m_views) {
		if (view.second->getViewData().getEntityName() == _entityName && view.second->getViewData().getViewType() == _type) {
			_owner = view.first;
			_view = view.second;
			return true;
		}
	}

	return false;
}

ot::WidgetView* ot::WidgetViewManager::getViewFromDockWidget(ads::CDockWidget* _dock) const {
	for (const ViewEntry& view : m_views) {
		if (view.second->getViewDockWidget() == _dock) {
			return view.second;
		}
	}

	return nullptr;
}

void ot::WidgetViewManager::closeView(const std::string& _entityName, WidgetViewBase::ViewType _type) {
	OTAssertNullptr(m_dockManager);

	WidgetView* view = this->findView(_entityName, _type);
	if (view == nullptr) {
		return;
	}
	if (view->getViewIsPermanent()) {
		return;
	}

	// Set the view as deleted by manager so it wont remove itself and remove it from the maps
	view->m_isDeletedByManager = true;
	this->forgetView(_entityName, _type);

	// Remove the toggle dock action
	view->getViewDockWidget()->toggleViewAction()->setVisible(false);
	m_dockToggleRoot->menu()->removeAction(view->getViewDockWidget()->toggleViewAction());

	// Remove the dock widget itself
	m_dockManager->removeDockWidget(view->getViewDockWidget());

	// Finally destroy the view
	delete view;
}

void ot::WidgetViewManager::closeViews(const BasicServiceInformation& _owner) {
	OTAssertNullptr(m_dockManager);

	ViewNameTypeList* lst = this->findViewNameTypeList(_owner);
	if (lst) {
		ViewNameTypeList tmp = *lst;
		for (const ViewNameTypeListEntry& i : tmp) {
			this->closeView(i.first, i.second);
		}
	}
}

void ot::WidgetViewManager::closeViews(void) {
	OTAssertNullptr(m_dockManager);

	ViewNameTypeList tmp;
	for (const ViewEntry& view : m_views) {
		ViewNameTypeListEntry entry;
		entry.first = view.second->getViewData().getEntityName();
		entry.second = view.second->getViewData().getViewType();
		tmp.push_back(entry);
	}
	for (const ViewNameTypeListEntry& i : tmp) {
		this->closeView(i.first, i.second);
	}
}

void ot::WidgetViewManager::forgetView(WidgetView* _view) {
	OTAssertNullptr(m_dockManager);
	OTAssertNullptr(_view);
	this->forgetView(_view->getViewData().getEntityName(), _view->getViewData().getViewType());
}

ot::WidgetView* ot::WidgetViewManager::forgetView(const std::string& _entityName, WidgetViewBase::ViewType _type) {
	OTAssertNullptr(m_dockManager);

	// Find view and owner
	WidgetView* view = nullptr;
	BasicServiceInformation owner;
	if (!this->findViewAndOwner(_entityName, _type, view, owner)) {
		return nullptr;
	}
	
	// Disconnect signals
	this->disconnect(view->getViewDockWidget(), &ads::CDockWidget::closeRequested, this, &WidgetViewManager::slotViewCloseRequested);

	// If the view is the current central, set current central to 0
	if (view == m_focusInfo.last) m_focusInfo.last = nullptr;
	if (view == m_focusInfo.lastSide) m_focusInfo.lastSide = nullptr;
	if (view == m_focusInfo.lastTool) m_focusInfo.lastTool = nullptr;
	if (view == m_focusInfo.lastCentral) m_focusInfo.lastCentral = nullptr;

	// Find name list from owner and erase the view entry
	ViewNameTypeList* lst = this->findViewNameTypeList(owner);
	if (lst) {
		ViewNameTypeListEntry entry;
		entry.first = _entityName;
		entry.second = _type;

		auto lstIt = std::find(lst->begin(), lst->end(), entry);
		if (lstIt != lst->end()) {
			lst->erase(lstIt);
		}
		// If the owner has no more views, erase the owner.
		if (lst->empty()) {
			m_viewOwnerMap.erase(owner);
		}
	}
	else {
		OT_LOG_EAS("ViewNameTypeList not found");
	}
	
	ViewEntry viewsEntry;
	viewsEntry.first = owner;
	viewsEntry.second = view;

	auto viewIterator = std::find(m_views.begin(), m_views.end(), viewsEntry);
	if (viewIterator == m_views.end()) {
		OT_LOG_EA("View entry not found");
	}
	else {
		m_views.erase(viewIterator);
	}

	return view;
}

// ###########################################################################################################################################################################################################################################################################################################################

// View manipulation

void ot::WidgetViewManager::setCurrentView(const std::string& _entityName, WidgetViewBase::ViewType _type) {
	WidgetView* view = this->findView(_entityName, _type);
	if (view) {
		view->setAsCurrentViewTab();
	}
}

void ot::WidgetViewManager::setCurrentViewFromTitle(const std::string& _viewTitle) {
	WidgetView* view = this->findViewFromTitle(_viewTitle);
	if (view) {
		view->setAsCurrentViewTab();
	}
}

std::string ot::WidgetViewManager::saveState(int _version) const {
	OTAssertNullptr(m_dockManager);

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
	OTAssertNullptr(m_dockManager);

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

	bool result = m_dockManager->restoreState(tmp, _version);

	this->slotUpdateViewVisibility();
	
	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Information gathering

bool ot::WidgetViewManager::getViewExists(const std::string& _entityName, WidgetViewBase::ViewType _type) const {
	return this->findView(_entityName, _type) != nullptr;
}

bool ot::WidgetViewManager::getViewTitleExists(const std::string& _title) const {
	return this->findViewFromTitle(_title) != nullptr;
}

bool ot::WidgetViewManager::getAnyViewContentModified(void) {
	for (const ViewEntry& entry : m_views) {
		if (entry.second->getViewContentModified()) {
			return true;
		}
	}
	return false;
}

ot::WidgetView* ot::WidgetViewManager::getCurrentlyFocusedView(void) const {
	return this->getViewFromDockWidget(m_dockManager->focusedDockWidget());
}

// ###########################################################################################################################################################################################################################################################################################################################

void ot::WidgetViewManager::slotViewFocused(ads::CDockWidget* _oldFocus, ads::CDockWidget* _newFocus) {
	WidgetView* o = this->getViewFromDockWidget(_oldFocus);
	WidgetView* n = this->getViewFromDockWidget(_newFocus);

	if (o) {
		Q_EMIT viewFocusLost(o);
	}

	if (n) {
		m_focusInfo.last = n;
		if (n->getViewData().getViewFlags() & WidgetViewBase::ViewIsCentral) m_focusInfo.lastCentral = n;
		if (n->getViewData().getViewFlags() & WidgetViewBase::ViewIsSide) m_focusInfo.lastSide = n;
		if (n->getViewData().getViewFlags() & WidgetViewBase::ViewIsTool) m_focusInfo.lastTool = n;
		Q_EMIT viewFocused(n);
	}
}

void ot::WidgetViewManager::slotViewCloseRequested(void) {
	WidgetView* view = this->getViewFromDockWidget(dynamic_cast<ads::CDockWidget*>(sender()));
	if (!view) {
		OT_LOG_E("View not found");
		return;
	}

	if (view->getViewData().getViewFlags() & WidgetViewBase::ViewDefaultCloseHandling) {
		view->getViewDockWidget()->toggleView(view->getViewDockWidget()->isClosed());
	}
	else {
		Q_EMIT viewCloseRequested(view);
	}
}

void ot::WidgetViewManager::slotUpdateViewVisibility(void) {
	for (const ViewEntry& entry : m_views) {
		if (!(entry.second->getViewData().getViewFlags() & WidgetViewBase::ViewFlag::ViewIsCloseable) && !entry.second->getViewDockWidget()->dockAreaWidget()) {
			bool added = false;
			ads::CDockAreaWidget* area = this->determineBestRestoreArea(entry.second);
			if (area) {
				ads::CDockContainerWidget* container = area->dockContainer();
				if (container) {
					m_dockManager->addDockWidgetToContainer(ads::CenterDockWidgetArea, entry.second->getViewDockWidget(), container);
					added = true;
				}
				else {
					m_dockManager->addDockWidget(ads::CenterDockWidgetArea, entry.second->getViewDockWidget(), area);
				}
			}
			
			if (!m_dockManager->dockContainers().empty()) {
				m_dockManager->addDockWidgetToContainer(ads::CenterDockWidgetArea, entry.second->getViewDockWidget(), m_dockManager->dockContainers().first());
				added = true;
			}
			else {
				ads::CDockAreaWidget* defaultarea = m_dockManager->dockArea(0);
				if (defaultarea) {
					ads::CDockContainerWidget* container = defaultarea->dockContainer();
					if (container) {
						m_dockManager->addDockWidgetToContainer(ads::CenterDockWidgetArea, entry.second->getViewDockWidget(), container);
						added = true;
					}
				}
			}

			if (!added) {
				OT_LOG_W("No suitable dock location found");
				m_dockManager->addDockWidget(ads::CenterDockWidgetArea, entry.second->getViewDockWidget());
			}
		}
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

bool ot::WidgetViewManager::getViewExists(const ViewNameTypeListEntry& _entry) const {
	return this->findView(_entry) != nullptr;
}

ot::WidgetView* ot::WidgetViewManager::findView(const ViewNameTypeListEntry& _entry) const {
	for (const ViewEntry& view : m_views) {
		if (view.second->getViewData().getEntityName() == _entry.first && view.second->getViewData().getViewType() == _entry.second) {
			return view.second;
		}
	}

	return nullptr;
}

bool ot::WidgetViewManager::addViewImpl(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _parentArea, ads::DockWidgetArea _insertArea) {
	OTAssertNullptr(m_dockManager);
	OTAssertNullptr(_view);
	// Ensure view does not exist
	ViewNameTypeListEntry nameTypeEntry;
	nameTypeEntry.first = _view->getViewData().getEntityName();
	nameTypeEntry.second = _view->getViewData().getViewType();

	if (this->getViewExists(nameTypeEntry)) {
		OT_LOG_W("WidgetView already exists { \"EntityName\": \"" + nameTypeEntry.first + "\", \"ViewType\": \"" + WidgetViewBase::toString(nameTypeEntry.second) + "\" }");
		return false;
	}

	// Get view name list for given owner
	ViewNameTypeList* lst = this->findOrCreateViewNameTypeList(_owner);
	auto lstIt = std::find(lst->begin(), lst->end(), nameTypeEntry);
	if (lstIt != lst->end()) {
		OT_LOG_E("Invalid entry");
		return false;
	}

	// Check if the title already exists and try to create a unique one
	if (this->getViewTitleExists(_view->getViewData().getTitle())) {
		WidgetViewBase newViewData = _view->getViewData();
		newViewData.setTitle(_view->getViewData().getTitle() + " - " + WidgetViewBase::toString(_view->getViewData().getViewType()));
		if (!this->getViewTitleExists(newViewData.getTitle())) {
			_view->setViewData(newViewData);
		}
	}

	_view->getViewDockWidget()->setWindowIcon(ot::IconManager::getApplicationIcon());

	// Add view
	if (!_parentArea) {
		_parentArea = this->determineBestParentArea(_view);
	}

	if (_parentArea) {
		m_dockManager->addDockWidget(_insertArea, _view->getViewDockWidget(), _parentArea);
	}
	else {
		m_dockManager->addDockWidgetTab(_insertArea, _view->getViewDockWidget());
	}

	// Add view toggle (if view is closeable)
	if (_view->getViewDockWidget()->features() & ads::CDockWidget::DockWidgetClosable) {
		m_dockToggleRoot->menu()->addAction(_view->getViewDockWidget()->toggleViewAction());
	}
	
	// Store information
	lst->push_back(nameTypeEntry);
	ViewEntry newViewEntry;
	newViewEntry.first = _owner;
	newViewEntry.second = _view;
	m_views.push_back(newViewEntry);

	// Update focus information
	this->slotViewFocused((m_focusInfo.last ? m_focusInfo.last->getViewDockWidget() : nullptr), _view->getViewDockWidget());

	// Connect signals
	this->connect(_view->getViewDockWidget(), &ads::CDockWidget::closeRequested, this, &WidgetViewManager::slotViewCloseRequested);

	return true;
}

ads::CDockAreaWidget* ot::WidgetViewManager::determineBestParentArea(WidgetView* _newView) const {
	if (_newView->getViewData().getViewFlags() & WidgetViewBase::ViewIsSide) {
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
	else if (m_focusInfo.last) {
		return m_focusInfo.last->getViewDockWidget()->dockAreaWidget();
	}
	else {
		return nullptr;
	}
}

ads::CDockAreaWidget* ot::WidgetViewManager::determineBestRestoreArea(WidgetView* _view) const {
	ads::CDockAreaWidget* area = nullptr;
	if (_view->getViewData().getViewFlags() & WidgetViewBase::ViewIsCentral) {
		area = this->determineBestRestoreArea(_view, WidgetViewBase::ViewIsCentral);
		if (area) return area;
	}
	if (_view->getViewData().getViewFlags() & WidgetViewBase::ViewIsSide) {
		area = this->determineBestRestoreArea(_view, WidgetViewBase::ViewIsSide);
		if (area) return area;
	}
	if (_view->getViewData().getViewFlags() & WidgetViewBase::ViewIsTool) {
		area = this->determineBestRestoreArea(_view, WidgetViewBase::ViewIsTool);
		if (area) return area;
	}

	area = this->determineBestRestoreArea(_view, WidgetViewBase::ViewIsCentral);
	if (area) return area;
	area = this->determineBestRestoreArea(_view, WidgetViewBase::ViewIsSide);
	if (area) return area;
	area = this->determineBestRestoreArea(_view, WidgetViewBase::ViewIsTool);
	return area;
}

ads::CDockAreaWidget* ot::WidgetViewManager::determineBestRestoreArea(WidgetView* _view, WidgetViewBase::ViewFlag _viewType) const {
	for (const ViewEntry& entry : m_views) {
		if (entry.second != _view && entry.second->getViewData().getViewFlags() & _viewType) {
			if (entry.second->getViewDockWidget()->dockAreaWidget()) {
				return entry.second->getViewDockWidget()->dockAreaWidget();
			}
		}
	}
	return nullptr;
}

ot::WidgetViewManager::ViewNameTypeList* ot::WidgetViewManager::findViewNameTypeList(const BasicServiceInformation& _owner) {
	auto it = m_viewOwnerMap.find(_owner);
	if (it == m_viewOwnerMap.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

ot::WidgetViewManager::ViewNameTypeList* ot::WidgetViewManager::findOrCreateViewNameTypeList(const BasicServiceInformation& _owner) {
	ViewNameTypeList* ret = this->findViewNameTypeList(_owner);
	if (!ret) {
		ret = new ViewNameTypeList;
		m_viewOwnerMap.insert_or_assign(_owner, ret);
	}
	return ret;
}