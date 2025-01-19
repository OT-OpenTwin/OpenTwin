// Viewer header
#include "stdafx.h"
#include "FrontendAPI.h"
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

void ViewerToolBar::viewDataModifiedHasChanged(ot::WidgetViewBase::ViewType _type, bool _hasChanges) {
	// The method is only called when changing from content unchanged->changed or content changed->unchanged

	switch (_type) {
	case ot::WidgetViewBase::ViewText:
		this->updateTextEditorSaveEnabledState();
		break;
	default:
		break;
	}
}

void ViewerToolBar::setupUIControls3D(void) {
	assert(FrontendAPI::instance() != nullptr);
	if (FrontendAPI::instance() == nullptr) return;
	if (!m_removeItemIDList.empty()) return;

	m_removeItemIDList.push_front(m_viewPageID = FrontendAPI::instance()->addMenuPage("View"));

	m_removeItemIDList.push_front(m_operationsGroupID = FrontendAPI::instance()->addMenuGroup(m_viewPageID, "Operations"));
	m_removeItemIDList.push_front(m_visiblityGroupID = FrontendAPI::instance()->addMenuGroup(m_viewPageID, "Visibility"));
	m_removeItemIDList.push_front(m_styleGroupID = FrontendAPI::instance()->addMenuGroup(m_viewPageID, "Display Style"));

	m_removeItemIDList.push_front(m_resetView3DButtonID = FrontendAPI::instance()->addMenuPushButton(m_operationsGroupID, "Reset View", "ResetView", "Space"));

	m_removeItemIDList.push_front(m_showAllButtonID = FrontendAPI::instance()->addMenuPushButton(m_visiblityGroupID, "Show All", "ShowAll"));
	m_removeItemIDList.push_front(m_showSelectedButtonID = FrontendAPI::instance()->addMenuPushButton(m_visiblityGroupID, "Show Selected", "ShowSelected", "Ctrl+S"));
	m_removeItemIDList.push_front(m_hideSelectedButtonID = FrontendAPI::instance()->addMenuPushButton(m_visiblityGroupID, "Hide Selected", "HideSelected", "Ctrl+H"));
	m_removeItemIDList.push_front(m_hideUnselectedButtonID = FrontendAPI::instance()->addMenuPushButton(m_visiblityGroupID, "Hide Unselected", "HideUnselected"));

	m_removeItemIDList.push_front(m_wireframeButtonID = FrontendAPI::instance()->addMenuPushButton(m_styleGroupID, "Wireframe", "Wireframe"));
	m_removeItemIDList.push_front(m_workingPlaneButtonID = FrontendAPI::instance()->addMenuPushButton(m_styleGroupID, "Working plane", "WorkingPlane"));
	m_removeItemIDList.push_front(m_axisCrossButtonID = FrontendAPI::instance()->addMenuPushButton(m_styleGroupID, "Axis cross", "AxisCross"));
	m_removeItemIDList.push_front(m_centerAxisCrossButtonID = FrontendAPI::instance()->addMenuPushButton(m_styleGroupID, "Center axis cross", "CenterAxisCross"));
	m_removeItemIDList.push_front(m_cutplaneButtonID = FrontendAPI::instance()->addMenuPushButton(m_styleGroupID, "Cutplane", "Cutplane"));

	// Send an initial notification to properly set the state of the new controls
	this->updateViewEnabledState(ot::UIDList());
}

void ViewerToolBar::setupUIControls1D(void) {
	assert(FrontendAPI::instance() != nullptr);
	if (FrontendAPI::instance() == nullptr) return;
	if (!m_removeItemIDList.empty()) return;

	m_removeItemIDList.push_front(m_viewPageID = FrontendAPI::instance()->addMenuPage("View"));

	m_removeItemIDList.push_front(m_operationsGroupID = FrontendAPI::instance()->addMenuGroup(m_viewPageID, "Operations"));
	m_removeItemIDList.push_front(m_visiblityGroupID = FrontendAPI::instance()->addMenuGroup(m_viewPageID, "Visibility"));

	m_removeItemIDList.push_front(m_resetView1DButtonID = FrontendAPI::instance()->addMenuPushButton(m_operationsGroupID, "Reset View", "ResetView", "Space"));

	m_removeItemIDList.push_front(m_showSelectedButtonID = FrontendAPI::instance()->addMenuPushButton(m_visiblityGroupID, "Show Selected", "ShowSelected", "Ctrl+S"));
	m_removeItemIDList.push_front(m_hideSelectedButtonID = FrontendAPI::instance()->addMenuPushButton(m_visiblityGroupID, "Hide Selected", "HideSelected", "Ctrl+H"));

	// Send an initial notification to properly set the state of the new controls
	this->updateViewEnabledState(ot::UIDList());
}

void ViewerToolBar::setupUIControlsText(void) {
	assert(FrontendAPI::instance() != nullptr);
	if (FrontendAPI::instance() == nullptr) return;
	if (!m_removeItemIDList.empty()) return;

	m_removeItemIDList.push_front(m_textEditorPageID = FrontendAPI::instance()->addMenuPage("Text Editor"));

	m_removeItemIDList.push_front(m_textEditorDataID = FrontendAPI::instance()->addMenuGroup(m_textEditorPageID, "Data"));

	m_removeItemIDList.push_front(m_textEditorSaveID = FrontendAPI::instance()->addMenuPushButton(m_textEditorDataID, "Save (Ctrl+S)", "Save"));
	m_removeItemIDList.push_front(m_textEditorExportID = FrontendAPI::instance()->addMenuPushButton(m_textEditorDataID, "Save To File", "Export"));

	// Send an initial notification to properly set the state of the new controls
	this->updateTextEditorEnabledState();
	FrontendAPI::instance()->setCurrentMenuPage("Text Editor");
}

void ViewerToolBar::removeUIControls(void) {
	if (m_removeItemIDList.empty()) return;

	OTAssert(FrontendAPI::instance() != nullptr, "No notifier found");
	if (FrontendAPI::instance() == nullptr) return;

	FrontendAPI::instance()->removeUIElements(m_removeItemIDList);
	m_removeItemIDList.clear();
	this->resetControlsData();
}

void ViewerToolBar::updateViewEnabledState(const ot::UIDList& _selectedTreeItems) {
	if (!m_removeItemIDList.empty()) {
		std::list<unsigned long long> enabled;
		std::list<unsigned long long> disabled;

		if (_selectedTreeItems.empty()) {
			disabled.push_back(m_showSelectedButtonID);
			disabled.push_back(m_hideSelectedButtonID);
			disabled.push_back(m_hideUnselectedButtonID);
		}
		else {
			enabled.push_back(m_showSelectedButtonID);
			enabled.push_back(m_hideSelectedButtonID);
			enabled.push_back(m_hideUnselectedButtonID);
		}

		FrontendAPI::instance()->enableDisableControls(enabled, disabled);
	}
}

void ViewerToolBar::updateTextEditorEnabledState(void) {
	this->updateTextEditorSaveEnabledState();
}

void ViewerToolBar::updateTextEditorSaveEnabledState(void) {
	if (!m_removeItemIDList.empty()) {
		std::list<unsigned long long> enabled;
		std::list<unsigned long long> disabled;

		// The text editor tab is only visible when a text editor is currently focused
		if (FrontendAPI::instance()->getCurrentViewIsModified()) {
			enabled.push_back(m_textEditorSaveID);
		}
		else {
			disabled.push_back(m_textEditorSaveID);
		}

		FrontendAPI::instance()->enableDisableControls(enabled, disabled);
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
