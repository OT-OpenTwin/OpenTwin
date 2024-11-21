/*
 * ViewerComponent.h
 *
 *  Created on: September 21, 2020
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// C++ header
#include <vector>

// Wrapper header
#include <uiServiceTypes.h>				// Model and View types

// Viewer header
#include "ViewerAPI.h"

// openTwin header
#include "OTCore/ServiceBase.h"

// AK header
#include <akCore/aNotifier.h>
#include <akGui/aColor.h>
#include <QTableWidgetItem>

namespace ak { class aTreeWidget; };
namespace ot { class Property; }
namespace ot { class WidgetView; }

#define VIEWER_SETTINGS_NAME "Viewer"

class ViewerComponent : public ViewerAPI::Notifier , public ak::aNotifier, public ot::ServiceBase
{
public:
	ViewerComponent();
	virtual ~ViewerComponent();

	// #####################################################################################################################################

	// Extern calls
	
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
	virtual void addKeyShortcut(const std::string &keySequence) override;
	virtual void fillPropertyGrid(const ot::PropertyGridCfg& configuration) override;
	virtual void setDoublePropertyValue(const std::string& _groupName, const std::string& _itemName, double value) override;
	virtual double getDoublePropertyValue(const std::string& _groupName, const std::string& _itemName) override;
	virtual void lockSelectionAndModification(bool flag) override;
	virtual void removeViewer(ot::UID viewerID) override;


	// Menu/Widgets

	virtual ViewerUIDtype addMenuPage(const std::string &pageName) override;
	virtual ViewerUIDtype addMenuGroup(ViewerUIDtype menuPageID, const std::string &groupName) override;
	virtual ViewerUIDtype addMenuPushButton(ViewerUIDtype menuGroupID, const std::string &buttonName, const std::string &iconName, const ot::ContextMenu& _contextMenu = ot::ContextMenu("")) override;
	virtual ViewerUIDtype addMenuPushButton(ViewerUIDtype menuGroupID, const std::string &buttonName, const std::string &iconName, const std::string &keySequence, const ot::ContextMenu& _contextMenu = ot::ContextMenu("")) override;
	virtual void removeUIElements(std::list<ViewerUIDtype> &itemIDList) override;

	virtual void displayText(const std::string &text) override;

	virtual void setCurrentVisualizationTabFromTitle(const std::string& _tabTitle) override;
	virtual std::string getCurrentVisualizationTabTitle(void) override;

	virtual void enableDisableControls(std::list<ViewerUIDtype> &enabled, std::list<ViewerUIDtype> &disabled) override;

	virtual void entitiesSelected(ot::serviceID_t replyTo, const std::string &selectionAction, const std::string &selectionInfo, std::list<std::string> &optionNames, std::list<std::string> &optionValues) override;

	virtual void rubberbandFinished(ot::serviceID_t creatorId, const std::string &note, const std::string &pointJson, const std::vector<double> &transform) override;

	virtual void updateSettings(const ot::PropertyGridCfg& _config) override;

	virtual void loadSettings(ot::PropertyGridCfg& _config) override;

	virtual void saveSettings(const ot::PropertyGridCfg& _config) override;

	virtual void updateVTKEntity(unsigned long long modelEntityID) override;

	virtual void messageModelService(const std::string& _message) override;

	// #####################################################################################################################################

	void setProcessingGroupOfMessages(bool _flag);

	// Intern calls

	virtual void notify(
		ak::UID							_senderId,
		ak::eventType					_event,
		int								_info1,
		int								_info2
	) override;

	ViewerUIDtype getActiveDataModel();
	ViewerUIDtype getActiveViewerModel();
	void resetAllViews3D(ViewerUIDtype visualizationModelID);
	void refreshAllViews(ViewerUIDtype visualizationModelID);
	void clearSelection(ViewerUIDtype visualizationModelID);
	void refreshSelection(ViewerUIDtype visualizationModelID);
	void selectObject(ModelUIDtype visualizationModelID, ak::UID entityID);
	void addNodeFromFacetData(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3],
		double edgeColorRGB[3], ViewerUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes,
		std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNameMap, std::string &errors, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addNodeFromFacetDataBase(ViewerUIDtype visModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
		double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, ak::UID entityID, ak::UID entityVersion, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	void addVisualizationContainerNode(ViewerUIDtype visModelID, const std::string &treeName, ViewerUIDtype modelEntityID, const TreeIcon &treeIcons, bool editable);

	virtual void addVisualizationVis2D3DNode(ViewerUIDtype visModelID, const std::string &treeName, ModelUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion);
	virtual void updateVisualizationVis2D3DNode(ViewerUIDtype visModelID, ViewerUIDtype modelEntityID, const std::string &projectName, ViewerUIDtype visualizationDataID, ViewerUIDtype visualizationDataVersion);

	void addVisualizationAnnotationNode(ViewerUIDtype visModelID, const std::string &treeName, ViewerUIDtype modelEntityID, const TreeIcon &treeIcons, bool isHidden, 
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

	ot::WidgetView* getPlotWidget(ViewerUIDtype _viewerID);
	ot::WidgetView* getTableWidget(ViewerUIDtype _viewerID);

	void viewerTabChanged(const std::string& _tabTitle, ot::WidgetViewBase::ViewType _type);

	ViewerUIDtype createViewer(ModelUIDtype _modelUid, double _scaleWidth, double _scaleHeight,
		int _backgroundR, int _backgroundG, int _backgroundB, int _overlayR, int _overlayG, int _overlayB);

	ot::WidgetView* getViewerWidget(ViewerUIDtype _viewerUID);
	void setDataModel(ViewerUIDtype viewerUID, ModelUIDtype modelUID);
	void activateModel(ViewerUIDtype viewerUID);
	void deactivateCurrentlyActiveModel(void);

	void setColors(const ak::aColor & _background, const ak::aColor & _foreground);

	//! @brief Will register this component as notifier in the Viewer API
	void registerAtNotifier(void);

	void setDataBaseConnectionInformation(const std::string &databaseURL, const std::string &userName, const std::string &encryptedPassword);
	void handleSelectionChanged(bool _selectionFromTree);

	void setTabTitles(ViewerUIDtype visualizationModelID, const std::string & _tabName3D, const std::string & _tabName1D, const std::string & _tabNameVersions);

	void shortcutActivated(const std::string &keySequence);

	void settingsItemChanged(const ot::Property* _property);

	void contextMenuItemClicked(const std::string& _menuName, const std::string& _itemName);

	void contextMenuItemCheckedChanged(const std::string& _menuName, const std::string& _itemName, bool _isChecked);

	bool propertyGridValueChanged(const ot::Property* _property);
	
private:
	std::vector<ViewerUIDtype>		m_viewers;

	ViewerComponent(const ViewerComponent &) = delete;
	ViewerComponent & operator = (const ViewerComponent &) = delete;

	int processingGroupCounter;
	bool treeSelectionReceived;

	// Attributes
};
