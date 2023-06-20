#pragma once

#include "Geometry.h"
#include "Types.h"

#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/Color.h"
#include "OpenTwinFoundation/ContextMenu.h"
#include "OpenTwinFoundation/TableRange.h"

#include <list>
#include <vector>
#include <string>

#include <qcolor.h>

class QWidget;
class Model;
class QPen;
namespace ot { class SettingsData; }
namespace ot { class AbstractSettingsItem; }

namespace ViewerAPI
{
	class __declspec(dllexport) Notifier
	{
	public:
		Notifier() {};
		virtual ~Notifier() {};

		virtual void createTree(void) {};
		virtual void clearTree(void) {};
		virtual ot::UID addTreeItem(const std::string &treePath, bool editable, bool selectChildren) { return 0; };
		virtual void removeTreeItems(std::list<ot::UID> treeItemIDList) {};
		virtual void selectTreeItem(ot::UID treeItemID) {};
		virtual void selectSingleTreeItem(ot::UID treeItemID) {};
		virtual void expandSingleTreeItem(ot::UID treeItemID) {};
		virtual bool isTreeItemExpanded(ot::UID treeItemID) { return false; };
		virtual void toggleTreeItemSelection(ot::UID treeItemID, bool considerChilds) {};
		virtual void clearTreeSelection(void) {};
		virtual void setTreeItemIcon(ot::UID treeItemID, int iconSize, const std::string &iconName) {};
		virtual void setTreeItemText(ot::UID treeItemID, const std::string &text) {};
		virtual void refreshSelection(void) {};
		virtual void addKeyShortcut(const std::string &keySequence) {};
		virtual void fillPropertyGrid(const std::string &settings) {};
		virtual void setDoublePropertyValue(int itemID, double value) {};
		virtual double getDoublePropertyValue(int itemID) { return 0.0; };
		virtual int findItemID(const std::string &name) { return 0; };
		virtual void lockSelectionAndModification(bool flag) {};
		virtual void removeViewer(ot::UID viewerID) {};

		virtual unsigned long long addMenuPage(const std::string &pageName) { return 0; };
		virtual unsigned long long addMenuGroup(unsigned long long menuPageID, const std::string &groupName) { return 0; };
		virtual unsigned long long addMenuPushButton(unsigned long long menuGroupID, const std::string &buttonName, const std::string &iconName, const ot::ContextMenu& _contextMenu = ot::ContextMenu("")) { return 0; };
		virtual unsigned long long addMenuPushButton(unsigned long long menuGroupID, const std::string &buttonName, const std::string &iconName, const std::string &keySequence, const ot::ContextMenu& _contextMenu = ot::ContextMenu("")) { return 0; };

		virtual void removeUIElements(std::list<unsigned long long> &itemIDList) {};

		virtual void displayText(const std::string &text) { };

		virtual void setCurrentVisualizationTab(const std::string & _tabName) {}
		virtual std::string getCurrentVisualizationTab(void) { return ""; }

		virtual void enableDisableControls(std::list<unsigned long long> &enabled, std::list<unsigned long long> &disabled) {};

		virtual void entitiesSelected(ot::serviceID_t replyTo, const std::string &selectionAction, const std::string &selectionInfo, std::list<std::string> &optionNames, std::list<std::string> &optionValues) {};

		virtual void fatalError(const std::string &message) {};

		virtual void rubberbandFinished(ot::serviceID_t creatorId, const std::string &note, const std::string &pointJson, const std::vector<double> &transform) {}

		virtual void updateSettings(ot::SettingsData * _data) {};

		virtual void loadSettings(ot::SettingsData * _data) {};

		virtual void saveSettings(ot::SettingsData * _data) {};

		virtual void updateVTKEntity(unsigned long long modelEntityID) {};

		virtual void activateVersion(const std::string &version) {};
	};

	__declspec(dllexport) void registerNotifier(Notifier *notifier);

	__declspec(dllexport) ot::UID createModel(void);
	__declspec(dllexport) void activateModel(ot::UID osgModelID);
	__declspec(dllexport) void deactivateCurrentlyActiveModel(void);
	__declspec(dllexport) void deleteModel(ot::UID osgModelID);

	__declspec(dllexport) ot::UID getActiveDataModel(void);
	__declspec(dllexport) ot::UID getActiveViewerModel(void);

	__declspec(dllexport) ot::UID createViewer(ot::UID osgModelID, double scaleWidth, double scaleHeight, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB);
	__declspec(dllexport) QWidget* getViewerWidget(ot::UID viewer);

	__declspec(dllexport) void resetAllViews1D(ot::UID osgModelID);
	__declspec(dllexport) void resetAllViews3D(ot::UID osgModelID);
	__declspec(dllexport) void refreshAllViews(ot::UID osgModelID);
	__declspec(dllexport) void clearSelection(ot::UID osgModelID);
	__declspec(dllexport) void refreshSelection(ot::UID osgModelID);
	__declspec(dllexport) void selectObject(ot::UID osgModelID, ot::UID entityID);

	__declspec(dllexport) void setTreeStateRecording(ot::UID osgModelID, bool flag);

	__declspec(dllexport) void setSelectedTreeItems(std::list<ot::UID> &selected, std::list<unsigned long long> &selectedModelItems, std::list<unsigned long long> &selectedVisibleModelItems);
	__declspec(dllexport) void executeAction(unsigned long long buttonID);

	__declspec(dllexport) void setHoverTreeItem(ot::UID hoverItemID);

	__declspec(dllexport) void addNodeFromFacetData(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
												    double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors,
													bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	__declspec(dllexport) void addNodeFromFacetDataBase(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
														double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion,
													    bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	__declspec(dllexport) void addMeshNodeFromDataBase(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const TreeIcon &treeIcons,
											    	   bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion);
	__declspec(dllexport) void addMeshItemNodeFromDataBase(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const TreeIcon &treeIcons,
													       bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion);

	__declspec(dllexport) void addVisualizationContainerNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool editable);
	__declspec(dllexport) void addVTKNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);
	__declspec(dllexport) void updateVTKNode(ot::UID osgModelID, unsigned long long modelEntityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);

	__declspec(dllexport) void addVisualizationAnnotationNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID,
															  const TreeIcon &treeIcons, bool isHidden,
															  const double edgeColorRGB[3],
															  const std::vector<std::array<double, 3>> &points,
															  const std::vector<std::array<double, 3>> &points_rgb,
															  const std::vector<std::array<double, 3>> &triangle_p1,
															  const std::vector<std::array<double, 3>> &triangle_p2,
															  const std::vector<std::array<double, 3>> &triangle_p3,
															  const std::vector<std::array<double, 3>> &triangle_rgb);
	__declspec(dllexport) void addVisualizationAnnotationNodeDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion);
	__declspec(dllexport) void addVisualizationMeshNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion);
	__declspec(dllexport) void addVisualizationMeshItemNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion, long long tetEdgesID, long long tetEdgesVersion);
	__declspec(dllexport) void addVisualizationCartesianMeshNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
																 const std::string &projectName, unsigned long long faceListEntityID, unsigned long long faceListEntityVersion, unsigned long long nodeListEntityID, unsigned long long nodeListEntityVersion);
	__declspec(dllexport) void visualizationCartesianMeshNodeShowLines(ot::UID osgModelID, unsigned long long modelEntityID, bool showMeshLines);
	__declspec(dllexport) void addVisualizationCartesianMeshItemNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3]);
	__declspec(dllexport) void visualizationTetMeshNodeTetEdges(ot::UID osgModelID, unsigned long long modelEntityID, bool displayTetEdges);

	__declspec(dllexport) void addVisualizationPlot1DNode(ot::UID osgModelID, const std::string &name, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
														  bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax, std::list<ot::UID> &curvesID, std::list<ot::UID> &curvesVersions,  std::list<std::string> &curvesNames);
	__declspec(dllexport) void visualizationResult1DPropertiesChanged(ot::UID osgModelID, unsigned long long entityID, unsigned long long entityVersion);
	__declspec(dllexport) void visualizationPlot1DPropertiesChanged(ot::UID osgModelID, ot::UID modelEntityID, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
																	bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax);
	__declspec(dllexport) void addVisualizationTextNode(ot::UID osgModelID, const std::string &name, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName,
														ot::UID textEntityID, ot::UID textEntityVersion);
	__declspec(dllexport) void addVisualizationTableNode(ot::UID osgModelID, const std::string &name, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName,
														ot::UID tableEntityID, ot::UID tableEntityVersion);

	__declspec(dllexport) void setEntityName(ot::UID modelEntityID, const std::string &newName);
	__declspec(dllexport) void renameEntityPath(const std::string &oldPath, const std::string &newPath);

	__declspec(dllexport) void updateObjectColor(ot::UID osgModelID, unsigned long long modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	__declspec(dllexport) void updateMeshColor(ot::UID osgModelID, unsigned long long modelEntityID, double colorRGB[3]);
	__declspec(dllexport) void updateObjectFacetsFromDataBase(ot::UID osgModelID, unsigned long long modelEntityID, unsigned long long entityID, unsigned long long entityVersion);

	__declspec(dllexport) void setClearColor(ot::UID viewrID, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB);

	__declspec(dllexport) void setDataModel(ot::UID osgModelID, ot::UID dataModelID);

	__declspec(dllexport) void getSelectedModelEntityIDs(std::list<unsigned long long> &selected);
	__declspec(dllexport) void getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selected);
	__declspec(dllexport) ot::UID getModelEntityIDFromTreeID(ot::UID treeID);

	__declspec(dllexport) void removeShapes(ot::UID osgModelID, std::list<unsigned long long> modelEntityID);
	__declspec(dllexport) void setShapeVisibility(ot::UID osgModelID, std::list<unsigned long long> visibleID, std::list<unsigned long long> hiddenID);
	__declspec(dllexport) void hideEntities(ot::UID osgModelID, std::list<unsigned long long> hiddenID);

	__declspec(dllexport) void showBranch(ot::UID osgModelID, const std::string &branchName);
	__declspec(dllexport) void hideBranch(ot::UID osgModelID, const std::string &branchName);

	__declspec(dllexport) void enterEntitySelectionMode(ot::UID osgModelID, ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage, std::list<std::string> &optionNames, std::list<std::string> &optionValues);

	__declspec(dllexport) void setFontPath(const std::string &fontPath);

	__declspec(dllexport) void setDataBaseConnection(const std::string &databaseURL, const std::string &userName, const std::string &password);

	__declspec(dllexport) void prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);

	__declspec(dllexport) void setPlotStyles(const std::string & _plotBackgroundSheet, const std::string & _xyPlotStyleSheet, const std::string & _xyCanvasStyleSheet, const QColor & _polarPlotColor, const QPen & _zoomerPen);

	__declspec(dllexport) void setPlotStyle(ot::UID _viewerID, const std::string & _plotBackgroundSheet, const std::string & _xyPlotStyleSheet, const std::string & _xyCanvasStyleSheet, const QColor & _polarPlotColor, const QPen & _zoomerPen);
	
	__declspec(dllexport) void setVersionGraphStyle(ot::UID _viewerID, const std::string & _plotBackgroundSheet, const QColor & _foregroundColor, const QColor & _boxColor, const QColor &_highlightBoxColor, const QColor &_highlightForegroundColor);

	__declspec(dllexport) void setVersionGraphStyles(const std::string & _plotBackgroundSheet, const QColor & _foregroundColor, const QColor & _boxColor, const QColor &_highlightBoxColor, const QColor &_highlightForegroundColor);

	__declspec(dllexport) void setVersionGraph(ot::UID _viewerID, std::list<std::tuple<std::string, std::string, std::string>> &versionGraph, const std::string &activeVersion, const std::string &activeBranch);

	__declspec(dllexport) void setVersionGraphActive(ot::UID _viewerID, const std::string &activeVersion, const std::string &activeBranch);

	__declspec(dllexport) void removeVersionGraphVersions(ot::UID _viewerID, const std::list<std::string> &versions);

	__declspec(dllexport) void addNewVersionGraphStateAndActivate(ot::UID _viewerID, const std::string &newVersion, const std::string &activeBranch, const std::string &parentVersion, const std::string &description);

	__declspec(dllexport) void setTabNames(ot::UID _viewerID, const std::string & _osgViewTabName, const std::string & _plotTabName, const std::string & _versionGraphTabName);

	__declspec(dllexport) QWidget * getPlotWidget(ot::UID _viewerID);

	__declspec(dllexport) QWidget* getBlockDiagramEditorWidget(ot::UID _viewerID);

	/***************************************** Table API *****************************************/
	__declspec(dllexport) QWidget* getTable (ot::UID _viewerID);
	__declspec(dllexport) bool setTable (ot::UID _viewerID, ot::UID tableEntityID, ot::UID tableEntityVersion);
	__declspec(dllexport) void setTableSelection (ot::UID _viewerID, std::vector<ot::TableRange> ranges);
	__declspec(dllexport) std::vector<ot::TableRange> GetSelectedTableRange(ot::UID _viewerID);
	__declspec(dllexport) void ChangeColourOfSelection(ot::UID _viewerID, ot::Color backGroundColour);
	__declspec(dllexport) std::pair<ot::UID, ot::UID> GetActiveTableIdentifyer(ot::UID _viewerID);
	__declspec(dllexport) std::string getTableName(ot::UID _viewerID);
	__declspec(dllexport) void AddToSelectedTableRow(bool _insertAbove, ot::UID _viewerID);
	__declspec(dllexport) void AddToSelectedTableColumn(bool _insertLeft, ot::UID _viewerID);
	__declspec(dllexport) void DeleteFromSelectedTableColumn(ot::UID _viewerID);
	__declspec(dllexport) void DeleteFromSelectedTableRow(ot::UID _viewerID);


	/********************************************************************************************/

	__declspec(dllexport) QWidget * getVersionGraphWidget(ot::UID _viewerID);

	__declspec(dllexport) void viewerTabChanged(const std::string & _tabTitle);

	__declspec(dllexport) void shortcutActivated(const std::string & _keySequence);

	__declspec(dllexport) void createRubberband(ot::UID _viewerID, ot::serviceID_t _senderId, std::string & _note, const std::string & _configurationJson);

	__declspec(dllexport) void settingsItemChanged(ot::UID _viewerID, ot::AbstractSettingsItem * _item);

	__declspec(dllexport) void contextMenuItemClicked(ot::UID _viewerID, const std::string& _menuName, const std::string& _itemName);

	__declspec(dllexport) void contextMenuItemCheckedChanged(ot::UID _viewerID, const std::string& _menuName, const std::string& _itemName, bool _isChecked);

	__declspec(dllexport) bool propertyGridValueChanged(ot::UID _viewerID, int itemID);

	__declspec(dllexport) void freeze3DView(unsigned long long osgModelID, bool flag);

	Model *getModelFromID(ot::UID osgModelID);
}
