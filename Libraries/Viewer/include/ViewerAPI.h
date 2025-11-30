// @otlicense
// File: ViewerAPI.h
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

#include "Geometry.h"

#include "OTCore/JSON.h"
#include "OTCore/Color.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/EntityTreeItem.h"
#include "OTGui/WidgetViewBase.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/Plot1DDataBaseCfg.h"
#include "OTGui/VisualisationTypes.h"
#include "OTGui/TableRange.h"
#include "OTWidgets/SelectionData.h"
#include "ViewChangedStates.h"

#include <list>
#include <vector>
#include <string>

#include <qcolor.h>

class Model;
class QPen;
class FrontendAPI;
namespace ot { class Property; }
namespace ot { class WidgetView; }

namespace ViewerAPI {

	__declspec(dllexport) void setFrontendAPI(FrontendAPI* _api);

	__declspec(dllexport) void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator);

	__declspec(dllexport) ot::UID createModel(void);
	__declspec(dllexport) void activateModel(ot::UID _osgModelID);
	__declspec(dllexport) void deactivateCurrentlyActiveModel(void);
	__declspec(dllexport) void deleteModel(ot::UID _osgModelID);

	__declspec(dllexport) ot::UID getActiveDataModel(void);
	__declspec(dllexport) ot::UID getActiveViewerModel(void);

	__declspec(dllexport) ot::UID createViewer(ot::UID _osgModelID, double scaleWidth, double scaleHeight, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB, QWidget* _parent);
	__declspec(dllexport) ot::WidgetView* getViewerWidget(ot::UID viewer);

	__declspec(dllexport) void resetAllViews3D(ot::UID _osgModelID);
	__declspec(dllexport) void refreshAllViews(ot::UID _osgModelID);
	__declspec(dllexport) void clearSelection(ot::UID _osgModelID);
	__declspec(dllexport) void refreshSelection(ot::UID _osgModelID);
	__declspec(dllexport) void selectObject(ot::UID _osgModelID, ot::UID entityID);

	__declspec(dllexport) void setTreeStateRecording(ot::UID _osgModelID, bool flag);

	__declspec(dllexport) ot::SelectionHandlingResult setSelectedTreeItems(const ot::SelectionData& _selectionData, std::list<ot::UID>& _selectedModelItems, std::list<ot::UID>& _selectedVisibleModelItems);
	__declspec(dllexport) void executeAction(ot::UID _buttonID);

	__declspec(dllexport) void setHoverTreeItem(ot::UID _hoverItemID);

	__declspec(dllexport) void addNodeFromFacetData(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], bool _backFaceCulling,
												    double _offsetFactor, std::vector<Geometry::Node>& _nodes, std::list<Geometry::Triangle>& _triangles, std::list<Geometry::Edge>& _edges, std::map<ot::UID, std::string>& _faceNameMap, std::string& _errors,
													bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected);
	__declspec(dllexport) void addNodeFromFacetDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], const std::string& _materialType, const std::string& _textureType, bool _reflective, bool _backFaceCulling,
														double _offsetFactor, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion,
													    bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected, std::vector<double>& _transformation);
	
	__declspec(dllexport) void addVisualizationContainerNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, const ot::VisualisationTypes& _visualisationTypes);
	
	__declspec(dllexport) void addVisualizationNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, ot::VisualisationTypes _visualisationTypes);
	__declspec(dllexport) void addVTKNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion);
	__declspec(dllexport) void updateVTKNode(ot::UID _osgModelID, ot::UID _entityID, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion);

	__declspec(dllexport) void addVisualizationAnnotationNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem,
															  bool _isHidden, const double _edgeColorRGB[3],
															  const std::vector<std::array<double, 3>>& _points,
															  const std::vector<std::array<double, 3>>& _pointsRgb,
															  const std::vector<std::array<double, 3>>& _triangleP1,
															  const std::vector<std::array<double, 3>>& _triangleP2,
															  const std::vector<std::array<double, 3>>& _triangleP3,
															  const std::vector<std::array<double, 3>>& _triangleRgb);
	__declspec(dllexport) void addVisualizationAnnotationNodeDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion);
	__declspec(dllexport) void addVisualizationMeshNodeFromFacetDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, double _edgeColorRGB[3], bool _displayTetEdges, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion);
	__declspec(dllexport) void addVisualizationMeshItemNodeFromFacetDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _tetEdgesID, ot::UID _tetEdgesVersion);
	__declspec(dllexport) void addVisualizationCartesianMeshNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, double _edgeColorRGB[3], double _meshLineColorRGB[3], bool _showMeshLines, const std::vector<double>& _meshCoordsX, const std::vector<double>& _meshCoordsY, const std::vector<double>& _meshCoordsZ,
																 const std::string& _projectName, ot::UID _faceListEntityID, ot::UID _faceListEntityVersion, ot::UID _nodeListEntityID, ot::UID _nodeListEntityVersion);
	__declspec(dllexport) void visualizationCartesianMeshNodeShowLines(ot::UID _osgModelID, ot::UID _modelEntityID, bool _showMeshLines);
	__declspec(dllexport) void addVisualizationCartesianMeshItemNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::vector<int>& _facesList, double _color[3]);
	__declspec(dllexport) void visualizationTetMeshNodeTetEdges(ot::UID _osgModelID, ot::UID modelEntityID, bool displayTetEdges);
	
	__declspec(dllexport) void notifySceneNodeAboutViewChange(ot::UID _osgModelID, const std::string& _sceneNodeName, const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType);
	
	__declspec(dllexport) void setEntityName(ot::UID _modelEntityID, const std::string& _newName);
	__declspec(dllexport) std::string getEntityName(ot::UID _modelEntityID);
	__declspec(dllexport) ot::UID getEntityID(const std::string& _entityName);
	__declspec(dllexport) void renameEntityPath(const std::string &oldPath, const std::string &newPath);

	__declspec(dllexport) void updateObjectColor(ot::UID _osgModelID, ot::UID modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	__declspec(dllexport) void updateMeshColor(ot::UID _osgModelID, ot::UID modelEntityID, double colorRGB[3]);
	__declspec(dllexport) void updateObjectFacetsFromDataBase(ot::UID _osgModelID, ot::UID modelEntityID, ot::UID entityID, ot::UID entityVersion);

	__declspec(dllexport) void setClearColor(ot::UID viewrID, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB);

	__declspec(dllexport) void setDataModel(ot::UID _osgModelID, ot::UID dataModelID);

	__declspec(dllexport) void getSelectedModelEntityIDs(std::list<ot::UID> &selected);
	__declspec(dllexport) void getSelectedVisibleModelEntityIDs(std::list<ot::UID> &selected);
	__declspec(dllexport) ot::UID getModelEntityIDFromTreeID(ot::UID treeID);
	__declspec(dllexport) ot::UID getTreeIDFromModelEntityID(ot::UID treeID);

	__declspec(dllexport) void removeShapes(ot::UID _osgModelID, std::list<ot::UID> modelEntityID);
	__declspec(dllexport) void setShapeVisibility(ot::UID _osgModelID, std::list<ot::UID> visibleID, std::list<ot::UID> hiddenID);
	__declspec(dllexport) void hideEntities(ot::UID _osgModelID, std::list<ot::UID> hiddenID);

	__declspec(dllexport) void showBranch(ot::UID _osgModelID, const std::string &branchName);
	__declspec(dllexport) void hideBranch(ot::UID _osgModelID, const std::string &branchName);

	__declspec(dllexport) void enterEntitySelectionMode(ot::UID _osgModelID, ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage, std::list<std::string> &optionNames, std::list<std::string> &optionValues);

	__declspec(dllexport) void setFontPath(const std::string& _fontPath);

	__declspec(dllexport) void setDataBaseConnection(const std::string &databaseURL, const std::string &userName, const std::string &password);

	__declspec(dllexport) void prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<ot::UID, ot::UID>> &prefetchIDs);

	__declspec(dllexport) void setTabNames(ot::UID _viewerID, const std::string & _osgViewTabName, const std::string & _versionGraphTabName);

	/********************************************************************************************/

	__declspec(dllexport) void viewerTabChanged(const ot::WidgetViewBase& _viewInfo);

	__declspec(dllexport) void loadNextDataChunk(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex);
	__declspec(dllexport) void loadRemainingData(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex);

	__declspec(dllexport) void viewDataModifiedChanged(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, bool _isModified);

	__declspec(dllexport) void shortcutActivated(const std::string & _keySequence);

	__declspec(dllexport) void createRubberband(ot::UID _viewerID, ot::serviceID_t _senderId, std::string& _note, const std::string& _configurationJson);
	__declspec(dllexport) void cancelAllRubberbands(ot::UID osgModelID);

	__declspec(dllexport) void settingsItemChanged(ot::UID _viewerID, const ot::Property* _property);

	__declspec(dllexport) bool propertyGridValueChanged(ot::UID _viewerID, const ot::Property* _property);

	__declspec(dllexport) void freeze3DView(ot::UID osgModelID, bool flag);

	Model *getModelFromID(ot::UID osgModelID);
}
