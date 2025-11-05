// @otlicense
// File: ViewerComponent.h
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

#pragma once

// C++ header
#include <vector>

// Wrapper header
#include <uiServiceTypes.h>				// Model and View types

// Viewer header
#include "ViewerAPI.h"
#include "FrontendAPI.h"

// openTwin header
#include "OTCore/ServiceBase.h"
#include "OTWidgets/SelectionData.h"
#include "OTWidgets/SelectionInformation.h"

// AK header
#include <akCore/aNotifier.h>

namespace ak { class aTreeWidget; };
namespace ot { class Property; }
namespace ot { class WidgetView; }

#define VIEWER_SETTINGS_NAME "Viewer"

class ViewerComponent : public FrontendAPI, public ak::aNotifier, public ot::ServiceBase
{
public:
	ViewerComponent();
	virtual ~ViewerComponent();

	// ###########################################################################################################################################################################################################################################################################################################################

	// General

	virtual void addKeyShortcut(const std::string& keySequence) override;
	virtual void lockSelectionAndModification(bool flag) override;

	virtual void removeViewer(ot::UID viewerID) override;

	virtual void removeUIElements(std::list<ViewerUIDtype>& itemIDList) override;

	virtual void displayText(const std::string& text) override;

	virtual void enableDisableControls(const ot::UIDList& _enabledControls, bool _resetDisabledCounterForEnabledControls, const ot::UIDList& _disabledControls) override;

	virtual void entitiesSelected(ot::serviceID_t replyTo, const std::string& selectionAction, const std::string& selectionInfo, std::list<std::string>& optionNames, std::list<std::string>& optionValues) override;

	virtual void rubberbandFinished(ot::serviceID_t creatorId, const std::string& note, const std::string& pointJson, const std::vector<double>& transform) override;

	virtual void updateSettings(const ot::PropertyGridCfg& _config) override;

	virtual void loadSettings(ot::PropertyGridCfg& _config) override;

	virtual void saveSettings(const ot::PropertyGridCfg& _config) override;

	virtual void updateVTKEntity(unsigned long long modelEntityID) override;

	virtual void messageModelService(const std::string& _message) override;

	virtual void removeGraphicsElements(ot::UID _modelID) override;

	virtual std::string getOpenFileName(const std::string& _title, const std::string& _path, const std::string& _filters) override;

	virtual std::string getSaveFileName(const std::string& _title, const std::string& _path, const std::string& _filters) override;

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// Tree

	virtual void clearTree(void) override;
	virtual ot::UID addTreeItem(const std::string &treePath, bool editable, bool selectChildren) override;
	virtual void removeTreeItems(std::list<ot::UID> treeItemIDList) override;
	virtual void selectTreeItem(ot::UID treeItemID) override;
	virtual void selectSingleTreeItem(ot::UID treeItemID) override;
	virtual void expandSingleTreeItem(ot::UID treeItemID) override;
	virtual bool isTreeItemExpanded(ot::UID treeItemID) override;
	virtual void toggleTreeItemSelection(ot::UID treeItemID, bool considerChilds) override;
	virtual void clearTreeSelection(void) override;
	virtual void setTreeItemIcon(ot::UID treeItemID, int iconSize, const std::string &iconName) override;
	virtual void setTreeItemText(ot::UID treeItemID, const std::string &text) override;
	virtual void refreshSelection(void) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// PropertyGrid

	virtual void fillPropertyGrid(const ot::PropertyGridCfg& configuration) override;
	virtual void clearModalPropertyGrid() override;
	virtual void setDoublePropertyValue(const std::string& _groupName, const std::string& _itemName, double value) override;
	virtual double getDoublePropertyValue(const std::string& _groupName, const std::string& _itemName) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Plot

	virtual void setCurveDimmed(const std::string& _plotName, ot::UID _entityID, bool _setDimmed) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Views

	virtual void closeView(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) override;
	virtual bool hasViewFocus(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) override;

	virtual void addVisualizingEntityToView(ot::UID _treeItemId, const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) override;
	virtual void removeVisualizingEntityFromView(ot::UID _treeItemId, const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) override;
	virtual void clearVisualizingEntitesFromView(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) override;

	virtual ot::WidgetView* getCurrentView(void) override;
	virtual ot::WidgetView* getLastFocusedCentralView(void) override;

	virtual bool getCurrentViewIsModified(void) override;

	virtual void setCurrentVisualizationTabFromEntityName(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) override;
	virtual void setCurrentVisualizationTabFromTitle(const std::string& _tabTitle) override;
	virtual std::string getCurrentVisualizationTabTitle(void) override;

	virtual void requestSaveForCurrentVisualizationTab(void) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// ToolBar

	virtual ViewerUIDtype addMenuPage(const std::string &pageName) override;
	virtual ViewerUIDtype addMenuGroup(ViewerUIDtype menuPageID, const std::string &groupName) override;
	virtual ViewerUIDtype addMenuSubGroup(ViewerUIDtype _menuGroupID, const std::string& _subGroupName) override;
	virtual ViewerUIDtype addMenuPushButton(ViewerUIDtype menuGroupID, const std::string &buttonName, const std::string &iconName) override;
	virtual ViewerUIDtype addMenuPushButton(ViewerUIDtype menuGroupID, const std::string &buttonName, const std::string &iconName, const std::string &keySequence) override;
	virtual void setMenuPushButtonToolTip(ViewerUIDtype _buttonID, const std::string& _toolTip) override;

	virtual void setCurrentMenuPage(const std::string& _pageName) override;
	virtual std::string getCurrentMenuPage(void) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	void setProcessingGroupOfMessages(bool _flag);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Internal calls

	virtual void notify(
		ot::UID							_senderId,
		ak::eventType					_event,
		int								_info1,
		int								_info2
	) override;

	void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

	ViewerUIDtype getActiveDataModel();
	ViewerUIDtype getActiveViewerModel();
	void resetAllViews3D(ViewerUIDtype visualizationModelID);
	void refreshAllViews(ViewerUIDtype visualizationModelID);
	void clearSelection(ViewerUIDtype visualizationModelID);
	void refreshSelection(ViewerUIDtype visualizationModelID);
	void selectObject(ModelUIDtype visualizationModelID, ot::UID entityID);
	void addNodeFromFacetData(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3],
		double edgeColorRGB[3], ViewerUIDtype modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes,
		std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNameMap, std::string &errors, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addNodeFromFacetDataBase(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, ModelUIDtype modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
		double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ot::UID entityID, ot::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	void addVisualizationContainerNode(ViewerUIDtype visModelID, const std::string &treeName, ViewerUIDtype modelEntityID, const OldTreeIcon &treeIcons, bool editable, const ot::VisualisationTypes& _visualisationTypes);

	virtual void addVisualizationVis2D3DNode(ViewerUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion);
	virtual void updateVisualizationVis2D3DNode(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, const std::string &projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion);

	void addVisualizationAnnotationNode(ViewerUIDtype visModelID, const std::string &treeName, ViewerUIDtype modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, 
		const double edgeColorRGB[3],
		const std::vector<std::array<double, 3>> &points,
		const std::vector<std::array<double, 3>> &points_rgb,
		const std::vector<std::array<double, 3>> &triangle_p1,
		const std::vector<std::array<double, 3>> &triangle_p2,
		const std::vector<std::array<double, 3>> &triangle_p3,
		const std::vector<std::array<double, 3>> &triangle_rgb);

	void updateObjectColor(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	void updateMeshColor(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, double colorRGB[3]);
	void updateObjectFacetsFromDataBase(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, unsigned long long entityID, unsigned long long entityVersion);
	void enterEntitySelectionMode(ViewerUIDtype visualizationModelID, ot::serviceID_t replyTo, const std::string &selectionType,
		bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage,
		std::list<std::string> &optionNames, std::list<std::string> &optionValues);
	void freeze3DView(ViewerUIDtype visModelID, bool flag);
	void removeShapes(ViewerUIDtype visualizationModelID, std::list<ViewerUIDtype> entityID);
	void setTreeStateRecording(ViewerUIDtype visualizationModelID, bool flag);
	void setShapeVisibility(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> visibleID, std::list<ModelUIDtype> hiddenID);
	void hideEntities(ModelUIDtype visualizationModelID, std::list<ModelUIDtype> hiddenID);
	void showBranch(ModelUIDtype visualizationModelID, const std::string &branchName);
	void hideBranch(ModelUIDtype visualizationModelID, const std::string &branchName);
	void getSelectedModelEntityIDs(std::list<ViewerUIDtype> &selected);
	void getSelectedVisibleModelEntityIDs(std::list<ViewerUIDtype> &selected);
	void setFontPath(const QString & _path);
	ViewerUIDtype createModel(void);
	void deleteModel(ViewerUIDtype viewerUID);
	void isModified(ViewerUIDtype viewerUID, bool modifiedState);
	void prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);

	void viewerTabChanged(const ot::WidgetViewBase& _viewInfo);

	ViewerUIDtype createViewer(ModelUIDtype _modelUid, double _scaleWidth, double _scaleHeight,
		int _backgroundR, int _backgroundG, int _backgroundB, int _overlayR, int _overlayG, int _overlayB, QWidget* _parent);

	ot::WidgetView* getViewerWidget(ViewerUIDtype _viewerUID);
	void setDataModel(ViewerUIDtype viewerUID, ModelUIDtype modelUID);
	void activateModel(ViewerUIDtype viewerUID);
	void deactivateCurrentlyActiveModel(void);

	void setColors(const ot::Color & _background, const ot::Color & _foreground);

	void setDataBaseConnectionInformation(const std::string &databaseURL, const std::string &userName, const std::string &encryptedPassword);
	
	//! \brief Handle navigation tree selection changed event.
	//! \param _selectionFromTree If true the selection event was emitted from the tree or tree operation.
	//! \return Returns true if the selection has requested a new view.
	ot::SelectionHandlingResult handleSelectionChanged(const ot::SelectionData& _selectionData);

	void setTabTitles(ViewerUIDtype visualizationModelID, const std::string & _tabName3D, const std::string & _tabNameVersions);

	void shortcutActivated(const std::string &keySequence);

	void settingsItemChanged(const ot::Property* _property);

	bool propertyGridValueChanged(const ot::Property* _property);
	
	void viewDataModifiedChanged(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType, bool _isModified);

private:
	std::vector<ViewerUIDtype>		m_viewers;

	ViewerComponent(const ViewerComponent &) = delete;
	ViewerComponent & operator = (const ViewerComponent &) = delete;

	int processingGroupCounter;
	bool treeSelectionReceived;

	// Attributes
};
