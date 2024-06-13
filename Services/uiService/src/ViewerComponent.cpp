/*
 * ViewerComponent.cpp
 *
 *  Created on: September 21, 2020
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

// Wrapper header
#include <ViewerComponent.h>				// Corresponding header
#include <ExternalServicesComponent.h>		// Model Component
#include "AppBase.h"
#include "ToolBar.h"
#include "ControlsManager.h"
#include "ShortcutManager.h"
#include "UserSettings.h"
#include "ContextMenuManager.h"

#include "OTServiceFoundation/SettingsData.h"
#include "OTCore/OTAssert.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyInputDouble.h"

// C++ header
#include <exception>

// Qt header
#include <qwidget.h>

// AK header
#include <akAPI/uiAPI.h>
#include <akCore/aException.h>


ViewerComponent::ViewerComponent()
	: ot::ServiceBase("ViewerComponent", "Viewer", "127.0.0.1", ot::invalidServiceID), processingGroupCounter(0), treeSelectionReceived(false)
{
}

ViewerComponent::~ViewerComponent() {}

// #####################################################################################################################################

// Extern calls

// Tree

void ViewerComponent::clearTree(void)
{
	try {
		try {
			AppBase::instance()->clearNavigationTree();
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::clearTree()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::clearTree()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::clearTree()"); }
	} catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

ot::UID ViewerComponent::addTreeItem(const std::string &treePath, bool editable, bool selectChildren)
{
	try {
		try {
			ak::ID id = AppBase::instance()->addNavigationTreeItem(treePath.c_str(), '/', editable, selectChildren);
			return id;
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addTreeItem()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addTreeItem()"); }
	} catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

void ViewerComponent::setTreeItemIcon(ot::UID treeItemID, int iconSize, const std::string &iconName)
{
	try {
		try {
			if (!iconName.empty())
			{
				//NOTE, add proper item path
				AppBase::instance()->setNavigationTreeItemIcon(treeItemID, iconName.c_str(), "Default");
			}
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setTreeItemIcon()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setTreeItemIcon()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setTreeItemIcon()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::setTreeItemText(ot::UID treeItemID, const std::string &text)
{
	try {
		try {
			AppBase::instance()->setNavigationTreeItemText(treeItemID, text.c_str());
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setTreeItemIcon()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setTreeItemIcon()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setTreeItemIcon()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::removeTreeItems(std::list<ot::UID> treeItemIDList)
{
	try {
		try {
			std::vector<ak::ID> items;
			for (auto itm : treeItemIDList) { items.push_back(itm); }
			AppBase::instance()->removeNavigationTreeItems(items);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::removeTreeItems()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::removeTreeItems()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::removeTreeItems()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::selectTreeItem(ot::UID treeItemID)
{
	try {
		try { AppBase::instance()->setNavigationTreeItemSelected(treeItemID, true); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::selectTreeItem()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::selectTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::selectTreeItem()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::selectSingleTreeItem(ot::UID treeItemID)
{
	try {
		try { AppBase::instance()->setSingleNavigationTreeItemSelected(treeItemID, true); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::selectSingleTreeItem()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::selectSingleTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::selectSingleTreeItem()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::expandSingleTreeItem(ot::UID treeItemID)
{
	try {
		try { AppBase::instance()->expandSingleNavigationTreeItem(treeItemID, true); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::expandSingleTreeItem()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::expandSingleTreeItem()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::expandSingleTreeItem()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

bool ViewerComponent::isTreeItemExpanded(ot::UID treeItemID)
{
	try {
		try { return AppBase::instance()->isTreeItemExpanded(treeItemID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::isTreeItemExpanded()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::isTreeItemExpanded()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::isTreeItemExpanded()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }

	return false;
}

void ViewerComponent::toggleTreeItemSelection(ot::UID treeItemID, bool considerChilds)
{
	try {
		try { 
			AppBase::instance()->toggleNavigationTreeItemSelection(treeItemID, considerChilds);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::toggleTreeItemSelection()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::toggleTreeItemSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::toggleTreeItemSelection()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::clearTreeSelection(void)
{
	try {
		try { AppBase::instance()->clearNavigationTreeSelection(); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::clearTreeSelection()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::clearTreeSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::clearTreeSelection()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::refreshSelection(void)
{
	sendSelectionChangedNotification();
}

void ViewerComponent::addKeyShortcut(const std::string &keySequence) {
	KeyboardCommandHandler * newHandler = new KeyboardCommandHandler(nullptr, AppBase::instance(), keySequence.c_str());
	newHandler->setAsViewerHandler(true);
	AppBase::instance()->shortcutManager()->addHandler(newHandler);
}

void ViewerComponent::fillPropertyGrid(const ot::PropertyGridCfg& _configuration) {
	AppBase::instance()->setupPropertyGrid(_configuration);
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
		OT_LOG_E("Property not found { \"group\": \""+ _groupName + "\", \"name\": \"" + _itemName + "\" }");
		return 0.;
	}
	ot::PropertyInputDouble* inp = dynamic_cast<ot::PropertyInputDouble*>(itm->getInput());
	if (!inp) {
		OT_LOG_E("PropertyInput cast failed");
		return 0.;
	}
	return inp->getValue();
}

void ViewerComponent::lockSelectionAndModification(bool flag) {
	AppBase::instance()->lockSelectionAndModification(flag);
}

void ViewerComponent::removeViewer(ot::UID viewerID) {
	for (auto pos = m_viewers.begin(); pos != m_viewers.end(); pos++)
	{
		if (*pos == viewerID)
		{
			m_viewers.erase(pos);
			return;
		}
	}
}


// Menu/Widgets

ViewerUIDtype ViewerComponent::addMenuPage(const std::string &pageName)
{
	try {
		try {
			ak::UID page = AppBase::instance()->getToolBar()->addPage(AppBase::instance()->getViewerUID(), pageName.c_str());
			AppBase::instance()->controlsManager()->uiElementCreated(this, page, false);
			return page;
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addMenuPage()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addMenuPage()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addMenuPage()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

ViewerUIDtype ViewerComponent::addMenuGroup(ViewerUIDtype menuPageID, const std::string &groupName)
{
	try {
		try {
			ak::UID group = AppBase::instance()->getToolBar()->addGroup(AppBase::instance()->getViewerUID(), menuPageID, groupName.c_str());
			AppBase::instance()->controlsManager()->uiElementCreated(this, group, false);
			return group;
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addMenuGroup()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addMenuGroup()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addMenuGroup()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

ViewerUIDtype ViewerComponent::addMenuPushButton(ViewerUIDtype menuGroupID, const std::string &buttonName, const std::string &iconName, const ot::ContextMenu& _contextMenu)
{
	try {
		try {
			//NOTE, add actual icon path
			ViewerUIDtype btnUid = AppBase::instance()->getToolBar()->addToolButton(AppBase::instance()->getViewerUID(), menuGroupID, iconName.c_str(), "Default", buttonName.c_str(), this);
			AppBase::instance()->controlsManager()->uiElementCreated(this, btnUid, true);
			ot::LockTypeFlags flags;
			flags.setFlag(ot::LockAll);
			//flags.setFlag(ot::LockViewWrite);
			flags.setFlag(ot::LockViewRead);
			AppBase::instance()->lockManager()->uiElementCreated(this, btnUid, flags);

			if (_contextMenu.hasItems()) {
				AppBase::instance()->contextMenuManager()->createItem(nullptr, AppBase::instance()->getViewerUID(), btnUid, _contextMenu)->setIsViewerContext(true);
			}
			return btnUid;
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addMenuPushButton()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addMenuPushButton()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addMenuPushButton()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

ViewerUIDtype ViewerComponent::addMenuPushButton(ViewerUIDtype menuGroupID, const std::string &buttonName, const std::string &iconName, const std::string &keySequence, const ot::ContextMenu& _contextMenu) {
	ViewerUIDtype uid = addMenuPushButton(menuGroupID, buttonName, iconName, _contextMenu);

	KeyboardCommandHandler * newHandler = new KeyboardCommandHandler(nullptr, AppBase::instance(), keySequence.c_str());
	newHandler->setAsViewerHandler(true);
	newHandler->attachToEvent(uid, ak::etClicked, 0, 0);
	AppBase::instance()->shortcutManager()->addHandler(newHandler);

	ak::uiAPI::toolButton::setToolTip(uid, (buttonName + " (" + keySequence + ")").c_str());

	return uid;
}

void ViewerComponent::removeUIElements(std::list<ViewerUIDtype> &itemIDList)
{
	try {
		try {
			std::vector<ViewerUIDtype> i;
			for (auto itm : itemIDList) { i.push_back(itm); }
			AppBase * app = AppBase::instance();
			app->destroyObjects(i);
			for (auto itm : i) {
				app->controlsManager()->uiControlWasDestroyed(itm);
				app->lockManager()->uiElementDestroyed(itm);
				app->contextMenuManager()->uiItemDestroyed(itm);
			}

			AppBase::instance()->shortcutManager()->clearViewerHandler();
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::removeUIElements()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::removeUIElements()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::removeUIElements()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::displayText(const std::string &text)
{
	AppBase::instance()->appendInfoMessage(QString::fromStdString(text));
}

void ViewerComponent::setCurrentVisualizationTab(const std::string & _tabName) {
	AppBase::instance()->setCurrentVisualizationTab(_tabName);
}

std::string ViewerComponent::getCurrentVisualizationTab(void)
{
	return AppBase::instance()->getCurrentVisualizationTab();
}

void ViewerComponent::enableDisableControls(std::list<ak::UID> &enabled, std::list<ak::UID> &disabled)
{
	try {
		try {
			for (auto objectID : enabled) { 
				if (ak::uiAPI::object::exists(objectID)) {
					ak::uiAPI::object::setEnabled(objectID, true);
				}
			}
			for (auto objectID : disabled) {
				if (ak::uiAPI::object::exists(objectID)) {
					ak::uiAPI::object::setEnabled(objectID, false);
				}
			}
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::enableDisableControls()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::enableDisableControls()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::enableDisableControls()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::entitiesSelected(ot::serviceID_t replyTo, const std::string &selectionAction, const std::string &selectionInfo, std::list<std::string> &optionNames, std::list<std::string> &optionValues)
{
	try {
		try {
			AppBase::instance()->getExternalServicesComponent()->entitiesSelected(ViewerAPI::getActiveDataModel(), replyTo, selectionAction, selectionInfo, optionNames, optionValues);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::entitiesSelected()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::entitiesSelected()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::entitiesSelected()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::rubberbandFinished(ot::serviceID_t creatorId, const std::string &note, const std::string &pointJson, const std::vector<double> &transform)
{
	AppBase::instance()->getExternalServicesComponent()->sendRubberbandResultsToService(creatorId, note, pointJson, transform);
}

void ViewerComponent::updateSettings(ot::SettingsData * _data) 
{
	UserSettings::instance()->updateViewerSettings(_data);
}

void ViewerComponent::loadSettings(ot::SettingsData * _data) 
{

	if (!_data->refreshValuesFromDatabase(AppBase::instance()->getCurrentLoginData().getDatabaseUrl(), std::to_string(AppBase::instance()->getSiteID()), AppBase::instance()->getCurrentLoginData().getSessionUser(), AppBase::instance()->getCurrentLoginData().getSessionPassword(), AppBase::instance()->getCurrentUserCollection())) {
		AppBase::instance()->appendInfoMessage("[ERROR] Failed to import viewer settings data from database\n");
	}
}

void ViewerComponent::saveSettings(ot::SettingsData * _data) 
{
	if (!_data->saveToDatabase(AppBase::instance()->getCurrentLoginData().getDatabaseUrl(), std::to_string(AppBase::instance()->getSiteID()), AppBase::instance()->getCurrentLoginData().getSessionUser(), AppBase::instance()->getCurrentLoginData().getSessionPassword(), AppBase::instance()->getCurrentUserCollection())) {
		AppBase::instance()->appendInfoMessage("[ERROR] Failed to export viewer settings data to database\n");
	}
}

void ViewerComponent::updateVTKEntity(unsigned long long modelEntityID)
{
	AppBase::instance()->getExternalServicesComponent()->requestUpdateVTKEntity(modelEntityID);
}

void ViewerComponent::activateVersion(const std::string &version)
{
	AppBase::instance()->getExternalServicesComponent()->activateVersion(version);
}

void ViewerComponent::setProcessingGroupOfMessages(bool flag)
{
	if (flag)
	{
		if (processingGroupCounter == 0)
		{
			treeSelectionReceived = false;
		}
		processingGroupCounter++;
	}
	else
	{
		assert(processingGroupCounter > 0);
		processingGroupCounter--;
		if (processingGroupCounter == 0)
		{
			// Process all delayed actions 
			if (treeSelectionReceived)
			{
				sendSelectionChangedNotification();
			}
		}
	}
}

// #####################################################################################################################################

// Intern calls

void ViewerComponent::notify(
	ak::UID									_senderId,
	ak::eventType						_event,
	int										_info1,
	int										_info2
)
{
	try {
		try {
			if (_event == ak::etClicked)
			{
				ViewerAPI::executeAction(_senderId);
			}
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::notify()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::notify()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::notify()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::sendSelectionChangedNotification()
{
	if (processingGroupCounter > 0)
	{
		treeSelectionReceived = true;
		return;
	}

	std::vector<int> selection = AppBase::instance()->getSelectedNavigationTreeItems();
	std::list<ot::UID> selectedTreeItems;
	for (auto itm : selection) {
		selectedTreeItems.push_back(itm);
		//						if (itm == 6) {
		//							QString path = uiServiceAPI::tree::getPathString(itm);
		//							int x = 0;
		//						}
	}

	// Send the selection changed notification to the viewer component and the model component
	std::list<ak::UID> selectedModelItems, selectedVisibleModelItems;
	ViewerAPI::setSelectedTreeItems(selectedTreeItems, selectedModelItems, selectedVisibleModelItems);

	// Model function
	ot::UID activeModel = ViewerAPI::getActiveDataModel();
	if (activeModel > 0)
	{
		AppBase::instance()->getExternalServicesComponent()->modelSelectionChangedNotification(activeModel, selectedModelItems, selectedVisibleModelItems);
	}
}

ViewerUIDtype ViewerComponent::getActiveDataModel() {
	try {
		try {
			return ViewerAPI::getActiveDataModel();
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::getActiveDataModel()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::getActiveDataModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getActiveDataModel()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

ViewerUIDtype ViewerComponent::getActiveViewerModel() {
	try {
		try {
			return ViewerAPI::getActiveViewerModel();
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::getActiveViewerModel()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::getActiveViewerModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getActiveViewerModel()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

void ViewerComponent::resetAllViews3D(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::resetAllViews3D(visualizationModelID);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::resetAllViews3D()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::resetAllViews3D()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::resetAllViews3D()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::refreshAllViews(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::refreshAllViews(visualizationModelID);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::refreshAllViews()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::refreshAllViews()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::refreshAllViews()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::setTreeStateRecording(ViewerUIDtype visualizationModelID, bool flag) {
	try {
		try {
			ViewerAPI::setTreeStateRecording(visualizationModelID, flag);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setTreeStateRecording()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setTreeStateRecording()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setTreeStateRecording()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::clearSelection(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::clearSelection(visualizationModelID);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::clearSelection()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::clearSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::clearSelection()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::refreshSelection(ViewerUIDtype visualizationModelID) {
	try {
		try {
			ViewerAPI::refreshSelection(visualizationModelID);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::refreshSelection()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::refreshSelection()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::refreshSelection()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::selectObject(ModelUIDtype visualizationModelID, ot::UID entityID) {
	try {
		try {
			ViewerAPI::selectObject(visualizationModelID, entityID);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::selectObject()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::selectObject()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::selectObject()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::addNodeFromFacetData(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3],
										   double edgeColorRGB[3], ViewerUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes,
										   std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors,
										   bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected)
{
	try {
		try {
			ViewerAPI::addNodeFromFacetData(visModelID, treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons,
				backFaceCulling, offsetFactor, isEditable, nodes, triangles, edges, errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addNodeFromFacetData()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addNodeFromFacetData()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addNodeFromFacetData()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::addNodeFromFacetDataBase(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
		double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ak::UID entityID, ak::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation
) {
	try {
		ViewerAPI::addNodeFromFacetDataBase(visModelID, treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective, modelEntityID, treeIcons,
			backFaceCulling, offsetFactor, isHidden, isEditable, projectName, entityID, entityVersion, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
	}
	catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addNodeFromFacetData()"); }
	catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addNodeFromFacetData()"); }
	catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addNodeFromFacetData()"); }
}

void ViewerComponent::addVisualizationContainerNode(ViewerUIDtype visModelID, const std::string &treeName, ViewerUIDtype modelEntityID, const TreeIcon &treeIcons, bool editable)
{
	try {
		try {
			ViewerAPI::addVisualizationContainerNode(visModelID, treeName, modelEntityID, treeIcons, editable);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addVisualizationContainerNode()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addVisualizationContainerNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addVisualizationContainerNode()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::addVisualizationVis2D3DNode(ViewerUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion)
{
	try {
		try {
			ViewerAPI::addVTKNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, editable, projectName, visualizationDataID, visualizationDataVersion);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addVisualizationVis2D3DNode()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addVisualizationVis2D3DNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addVisualizationVis2D3DNode()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::updateVisualizationVis2D3DNode(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, const std::string &projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion)
{
	try {
		try {
			ViewerAPI::updateVTKNode(visModelID, modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::updateVisualizationVis2D3DNode()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::updateVisualizationVis2D3DNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateVisualizationVis2D3DNode()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::addVisualizationAnnotationNode(ViewerUIDtype visModelID, const std::string &treeName, ViewerUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden,
	const double edgeColorRGB[3],
	const std::vector<std::array<double, 3>> &points,
	const std::vector<std::array<double, 3>> &points_rgb,
	const std::vector<std::array<double, 3>> &triangle_p1,
	const std::vector<std::array<double, 3>> &triangle_p2,
	const std::vector<std::array<double, 3>> &triangle_p3,
	const std::vector<std::array<double, 3>> &triangle_rgb)
{
	try {
		try {
			ViewerAPI::addVisualizationAnnotationNode(visModelID, treeName, modelEntityID, treeIcons, isHidden, edgeColorRGB,
				points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::addVisualizationAnnotationNode()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::addVisualizationAnnotationNode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::addVisualizationAnnotationNode()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::updateObjectColor(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective)
{
	try {
		try { ViewerAPI::updateObjectColor(visModelID, modelEntityID, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::updateObjectColor()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::updateObjectColor()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateObjectColor()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::updateMeshColor(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, double colorRGB[3])
{
	try {
		try { ViewerAPI::updateMeshColor(visModelID, modelEntityID, colorRGB); }
		catch (const ak::aException& e) { throw ak::aException(e, "ViewerComponent::updateMeshColor()"); }
		catch (const std::exception& e) { throw ak::aException(e.what(), "ViewerComponent::updateMeshColor()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateMeshColor()"); }
	}
	catch (const ak::aException& _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::updateObjectFacetsFromDataBase(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, unsigned long long entityID, unsigned long long entityVersion)
{
	try {
		try { ViewerAPI::updateObjectFacetsFromDataBase(visModelID, modelEntityID, entityID, entityVersion); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::updateObjectFacetsFromDataBase()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::updateObjectFacetsFromDataBase()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::updateObjectFacetsFromDataBase()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::enterEntitySelectionMode(ViewerUIDtype visualizationModelID, ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection,
	const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage,
	std::list<std::string> &optionNames, std::list<std::string> &optionValues)
{
	try {
		try { ViewerAPI::enterEntitySelectionMode(visualizationModelID, replyTo, selectionType, allowMultipleSelection, selectionFilter, selectionAction, selectionMessage, optionNames, optionValues); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::enterEntitySelectionMode()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::enterEntitySelectionMode()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::enterEntitySelectionMode()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::freeze3DView(ViewerUIDtype visModelID, bool flag)
{
	try {
		try { ViewerAPI::freeze3DView(visModelID, flag); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::freeze3DView()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::freeze3DView()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::freeze3DView()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::isModified(ViewerUIDtype visualizationModelID, bool modifiedState)
{
	try {
		try {
			ot::UID activeModel = ViewerAPI::getActiveDataModel();
			if (visualizationModelID == activeModel)
			{
				AppBase::instance()->setCurrentProjectIsModified(modifiedState);
				std::cout << "Model is modified: " << modifiedState << std::endl;
			}
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::isModified()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::isModified()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::isModified()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::removeShapes(ViewerUIDtype visualizationModelID, std::list<ViewerUIDtype> entityID)
{
	try {
		try { ViewerAPI::removeShapes(visualizationModelID, entityID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::removeShapes()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::removeShapes()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::removeShapes()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::setShapeVisibility(ViewerUIDtype visualizationModelID, std::list<ModelUIDtype> visibleID, std::list<ModelUIDtype> hiddenID)
{
	try {
		try { ViewerAPI::setShapeVisibility(visualizationModelID, visibleID, hiddenID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setShapeVisibility()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setShapeVisibility()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setShapeVisibility()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::hideEntities(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> hiddenID)
{
	try {
		try { ViewerAPI::hideEntities(visualizationModelID, hiddenID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::hideAllOtherEntities()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::hideAllOtherEntities()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::hideAllOtherEntities()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::showBranch(ModelUIDtype visualizationModelID, const std::string &branchName)
{
	try {
		try { ViewerAPI::showBranch(visualizationModelID, branchName); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::showBranch()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::showBranch()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::showBranch()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::hideBranch(ModelUIDtype visualizationModelID, const std::string &branchName)
{
	try {
		try { ViewerAPI::hideBranch(visualizationModelID, branchName); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::hideBranch()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::hideBranch()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::hideBranch()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::getSelectedModelEntityIDs(std::list<ViewerUIDtype> &selected)
{
	try {
		try { ViewerAPI::getSelectedModelEntityIDs(selected); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::getSelectedModelEntityIDs()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::getSelectedModelEntityIDs()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getSelectedModelEntityIDs()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::getSelectedVisibleModelEntityIDs(std::list<ViewerUIDtype> &selected)
{
	try {
		try { ViewerAPI::getSelectedVisibleModelEntityIDs(selected); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::getSelectedVisibleModelEntityIDs()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::getSelectedVisibleModelEntityIDs()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getSelectedVisibleModelEntityIDs()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::setFontPath(const QString & _path) {
	try {
		try { ViewerAPI::setFontPath(_path.toStdString()); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setFontPath()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setFontPath()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setFontPath()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

ViewerUIDtype ViewerComponent::createModel(void) {
	try {
		try { return ViewerAPI::createModel(); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::createModel()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::createModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::createModel()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

void ViewerComponent::deleteModel(ViewerUIDtype viewerUID) {
	try {
		try { return ViewerAPI::deleteModel(viewerUID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::deleteModel()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::deleteModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::deleteModel()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	try {
		try { return ViewerAPI::prefetchDocumentsFromStorage(projectName, prefetchIDs); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::prefetchDocumentsFromStorage()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::prefetchDocumentsFromStorage()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::prefetchDocumentsFromStorage()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

ot::WidgetView* ViewerComponent::getPlotWidget(ViewerUIDtype _viewerID) {
	return ViewerAPI::getPlotWidget(_viewerID);
}

ot::WidgetView* ViewerComponent::getVersionGraphWidget(ViewerUIDtype _viewerID) {
	return ViewerAPI::getVersionGraphWidget(_viewerID);
}


ot::WidgetView* ViewerComponent::getTableWidget(ViewerUIDtype _viewerID)
{
	return ViewerAPI::getTable(_viewerID);
}


void ViewerComponent::viewerTabChanged(const std::string & _tabTitle) {
	ViewerAPI::viewerTabChanged(_tabTitle);
}


ViewerUIDtype ViewerComponent::createViewer(ModelUIDtype _modelUid, double _scaleWidth, double _scaleHeight,
	int _backgroundR, int _backgroundG, int _backgroundB, int _overlayR, int _overlayG, int _overlayB)
{
	try {
		try {
			ViewerUIDtype uid = ViewerAPI::createViewer(_modelUid, _scaleWidth, _scaleHeight, _backgroundR, _backgroundG, _backgroundB,
				_overlayR, _overlayG, _overlayB);
			m_viewers.push_back(uid);
			return uid;
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::createViewer()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::createViewer()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::createViewer()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return 0;
}

ot::WidgetView* ViewerComponent::getViewerWidget(ViewerUIDtype _viewerUID) {
	try {
		try { return ViewerAPI::getViewerWidget(_viewerUID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::getViewerWidget()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::getViewerWidget()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::getViewerWidget()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
	return nullptr;
}

void ViewerComponent::setDataModel(ViewerUIDtype viewerUID, ModelUIDtype modelUID) {
	try {
		try { ViewerAPI::setDataModel(viewerUID, modelUID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setDataModel()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setDataModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setDataModel()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::activateModel(ViewerUIDtype viewerUID) {
	try {
		try { ViewerAPI::activateModel(viewerUID); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::activateModel()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::activateModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::activateModel()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::deactivateCurrentlyActiveModel(void) {
	try {
		try { ViewerAPI::deactivateCurrentlyActiveModel(); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::activateModel()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::activateModel()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::activateModel()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::setColors(const ak::aColor & _background, const ak::aColor & _foreground) {
	try {
		try {
			for (auto itm : m_viewers) {
				ViewerAPI::setClearColor(itm, _background.r(), _background.g(), _background.b(), _foreground.r(), _foreground.g(), _foreground.b());
			}
		}
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setColors()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setColors()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setColors()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::registerAtNotifier(void) {
	try {
		try { ViewerAPI::registerNotifier(this); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::registerAtNotifier()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::registerAtNotifier()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::registerAtNotifier()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::setDataBaseConnectionInformation(const std::string &databaseURL, const std::string &userName, const std::string &encryptedPassword)
{
	try {
		try { ViewerAPI::setDataBaseConnection(databaseURL, userName, encryptedPassword); }
		catch (const ak::aException & e) { throw ak::aException(e, "ViewerComponent::setDataBaseConnectionInformation()"); }
		catch (const std::exception & e) { throw ak::aException(e.what(), "ViewerComponent::setDataBaseConnectionInformation()"); }
		catch (...) { throw ak::aException("Unknown error", "ViewerComponent::setDataBaseConnectionInformation()"); }
	}
	catch (const ak::aException & _e) { AppBase::instance()->showErrorPrompt(_e.what(), "Error"); }
}

void ViewerComponent::setTabTitles(ViewerUIDtype visualizationModelID, const std::string & _tabName3D, const std::string & _tabName1D, const std::string & _tabNameVersions) {
	ViewerAPI::setTabNames(visualizationModelID, _tabName3D, _tabName1D, _tabNameVersions);
}

void ViewerComponent::shortcutActivated(const std::string &keySequence) {
	ViewerAPI::shortcutActivated(keySequence);
}

void ViewerComponent::settingsItemChanged(ot::AbstractSettingsItem * _item) {
	for (auto vId : m_viewers) {
		ViewerAPI::settingsItemChanged(vId, _item);
	}
}

void ViewerComponent::contextMenuItemClicked(const std::string& _menuName, const std::string& _itemName) {
	for (auto vId : m_viewers) {
		ViewerAPI::contextMenuItemClicked(vId, _menuName, _itemName);
	}
}

void ViewerComponent::contextMenuItemCheckedChanged(const std::string& _menuName, const std::string& _itemName, bool _isChecked) {
	for (auto vId : m_viewers) {
		ViewerAPI::contextMenuItemCheckedChanged(vId, _menuName, _itemName, _isChecked);
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
