// Viewer header
#include "stdafx.h"
#include "Notifier.h"
#include "ViewerToolBar.h"

// OpenTwin header
#include "OTCore/Logger.h"

ViewerToolBar& ViewerToolBar::instance(void) {
	static ViewerToolBar g_instance;
	return g_instance;
}

ViewerToolBar::ButtonType ViewerToolBar::getButtonTypeFromUID(ot::UID _uid) const {
	if (_uid == m_resetView3DButtonID) return ButtonType::Reset3DViewButton;
	else if (_uid == m_resetView1DButtonID) return ButtonType::Reset1DViewButton;
	else if (_uid == m_showAllButtonID) return ButtonType::ShowAllButton;
	else if (_uid == m_showSelectedButtonID) return ButtonType::ShowSelectedButton;
	else if (_uid == m_hideSelectedButtonID) return ButtonType::HideSelectedButton;
	else if (_uid == m_hideUnselectedButtonID) return ButtonType::HideUnselectedButton;
	else if (_uid == m_wireframeButtonID) return ButtonType::WireframeButton;
	else if (_uid == m_workingPlaneButtonID) return ButtonType::WorkingPlaneButton;
	else if (_uid == m_axisCrossButtonID) return ButtonType::AxisCrossButton;
	else if (_uid == m_centerAxisCrossButtonID) return ButtonType::CenterAxisCrossButton;
	else if (_uid == m_cutplaneButtonID) return ButtonType::CutplaneButton;

	else if (_uid == m_textEditorSaveID) return ButtonType::TextEditorSaveButton;
	else if (_uid == m_textEditorExportID) return ButtonType::TextEditorExportButton;

	else return ButtonType::NoButton;
}

void ViewerToolBar::setupUIControls3D(void) {
	assert(getNotifier() != nullptr);
	if (getNotifier() == nullptr) return;
	if (!removeItemIDList.empty()) return;

	removeItemIDList.push_front(m_viewPageID = getNotifier()->addMenuPage("View"));

	removeItemIDList.push_front(m_operationsGroupID = getNotifier()->addMenuGroup(m_viewPageID, "Operations"));
	removeItemIDList.push_front(m_visiblityGroupID = getNotifier()->addMenuGroup(m_viewPageID, "Visibility"));
	removeItemIDList.push_front(m_styleGroupID = getNotifier()->addMenuGroup(m_viewPageID, "Display Style"));

	removeItemIDList.push_front(m_resetView3DButtonID = getNotifier()->addMenuPushButton(m_operationsGroupID, "Reset View", "ResetView", "Space"));

	removeItemIDList.push_front(m_showAllButtonID = getNotifier()->addMenuPushButton(m_visiblityGroupID, "Show All", "ShowAll"));
	removeItemIDList.push_front(m_showSelectedButtonID = getNotifier()->addMenuPushButton(m_visiblityGroupID, "Show Selected", "ShowSelected", "Ctrl+S"));
	removeItemIDList.push_front(m_hideSelectedButtonID = getNotifier()->addMenuPushButton(m_visiblityGroupID, "Hide Selected", "HideSelected", "Ctrl+H"));
	removeItemIDList.push_front(m_hideUnselectedButtonID = getNotifier()->addMenuPushButton(m_visiblityGroupID, "Hide Unselected", "HideUnselected"));

	removeItemIDList.push_front(m_wireframeButtonID = getNotifier()->addMenuPushButton(m_styleGroupID, "Wireframe", "Wireframe"));
	removeItemIDList.push_front(m_workingPlaneButtonID = getNotifier()->addMenuPushButton(m_styleGroupID, "Working plane", "WorkingPlane"));
	removeItemIDList.push_front(m_axisCrossButtonID = getNotifier()->addMenuPushButton(m_styleGroupID, "Axis cross", "AxisCross"));
	removeItemIDList.push_front(m_centerAxisCrossButtonID = getNotifier()->addMenuPushButton(m_styleGroupID, "Center axis cross", "CenterAxisCross"));
	removeItemIDList.push_front(m_cutplaneButtonID = getNotifier()->addMenuPushButton(m_styleGroupID, "Cutplane", "Cutplane"));

	// Send an initial notification to properly set the state of the new controls
	this->updateEnabledState(ot::UIDList());
}

void ViewerToolBar::setupUIControls1D(void) {
	assert(getNotifier() != nullptr);
	if (getNotifier() == nullptr) return;
	if (!removeItemIDList.empty()) return;

	removeItemIDList.push_front(m_viewPageID = getNotifier()->addMenuPage("View"));

	removeItemIDList.push_front(m_operationsGroupID = getNotifier()->addMenuGroup(m_viewPageID, "Operations"));
	removeItemIDList.push_front(m_visiblityGroupID = getNotifier()->addMenuGroup(m_viewPageID, "Visibility"));

	removeItemIDList.push_front(m_resetView1DButtonID = getNotifier()->addMenuPushButton(m_operationsGroupID, "Reset View", "ResetView", "Space"));

	removeItemIDList.push_front(m_showSelectedButtonID = getNotifier()->addMenuPushButton(m_visiblityGroupID, "Show Selected", "ShowSelected", "Ctrl+S"));
	removeItemIDList.push_front(m_hideSelectedButtonID = getNotifier()->addMenuPushButton(m_visiblityGroupID, "Hide Selected", "HideSelected", "Ctrl+H"));

	// Send an initial notification to properly set the state of the new controls
	this->updateEnabledState(ot::UIDList());
}

void ViewerToolBar::setupUIControlsText(void) {
	assert(getNotifier() != nullptr);
	if (getNotifier() == nullptr) return;
	if (!removeItemIDList.empty()) return;

	removeItemIDList.push_front(m_textEditorPageID = getNotifier()->addMenuPage("Text Editor"));

	removeItemIDList.push_front(m_textEditorDataID = getNotifier()->addMenuGroup(m_textEditorPageID, "Data"));

	removeItemIDList.push_front(m_textEditorSaveID = getNotifier()->addMenuPushButton(m_textEditorDataID, "Save", "Save"));
	removeItemIDList.push_front(m_textEditorExportID = getNotifier()->addMenuPushButton(m_textEditorDataID, "Save To File", "Export"));

	// Send an initial notification to properly set the state of the new controls
	this->updateEnabledState(ot::UIDList());
	getNotifier()->setCurrentMenuPage("Text Editor");
}

void ViewerToolBar::removeUIControls(void) {
	if (removeItemIDList.empty()) return;

	OTAssert(getNotifier() != nullptr, "No notifier found");
	if (getNotifier() == nullptr) return;

	getNotifier()->removeUIElements(removeItemIDList);
	removeItemIDList.clear();
}

void ViewerToolBar::updateEnabledState(const ot::UIDList& _selectedTreeItems) {
	if (!removeItemIDList.empty()) {
		std::list<unsigned long long> enabled;
		std::list<unsigned long long> disabled;

		if (_selectedTreeItems.empty()) {
			disabled.push_back(m_showSelectedButtonID);
			disabled.push_back(m_hideSelectedButtonID);
			disabled.push_back(m_hideUnselectedButtonID);

			disabled.push_back(m_textEditorSaveID);
			disabled.push_back(m_textEditorExportID);
		}
		else {
			enabled.push_back(m_showSelectedButtonID);
			enabled.push_back(m_hideSelectedButtonID);
			enabled.push_back(m_hideUnselectedButtonID);
		}

		getNotifier()->enableDisableControls(enabled, disabled);
	}
}

void ViewerToolBar::resetControlsData(void) {
	m_viewPageID = 0;
	m_operationsGroupID = 0;
	m_visiblityGroupID = 0;
	m_styleGroupID = 0;
	m_resetView1DButtonID = 0;
	m_resetView3DButtonID = 0;
	m_showAllButtonID = 0;
	m_showSelectedButtonID = 0;
	m_hideSelectedButtonID = 0;
	m_hideUnselectedButtonID = 0;
	m_wireframeButtonID = 0;
	m_workingPlaneButtonID = 0;
	m_axisCrossButtonID = 0;
	m_centerAxisCrossButtonID = 0;
	m_cutplaneButtonID = 0;

	m_textEditorPageID = 0;
	m_textEditorDataID = 0;
	m_textEditorExportID = 0;
	m_textEditorSaveID = 0;
}

ViewerToolBar::ViewerToolBar() {
	this->resetControlsData();
}

ViewerToolBar::~ViewerToolBar() {

}
