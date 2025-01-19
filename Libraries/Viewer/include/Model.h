#pragma once

#include "OTCore/CoreTypes.h"
#include "OTGui/WidgetViewBase.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/Plot1DDataBaseCfg.h"
#include "OldTreeIcon.h"
#include "Geometry.h"
#include "SceneNodeBase.h"
#include "ViewChangedStates.h"

// std header
#include <list>
#include <tuple>

// OSG header
#include <osg/ref_ptr>
#include <osg/Group>
#include <osgUtil/IntersectionVisitor>
#include "OTGui/VisualisationTypes.h"

class Viewer;
class SceneNodeGeometry;
class SceneNodePlot1D;
class SceneNodePlot1DCurve;
class ManipulatorBase;

class FaceSelection
{
public:
	FaceSelection() : modelID(0), selectedItem(nullptr), faceId(0) {}
	virtual ~FaceSelection() {}

	void setData(unsigned long long _modelID) { modelID = _modelID; }

	unsigned long long getModelID(void) const { return modelID; }

	void setSelectedItem(SceneNodeGeometry *item) { selectedItem = item; }
	SceneNodeGeometry *getSelectedItem(void) const { return selectedItem; }

	void setFaceId(unsigned long long id) { faceId = id; }
	unsigned long long getFaceId(void) const { return faceId; }

	void setFaceName(const std::string& name) { faceName = name; }
	std::string getFaceName(void) { return faceName; }

	bool operator==(const FaceSelection &other) { return (selectedItem == other.getSelectedItem() && faceId == other.getFaceId()); }

private:
	unsigned long long modelID;
	SceneNodeGeometry *selectedItem;
	unsigned long long faceId;
	std::string faceName;
};

class EdgeSelection
{
public:
	EdgeSelection() : modelID(0), selectedItem(nullptr), faceId1(0), faceId2(0), node(nullptr) {}
	virtual ~EdgeSelection() {}

	void setData(unsigned long long _modelID) { modelID = _modelID; }
	unsigned long long getModelID(void) const { return modelID; }

	void setSelectedItem(SceneNodeGeometry* item) { selectedItem = item; }
	SceneNodeGeometry* getSelectedItem(void) const { return selectedItem; }

	void setFaceIds(unsigned long long id1, unsigned long long id2) { faceId1 = id1; faceId2 = id2; }
	unsigned long long getFaceId1(void) const { return faceId1; }
	unsigned long long getFaceId2(void) const { return faceId2; }

	void setEdgeName(const std::string& name) { edgeName = name; }
	std::string getEdgeName(void) { return edgeName; }

	bool operator==(const EdgeSelection& other) const { return (selectedItem == other.getSelectedItem() 
		                                                  && (   faceId1 == other.getFaceId1() && faceId2 == other.getFaceId2()
															  || faceId1 == other.getFaceId2() && faceId2 == other.getFaceId1())); }

	void setNode(osg::Node* _node) { node = _node; };
	osg::Node* getNode() { return node; };

private:
	unsigned long long modelID;
	SceneNodeGeometry* selectedItem;
	unsigned long long faceId1;
	unsigned long long faceId2;
	osg::Node* node;
	std::string edgeName;
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

	void addNodeFromFacetData(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isHidden, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNameMap, std::string &errors,
							  bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	void addNodeFromFacetDataBase(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, 
								  bool reflective, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isHidden, 
								  bool isEditable, const std::string &projectName, unsigned long long entityID, unsigned long long version,
							      bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	void addSceneNode(const std::string& _treeName, ot::UID _modelEntityID, const OldTreeIcon& _treeIcons, bool _editable, ot::VisualisationTypes _visualisationTypes);
	
	void addVisualizationContainerNode(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool editable);
	void addVisualizationAnnotationNode(const std::string &treeName, unsigned long long modelEntityID, 
									const OldTreeIcon &treeIcons, bool isHidden,
									const double edgeColorRGB[3],
									const std::vector<std::array<double, 3>> &points,
									const std::vector<std::array<double, 3>> &points_rgb,
									const std::vector<std::array<double, 3>> &triangle_p1,
									const std::vector<std::array<double, 3>> &triangle_p2,
									const std::vector<std::array<double, 3>> &triangle_p3,
									const std::vector<std::array<double, 3>> &triangle_rgb);
	void addVisualizationAnnotationNodeDataBase(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version);
	void addVisualizationMeshNodeFromFacetDataBase(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, unsigned long long entityID, unsigned long long version);
	void addVisualizationMeshItemNodeFromFacetDataBase(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version, long long tetEdgesID, long long tetEdgesVersion);
	void addVisualizationCartesianMeshNode(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
										   const std::string &projectName, unsigned long long faceListEntityID, unsigned long long faceListEntityVersion, unsigned long long nodeListEntityID, unsigned long long nodeListEntityVersion);
	void visualizationCartesianMeshNodeShowLines(unsigned long long modelEntityID, bool showMeshLines);
	void addVisualizationCartesianMeshItemNode(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3]);
	void visualizationTetMeshNodeTetEdges(unsigned long long modelEntityID, bool displayTetEdges);

	void addVisualizationPlot1DNode(const ot::Plot1DDataBaseCfg& _config);
	void addVisualizationResult1DNode(const ot::Plot1DCurveInfoCfg& _curveInfo, const OldTreeIcon& _treeIcons, bool _isHidden);
	
	void visualizationResult1DPropertiesChanged(ot::UID _entityID, ot::UID _version);
	void visualizationPlot1DPropertiesChanged(const ot::Plot1DCfg& _config);

	void addVTKNode(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, bool isEditable, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);
	void updateVTKNode(unsigned long long modelEntityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);

	void setEntityName(unsigned long long modelEntityID, const std::string &newName);
	void renameEntityPath(const std::string &oldPath, const std::string &newPath);

	void updateObjectColor(unsigned long long modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	void updateMeshColor(unsigned long long modelEntityID, double colorRGB[3]);
	void updateObjectFacetsFromDataBase(unsigned long long modelEntityID, unsigned long long entityID, unsigned long long entityVersion);

	void setSelectedTreeItems(const std::list<ot::UID>& _selectedTreeItems, std::list<unsigned long long>& _selectedModelItems, std::list<unsigned long long>& _selectedVisibleModelItems, bool _selectionFromTree);
	void executeAction(unsigned long long _buttonID);
	void setHoverTreeItem(ot::UID hoverTreeItemID);
	void clearHoverView(void);
	void setHoverView(SceneNodeBase *selectedItem);
	unsigned long long getModelEntityIDFromTreeID(ot::UID treeItem);
	unsigned long long getTreeIDFromModelID(ot::UID modelID);

	void setDataModel(unsigned long long modelID) { dataModelID = modelID; };
	unsigned long long getDataModel(void) { return dataModelID; };

	osg::Node *getOSGRootNode(void) { return osgRootNode; };

	void getSelectedModelEntityIDs(std::list<unsigned long long> &selectedModelEntityID);
	void getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selectedVisibleModelEntityID);
	void getSelectedTreeItemIDs(std::list<ot::UID> &selectedTreeItemID);

	void removeShapes(std::list<unsigned long long> modelEntityIDList);
	void setShapeVisibility(std::list<unsigned long long> visibleID, std::list<unsigned long long> hiddenID);

	void enterEntitySelectionMode(ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage, std::list<std::string> &optionNames, std::list<std::string> &optionValues);

	enum eSelectionMode { ENTITY, FACE, SHAPE, EDGE };
	eSelectionMode getCurrentSelectionMode(void) { return currentSelectionMode; };
	void escapeKeyPressed(void);
	void returnKeyPressed(void);
	void processCurrentSelectionMode(osgUtil::Intersector *intersector, double sceneRadius, bool bCtrlKeyPressed);
	void processHoverView(osgUtil::Intersector *intersector, double sceneRadius);

	void addSceneNode(SceneNodeBase *node);
	void setSceneNode(osg::Node *node, SceneNodeBase *sceneNode);
	void removeSceneNode(osg::Node *node);

	void notifySceneNodeAboutViewChange(const std::string& _sceneNodeName,const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType);
	std::list<Viewer *> getViewerList(void) { return viewerList; };

	unsigned int getCurrentTraversalMask(void);
	unsigned int getFaceSelectionTraversalMask(void);
	unsigned int getEdgeSelectionTraversalMask(void);

	void viewerTabChanged(const std::string& _tabTitle, ot::WidgetViewBase::ViewType _type);

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

	void   fillPropertyGrid(const ot::PropertyGridCfg& _configuration);
	void   setDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName, double value);
	double getDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName);
	bool   propertyGridValueChanged(const ot::Property* _property);

	void lockSelectionAndModification(bool flag);

	void updateCapGeometry(osg::Vec3d normal, osg::Vec3d point);
	std::list<std::string> getSelectedCurves();
	void removedSelectedCurveNodes();

	void setCursorText(const std::string& text);

private:
	// Methods
	void	   fillTree(void);
	
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

	void exportTextEditor(void);
	void saveTextEditor(void);

	void	   resetSelection(SceneNodeBase *root);
	void       setAllShapesOpaque(SceneNodeBase *root);
	void       setSelectedShapesOpaqueAndOthersTransparent(SceneNodeBase *root);
	void       setWireframeStateForAllSceneNodes(SceneNodeBase *root, bool wireframe);
	void	   addSceneNodesToTree(SceneNodeBase *root);
	void	   addSelectedModelEntityIDToList(SceneNodeBase *root, std::list<unsigned long long> &selectedModelEntityID);
	void	   addSelectedVisibleModelEntityIDToList(SceneNodeBase *root, std::list<unsigned long long> &selectedVisibleModelEntityID);
	void	   addSelectedTreeItemIDToList(SceneNodeBase *root, std::list<ot::UID> &selectedTreeItemID);
	void	   removeSceneNodeAndChildren(SceneNodeBase *node, std::list<ot::UID> &treeItemDeleteList);
	SceneNodeBase* findSelectedItemByLineSegment(osgUtil::Intersector* intersector, double sceneRadius, osg::Vec3d& intersectionPoint, unsigned long long& hitIndex);
	SceneNodeBase* findSelectedItemByPolytope(osgUtil::Intersector* intersector, double sceneRadius, osg::Vec3d& intersectionPoint, ot::UID& faceId1, ot::UID& faceId2);
	void	   selectSceneNode(SceneNodeBase *selectedItem, bool bCtrlKeyPressed);
	void	   faceSelected(unsigned long long modelID, SceneNodeGeometry* selectedItem, unsigned long long faceId);
	void	   edgeSelected(unsigned long long modelID, SceneNodeGeometry* selectedItem, unsigned long long faceId1, unsigned long long faceId2);
	SceneNodeBase *getParentNode(const std::string &treeName);
	void	   endCurrentSelectionMode(bool cancelled);
	SceneNodeGeometry *createNewGeometryNode(const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, bool isEditable,
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
	void       manageParentVisibility(SceneNodeBase *item);
	void	   updateWorkingPlaneTransform(void);
	bool       getTransformationOfSelectedShapes(SceneNodeBase *root, bool &first, osg::Matrix &matrix);
	bool       compareTransformations(osg::Matrix &matrix1, osg::Matrix &matrix2);
	void       updateCapGeometryForSceneNodes(SceneNodeBase *root, const osg::Vec3d &normal, const osg::Vec3d &point);
	void       updateCapGeometryForGeometryItem(SceneNodeGeometry *item, const osg::Vec3d &normal, const osg::Vec3d &point);
	bool	   isLineDrawable(osg::Drawable *drawable);
	void	   clearEdgeSelection(void);

	// Plot 1D
	void	   clear1DPlot(void);
	void	   set1DPlotIncompatibleData(void);
	void       remove1DPlotErrorState(void);
	void	   update1DPlot(SceneNodeBase* root);
	void	   add1DPlotItems(SceneNodeBase* _root, bool& _isFirstCurve, SceneNodePlot1D*& _commonPlot, bool& _isCompatible, ot::Plot1DDataBaseCfg& _config);
	SceneNodePlot1D* getPlotFromCurve(SceneNodePlot1DCurve* curve);
	bool	   updateCurveEntityVersion(SceneNodeBase* _root, ot::UID _entityID, ot::UID _version);
	void	   addCompatibleDimmedPlotItems(SceneNodeBase* _root, ot::Plot1DDataBaseCfg& _config);
	
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
	bool										   wireFrameState;
	SceneNodeBase*								   currentHoverItem;
	unsigned long long							   dataModelID;
	eSelectionMode								   currentSelectionMode;
	ot::serviceID_t								   currentSelectionReplyTo;
	std::string									   currentSelectionAction;
	std::list<std::string>						   currentSelectionOptionNames;
	std::list<std::string>						   currentSelectionOptionValues;
	bool										   currentSelectionMultiple;
	std::list<FaceSelection>					   currentFaceSelection;
	std::list<EdgeSelection>					   currentEdgeSelection;
	std::map < SceneNodeGeometry *, std::list<unsigned long long>> selectedFacesList;
	unsigned long long							   viewerModelID;
	bool										   singleItemSelected;
	bool										   treeStateRecording;
	std::map<std::string, char>					   treeInfoMap;
	osg::Matrix									   currentWorkingplaneTransform;
	osg::Matrix									   currentWorkingplaneTransformTransposedInverse;
	ManipulatorBase							      *currentManipulator;

	std::string m_currentTabTitle;
	ot::WidgetViewBase::ViewType m_currentViewType;
};

