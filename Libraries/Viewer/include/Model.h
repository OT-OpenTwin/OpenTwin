#pragma once

#include "OpenTwinCore/CoreTypes.h"

#include <list>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osgUtil/LineSegmentIntersector>

#include "Geometry.h"
#include "SceneNodeBase.h"
#include "Types.h"
#include "Plot.h"

class Viewer;
class SceneNodeGeometry;
class SceneNodePlot1DItem;
class SceneNodeResult1DItem;
class ManipulatorBase;

class UIControls
{
public:
	UIControls() :
		viewPageID(0),
		operationsGroupID(0),
		visiblityGroupID(0),
		styleGroupID(0),
		resetView1DButtonID(0),
		resetView3DButtonID(0),
		showAllButtonID(0),
		showSelectedButtonID(0),
		hideSelectedButtonID(0),
		hideUnselectedButtonID(0),
		stateWireframeButtonID(0),
		stateWorkingPlaneButtonID(0),
		stateAxisCrossButtonID(0),
		stateCenterAxisCrossButtonID(0),
		cutplaneButtonID(0)
	{};

	unsigned long long viewPageID;
	unsigned long long operationsGroupID;
	unsigned long long visiblityGroupID;
	unsigned long long styleGroupID;
	unsigned long long resetView1DButtonID;
	unsigned long long resetView3DButtonID;
	unsigned long long showAllButtonID;
	unsigned long long showSelectedButtonID;
	unsigned long long hideSelectedButtonID;
	unsigned long long hideUnselectedButtonID;
	unsigned long long stateWireframeButtonID;
	unsigned long long stateWorkingPlaneButtonID;
	unsigned long long stateAxisCrossButtonID;
	unsigned long long stateCenterAxisCrossButtonID;
	unsigned long long cutplaneButtonID;
};

class FaceSelection
{
public:
	FaceSelection() : modelID(0), x(0.0), y(0.0), z(0.0), selectedItem(nullptr), faceId(0) {}
	virtual ~FaceSelection() {}

	void setData(unsigned long long _modelID, double _x, double _y, double _z) { modelID = _modelID, x = _x; y = _y; z = _z; }
	unsigned long long getModelID(void) const { return modelID; }
	double getX(void) const { return x; }
	double getY(void) const { return y; }
	double getZ(void) const { return z; }

	void setSelectedItem(SceneNodeGeometry *item) { selectedItem = item; }
	SceneNodeGeometry *getSelectedItem(void) const { return selectedItem; }
	void setFaceId(unsigned long long id) { faceId = id; }
	unsigned long long getFaceId(void) const { return faceId; }

	bool operator==(const FaceSelection &other) { return (selectedItem == other.getSelectedItem() && faceId == other.getFaceId()); }

private:
	unsigned long long modelID;
	double x;
	double y;
	double z;
	SceneNodeGeometry *selectedItem;
	unsigned long long faceId;
};

bool operator==(const FaceSelection& left, const FaceSelection& right);

class Model
{
public:
	Model();
	virtual ~Model();

	void setID(unsigned long long id) { viewerModelID = id; }
	unsigned long long getID(void) { return viewerModelID; }

	bool isModelActive(void) { return isActive; };
	void activateModel(void);
	void deactivateModel(void);

	void attachViewer(Viewer *viewer);
	void detachViewer(Viewer *viewer);
	void resetAllViews1D(void);
	void resetAllViews3D(void);
	void refreshAllViews(void);
	void clearSelection(void);
	void refreshSelection(void);
	void selectObject(unsigned long long modelEntityID);

	void setTreeStateRecording(bool flag);

	void addNodeFromFacetData(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isHidden, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors,
							  bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addNodeFromFacetDataBase(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, 
								  bool reflective, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isHidden, 
								  bool isEditable, const std::string &projectName, unsigned long long entityID, unsigned long long version,
							      bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	void addVisualizationContainerNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool editable);
	void addVisualizationAnnotationNode(const std::string &treeName, unsigned long long modelEntityID, 
									const TreeIcon &treeIcons, bool isHidden,
									const double edgeColorRGB[3],
									const std::vector<std::array<double, 3>> &points,
									const std::vector<std::array<double, 3>> &points_rgb,
									const std::vector<std::array<double, 3>> &triangle_p1,
									const std::vector<std::array<double, 3>> &triangle_p2,
									const std::vector<std::array<double, 3>> &triangle_p3,
									const std::vector<std::array<double, 3>> &triangle_rgb);
	void addVisualizationAnnotationNodeDataBase(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version);
	void addVisualizationMeshNodeFromFacetDataBase(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, unsigned long long entityID, unsigned long long version);
	void addVisualizationMeshItemNodeFromFacetDataBase(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version, long long tetEdgesID, long long tetEdgesVersion);
	void addVisualizationCartesianMeshNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
										   const std::string &projectName, unsigned long long faceListEntityID, unsigned long long faceListEntityVersion, unsigned long long nodeListEntityID, unsigned long long nodeListEntityVersion);
	void visualizationCartesianMeshNodeShowLines(unsigned long long modelEntityID, bool showMeshLines);
	void addVisualizationCartesianMeshItemNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3]);
	void visualizationTetMeshNodeTetEdges(unsigned long long modelEntityID, bool displayTetEdges);

	void addVisualizationPlot1DNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden,
									const std::string &projectName, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
									bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax, std::list<unsigned long long> &curvesID, std::list<unsigned long long> &curvesVersions,
									std::list<std::string> &curvesNames);
	void addVisualizationResult1DNode(const std::string &treeName, unsigned long long curveEntityID, unsigned long long curveEntityVersion, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName);
	void visualizationResult1DPropertiesChanged(unsigned long long entityID, unsigned long long version);
	void visualizationPlot1DPropertiesChanged(unsigned long long modelEntityID, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
											  bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax);
	void addVisualizationTextNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long textEntityID, unsigned long long textEntityVersion);
	void addVisualizationTableNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long tableEntityID, unsigned long long tableEntityVersion);
	void addVTKNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool isEditable, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);
	void updateVTKNode(unsigned long long modelEntityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);

	void setEntityName(unsigned long long modelEntityID, const std::string &newName);
	void renameEntityPath(const std::string &oldPath, const std::string &newPath);

	void updateObjectColor(unsigned long long modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	void updateMeshColor(unsigned long long modelEntityID, double colorRGB[3]);
	void updateObjectFacetsFromDataBase(unsigned long long modelEntityID, unsigned long long entityID, unsigned long long entityVersion);

	void setSelectedTreeItems(std::list<ot::UID> &selected, std::list<unsigned long long> &selectedModelItems, std::list<unsigned long long> &selectedVisibleModelItems);
	void executeAction(unsigned long long buttonID);
	void setHoverTreeItem(ot::UID hoverTreeItemID);
	void clearHoverView(void);
	void setHoverView(SceneNodeBase *selectedItem);
	unsigned long long getModelEntityIDFromTreeID(ot::UID treeItem);

	void setDataModel(unsigned long long modelID) { dataModelID = modelID; };
	unsigned long long getDataModel(void) { return dataModelID; };

	osg::Node *getOSGRootNode(void) { return osgRootNode; };

	void getSelectedModelEntityIDs(std::list<unsigned long long> &selectedModelEntityID);
	void getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selectedVisibleModelEntityID);
	void getSelectedTreeItemIDs(std::list<ot::UID> &selectedTreeItemID);

	void removeShapes(std::list<unsigned long long> modelEntityIDList);
	void setShapeVisibility(std::list<unsigned long long> visibleID, std::list<unsigned long long> hiddenID);

	void enterEntitySelectionMode(ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage, std::list<std::string> &optionNames, std::list<std::string> &optionValues);

	enum eSelectionMode { ENTITY, FACE, SHAPE };
	eSelectionMode getCurrentSelectionMode(void) { return currentSelectionMode; };
	void escapeKeyPressed(void);
	void returnKeyPressed(void);
	void processCurrentSelectionMode(osgUtil::LineSegmentIntersector *intersector, double sceneRadius, bool bCtrlKeyPressed);
	void processHoverView(osgUtil::LineSegmentIntersector *intersector, double sceneRadius);

	void addSceneNode(SceneNodeBase *node);
	void setSceneNode(osg::Node *node, SceneNodeBase *sceneNode);
	void removeSceneNode(osg::Node *node);

	std::list<Viewer *> getViewerList(void) { return viewerList; };

	unsigned int getCurrentTraversalMask(void);
	unsigned int getFaceSelectionTraversalMask(void);

	void viewerTabChanged(const std::string & _tabTitle);

	void set1DPlotItemSelected(unsigned long long treeItemID, bool ctrlPressed);
	void reset1DPlotItemSelection(void);

	void hideEntities(std::list<unsigned long long> hiddenID);

	void showBranch(const std::string &branchName);
	void hideBranch(const std::string &branchName);

	void freeze3DView(bool flag);

	void ensure1DView(void);
	void ensure3DView(void);

	bool isSingleItemSelected(void) { return singleItemSelected; }

	void centerMouseCursor(void);
	bool isWireFrameMode(void) { return wireFrameState; }

	osg::Matrix getCurrentWorkingPlaneTransform(void) { return currentWorkingplaneTransform; }
	osg::Matrix getCurrentWorkingPlaneTransformTransposedInverse(void) { return currentWorkingplaneTransformTransposedInverse; }

	void   fillPropertyGrid(const std::string &settings);
	int    findPropertyItem(const std::string &name);
	void   setDoublePropertyGridValue(int itemID, double value);
	double getDoublePropertyGridValue(int itemID);
	bool   propertyGridValueChanged(int itemID);

	void lockSelectionAndModification(bool flag);

	void updateCapGeometry(osg::Vec3d normal, osg::Vec3d point);

private:
	// Methods
	void	   fillTree(void);
	void	   setupUIControls3D(void);
	void	   setupUIControls1D(void);
	void	   removeUIControls(void);
	void	   updateUIControlState(std::list<ot::UID> &selectedTreeItemID);
	void	   showAllSceneNodes(SceneNodeBase *root);
	void	   hideAllSceneNodes(SceneNodeBase *root);
	void	   showSelectedSceneNodes(SceneNodeBase *root);
	void	   hideSelectedSceneNodes(SceneNodeBase *root);
	void       hideUnselectedSceneNodes(SceneNodeBase *root);
	void	   hideSceneNodeAndChilds(SceneNodeBase *node);
	void	   setSceneNodeVisibility(SceneNodeBase *root, std::map<unsigned long long, bool> nodeVisibility);
	void	   toggleWireframeView(void);
	void       toggleWorkingPlane(void);
	void       toggleAxisCross(void);
	void       toggleCenterAxisCross(void);
	void	   toggleCutplane(void);
	void	   resetSelection(SceneNodeBase *root);
	void       setAllShapesOpaque(SceneNodeBase *root);
	void       setSelectedShapesOpaqueAndOthersTransparent(SceneNodeBase *root);
	void       setWireframeStateForAllSceneNodes(SceneNodeBase *root, bool wireframe);
	void	   addSceneNodesToTree(SceneNodeBase *root);
	void	   addSelectedModelEntityIDToList(SceneNodeBase *root, std::list<unsigned long long> &selectedModelEntityID);
	void	   addSelectedVisibleModelEntityIDToList(SceneNodeBase *root, std::list<unsigned long long> &selectedVisibleModelEntityID);
	void	   addSelectedTreeItemIDToList(SceneNodeBase *root, std::list<ot::UID> &selectedTreeItemID);
	void	   removeSceneNodeAndChildren(SceneNodeBase *node, std::list<ot::UID> &treeItemDeleteList);
	SceneNodeBase *Model::findSelectedItem(osgUtil::LineSegmentIntersector *intersector, double sceneRadius, osg::Vec3d &intersectionPoint, unsigned long long &hitIndex);
	void	   selectSceneNode(SceneNodeBase *selectedItem, bool bCtrlKeyPressed);
	void	   faceSelected(unsigned long long modelID, double x, double y, double z, SceneNodeGeometry *selectedItem, unsigned long long faceId);
	SceneNodeBase *getParentNode(const std::string &treeName);
	void	   endCurrentSelectionMode(bool cancelled);
	SceneNodeGeometry *createNewGeometryNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool isEditable,
											 bool selectChildren, bool manageParentVisibility, bool manageChildVisibility);
	void       setItemVisibleState(SceneNodeBase *item, bool visible);
	void	   showAllSceneNodesAction(void);
	void	   showSelectedSceneNodesAction(void);
	void	   hideSelectedSceneNodesAction(void);
	void	   hideUnselectedSceneNodesAction(void);
	void	   getPrefetchForSelectedSceneNodes(SceneNodeBase *root, std::string& projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);
	void	   getPrefetchForAllSceneNodes(SceneNodeBase *root, std::string& projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);
	void	   updateSelectedFacesHighlight(void);
	void	   setSelectedFacesHighlight(SceneNodeGeometry *selectedItem, unsigned long long faceId, bool highlight);
	void       clearSelectedFacesHighlight(void);
	bool       isFaceSelected(SceneNodeGeometry *selectedItem, unsigned long long faceId);
	void	   clear1DPlot(void);
	void	   set1DPlotIncompatibleData(void);
	void       remove1DPlotErrorState(void);
	void	   update1DPlot(SceneNodeBase *root);
	void	   add1DPlotItems(SceneNodeBase *root, bool &firstCurve, SceneNodePlot1DItem *&commonPlot, AbstractPlot::PlotType &plotType, PlotDataset::axisQuantity &yAxisQuantity, std::string &title, bool &grid, int gridColor[], bool &legend, bool &logscaleX, bool &logscaleY,
							  bool &autoscaleX, bool &autoscaleY, double &xmin, double &xmax, double &ymin, double &ymax, std::string &, std::list<PlotCurveItem> &selectedCurves, bool &compatible);
	SceneNodePlot1DItem *getPlotFromCurve(SceneNodeResult1DItem *curve);
	bool	   changeResult1DEntityVersion(SceneNodeBase *root, unsigned long long entityID, unsigned long long version);
	bool	   gridCompatible(bool grid1, int r1, int g1, int b1, bool grid2, int r2, int g2, int b2);
	bool	   axisCompatible(bool logscale1, bool autoscale1, double min1, double max1, bool logscale2, bool autoscale2, double min2, double max2);
	bool	   plotCompatible(AbstractPlot::PlotType type1, PlotDataset::axisQuantity quantity1, AbstractPlot::PlotType type2, PlotDataset::axisQuantity quantity2);
	void	   addCompatibleDimmedPlotItems(SceneNodeBase *root, AbstractPlot::PlotType &plotType, PlotDataset::axisQuantity &yAxisQuantity, std::string &title, bool &grid, int gridColor[], bool &legend, bool &logscaleX, bool &logscaleY,
											bool &autoscaleX, bool &autoscaleY, double &xmin, double &xmax, double &ymin, double &ymax, std::string &projectName, std::list<PlotCurveItem> &selectedCurves);
	void       manageParentVisibility(SceneNodeBase *item);
	void	   updateWorkingPlaneTransform(void);
	bool       getTransformationOfSelectedShapes(SceneNodeBase *root, bool &first, osg::Matrix &matrix);
	bool       compareTransformations(osg::Matrix &matrix1, osg::Matrix &matrix2);
	void       updateCapGeometryForSceneNodes(SceneNodeBase *root, const osg::Vec3d &normal, const osg::Vec3d &point);
	void       updateCapGeometryForGeometryItem(SceneNodeGeometry *item, const osg::Vec3d &normal, const osg::Vec3d &point);

	// Attributes
	enum { ITEM_SELECTED = 1, ITEM_EXPANDED = 2 };

	osg::ref_ptr<osg::Group>					   osgRootNode;
	std::list<Viewer *>							   viewerList;
	SceneNodeBase*     							   sceneNodesRoot;
	std::map<std::string, SceneNodeBase *>		   nameToSceneNodesMap;
	std::map <ot::UID, SceneNodeBase *>			   treeItemToSceneNodesMap;
	std::map <unsigned long long, SceneNodeBase *> modelItemToSceneNodesMap;
	std::map <osg::Node*, SceneNodeBase *>	       osgNodetoSceneNodesMap;
	bool										   isActive;
	UIControls									   uiControls;
	bool										   wireFrameState;
	SceneNodeBase*								   currentHoverItem;
	unsigned long long							   dataModelID;
	std::list<unsigned long long>				   removeItemIDList;
	eSelectionMode								   currentSelectionMode;
	ot::serviceID_t								   currentSelectionReplyTo;
	std::string									   currentSelectionAction;
	std::list<std::string>						   currentSelectionOptionNames;
	std::list<std::string>						   currentSelectionOptionValues;
	bool										   currentSelectionMultiple;
	std::list<FaceSelection>					   currentFaceSelection;
	std::map < SceneNodeGeometry *, std::list<unsigned long long>> selectedFacesList;
	unsigned long long							   viewerModelID;
	bool										   singleItemSelected;
	bool										   treeStateRecording;
	std::map<std::string, char>					   treeInfoMap;
	osg::Matrix									   currentWorkingplaneTransform;
	osg::Matrix									   currentWorkingplaneTransformTransposedInverse;
	ManipulatorBase							      *currentManipulator;
};

