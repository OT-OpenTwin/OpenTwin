// @otlicense
// File: Model.h
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

#include "OTCore/JSON/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/WidgetViewBase.h"
#include "OTGui/Plot1DDataBaseCfg.h"
#include "OTGui/Properties/PropertyGridCfg.h"
#include "OTWidgets/SelectionData.h"
#include "OTModelEntities/Geometry.h"
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
#include "EdgeSelection.h"
#include "FaceSelection.h"

class Viewer;
class SceneNodeGeometry;
class ManipulatorBase;

class Model
{
public:
	Model();
	virtual ~Model();

	void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const;

	void setID(unsigned long long id) { m_viewerModelID = id; }
	unsigned long long getID() { return m_viewerModelID; }

	bool isModelActive() { return m_isActive; };
	void activateModel();
	void deactivateModel();

	void attachViewer(Viewer *viewer);
	void detachViewer(Viewer *viewer);
	void resetAllViews3D();
	void refreshAllViews();
	void clearSelection();
	void refreshSelection();
	void selectObject(unsigned long long modelEntityID);

	void setTreeStateRecording(bool flag);

	void addNodeFromFacetData(const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], bool _backFaceCulling,
		double _offsetFactor, std::vector<Geometry::Node>& _nodes, std::list<Geometry::Triangle>& _triangles, std::list<Geometry::Edge>& _edges, std::map<ot::UID, std::string>& _faceNameMap, std::string& _errors,
		bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected);
	
	void addNodeFromFacetDataBase(const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], const std::string& _materialType, const std::string& _textureType, bool _reflective, bool _backFaceCulling,
		double _offsetFactor, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion,
		bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected, std::vector<double>& _transformation);

	void addSceneNode(const ot::EntityTreeItem& _treeItem, ot::VisualisationTypes _visualisationTypes);
	
	void addVisualizationContainerNode(const ot::EntityTreeItem& _treeItem, const ot::VisualisationTypes& _visualisationTypes);
	void addCoordinateSystemNode(const ot::EntityTreeItem& _treeItem, const ot::VisualisationTypes& _visualisationTypes, std::vector<double>& coordinateSettings);
	void updateCoordinateSystemNode(const ot::EntityTreeItem& _treeItem, std::vector<double>& coordinateSettings);
	void activateCoordinateSystemNode(const std::string &csName);

	void addVisualizationAnnotationNode(const ot::EntityTreeItem& _treeItem,
		bool _isHidden, const double _edgeColorRGB[3],
		const std::vector<std::array<double, 3>>& _points,
		const std::vector<std::array<double, 3>>& _pointsRgb,
		const std::vector<std::array<double, 3>>& _triangleP1,
		const std::vector<std::array<double, 3>>& _triangleP2,
		const std::vector<std::array<double, 3>>& _triangleP3,
		const std::vector<std::array<double, 3>>& _triangleRgb);

	void addVisualizationAnnotationNodeDataBase(const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion);
	void addVisualizationMeshNodeFromFacetDataBase(const ot::EntityTreeItem& _treeItem, double _edgeColorRGB[3], bool _displayTetEdges, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion);
	void addVisualizationMeshItemNodeFromFacetDataBase(const ot::EntityTreeItem& _treeItem, bool isHidden, const std::string& _projectName, ot::UID _tetEdgesID, ot::UID _tetEdgesVersion);
	void addVisualizationCartesianMeshNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, double _edgeColorRGB[3], double _meshLineColorRGB[3], bool _showMeshLines, const std::vector<double>& _meshCoordsX, const std::vector<double>& _meshCoordsY, const std::vector<double>& _meshCoordsZ,
		const std::string& _projectName, ot::UID _faceListEntityID, ot::UID _faceListEntityVersion, ot::UID _nodeListEntityID, ot::UID _nodeListEntityVersion);
	void visualizationCartesianMeshNodeShowLines(unsigned long long modelEntityID, bool showMeshLines);
	void addVisualizationCartesianMeshItemNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::vector<int>& _facesList, double _color[3]);
	void visualizationTetMeshNodeTetEdges(unsigned long long modelEntityID, bool displayTetEdges);

	void addVTKNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion);
	void updateVTKNode(ot::UID _entityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);

	SceneNodeBase* getSceneNodeByEntityID(ot::UID _modelEntityID) const;

	void setEntityName(ot::UID _modelEntityID, const std::string& _newName);
	std::string getEntityName(ot::UID _modelEntityID) const;
	ot::UID getEntityID(const std::string& _entityName) const;
	void renameEntityPath(const std::string &oldPath, const std::string &newPath);

	void updateObjectColor(unsigned long long modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	void updateMeshColor(unsigned long long modelEntityID, double colorRGB[3]);
	void updateObjectFacetsFromDataBase(unsigned long long modelEntityID, unsigned long long entityID, unsigned long long entityVersion);

	//! \return Returns true if the selection has requested a new view.
	ot::SelectionHandlingResult setSelectedTreeItems(const ot::SelectionData& _selectionData, std::list<unsigned long long>& _selectedModelItems, std::list<unsigned long long>& _selectedVisibleModelItems);
	void executeAction(ot::UID _buttonID);
	void setHoverTreeItem(ot::UID hoverTreeItemID);
	void clearHoverView();
	void setHoverView(SceneNodeBase *selectedItem);
	unsigned long long getModelEntityIDFromTreeID(ot::UID treeItem);
	unsigned long long getTreeIDFromModelID(ot::UID modelID);

	void setDataModel(unsigned long long modelID) { m_dataModelID = modelID; };
	unsigned long long getDataModel() { return m_dataModelID; };

	osg::Node *getOSGRootNode() { return m_osgRootNode; };

	void getSelectedModelEntityIDs(std::list<unsigned long long> &selectedModelEntityID);
	void getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selectedVisibleModelEntityID);
	void getSelectedTreeItemIDs(std::list<ot::UID> &selectedTreeItemID);

	void removeShapes(std::list<unsigned long long> modelEntityIDList);
	void setShapeVisibility(std::list<unsigned long long> visibleID, std::list<unsigned long long> hiddenID);

	void enterEntitySelectionMode(ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage, std::list<std::string> &optionNames, std::list<std::string> &optionValues);

	enum eSelectionMode { ENTITY, FACE, SHAPE, EDGE };
	eSelectionMode getCurrentSelectionMode() { return m_currentSelectionMode; };
	void escapeKeyPressed();
	void returnKeyPressed();
	void processCurrentSelectionMode(osgUtil::Intersector *intersector, double sceneRadius, bool bCtrlKeyPressed);
	void processHoverView(osgUtil::Intersector *intersector, double sceneRadius);

	void storeShapeNode(SceneNodeBase *node);
	void storeShapeNode(osg::Node *node, SceneNodeBase *sceneNode);
	void forgetShapeNode(osg::Node *node);

	void notifySceneNodeAboutViewChange(const std::string& _sceneNodeName,const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType);
	std::list<Viewer *> getViewerList() { return m_viewerList; };

	unsigned int getCurrentTraversalMask();
	unsigned int getFaceSelectionTraversalMask();
	unsigned int getEdgeSelectionTraversalMask();

	void viewerTabChanged(const ot::WidgetViewBase& _viewInfo);

	void loadNextDataChunk(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex);
	void loadRemainingData(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex);

	void hideEntities(std::list<unsigned long long> hiddenID);

	void showBranch(const std::string &branchName);
	void hideBranch(const std::string &branchName);

	void freeze3DView(bool flag);

	//! @return Return true if a view change was requested.
	bool ensure3DView();

	bool isSingleItemSelected() { return m_singleItemSelected; }

	void centerMouseCursor();
	bool isWireFrameMode() { return m_wireFrameState; }

	osg::Matrix getCurrentWorkingPlaneTransform() { return m_currentWorkingplaneTransform; }
	osg::Matrix getCurrentWorkingPlaneTransformTransposedInverse() { return m_currentWorkingplaneTransformTransposedInverse; }

	void   fillPropertyGrid(const ot::PropertyGridCfg& _configuration);
	void   clearModalPropertyGrid();
	void   setDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName, double value);
	double getDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName);
	bool   propertyGridValueChanged(const ot::Property* _property);

	void lockSelectionAndModification(bool flag);

	void updateCapGeometry(osg::Vec3d normal, osg::Vec3d point, double radius);
	void deleteCapGeometry();

	void setCursorText(const std::string& text);

	void setTransparency(double transparencyValue);
	void setHighlightColor(const ot::Color &colorValue);
	void setHighlightLineWidth(double lineWidthValue);
	void updateEdgeColorMode();
	void updateMeshEdgeColor();

private:
	// Methods
	void	   fillTree();
	
	void	   showAllSceneNodes(SceneNodeBase *root);
	void	   hideAllSceneNodes(SceneNodeBase *root);
	void	   showSelectedSceneNodes(SceneNodeBase *root);
	void	   hideSelectedSceneNodes(SceneNodeBase *root);
	void       hideUnselectedSceneNodes(SceneNodeBase *root);
	void	   hideSceneNodeAndChilds(SceneNodeBase *node);
	void	   setSceneNodeVisibility(SceneNodeBase *root, std::map<unsigned long long, bool> nodeVisibility);
	void	   toggleWireframeView();
	void       toggleWorkingPlane();
	void       toggleAxisCross();
	void       toggleCenterAxisCross();
	void	   toggleCutplane();

	void saveTextEditor();
	void exportTextEditor();

	void saveTable();
	void exportTableAsCSV();
	void addTableRowBefore();
	void addTableRowAfter();
	void removeTableRow();
	void addTableColumnBefore();
	void addTableColumnAfter();
	void removeTableColumn();

	void viewerTabChangedToCentral(const ot::WidgetViewBase& _viewInfo);

	void	   resetSelection(SceneNodeBase *root);
	void       setAllShapesOpaque(SceneNodeBase *root);
	void       setSelectedShapesOpaqueAndOthersTransparent(SceneNodeBase *root);
	void       setWireframeStateForAllSceneNodes(SceneNodeBase *root, bool wireframe);
	void	   addSceneNodesToTree(SceneNodeBase* _root);
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
	SceneNodeGeometry *createNewGeometryNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, bool _manageParentVisibility, bool _manageChildVisibility);
	void       setItemVisibleState(SceneNodeBase* _item, bool _visible);
	void	   showAllSceneNodesAction();
	void	   showSelectedSceneNodesAction();
	void	   hideSelectedSceneNodesAction();
	void	   hideUnselectedSceneNodesAction();
	void	   getPrefetchForSelectedSceneNodes(SceneNodeBase *root, std::string& projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);
	void	   getPrefetchForAllSceneNodes(SceneNodeBase *root, std::string& projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);
	void	   updateSelectedFacesHighlight();
	void	   setSelectedFacesHighlight(SceneNodeGeometry *selectedItem, unsigned long long faceId, bool highlight);
	void       clearSelectedFacesHighlight();
	bool       isFaceSelected(SceneNodeGeometry *selectedItem, unsigned long long faceId);
	void       manageParentVisibility(SceneNodeBase *item);
	void	   updateWorkingPlaneTransform();
	bool       getTransformationOfSelectedShapes(SceneNodeBase *root, bool &first, osg::Matrix &matrix);
	bool       compareTransformations(osg::Matrix &matrix1, osg::Matrix &matrix2);
	void       updateCapGeometryForSceneNodes(SceneNodeBase* root, const osg::Vec3d& normal, const osg::Vec3d& point, double radius);
	void       deleteCapGeometryForSceneNodes(SceneNodeBase* root);
	void       updateCapGeometryForGeometryItem(SceneNodeGeometry *item, const osg::Vec3d &normal, const osg::Vec3d &point, double radius);
	bool	   isLineDrawable(osg::Drawable *drawable);
	void	   clearEdgeSelection();
	osg::Matrix getActiveCoordinateSystemTransform();

	void storeInMaps(SceneNodeBase* _node);
	void removeFromMaps(const SceneNodeBase* _node);

	void exportTextWorker(std::string _filePath, std::string _entityName);

	// Attributes
	enum { ITEM_SELECTED = 1, ITEM_EXPANDED = 2 };

	osg::ref_ptr<osg::Group>					   m_osgRootNode;
	std::list<Viewer *>							   m_viewerList;
	SceneNodeBase*     							   m_sceneNodesRoot;
	std::map<std::string, SceneNodeBase *>		   m_nameToSceneNodesMap;
	std::map <ot::UID, SceneNodeBase *>			   m_treeItemToSceneNodesMap;
	std::map <ot::UID, SceneNodeBase *>            m_modelItemToSceneNodesMap;
	std::map <osg::Node*, SceneNodeBase *>	       m_osgNodetoSceneNodesMap;
	bool										   m_isActive;
	bool										   m_wireFrameState;
	SceneNodeBase*								   m_currentHoverItem;
	unsigned long long							   m_dataModelID;
	eSelectionMode								   m_currentSelectionMode;
	ot::serviceID_t								   m_currentSelectionReplyTo;
	std::string									   m_currentSelectionAction;
	std::list<std::string>						   m_currentSelectionOptionNames;
	std::list<std::string>						   m_currentSelectionOptionValues;
	bool										   m_currentSelectionMultiple;
	std::list<FaceSelection>					   m_currentFaceSelection;
	std::list<EdgeSelection>					   m_currentEdgeSelection;
	std::map < SceneNodeGeometry *, std::list<unsigned long long>> m_selectedFacesList;
	unsigned long long							   m_viewerModelID;
	bool										   m_singleItemSelected;
	bool										   m_treeStateRecording;
	std::map<std::string, char>					   m_treeInfoMap;
	osg::Matrix									   m_currentWorkingplaneTransform;
	osg::Matrix									   m_currentWorkingplaneTransformTransposedInverse;
	ManipulatorBase*                               m_currentManipulator;
	std::string									   m_activeCoordinateSystem;

	bool m_hasModalMenu;
	std::string m_currentMenu;
	std::string m_previousMenu;

	ot::WidgetViewBase m_currentCentralView;
};

