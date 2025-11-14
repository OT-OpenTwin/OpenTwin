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
#include "OldTreeIcon.h"

#include "OTCore/JSON.h"
#include "OTCore/Color.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTGui/GuiTypes.h"
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
	__declspec(dllexport) void activateModel(ot::UID osgModelID);
	__declspec(dllexport) void deactivateCurrentlyActiveModel(void);
	__declspec(dllexport) void deleteModel(ot::UID osgModelID);

	__declspec(dllexport) ot::UID getActiveDataModel(void);
	__declspec(dllexport) ot::UID getActiveViewerModel(void);

	__declspec(dllexport) ot::UID createViewer(ot::UID osgModelID, double scaleWidth, double scaleHeight, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB, QWidget* _parent);
	__declspec(dllexport) ot::WidgetView* getViewerWidget(ot::UID viewer);

	__declspec(dllexport) void resetAllViews3D(ot::UID osgModelID);
	__declspec(dllexport) void refreshAllViews(ot::UID osgModelID);
	__declspec(dllexport) void clearSelection(ot::UID osgModelID);
	__declspec(dllexport) void refreshSelection(ot::UID osgModelID);
	__declspec(dllexport) void selectObject(ot::UID osgModelID, ot::UID entityID);

	__declspec(dllexport) void setTreeStateRecording(ot::UID osgModelID, bool flag);

	__declspec(dllexport) ot::SelectionHandlingResult setSelectedTreeItems(const ot::SelectionData& _selectionData, std::list<unsigned long long>& _selectedModelItems, std::list<unsigned long long>& _selectedVisibleModelItems);
	__declspec(dllexport) void executeAction(unsigned long long buttonID);

	__declspec(dllexport) void setHoverTreeItem(ot::UID hoverItemID);

	__declspec(dllexport) void addNodeFromFacetData(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
												    double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNameMap, std::string &errors,
													bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected);
	__declspec(dllexport) void addNodeFromFacetDataBase(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
														double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion,
													    bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation);
	
	__declspec(dllexport) void addVisualizationContainerNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool editable, const ot::VisualisationTypes& _visualisationTypes);
	
	__declspec(dllexport) void addVisualizationNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool editable, ot::VisualisationTypes _visualisationTypes);
	__declspec(dllexport) void addVTKNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);
	__declspec(dllexport) void updateVTKNode(ot::UID osgModelID, unsigned long long modelEntityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);

	__declspec(dllexport) void addVisualizationAnnotationNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID,
															  const OldTreeIcon &treeIcons, bool isHidden,
															  const double edgeColorRGB[3],
															  const std::vector<std::array<double, 3>> &points,
															  const std::vector<std::array<double, 3>> &points_rgb,
															  const std::vector<std::array<double, 3>> &triangle_p1,
															  const std::vector<std::array<double, 3>> &triangle_p2,
															  const std::vector<std::array<double, 3>> &triangle_p3,
															  const std::vector<std::array<double, 3>> &triangle_rgb);
	__declspec(dllexport) void addVisualizationAnnotationNodeDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion);
	__declspec(dllexport) void addVisualizationMeshNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion);
	__declspec(dllexport) void addVisualizationMeshItemNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion, long long tetEdgesID, long long tetEdgesVersion);
	__declspec(dllexport) void addVisualizationCartesianMeshNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
																 const std::string &projectName, unsigned long long faceListEntityID, unsigned long long faceListEntityVersion, unsigned long long nodeListEntityID, unsigned long long nodeListEntityVersion);
	__declspec(dllexport) void visualizationCartesianMeshNodeShowLines(ot::UID osgModelID, unsigned long long modelEntityID, bool showMeshLines);
	__declspec(dllexport) void addVisualizationCartesianMeshItemNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3]);
	__declspec(dllexport) void visualizationTetMeshNodeTetEdges(ot::UID osgModelID, unsigned long long modelEntityID, bool displayTetEdges);
	
	__declspec(dllexport) void notifySceneNodeAboutViewChange(ot::UID osgModelID, const std::string& _sceneNodeName, const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType);
	
	__declspec(dllexport) void setEntityName(ot::UID _modelEntityID, const std::string& _newName);
	__declspec(dllexport) std::string getEntityName(ot::UID _modelEntityID);
	__declspec(dllexport) ot::UID getEntityID(const std::string& _entityName);
	__declspec(dllexport) void renameEntityPath(const std::string &oldPath, const std::string &newPath);

	__declspec(dllexport) void updateObjectColor(ot::UID osgModelID, unsigned long long modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string& materialType, const std::string& textureType, bool reflective);
	__declspec(dllexport) void updateMeshColor(ot::UID osgModelID, unsigned long long modelEntityID, double colorRGB[3]);
	__declspec(dllexport) void updateObjectFacetsFromDataBase(ot::UID osgModelID, unsigned long long modelEntityID, unsigned long long entityID, unsigned long long entityVersion);

	__declspec(dllexport) void setClearColor(ot::UID viewrID, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB);

	__declspec(dllexport) void setDataModel(ot::UID osgModelID, ot::UID dataModelID);

	__declspec(dllexport) void getSelectedModelEntityIDs(std::list<unsigned long long> &selected);
	__declspec(dllexport) void getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selected);
	__declspec(dllexport) ot::UID getModelEntityIDFromTreeID(ot::UID treeID);
	__declspec(dllexport) ot::UID getTreeIDFromModelEntityID(ot::UID treeID);

	__declspec(dllexport) void removeShapes(ot::UID osgModelID, std::list<unsigned long long> modelEntityID);
	__declspec(dllexport) void setShapeVisibility(ot::UID osgModelID, std::list<unsigned long long> visibleID, std::list<unsigned long long> hiddenID);
	__declspec(dllexport) void hideEntities(ot::UID osgModelID, std::list<unsigned long long> hiddenID);

	__declspec(dllexport) void showBranch(ot::UID osgModelID, const std::string &branchName);
	__declspec(dllexport) void hideBranch(ot::UID osgModelID, const std::string &branchName);

	__declspec(dllexport) void enterEntitySelectionMode(ot::UID osgModelID, ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage, std::list<std::string> &optionNames, std::list<std::string> &optionValues);

	__declspec(dllexport) void setFontPath(const std::string& _fontPath);

	__declspec(dllexport) void setDataBaseConnection(const std::string &databaseURL, const std::string &userName, const std::string &password);

	__declspec(dllexport) void prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);

	__declspec(dllexport) void setTabNames(ot::UID _viewerID, const std::string & _osgViewTabName, const std::string & _versionGraphTabName);

	/********************************************************************************************/

	__declspec(dllexport) void viewerTabChanged(const ot::WidgetViewBase& _viewInfo);

	__declspec(dllexport) void loadNextDataChunk(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _curentChunkEndIndex);

	__declspec(dllexport) void viewDataModifiedChanged(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, bool _isModified);

	__declspec(dllexport) void shortcutActivated(const std::string & _keySequence);

	__declspec(dllexport) void createRubberband(ot::UID _viewerID, ot::serviceID_t _senderId, std::string& _note, const std::string& _configurationJson);
	__declspec(dllexport) void cancelAllRubberbands(ot::UID osgModelID);

	__declspec(dllexport) void settingsItemChanged(ot::UID _viewerID, const ot::Property* _property);

	__declspec(dllexport) bool propertyGridValueChanged(ot::UID _viewerID, const ot::Property* _property);

	__declspec(dllexport) void freeze3DView(unsigned long long osgModelID, bool flag);

	Model *getModelFromID(ot::UID osgModelID);
}
