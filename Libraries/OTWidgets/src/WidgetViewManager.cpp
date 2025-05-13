//! @file WidgetViewManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/WidgetViewDockComponentsFactory.h"

// ADS header
#include <ads/DockManager.h>
#include <ads/IconProvider.h>
#include <ads/DockWidgetTab.h>
#include <ads/DockAreaWidget.h>

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qmenu.h>

ot::WidgetViewManager& ot::WidgetViewManager::instance(void) {
	static WidgetViewManager g_instance;
	return g_instance;
}

void ot::WidgetViewManager::initialize(WidgetViewDockManager* _dockManager) {
	if (m_dockManager) {
		OT_LOG_WA("WidgetViewManager already initialized");
		return;
	}

	m_dockManager = _dockManager;

	if (!m_dockManager) {
		using namespace ads;
		
		CDockManager::setConfigFlags(
			CDockManager::AllTabsHaveCloseButton | 
			CDockManager::MiddleMouseButtonClosesTab | 
			CDockManager::MiddleMouseButtonClosesTab |
			CDockManager::FloatingContainerHasWidgetTitle |
			CDockManager::DisableTabTextEliding |
			CDockManager::FocusHighlighting |
			CDockManager::XmlCompressionEnabled
		);

		m_dockManager = new WidgetViewDockManager;
		m_dockManager->setStyleSheet("");
	}

	ads::CDockComponentsFactory::setFactory(new WidgetViewDockComponentsFactory(m_dockManager));
	
	// Initialize custom icons
	ads::CIconProvider& iconProvider = m_dockManager->iconProvider();

	QPixmap pixmap(8, 8);
	pixmap.fill(QColor(0, 0, 0, 0));
	QIcon transparentIcon(pixmap);

	iconProvider.registerCustomIcon(ads::TabCloseIcon, transparentIcon);

	// Initialze dock toggle action
	m_dockToggleRoot = new QAction("Windows");
	QMenu* newMenu = new QMenu;
	m_dockToggleRoot->setMenu(newMenu);

	// Connect signals
	this->connect(m_dockManager, &ads::CDockManager::focusedDockWidgetChanged, this, &WidgetViewManager::slotViewFocused);
}

// ###########################################################################################################################################################################################################################################################################################################################

// View Management

bool ot::WidgetViewManager::addView(const BasicServiceInformation& _owner, WidgetView* _view, const WidgetView::InsertFlags& _insertFlags, WidgetView* _parentView) {
	return this->addViewImpl(_owner, _view, _insertFlags, _parentView);
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

void ot::WidgetViewManager::closeView(WidgetView* _view) {
	OTAssertNullptr(_view);
	if (_view->getViewIsPermanent()) {
		return;
	}

	auto bck = m_state;
	m_state |= CloseViewState;

	// Set the view as deleted by manager so it wont remove itself and remove it from the maps
	_view->m_isDeletedByManager = true;
	this->forgetView(_view);

	// Remove the toggle dock action
	_view->getViewDockWidget()->toggleViewAction()->setVisible(false);
	m_dockToggleRoot->menu()->removeAction(_view->getViewDockWidget()->toggleViewAction());
	_view->getViewDockWidget()->blockSignals(true);
	_view->blockSignals(true);

	// Remove the dock widget itself
	m_dockManager->removeDockWidget(_view->getViewDockWidget());

	// Finally destroy the view
	delete _view;

	m_state = bck;
}

void ot::WidgetViewManager::closeView(const std::string& _entityName, WidgetViewBase::ViewType _type) {
	OTAssertNullptr(m_dockManager);

	WidgetView* view = this->findView(_entityName, _type);
	if (view) {
		this->closeView(view);
	}
}

void ot::WidgetViewManager::closeViews(const BasicServiceInformation& _owner) {
	OTAssertNullptr(m_dockManager);

	ViewNameTypeList* lst = this->findViewNameTypeList(_owner);
	if (lst) {
		ViewNameTypeList tmp = *lst;
		auto bck = m_state;
		m_state |= MulticloseViewState;
		for (const ViewNameTypeListEntry& i : tmp) {
			this->closeView(i.first, i.second);
		}
		m_state = bck;
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

	auto bck = m_state;
	m_state |= MulticloseViewState;
	for (const ViewNameTypeListEntry& i : tmp) {
		this->closeView(i.first, i.second);
	}
	m_state = bck;
}

void ot::WidgetViewManager::requestCloseUnpinnedViews(const WidgetViewBase::ViewFlags& _flags, const SelectionInformation& _activeSelection, bool _ignoreCurrent) {
	std::list<WidgetView*> views;
	for (const ViewEntry& view : m_views) {
		const WidgetViewDock* dock = view.second->getViewDockWidget();
		OTAssertNullptr(dock);
		if ((view.second->getViewData().getViewFlags() & _flags) == _flags && !dock->getIsPinned()) {
			bool concider = true;
			if (!_ignoreCurrent) {
				concider = view.second != m_focusInfo.last &&
					view.second != m_focusInfo.lastCentral &&
					view.second != m_focusInfo.lastSide &&
					view.second != m_focusInfo.lastTool;

				
			}

			if (concider && !_activeSelection.getSelectedNavigationItems().empty()) {
				concider = false;
				for (const UID& active : _activeSelection.getSelectedNavigationItems()) {
					for (const UID& viewSelection : view.second->getSelectionInformation().getSelectedNavigationItems()) {
						if (active == viewSelection) {
							concider = true;
							break;
						}
					}

					if (concider) {
						break;
					}
				}
			}

			if (concider) {
				views.push_back(view.second);
			}
			
		}
	}

	for (WidgetView* view : views) {
		this->handleViewCloseRequest(view);
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

std::string ot::WidgetViewManager::saveState(int _version) {
	OTAssertNullptr(m_dockManager);

	QByteArray tmp = m_dockManager->saveState(_version);
	if (tmp.isEmpty()) {
		return std::string();
	}

	std::string ret;
	ret.reserve((tmp.size() * 3) - 1);

	for (char c : tmp) {
		if (!ret.empty()) ret.push_back(';');
		ret.append(std::to_string((int)c));
	}

	if (m_initialState.empty()) {
		m_initialState = ret;
		m_initialStateVersion = _version;
	}

	return ret;
}

bool ot::WidgetViewManager::restoreState(std::string _state, int _version) {
	OTAssertNullptr(m_dockManager);

	if (_state.empty()) return false;

	// Save the current state as the initial state if no initial state found yet.
	if (m_initialState.empty()) {
		this->saveState(_version);
	}

	QByteArray tmp;

	size_t ix = _state.find(';');
	bool convertFail = false;
	while (ix != std::string::npos) {
		tmp.append((char)ot::String::toNumber<int>(_state.substr(0, ix), convertFail));
		if (convertFail) {
			OT_LOG_E("State contains invalid characters");
			return false;
		}
		_state.erase(0, ix + 1);
		ix = _state.find(';');
	}

	if (!_state.empty()) {
		tmp.append((char)ot::String::toNumber<int>(_state, convertFail));
	}

	bool result = m_dockManager->restoreState(tmp, _version);

	this->slotUpdateViewVisibility();
	
	return result;
}

void ot::WidgetViewManager::applyInitialState(void) {
	if (m_initialState.empty()) {
		OT_LOG_D("Initial state is empty.");
	}
	else {
		this->restoreState(m_initialState, m_initialStateVersion);
	}	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Information gathering

ot::BasicServiceInformation ot::WidgetViewManager::getOwnerFromView(WidgetView* _view) const {
	for (const auto& it : m_views) {
		if (it.second == _view) {
			return it.first;
		}
	}

	OT_LOG_EA("View not found");
	return BasicServiceInformation();
}

ot::WidgetView* ot::WidgetViewManager::findViewFromWidget(QWidget* _widget) const {
	for (const ViewEntry& view : m_views) {
		if (view.second->getViewWidget() == _widget) {
			return view.second;
		}
	}
	return nullptr;
}

bool ot::WidgetViewManager::getViewExists(const std::string& _entityName, WidgetViewBase::ViewType _type) const {
	return this->findView(_entityName, _type) != nullptr;
}

bool ot::WidgetViewManager::getViewTitleExists(const std::string& _title) const {
	return this->findViewFromTitle(_title) != nullptr;
}

ot::WidgetViewManager::ViewNameTypeList ot::WidgetViewManager::getViewNamesFromOwner(const BasicServiceInformation& _owner) const {
	auto it = m_viewOwnerMap.find(_owner);
	if (it == m_viewOwnerMap.end()) {
		return ViewNameTypeList();
	}
	else {
		return std::move(ViewNameTypeList(*it->second));
	}
}

std::list<std::string> ot::WidgetViewManager::getViewNamesFromOwner(const BasicServiceInformation& _owner, WidgetViewBase::ViewType _type) const {
	std::list<std::string> result;
	for (const ViewNameTypeListEntry& entry : this->getViewNamesFromOwner(_owner)) {
		if (entry.second == _type) {
			result.push_back(entry.first);
		}
	}
	return std::move(result);
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
	if (m_state & MulticloseViewState) {
		return;
	}

	WidgetView* o = this->getViewFromDockWidget(_oldFocus);
	WidgetView* n = this->getViewFromDockWidget(_newFocus);

	if (n) {
		m_focusInfo.last = n;
		if (n->getViewData().getViewFlags() & WidgetViewBase::ViewIsCentral) {
			m_focusInfo.lastCentral = n;
			if (!(m_config & IgnoreInputFocusOnViewInsert && m_state & InsertViewState) && m_config & InputFocusCentralViewOnFocusChange) {
				n->setViewWidgetFocus();
			}
		}
		if (n->getViewData().getViewFlags() & WidgetViewBase::ViewIsSide) {
			m_focusInfo.lastSide = n;
			if (!(m_config & IgnoreInputFocusOnViewInsert && m_state & InsertViewState) && m_config & InputFocusSideViewOnFocusChange) {
				n->setViewWidgetFocus();
			}
		}
		if (n->getViewData().getViewFlags() & WidgetViewBase::ViewIsTool) {
			m_focusInfo.lastTool = n;
			if (!(m_config & IgnoreInputFocusOnViewInsert && m_state & InsertViewState) && m_config & InputFocusToolViewOnFocusChange) {
				n->setViewWidgetFocus();
			}
		}
		Q_EMIT viewFocusChanged(n, o);
	}
}

void ot::WidgetViewManager::slotViewCloseRequested(void) {
	if (m_state & MulticloseViewState) {
		OT_LOG_E("Enexpected event");
		return;
	}

	WidgetView* view = dynamic_cast<WidgetView*>(sender());
	if (view) {
		this->handleViewCloseRequest(view);
	}
	else {
		OT_LOG_E("View not found");
	}
}

void ot::WidgetViewManager::slotUpdateViewVisibility(void) {
	if (m_state & MulticloseViewState) {
		return;
	}

	for (const ViewEntry& entry : m_views) {
		if (!(entry.second->getViewData().getViewFlags() & WidgetViewBase::ViewFlag::ViewIsCloseable) && 
			!entry.second->getViewDockWidget()->dockAreaWidget()) 
		{
			OT_LOG_W("Restored");
			m_dockManager->addView(entry.second, nullptr, ot::WidgetView::NoInsertFlags);
		}
	}
}

void ot::WidgetViewManager::slotViewTabClicked(void) {
	if (m_state & MulticloseViewState) {
		OT_LOG_E("Enexpected event");
		return;
	}

	ads::CDockWidgetTab* tab = dynamic_cast<ads::CDockWidgetTab*>(sender());
	if (!tab) {
		return;
	}

	WidgetView* view = this->getViewFromDockWidget(tab->dockWidget());
	if (!view) {
		return;
	}

	Q_EMIT viewTabClicked(view);
}

void ot::WidgetViewManager::slotViewDataModifiedChanged(void) {
	if (m_state & MulticloseViewState) {
		return;
	}

	ot::WidgetView* view = dynamic_cast<ot::WidgetView*>(sender());
	if (!view) {
		return;
	}

	Q_EMIT viewDataModifiedChanged(view);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

ot::WidgetViewManager::WidgetViewManager() :
	m_dockManager(nullptr), m_dockToggleRoot(nullptr), m_config(NoFlags), m_state(DefaultState), 
	m_dockComponentsFactory(nullptr), m_initialStateVersion(0)
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

bool ot::WidgetViewManager::addViewImpl(const BasicServiceInformation& _owner, WidgetView* _view, const WidgetView::InsertFlags& _insertFlags, WidgetView* _parentView) {
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

	// Determine parent area
	ads::CDockAreaWidget* area = nullptr;
	if (_parentView) {
		area = _parentView->getViewDockWidget()->dockAreaWidget();
	}
	else if (m_config & UseBestAreaFinderOnViewInsert) {
		area = this->getBestDockArea(_view);
	}

	// If the current focus settings should not change store the current active views
	std::list<WidgetView*> focusedViews;
	bool dockSignalsBlocked = m_dockManager->signalsBlocked();

	if (_insertFlags & WidgetView::KeepCurrentFocus) {
		for (const ViewEntry& entry : m_views) {
			if (entry.second->isCurrentViewTab()) {
				focusedViews.push_back(entry.second);
			}
		}

		m_dockManager->blockSignals(true);
	}

	// Add the new view
	m_state |= InsertViewState;
	m_dockManager->addView(_view, area, _insertFlags);
	m_state &= (~InsertViewState);

	// Add view toggle (only visible if dock is closeable)
	m_dockToggleRoot->menu()->addAction(_view->getViewDockWidget()->toggleViewAction());
	
	// Store information
	lst->push_back(nameTypeEntry);
	ViewEntry newViewEntry;
	newViewEntry.first = _owner;
	newViewEntry.second = _view;
	m_views.push_back(newViewEntry);

	// Update focus information or reset the current focus depending on the insert mode
	if (_insertFlags & WidgetView::KeepCurrentFocus) {
		for (WidgetView* view : focusedViews) {
			view->setAsCurrentViewTab();
		}
		m_dockManager->blockSignals(dockSignalsBlocked);
	}
	else {
		m_state |= InsertViewState;
		this->slotViewFocused((m_focusInfo.last ? m_focusInfo.last->getViewDockWidget() : nullptr), _view->getViewDockWidget());
		m_state &= (~InsertViewState);
	}

	// Connect signals
	this->connect(_view, &WidgetView::closeRequested, this, &WidgetViewManager::slotViewCloseRequested);
	this->connect(_view, &WidgetView::viewDataModifiedChanged, this, &WidgetViewManager::slotViewDataModifiedChanged);
	if (_view->getViewDockWidget()->tabWidget()) {
		this->connect(_view->getViewDockWidget()->tabWidget(), &ads::CDockWidgetTab::clicked, this, &WidgetViewManager::slotViewTabClicked);
	}
	
	return true;
}

ads::CDockAreaWidget* ot::WidgetViewManager::getBestDockArea(const WidgetView* _view) const {
	if (_view->getViewData().getViewFlags() & WidgetViewBase::ViewIsTool) {
		if (m_focusInfo.lastTool) {
			return m_focusInfo.lastTool->getViewDockWidget()->dockAreaWidget();
		}
		else {
			ads::CDockAreaWidget* area = this->getFirstMatchingView(WidgetViewBase::ViewIsTool);
			if (area) {
				return area;
			}
		}
	}

	if (_view->getViewData().getViewFlags() & (WidgetViewBase::ViewIsTool | WidgetViewBase::ViewIsSide)) {
		if (m_focusInfo.lastSide) {
			return m_focusInfo.lastSide->getViewDockWidget()->dockAreaWidget();
		}
		else {
			ads::CDockAreaWidget* area = this->getFirstMatchingView(WidgetViewBase::ViewIsSide);
			if (area) {
				return area;
			}
		}
	}

	if (m_focusInfo.lastCentral) {
		return m_focusInfo.lastCentral->getViewDockWidget()->dockAreaWidget();
	}
	else {
		return this->getFirstMatchingView(WidgetViewBase::ViewIsCentral);
	}
}

ads::CDockAreaWidget* ot::WidgetViewManager::getFirstMatchingView(WidgetViewBase::ViewFlag _viewTypeFlag) const {
	for (const ViewEntry& entry : m_views) {
		if (entry.second->getViewData().getViewFlags() & _viewTypeFlag) {
			return entry.second->getViewDockWidget()->dockAreaWidget();
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

void ot::WidgetViewManager::handleViewCloseRequest(WidgetView* _view) {
	if (_view->getViewData().getViewFlags() & WidgetViewBase::ViewDefaultCloseHandling) {
		_view->getViewDockWidget()->toggleView(_view->getViewDockWidget()->isClosed());
	}
	else {
		Q_EMIT viewCloseRequested(_view);
	}
}
