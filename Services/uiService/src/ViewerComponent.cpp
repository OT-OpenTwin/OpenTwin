// @otlicense
// File: ViewerComponent.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

 // Wrapper header
#include <ViewerComponent.h>				// Corresponding header
#include <ExternalServicesComponent.h>		// Model Component
#include "AppBase.h"
#include "ToolBar.h"
#include "UITestLogs.h"
#include "ControlsManager.h"
#include "ShortcutManager.h"
#include "UserSettings.h"
#include "OTCore/ReturnMessage.h"

#include "OTSystem/OTAssert.h"
#include "OTCore/RuntimeTests.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/TableView.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PlotView.h"
#include "OTWidgets/PlotDataset.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/GraphicsViewView.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/GlobalWidgetViewManager.h"

// C++ header
#include <exception>

// Qt header
#include <qwidget.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qapplication.h>

// AK header
#include <akAPI/uiAPI.h>
#include <akCore/aException.h>

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_VIEWECOMPONENT_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_VIEWECOMPONENT_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_VIEWECOMPONENT_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_VIEWECOMPONENT_PERFORMANCETEST_ENABLED==true
#define OT_TEST_VIEWECOMPONENT_Interval(___testText) OT_TEST_Interval(ot_intern_viewercomponent_lcl_performancetest, "ViewerComponent", ___testText)
#else
#define OT_TEST_VIEWECOMPONENT_Interval(___testText)
#endif

ViewerComponent::ViewerComponent()
	: ot::ServiceBase("ViewerComponent", "Viewer", "127.0.0.1", ot::invalidServiceID), processingGroupCounter(0), treeSelectionReceived(false) {
}

ViewerComponent::~ViewerComponent() {}

// ###########################################################################################################################################################################################################################################################################################################################

// General

void ViewerComponent::addKeyShortcut(const std::string& keySequence) {
	KeyboardCommandHandler* newHandler = new KeyboardCommandHandler(nullptr, AppBase::instance(), keySequence.c_str());
	newHandler->setAsViewerHandler(true);
	AppBase::instance()->shortcutManager()->addHandler(newHandler);
}

void ViewerComponent::lockSelectionAndModification(bool flag) {
	ot::WindowAPI::lockUI(flag);
}

void ViewerComponent::removeViewer(ot::UID viewerID) {
	for (auto pos = m_viewers.begin(); pos != m_viewers.end(); pos++) {
		if (*pos == viewerID) {
			m_viewers.erase(pos);
			return;
		}
	}
}

void ViewerComponent::removeUIElements(std::list<ViewerUIDtype>& itemIDList) {
	try {
		try {
			std::vector<ViewerUIDtype> i;
			for (auto itm : itemIDList) { i.push_back(itm); }
			AppBase* app = AppBase::instance();
			app->destroyObjects(i);
			for (auto itm : i) {
				app->controlsManager()->uiControlWasDestroyed(itm);
				app->lockManager()->uiElementDestroyed(itm);
			}

			AppBase::instance()->shortcutManager()->clearViewerHandler();
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::removeUIElements()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::removeUIElements()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::removeUIElements()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to remove UI Elements.", _e.what()); }
}

void ViewerComponent::displayText(const std::string& text) {
	AppBase::instance()->appendInfoMessage(QString::fromStdString(text));
}

void ViewerComponent::enableDisableControls(const ot::UIDList& _enabledControls, bool _resetDisabledCounterForEnabledControls, const ot::UIDList& _disabledControls) {
	try {
		try {
			LockManager* lockManager = AppBase::instance()->lockManager();
			OTAssertNullptr(lockManager);

			ot::BasicServiceInformation bsi = this->getBasicServiceInformation();

			for (ot::UID objectID : _enabledControls) {
				lockManager->enable(bsi, objectID, _resetDisabledCounterForEnabledControls);
			}
			for (ot::UID objectID : _disabledControls) {
				lockManager->disable(bsi, objectID);
			}
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::enableDisableControls()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::enableDisableControls()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::enableDisableControls()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to update controls enabled state.", _e.what()); }
}

void ViewerComponent::entitiesSelected(ot::serviceID_t replyTo, const std::string& selectionAction, const std::string& selectionInfo, std::list<std::string>& optionNames, std::list<std::string>& optionValues) {
	try {
		try {
			AppBase::instance()->getExternalServicesComponent()->entitiesSelected(ViewerAPI::getActiveDataModel(), replyTo, selectionAction, selectionInfo, optionNames, optionValues);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::entitiesSelected()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::entitiesSelected()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::entitiesSelected()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to handle entities selected.", _e.what()); }
}

void ViewerComponent::rubberbandFinished(ot::serviceID_t creatorId, const std::string& note, const std::string& pointJson, const std::vector<double>& transform) {
	AppBase::instance()->getExternalServicesComponent()->sendRubberbandResultsToService(creatorId, note, pointJson, transform);
}

void ViewerComponent::updateSettings(const ot::PropertyGridCfg& _config) {
	UserSettings::instance().addSettings(VIEWER_SETTINGS_NAME, _config);
}

void ViewerComponent::loadSettings(ot::PropertyGridCfg& _config) {
	ot::PropertyGridCfg oldConfig = AppBase::instance()->getSettingsFromDataBase(VIEWER_SETTINGS_NAME);
	_config.mergeWith(oldConfig, ot::PropertyBase::MergeValues | ot::PropertyBase::AddMissing);
}

void ViewerComponent::saveSettings(const ot::PropertyGridCfg& _config) {
	AppBase::instance()->storeSettingToDataBase(_config, VIEWER_SETTINGS_NAME);
}

void ViewerComponent::updateVTKEntity(unsigned long long modelEntityID) {
	AppBase::instance()->getExternalServicesComponent()->requestUpdateVTKEntity(modelEntityID);
}

std::string ViewerComponent::messageModelService(const std::string& _message) {
	std::string response;
	AppBase::instance()->getExternalServicesComponent()->sendToModelService(_message, response);
	return response;
}

void ViewerComponent::removeGraphicsElements(ot::UID _modelID) {
	//If entity is has a block item associated, it gets removed from all editors.
	std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
	for (auto view : views) {
		view->getGraphicsView()->removeItem(_modelID);
		view->getGraphicsView()->removeConnection(_modelID);
	}

}

std::string ViewerComponent::getOpenFileName(const std::string& _title, const std::string& _path, const std::string& _filters) {
	return QFileDialog::getOpenFileName(
		AppBase::instance()->mainWindow(),
		QString::fromStdString(_title),
		QString::fromStdString(_path),
		QString::fromStdString(_filters)
	).toStdString();
}

std::string ViewerComponent::getSaveFileName(const std::string& _title, const std::string& _path, const std::string& _filters) {
	return QFileDialog::getSaveFileName(
		AppBase::instance()->mainWindow(),
		QString::fromStdString(_title),
		QString::fromStdString(_path),
		QString::fromStdString(_filters)
	).toStdString();
}

void ViewerComponent::setProgressBarVisibility(const QString& _text, bool _visible, bool _continous) {
	ot::WindowAPI::setProgressBarVisibility(_text, _visible, _continous);
}

void ViewerComponent::setProgressBarValue(int _value) {
	ot::WindowAPI::setProgressBarValue(_value);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Tree

void ViewerComponent::clearTree(void) {
	try {
		try {
			AppBase::instance()->clearNavigationTree();
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::clearTree()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::clearTree()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::clearTree()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to clear naviagtion tree.", _e.what()); }
}

ot::UID ViewerComponent::addTreeItem(const std::string& treePath, bool editable, bool selectChildren) {
	try {
		try {
			ot::UID id = AppBase::instance()->addNavigationTreeItem(treePath.c_str(), '/', editable, selectChildren);
			return id;
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addTreeItem()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addTreeItem()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add tree item.", _e.what()); }
	return 0;
}

void ViewerComponent::setTreeItemIcon(ot::UID treeItemID, int iconSize, const std::string& iconName) {
	try {
		try {
			if (!iconName.empty()) {
				//NOTE, add proper item path
				AppBase::instance()->setNavigationTreeItemIcon(treeItemID, iconName.c_str(), "Default");
			}
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setTreeItemIcon()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setTreeItemIcon()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setTreeItemIcon()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set tree item icon.", _e.what()); }
}

void ViewerComponent::setTreeItemText(ot::UID treeItemID, const std::string& text) {
	try {
		try {
			AppBase::instance()->setNavigationTreeItemText(treeItemID, text.c_str());
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setTreeItemIcon()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setTreeItemIcon()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setTreeItemIcon()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set tree item text.", _e.what()); }
}

void ViewerComponent::removeTreeItems(std::list<ot::UID> treeItemIDList) {
	try {
		try {
			std::vector<ot::UID> items;
			for (auto itm : treeItemIDList) { items.push_back(itm); }
			AppBase::instance()->removeNavigationTreeItems(items);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::removeTreeItems()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::removeTreeItems()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::removeTreeItems()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to remove tree items.", _e.what()); }
}

void ViewerComponent::selectTreeItem(ot::UID treeItemID) {
	try {
		try {
			OT_SLECTION_TEST_LOG("Select item from viewer");
			AppBase::instance()->setNavigationTreeItemSelected(treeItemID, true);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::selectTreeItem()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::selectTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::selectTreeItem()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to select tree item.", _e.what()); }
}

void ViewerComponent::selectSingleTreeItem(ot::UID treeItemID) {
	try {
		try {
			OT_SLECTION_TEST_LOG("Select single item from viewer");
			AppBase::instance()->setSingleNavigationTreeItemSelected(treeItemID, true);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::selectSingleTreeItem()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::selectSingleTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::selectSingleTreeItem()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to select single tree item.", _e.what()); }
}

void ViewerComponent::expandSingleTreeItem(ot::UID treeItemID) {
	try {
		try { AppBase::instance()->expandSingleNavigationTreeItem(treeItemID, true); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::expandSingleTreeItem()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::expandSingleTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::expandSingleTreeItem()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to expand single tree item.", _e.what()); }
}

bool ViewerComponent::isTreeItemExpanded(ot::UID treeItemID) {
	try {
		try { return AppBase::instance()->isTreeItemExpanded(treeItemID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::isTreeItemExpanded()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::isTreeItemExpanded()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::isTreeItemExpanded()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to check if tree item is expanded.", _e.what()); }

	return false;
}

void ViewerComponent::toggleTreeItemSelection(ot::UID treeItemID, bool considerChilds) {
	try {
		try {
			OT_SLECTION_TEST_LOG("Toggle item selection from viewer");
			AppBase::instance()->toggleNavigationTreeItemSelection(treeItemID, considerChilds);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::toggleTreeItemSelection()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::toggleTreeItemSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::toggleTreeItemSelection()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to toggle tree item selection.", _e.what()); }
}

void ViewerComponent::clearTreeSelection(void) {
	try {
		try {
			OT_SLECTION_TEST_LOG("Clear item selection from viewer");
			AppBase::instance()->clearNavigationTreeSelection();
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::clearTreeSelection()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::clearTreeSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::clearTreeSelection()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to clear tree item selection.", _e.what()); }
}

void ViewerComponent::refreshSelection(void) {
	ot::SelectionData selectionData;
	selectionData.setSelectionOrigin(ot::SelectionOrigin::Custom);
	selectionData.setSelectedTreeItems(AppBase::instance()->getSelectedNavigationTreeItems().getSelectedNavigationItems());
	selectionData.setKeyboardModifiers(QApplication::keyboardModifiers());
	this->handleSelectionChanged(selectionData);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property Grid

void ViewerComponent::fillPropertyGrid(const ot::PropertyGridCfg& _configuration) {
	AppBase::instance()->setupPropertyGrid(_configuration);
}

void ViewerComponent::clearModalPropertyGrid() {
	AppBase::instance()->clearModalPropertyGrid();
}

void ViewerComponent::setDoublePropertyValue(const std::string& _groupName, const std::string& _itemName, double value) {
	ot::PropertyGridItem* itm = AppBase::instance()->findProperty(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property not found { \"group\": \"" + _groupName + "\", \"name\": \"" + _itemName + "\" }");
		return;
	}
	ot::PropertyInputDouble* inp = dynamic_cast<ot::PropertyInputDouble*>(itm->getInput());
	if (!inp) {
		OT_LOG_E("PropertyInput cast failed");
		return;
	}

	inp->setValue(value);
}

double ViewerComponent::getDoublePropertyValue(const std::string& _groupName, const std::string& _itemName) {
	ot::PropertyGridItem* itm = AppBase::instance()->findProperty(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property not found { \"group\": \"" + _groupName + "\", \"name\": \"" + _itemName + "\" }");
		return 0.;
	}
	ot::PropertyInputDouble* inp = dynamic_cast<ot::PropertyInputDouble*>(itm->getInput());
	if (!inp) {
		OT_LOG_E("PropertyInput cast failed");
		return 0.;
	}
	return inp->getValue();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Plot

void ViewerComponent::setCurveDimmed(const std::string& _plotName, ot::UID _entityID, bool _setDimmed) {
	const ot::PlotView* plotView = AppBase::instance()->findPlot(_plotName, {});
	if (!plotView) {
		OT_LOG_E("Plot not found \"" + _plotName + "\"");
		return;
	}
	ot::Plot* plot = plotView->getPlot();
	auto allCurves = plot->findDatasets(_entityID);

	for (auto curve : allCurves) {
		curve->setDimmed(_setDimmed, true);
	}

	plot->refresh();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Views

void ViewerComponent::closeView(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {
	switch (_viewType) {
	case ot::WidgetViewBase::ViewText:
		AppBase::instance()->closeTextEditor(_entityName);
		break;

	case ot::WidgetViewBase::ViewTable:
		AppBase::instance()->closeTable(_entityName);
		break;

	case ot::WidgetViewBase::View1D:
		AppBase::instance()->closePlot(_entityName);
		break;

	case ot::WidgetViewBase::ViewVersion:
	case ot::WidgetViewBase::ViewGraphics:
	case ot::WidgetViewBase::ViewGraphicsPicker:
	case ot::WidgetViewBase::ViewProperties:
	case ot::WidgetViewBase::ViewNavigation:
	case ot::WidgetViewBase::View3D:
	case ot::WidgetViewBase::CustomView:
		OT_LOG_EAS("The view type (" + std::to_string((int)_viewType) + ") is not supported for internal close request");
		break;

	default:
		OT_LOG_EAS("Unknown view type (" + std::to_string((int)_viewType) + ")");
		break;
	}
}

bool ViewerComponent::hasViewFocus(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {
	const ot::WidgetView* view = ot::GlobalWidgetViewManager::instance().getCurrentlyFocusedView();
	if (!view) {
		return false;
	}
	else if (view->getViewData().getEntityName() == _entityName && view->getViewData().getViewType() == _viewType) {
		return true;
	}
	else {
		return false;
	}
}

void ViewerComponent::addVisualizingEntityToView(ot::UID _treeItemId, const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {
	ot::WidgetView* view = ot::GlobalWidgetViewManager::instance().findView(_entityName, _viewType);
	if (view) {
		view->addVisualizingItem(_treeItemId);
	}
}

void ViewerComponent::removeVisualizingEntityFromView(ot::UID _treeItemId, const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {
	ot::WidgetView* view = ot::GlobalWidgetViewManager::instance().findView(_entityName, _viewType);
	if (view) {
		view->removeVisualizingItem(_treeItemId);
	}
}

void ViewerComponent::clearVisualizingEntitesFromView(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {
	ot::WidgetView* view = ot::GlobalWidgetViewManager::instance().findView(_entityName, _viewType);
	if (view) {
		view->clearVisualizingItems();
	}
}

ot::WidgetView* ViewerComponent::getCurrentView(void) {
	return ot::GlobalWidgetViewManager::instance().getCurrentlyFocusedView();
}

ot::WidgetView* ViewerComponent::getLastFocusedCentralView(void) {
	return ot::GlobalWidgetViewManager::instance().getLastFocusedCentralView();
}

bool ViewerComponent::getCurrentViewIsModified(void) {
	ot::WidgetView* view = this->getCurrentView();
	if (view) {
		return view->getViewContentModified();
	}
	else {
		return false;
	}
}

void ViewerComponent::setCurrentVisualizationTabFromEntityName(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {
	AppBase::instance()->setCurrentVisualizationTabFromEntityName(_entityName, _viewType);
}

void ViewerComponent::setCurrentVisualizationTabFromTitle(const std::string& _tabTitle) {
	AppBase::instance()->setCurrentVisualizationTabFromTitle(_tabTitle);
}

std::string ViewerComponent::getCurrentVisualizationTabTitle(void) {
	return AppBase::instance()->getCurrentVisualizationTabTitle();
}

void ViewerComponent::requestSaveForCurrentVisualizationTab(void) {
	ot::WidgetView* view = ot::GlobalWidgetViewManager::instance().getCurrentlyFocusedView();
	if (!view) {
		OT_LOG_W("No view focused");
		return;
	}

	if (!(view->getViewData().getViewFlags() & ot::WidgetViewBase::ViewIsCentral)) {
		OT_LOG_E("Non central view in focus");
		return;
	}

	switch (view->getViewData().getViewType()) {
	case ot::WidgetViewBase::View3D:
		break;
	case ot::WidgetViewBase::View1D:
		break;
	case ot::WidgetViewBase::ViewText:
	{
		ot::TextEditorView* actualView = dynamic_cast<ot::TextEditorView*>(view);
		if (!actualView) {
			OT_LOG_E("View cast failed");
			return;
		}
		actualView->getTextEditor()->slotSaveRequested();
	}
	break;
	case ot::WidgetViewBase::ViewTable:
	{
		ot::TableView* actualView = dynamic_cast<ot::TableView*>(view);
		if (!actualView) {
			OT_LOG_E("View cast failed");
			return;
		}
		actualView->getTable()->slotSaveRequested();
	}
	break;
	case ot::WidgetViewBase::ViewVersion:
		break;
	case ot::WidgetViewBase::ViewGraphics:
		break;
	case ot::WidgetViewBase::ViewGraphicsPicker:
		break;
	case ot::WidgetViewBase::ViewProperties:
		break;
	case ot::WidgetViewBase::ViewNavigation:
		break;
	case ot::WidgetViewBase::CustomView:
		break;
	default:
		break;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ToolBar

ViewerUIDtype ViewerComponent::addMenuPage(const std::string& pageName) {
	try {
		try {
			ot::UID page = AppBase::instance()->getToolBar()->addPage(AppBase::instance()->getViewerUID(), pageName.c_str());
			AppBase::instance()->controlsManager()->uiElementCreated(this->getBasicServiceInformation(), page, false);
			return page;
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addMenuPage()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addMenuPage()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addMenuPage()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add menu page.", _e.what()); }
	return 0;
}

ViewerUIDtype ViewerComponent::addMenuGroup(ViewerUIDtype menuPageID, const std::string& groupName) {
	try {
		try {
			ot::UID group = AppBase::instance()->getToolBar()->addGroup(AppBase::instance()->getViewerUID(), menuPageID, groupName.c_str());
			AppBase::instance()->controlsManager()->uiElementCreated(this->getBasicServiceInformation(), group, false);
			return group;
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addMenuGroup()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addMenuGroup()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addMenuGroup()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add menu group.", _e.what()); }
	return 0;
}

ViewerUIDtype ViewerComponent::addMenuSubGroup(ViewerUIDtype _menuGroupID, const std::string& _subGroupName) {
	ot::UID subGroup = AppBase::instance()->getToolBar()->addSubGroup(AppBase::instance()->getViewerUID(), _menuGroupID, QString::fromStdString(_subGroupName));
	AppBase::instance()->controlsManager()->uiElementCreated(this->getBasicServiceInformation(), subGroup, false);
	return subGroup;
}

ViewerUIDtype ViewerComponent::addMenuPushButton(ViewerUIDtype menuGroupID, const std::string& buttonName, const std::string& iconName) {
	try {
		try {
			//NOTE, add actual icon path
			ViewerUIDtype btnUid = AppBase::instance()->getToolBar()->addToolButton(AppBase::instance()->getViewerUID(), menuGroupID, iconName.c_str(), "Default", buttonName.c_str(), this);
			AppBase::instance()->controlsManager()->uiElementCreated(this->getBasicServiceInformation(), btnUid, true);
			ot::LockTypes flags;
			flags.set(ot::LockType::All);
			//flags.set(ot::LockType::ViewWrite);
			flags.set(ot::LockType::ViewRead);
			AppBase::instance()->lockManager()->uiElementCreated(this->getBasicServiceInformation(), btnUid, flags);

			return btnUid;
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addMenuPushButton()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addMenuPushButton()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addMenuPushButton()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add menu push button.", _e.what()); }
	return 0;
}

ViewerUIDtype ViewerComponent::addMenuPushButton(ViewerUIDtype menuGroupID, const std::string& buttonName, const std::string& iconName, const std::string& keySequence) {
	ViewerUIDtype uid = addMenuPushButton(menuGroupID, buttonName, iconName);

	KeyboardCommandHandler* newHandler = new KeyboardCommandHandler(nullptr, AppBase::instance(), keySequence.c_str());
	newHandler->setAsViewerHandler(true);
	newHandler->attachToEvent(uid, ak::etClicked, 0, 0);
	AppBase::instance()->shortcutManager()->addHandler(newHandler);

	ak::uiAPI::toolButton::setToolTip(uid, QString::fromStdString(buttonName + " (" + keySequence + ")"));

	return uid;
}

void ViewerComponent::setMenuPushButtonToolTip(ViewerUIDtype _buttonID, const std::string& _toolTip) {
	ak::uiAPI::toolButton::setToolTip(_buttonID, QString::fromStdString(_toolTip));
}

void ViewerComponent::setCurrentMenuPage(const std::string& _pageName) {
	AppBase::instance()->switchToMenuTab(_pageName);
}

std::string ViewerComponent::getCurrentMenuPage(void) {
	return AppBase::instance()->getCurrentMenuTab();
}

// ###########################################################################################################################################################################################################################################################################################################################

void ViewerComponent::setProcessingGroupOfMessages(bool flag) {
	if (flag) {
		if (processingGroupCounter == 0) {
			treeSelectionReceived = false;
		}
		processingGroupCounter++;
	}
	else {
		assert(processingGroupCounter > 0);
		processingGroupCounter--;
		if (processingGroupCounter == 0) {
			// Process all delayed actions 
			if (treeSelectionReceived) {
				ot::SelectionData selectionData;
				selectionData.setSelectionOrigin(ot::SelectionOrigin::Custom);
				selectionData.setSelectedTreeItems(AppBase::instance()->getSelectedNavigationTreeItems().getSelectedNavigationItems());
				selectionData.setKeyboardModifiers(QApplication::keyboardModifiers());
				this->handleSelectionChanged(selectionData);
			}
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Intern calls

void ViewerComponent::notify(
	ot::UID									_senderId,
	ak::eventType						_event,
	int										_info1,
	int										_info2
) {
	try {
		try {
			if (_event & ak::etClicked) {
				ViewerAPI::executeAction(_senderId);
			}
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::notify()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::notify()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::notify()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to handle internal notify.", _e.what()); }
}

void ViewerComponent::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const {
	ViewerAPI::getDebugInformation(_object, _allocator);
}

ot::SelectionHandlingResult ViewerComponent::handleSelectionChanged(const ot::SelectionData& _selectionData) {
	ot::SelectionHandlingResult result;

	if (processingGroupCounter > 0) {
		treeSelectionReceived = true;
		OT_SLECTION_TEST_LOG("Skipping selection changed");
		return result;
	}

	OT_SLECTION_TEST_LOG(std::string("Handling selection changed. Modifier pressed: ") + (_selectionData.getKeyboardModifiers() & Qt::ControlModifier ? "true" : "false"));

	// Send the selection changed notification to the viewer component and the model component
	std::list<ot::UID> selectedModelItems, selectedVisibleModelItems;
	result = ViewerAPI::setSelectedTreeItems(_selectionData, selectedModelItems, selectedVisibleModelItems);

	// If the model was already notified it means that the selection handling already triggered a notification.
	if (true || !(result & ot::SelectionHandlingEvent::ModelWasNotified)) {
		ot::UID activeModel = ViewerAPI::getActiveDataModel();
		if (activeModel > 0) {
			result |= ot::SelectionHandlingEvent::ModelWasNotified;
			AppBase::instance()->getExternalServicesComponent()->modelSelectionChangedNotification(activeModel, selectedModelItems, selectedVisibleModelItems);
		}
	}

	OT_SLECTION_TEST_LOG(">> Handle selection change completed");

	return result;
}

ViewerUIDtype ViewerComponent::getActiveDataModel() {
	try {
		try {
			return ViewerAPI::getActiveDataModel();
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::getActiveDataModel()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::getActiveDataModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getActiveDataModel()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to get active data model.", _e.what()); }
	return 0;
}

ViewerUIDtype ViewerComponent::getActiveViewerModel() {
	try {
		try {
			return ViewerAPI::getActiveViewerModel();
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::getActiveViewerModel()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::getActiveViewerModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getActiveViewerModel()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to get active viewer model.", _e.what()); }
	return 0;
}

void ViewerComponent::resetAllViews3D(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::resetAllViews3D(visualizationModelID);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::resetAllViews3D()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::resetAllViews3D()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::resetAllViews3D()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to reset all 3D views.", _e.what()); }
}

void ViewerComponent::refreshAllViews(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::refreshAllViews(visualizationModelID);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::refreshAllViews()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::refreshAllViews()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::refreshAllViews()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to refresh all views.", _e.what()); }
}

void ViewerComponent::setTreeStateRecording(ViewerUIDtype visualizationModelID, bool flag) {
	try {
		try {
			ViewerAPI::setTreeStateRecording(visualizationModelID, flag);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setTreeStateRecording()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setTreeStateRecording()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setTreeStateRecording()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set tree state recording flag.", _e.what()); }
}

void ViewerComponent::clearSelection(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::clearSelection(visualizationModelID);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::clearSelection()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::clearSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::clearSelection()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to clear selection.", _e.what()); }
}

void ViewerComponent::refreshSelection(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::refreshSelection(visualizationModelID);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::refreshSelection()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::refreshSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::refreshSelection()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to refresh selection.", _e.what()); }
}

void ViewerComponent::selectObject(ModelUIDtype visualizationModelID, ot::UID entityID) {
	try {
		try {
			ViewerAPI::selectObject(visualizationModelID, entityID);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::selectObject()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::selectObject()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::selectObject()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to select object.", _e.what()); }
}

void ViewerComponent::addNodeFromFacetData(ViewerUIDtype visModelID, const std::string& treeName, double surfaceColorRGB[3],
	double edgeColorRGB[3], ViewerUIDtype modelEntityID, const OldTreeIcon& treeIcons, bool backFaceCulling, double offsetFactor, bool isEditable, std::vector<Geometry::Node>& nodes,
	std::list<Geometry::Triangle>& triangles, std::list<Geometry::Edge>& edges, std::map<ot::UID, std::string>& faceNameMap, std::string& errors,
	bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected) {
	try {
		try {
			ViewerAPI::addNodeFromFacetData(visModelID, treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons,
				backFaceCulling, offsetFactor, isEditable, nodes, triangles, edges, faceNameMap, errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addNodeFromFacetData()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addNodeFromFacetData()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addNodeFromFacetData()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add node from facet data.", _e.what()); }
}

void ViewerComponent::addNodeFromFacetDataBase(ViewerUIDtype visModelID, const std::string& treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective, ModelUIDtype modelEntityID, const OldTreeIcon& treeIcons, bool backFaceCulling,
	double offsetFactor, bool isHidden, bool isEditable, const std::string& projectName, ot::UID entityID, ot::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double>& transformation
) {
	try {
		ViewerAPI::addNodeFromFacetDataBase(visModelID, treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective, modelEntityID, treeIcons,
			backFaceCulling, offsetFactor, isHidden, isEditable, projectName, entityID, entityVersion, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
	}
	catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addNodeFromFacetData()"); }
	catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addNodeFromFacetData()"); }
	catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addNodeFromFacetData()"); }
}

void ViewerComponent::addVisualizationContainerNode(ViewerUIDtype visModelID, const std::string& treeName, ViewerUIDtype modelEntityID, const OldTreeIcon& treeIcons, bool editable, const ot::VisualisationTypes& _visualisationTypes) {
	try {
		try {
			ViewerAPI::addVisualizationContainerNode(visModelID, treeName, modelEntityID, treeIcons, editable, _visualisationTypes);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addVisualizationContainerNode()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addVisualizationContainerNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addVisualizationContainerNode()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add visualization container node.", _e.what()); }
}

void ViewerComponent::addVisualizationVis2D3DNode(ViewerUIDtype visModelID, const std::string& treeName, ModelUIDtype modelEntityID, const OldTreeIcon& treeIcons, bool isHidden, bool editable, const std::string& projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion) {
	try {
		try {
			ViewerAPI::addVTKNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, editable, projectName, visualizationDataID, visualizationDataVersion);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addVisualizationVis2D3DNode()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addVisualizationVis2D3DNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addVisualizationVis2D3DNode()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add visualization 2D3D node.", _e.what()); }
}

void ViewerComponent::updateVisualizationVis2D3DNode(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, const std::string& projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion) {
	try {
		try {
			ViewerAPI::updateVTKNode(visModelID, modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::updateVisualizationVis2D3DNode()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::updateVisualizationVis2D3DNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateVisualizationVis2D3DNode()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to update visualization 2D3D node.", _e.what()); }
}

void ViewerComponent::addVisualizationAnnotationNode(ViewerUIDtype visModelID, const std::string& treeName, ViewerUIDtype modelEntityID, const OldTreeIcon& treeIcons, bool isHidden,
	const double edgeColorRGB[3],
	const std::vector<std::array<double, 3>>& points,
	const std::vector<std::array<double, 3>>& points_rgb,
	const std::vector<std::array<double, 3>>& triangle_p1,
	const std::vector<std::array<double, 3>>& triangle_p2,
	const std::vector<std::array<double, 3>>& triangle_p3,
	const std::vector<std::array<double, 3>>& triangle_rgb) {
	try {
		try {
			ViewerAPI::addVisualizationAnnotationNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, edgeColorRGB,
				points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::addVisualizationAnnotationNode()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::addVisualizationAnnotationNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addVisualizationAnnotationNode()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to add visualization annotation node.", _e.what()); }
}

void ViewerComponent::updateObjectColor(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective) {
	try {
		try { ViewerAPI::updateObjectColor(visModelID, modelEntityID, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::updateObjectColor()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::updateObjectColor()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateObjectColor()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to update object color.", _e.what()); }
}

void ViewerComponent::updateMeshColor(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, double colorRGB[3]) {
	try {
		try { ViewerAPI::updateMeshColor(visModelID, modelEntityID, colorRGB); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::updateMeshColor()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::updateMeshColor()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateMeshColor()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to update mesh color.", _e.what()); }
}

void ViewerComponent::updateObjectFacetsFromDataBase(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, unsigned long long entityID, unsigned long long entityVersion) {
	try {
		try { ViewerAPI::updateObjectFacetsFromDataBase(visModelID, modelEntityID, entityID, entityVersion); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::updateObjectFacetsFromDataBase()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::updateObjectFacetsFromDataBase()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateObjectFacetsFromDataBase()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to update objects facets from data base.", _e.what()); }
}

void ViewerComponent::enterEntitySelectionMode(ViewerUIDtype visualizationModelID, ot::serviceID_t replyTo, const std::string& selectionType, bool allowMultipleSelection,
	const std::string& selectionFilter, const std::string& selectionAction, const std::string& selectionMessage,
	std::list<std::string>& optionNames, std::list<std::string>& optionValues) {
	try {
		try { ViewerAPI::enterEntitySelectionMode(visualizationModelID, replyTo, selectionType, allowMultipleSelection, selectionFilter, selectionAction, selectionMessage, optionNames, optionValues); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::enterEntitySelectionMode()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::enterEntitySelectionMode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::enterEntitySelectionMode()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to enter entity selection mode.", _e.what()); }
}

void ViewerComponent::freeze3DView(ViewerUIDtype visModelID, bool flag) {
	try {
		try { ViewerAPI::freeze3DView(visModelID, flag); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::freeze3DView()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::freeze3DView()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::freeze3DView()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to freeze 3D view.", _e.what()); }
}

void ViewerComponent::isModified(ViewerUIDtype visualizationModelID, bool modifiedState) {
	try {
		try {
			ot::UID activeModel = ViewerAPI::getActiveDataModel();
			if (visualizationModelID == activeModel) {
				AppBase::instance()->setCurrentProjectIsModified(modifiedState);
			}
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::isModified()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::isModified()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::isModified()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to get is modified flag.", _e.what()); }
}

void ViewerComponent::removeShapes(ViewerUIDtype visualizationModelID, std::list<ViewerUIDtype> entityID) {
	try {
		try { ViewerAPI::removeShapes(visualizationModelID, entityID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::removeShapes()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::removeShapes()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::removeShapes()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to remove shapes.", _e.what()); }
}

void ViewerComponent::setShapeVisibility(ViewerUIDtype visualizationModelID, std::list<ModelUIDtype> visibleID, std::list<ModelUIDtype> hiddenID) {
	try {
		try { ViewerAPI::setShapeVisibility(visualizationModelID, visibleID, hiddenID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setShapeVisibility()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setShapeVisibility()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setShapeVisibility()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set shape visibility.", _e.what()); }
}

void ViewerComponent::hideEntities(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> hiddenID) {
	try {
		try { ViewerAPI::hideEntities(visualizationModelID, hiddenID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::hideAllOtherEntities()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::hideAllOtherEntities()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::hideAllOtherEntities()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to hide entities.", _e.what()); }
}

void ViewerComponent::showBranch(ModelUIDtype visualizationModelID, const std::string& branchName) {
	try {
		try { ViewerAPI::showBranch(visualizationModelID, branchName); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::showBranch()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::showBranch()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::showBranch()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to show branch.", _e.what()); }
}

void ViewerComponent::hideBranch(ModelUIDtype visualizationModelID, const std::string& branchName) {
	try {
		try { ViewerAPI::hideBranch(visualizationModelID, branchName); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::hideBranch()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::hideBranch()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::hideBranch()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to hide branch.", _e.what()); }
}

void ViewerComponent::getSelectedModelEntityIDs(std::list<ViewerUIDtype>& selected) {
	try {
		try { ViewerAPI::getSelectedModelEntityIDs(selected); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::getSelectedModelEntityIDs()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::getSelectedModelEntityIDs()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getSelectedModelEntityIDs()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to get selected model entity IDs.", _e.what()); }
}

void ViewerComponent::getSelectedVisibleModelEntityIDs(std::list<ViewerUIDtype>& selected) {
	try {
		try { ViewerAPI::getSelectedVisibleModelEntityIDs(selected); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::getSelectedVisibleModelEntityIDs()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::getSelectedVisibleModelEntityIDs()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getSelectedVisibleModelEntityIDs()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to get selected visible model entity IDs.", _e.what()); }
}

void ViewerComponent::setFontPath(const QString& _path) {
	try {
		try { ViewerAPI::setFontPath(_path.toStdString()); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setFontPath()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setFontPath()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setFontPath()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set font path.", _e.what()); }
}

ViewerUIDtype ViewerComponent::createModel(void) {
	try {
		try { return ViewerAPI::createModel(); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::createModel()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::createModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::createModel()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to create model.", _e.what()); }
	return 0;
}

void ViewerComponent::deleteModel(ViewerUIDtype viewerUID) {
	try {
		try { return ViewerAPI::deleteModel(viewerUID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::deleteModel()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::deleteModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::deleteModel()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to delete model.", _e.what()); }
}

void ViewerComponent::prefetchDocumentsFromStorage(const std::string& projectName, std::list<std::pair<unsigned long long, unsigned long long>>& prefetchIDs) {
	try {
		try { return ViewerAPI::prefetchDocumentsFromStorage(projectName, prefetchIDs); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::prefetchDocumentsFromStorage()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::prefetchDocumentsFromStorage()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::prefetchDocumentsFromStorage()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to prefetch documents from storage.", _e.what()); }
}

void ViewerComponent::viewerTabChanged(const ot::WidgetViewBase& _viewInfo) {
	ViewerAPI::viewerTabChanged(_viewInfo);
}

ViewerUIDtype ViewerComponent::createViewer(ModelUIDtype _modelUid, double _scaleWidth, double _scaleHeight,
	int _backgroundR, int _backgroundG, int _backgroundB, int _overlayR, int _overlayG, int _overlayB, QWidget* _parent) {
	try {
		try {
			ViewerUIDtype uid = ViewerAPI::createViewer(_modelUid, _scaleWidth, _scaleHeight, _backgroundR, _backgroundG, _backgroundB,
				_overlayR, _overlayG, _overlayB, _parent);
			m_viewers.push_back(uid);
			return uid;
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::createViewer()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::createViewer()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::createViewer()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to create viewer.", _e.what()); }
	return 0;
}

ot::WidgetView* ViewerComponent::getViewerWidget(ViewerUIDtype _viewerUID) {
	try {
		try { return ViewerAPI::getViewerWidget(_viewerUID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::getViewerWidget()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::getViewerWidget()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getViewerWidget()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to get viewer widget.", _e.what()); }
	return nullptr;
}

void ViewerComponent::setDataModel(ViewerUIDtype viewerUID, ModelUIDtype modelUID) {
	try {
		try { ViewerAPI::setDataModel(viewerUID, modelUID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setDataModel()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setDataModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setDataModel()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set data model.", _e.what()); }
}

void ViewerComponent::activateModel(ViewerUIDtype viewerUID) {
	try {
		try { ViewerAPI::activateModel(viewerUID); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::activateModel()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::activateModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::activateModel()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to activate model.", _e.what()); }
}

void ViewerComponent::deactivateCurrentlyActiveModel(void) {
	try {
		try { ViewerAPI::deactivateCurrentlyActiveModel(); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::activateModel()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::activateModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::activateModel()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to deactivate currently active model.", _e.what()); }
}

void ViewerComponent::setColors(const ot::Color& _background, const ot::Color& _foreground) {
	try {
		try {
			for (auto itm : m_viewers) {
				ViewerAPI::setClearColor(itm, _background.r(), _background.g(), _background.b(), _foreground.r(), _foreground.g(), _foreground.b());
			}
		}
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setColors()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setColors()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setColors()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set colors.", _e.what()); }
}

void ViewerComponent::setDataBaseConnectionInformation(const std::string& databaseURL, const std::string& userName, const std::string& encryptedPassword) {
	try {
		try { ViewerAPI::setDataBaseConnection(databaseURL, userName, encryptedPassword); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::setDataBaseConnectionInformation()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::setDataBaseConnectionInformation()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setDataBaseConnectionInformation()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->slotShowErrorPrompt("Error", "Failed to set data base connection.", _e.what()); }
}

void ViewerComponent::setTabTitles(ViewerUIDtype visualizationModelID, const std::string& _tabName3D, const std::string& _tabNameVersions) {
	ViewerAPI::setTabNames(visualizationModelID, _tabName3D, _tabNameVersions);
}

void ViewerComponent::shortcutActivated(const std::string& keySequence) {
	ViewerAPI::shortcutActivated(keySequence);
}

void ViewerComponent::settingsItemChanged(const ot::Property* _property) {
	for (auto vId : m_viewers) {
		ViewerAPI::settingsItemChanged(vId, _property);
	}
}

bool ViewerComponent::propertyGridValueChanged(const ot::Property* _property) {
	for (auto vId : m_viewers) {
		if (ViewerAPI::propertyGridValueChanged(vId, _property)) {
			return true;  // The viewer has handled the property grid change
		}
	}

	return false;  // No viewer has handled the change
}

void ViewerComponent::viewDataModifiedChanged(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType, bool _isModified) {
	ViewerAPI::viewDataModifiedChanged(_entityName, _viewType, _isModified);
}
