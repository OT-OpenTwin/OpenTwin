// @otlicense
// File: ViewerAPI.cpp
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

#include "stdafx.h"

#include "ViewerAPI.h"
#include "GlobalModel.h"
#include "Viewer.h"
#include "ViewerView.h"
#include "FrontendAPI.h"
#include "DataBase.h"
#include "PlotManager.h"
#include "PlotManagerView.h"
#include "Rubberband.h"
#include "EntityBase.h"
#include "ViewerToolBar.h"
#include "GlobalFontPath.h"

#include "OTCore/LogDispatcher.h"
#include "OTCore/VariableToStringConverter.h"

namespace ViewerAPI {
	namespace intern {
		class OsgModelManager {
			OT_DECL_STATICONLY(OsgModelManager)
		public:
			static std::map<ot::UID, Model*>& uidToModelMap(void) {
				static std::map<ot::UID, Model*> g_instance;
				return g_instance;
			}

			static ot::UID& modelCount(void) {
				static ot::UID g_instance;
				return g_instance;
			}
		};

		class ViewerManager {
			OT_DECL_STATICONLY(ViewerManager)
		public:
			static std::map<ot::UID, ot::ViewerView*>& uidToViewerMap(void) {
				static std::map<ot::UID, ot::ViewerView*> g_instance;
				return g_instance;
			}

			static ot::UID& viewerCount(void) {
				static ot::UID g_instance;
				return g_instance;
			}
		};
	}
}

void ViewerAPI::setFontPath(const std::string& _fontPath)
{
	GlobalFontPath::instance() = _fontPath;
}

void ViewerAPI::setFrontendAPI(FrontendAPI* _api) {
	FrontendAPI::setInstance(_api);
}

void ViewerAPI::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) {
	ot::JsonArray modelArr;
	for (const auto& it : intern::OsgModelManager::uidToModelMap()) {
		ot::JsonObject modelObj;
		it.second->getDebugInformation(modelObj, _allocator);
		modelArr.PushBack(modelObj, _allocator);
	}
	_object.AddMember("Models", modelArr, _allocator);

	ot::JsonArray viewerArr;
	for (const auto& it : intern::ViewerManager::uidToViewerMap()) {
		ot::JsonObject viewerObj;
		it.second->getViewer()->getDebugInformation(viewerObj, _allocator);
		viewerArr.PushBack(viewerObj, _allocator);
	}
	_object.AddMember("Viewers", viewerArr, _allocator);
}

ot::UID ViewerAPI::createModel(void)
{
	intern::OsgModelManager::modelCount()++;

	Model *model = new Model;
	intern::OsgModelManager::uidToModelMap()[intern::OsgModelManager::modelCount()] = model;
	model->setID(intern::OsgModelManager::modelCount());

	if (FrontendAPI::instance() != nullptr) {
		FrontendAPI::instance()->createTree();
	}

	return intern::OsgModelManager::modelCount();
}

void ViewerAPI::activateModel(ot::UID osgModelID)
{
	Model *newActiveModel = intern::OsgModelManager::uidToModelMap()[osgModelID];
	if (newActiveModel == GlobalModel::instance()) return;

	if (GlobalModel::instance() != nullptr) GlobalModel::instance()->deactivateModel();
	newActiveModel->activateModel();

	GlobalModel::setInstance(newActiveModel);
}

void ViewerAPI::deactivateCurrentlyActiveModel(void)
{
	if (GlobalModel::instance() != nullptr)
	{
		GlobalModel::instance()->deactivateModel();
	}

	GlobalModel::setInstance(nullptr);
}

void ViewerAPI::deleteModel(ot::UID osgModelID)
{
	Model *model = intern::OsgModelManager::uidToModelMap()[osgModelID];

	// Make sure that the to be deleted model is no longer active
	if (model == GlobalModel::instance()) {
		GlobalModel::setInstance(nullptr);
	}

	// Get all viewers and delete them
	std::list<Viewer *> viewerList = model->getViewerList();

	for (auto viewer : viewerList)
	{
		assert(FrontendAPI::instance() != nullptr);
		if (FrontendAPI::instance() != nullptr) FrontendAPI::instance()->removeViewer(viewer->getViewerID());

		intern::ViewerManager::uidToViewerMap().erase(viewer->getViewerID());
		viewer->detachFromModel();

		delete viewer;
		viewer = nullptr;
	}

	// Delete the model
	intern::OsgModelManager::uidToModelMap().erase(osgModelID);

	delete model;
	model = nullptr;
}

ot::UID ViewerAPI::getActiveDataModel(void)
{
	if (GlobalModel::instance() == nullptr) return 0;

	return GlobalModel::instance()->getDataModel();
}

ot::UID ViewerAPI::getActiveViewerModel(void)
{
	if (GlobalModel::instance() == nullptr) return 0;

	return GlobalModel::instance()->getID();
}

ot::UID ViewerAPI::createViewer(ot::UID osgModelID, double scaleWidth, double scaleHeight, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB, QWidget* _parent)
{
	intern::ViewerManager::viewerCount()++;

	ot::ViewerView* viewer = new ot::ViewerView(osgModelID, intern::ViewerManager::viewerCount(), scaleWidth, scaleHeight, backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB, _parent);
	intern::ViewerManager::uidToViewerMap()[intern::ViewerManager::viewerCount()] = viewer;

	return intern::ViewerManager::viewerCount();
}

ot::WidgetView* ViewerAPI::getViewerWidget(ot::UID viewerID)
{
	try
	{
		ot::WidgetView* viewer = intern::ViewerManager::uidToViewerMap().at(viewerID);

		return viewer;
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified viewer does not exist");
	}

	return nullptr;
}

void ViewerAPI::resetAllViews3D(ot::UID osgModelID)
{
	getModelFromID(osgModelID)->resetAllViews3D();
}

void ViewerAPI::refreshAllViews(ot::UID osgModelID)
{
	getModelFromID(osgModelID)->refreshAllViews();
}

void ViewerAPI::clearSelection(ot::UID osgModelID)
{
	getModelFromID(osgModelID)->clearSelection();
}

void ViewerAPI::refreshSelection(ot::UID osgModelID)
{
	getModelFromID(osgModelID)->refreshSelection();
}

void ViewerAPI::selectObject(ot::UID osgModelID, ot::UID enttiyID)
{
	getModelFromID(osgModelID)->selectObject(enttiyID);
}

void ViewerAPI::setTreeStateRecording(ot::UID osgModelID, bool flag)
{
	getModelFromID(osgModelID)->setTreeStateRecording(flag);
}

void ViewerAPI::freeze3DView(unsigned long long osgModelID, bool flag)
{
	getModelFromID(osgModelID)->freeze3DView(flag);
}

void ViewerAPI::addNodeFromFacetData(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], bool _backFaceCulling,
	double _offsetFactor, std::vector<Geometry::Node>& _nodes, std::list<Geometry::Triangle>& _triangles, std::list<Geometry::Edge>& _edges, std::map<ot::UID, std::string>& _faceNameMap, std::string& _errors,
	bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);
		
		model->addNodeFromFacetData(_treeItem, false, _surfaceColorRGB, _edgeColorRGB, _backFaceCulling, _offsetFactor, _nodes, _triangles, _edges, _faceNameMap,
								    _errors, _manageParentVisibility, _manageChildVisibility, _showWhenSelected);
		cancelAllRubberbands(_osgModelID);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addNodeFromFacetDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], const std::string& _materialType, const std::string& _textureType, bool _reflective, bool _backFaceCulling,
	double _offsetFactor, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion,
	bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected, std::vector<double>& _transformation)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addNodeFromFacetDataBase(_treeItem, _isHidden, _surfaceColorRGB, _edgeColorRGB, _materialType, _textureType, _reflective, _backFaceCulling, _offsetFactor, 
										_projectName, _dataEntityID, _dataEntityVersion, _manageParentVisibility, _manageChildVisibility, _showWhenSelected, _transformation);
		cancelAllRubberbands(_osgModelID);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationContainerNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, const ot::VisualisationTypes& _visualisationTypes)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVisualizationContainerNode(_treeItem, _visualisationTypes);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}


void ViewerAPI::addVisualizationNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, ot::VisualisationTypes _visualisationTypes)
{
	try
	{
		Model* model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addSceneNode(_treeItem, _visualisationTypes);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVTKNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVTKNode(_treeItem, _isHidden, _projectName, _dataEntityID, _dataEntityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::updateVTKNode(ot::UID _osgModelID, ot::UID _entityID, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->updateVTKNode(_entityID, _projectName, _dataEntityID, _dataEntityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationAnnotationNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem,
	bool _isHidden, const double _edgeColorRGB[3],
	const std::vector<std::array<double, 3>>& _points,
	const std::vector<std::array<double, 3>>& _pointsRgb,
	const std::vector<std::array<double, 3>>& _triangleP1,
	const std::vector<std::array<double, 3>>& _triangleP2,
	const std::vector<std::array<double, 3>>& _triangleP3,
	const std::vector<std::array<double, 3>>& _triangleRgb)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVisualizationAnnotationNode(_treeItem, _isHidden, _edgeColorRGB, _points, _pointsRgb, _triangleP1, _triangleP2, _triangleP3, _triangleRgb);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationAnnotationNodeDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVisualizationAnnotationNodeDataBase(_treeItem, _isHidden, _projectName, _dataEntityID, _dataEntityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationMeshNodeFromFacetDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, double _edgeColorRGB[3], bool _displayTetEdges, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVisualizationMeshNodeFromFacetDataBase(_treeItem, _edgeColorRGB, _displayTetEdges, _projectName, _dataEntityID, _dataEntityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationCartesianMeshNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, double _edgeColorRGB[3], double _meshLineColorRGB[3], bool _showMeshLines, const std::vector<double>& _meshCoordsX, const std::vector<double>& _meshCoordsY, const std::vector<double>& _meshCoordsZ,
	const std::string& _projectName, ot::UID _faceListEntityID, ot::UID _faceListEntityVersion, ot::UID _nodeListEntityID, ot::UID _nodeListEntityVersion)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVisualizationCartesianMeshNode(_treeItem, _isHidden, _edgeColorRGB, _meshLineColorRGB, _showMeshLines, _meshCoordsX, _meshCoordsY, _meshCoordsZ, _projectName, _faceListEntityID, _faceListEntityVersion, _nodeListEntityID, _nodeListEntityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::visualizationCartesianMeshNodeShowLines(ot::UID osgModelID, unsigned long long modelEntityID, bool showMeshLines)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->visualizationCartesianMeshNodeShowLines(modelEntityID, showMeshLines);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::visualizationTetMeshNodeTetEdges(ot::UID osgModelID, unsigned long long modelEntityID, bool displayTetEdges)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->visualizationTetMeshNodeTetEdges(modelEntityID, displayTetEdges);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::notifySceneNodeAboutViewChange(ot::UID osgModelID, const std::string& _sceneNodeName, const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType)
{
	try
	{
		Model* model = nullptr;
		if (osgModelID == -1)
		{ 
			model = GlobalModel::instance();
		}
		else
		{
			model = intern::OsgModelManager::uidToModelMap().at(osgModelID);
		}
		assert(model != nullptr);
		model->notifySceneNodeAboutViewChange(_sceneNodeName, _state, _viewType);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationCartesianMeshItemNode(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::vector<int>& _facesList, double _color[3])
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVisualizationCartesianMeshItemNode(_treeItem, _isHidden, _facesList, _color);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationMeshItemNodeFromFacetDataBase(ot::UID _osgModelID, const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _tetEdgesID, ot::UID _tetEdgesVersion)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(_osgModelID);

		model->addVisualizationMeshItemNodeFromFacetDataBase(_treeItem, _isHidden, _projectName, _tetEdgesID, _tetEdgesVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}


void ViewerAPI::updateObjectColor(ot::UID osgModelID, unsigned long long modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->updateObjectColor(modelEntityID, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::updateMeshColor(ot::UID osgModelID, unsigned long long modelEntityID, double colorRGB[3])
{
	try
	{
		Model* model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->updateMeshColor(modelEntityID, colorRGB);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::updateObjectFacetsFromDataBase(ot::UID osgModelID, unsigned long long modelEntityID, unsigned long long entityID, unsigned long long entityVersion)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->updateObjectFacetsFromDataBase(modelEntityID, entityID, entityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::setClearColor(ot::UID viewerID, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB) {
	ot::ViewerView * v = intern::ViewerManager::uidToViewerMap()[viewerID];
	if (v != nullptr) {
		v->getViewer()->setClearColorAutomatic(backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);
	}
}

Model *ViewerAPI::getModelFromID(ot::UID osgModelID)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);
		return model;
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}

	return nullptr;
}

ot::SelectionHandlingResult ViewerAPI::setSelectedTreeItems(const ot::SelectionData& _selectionData, std::list<unsigned long long>& _selectedModelItems, std::list<unsigned long long>& _selectedVisibleModelItems)
{
	ot::SelectionHandlingResult result;
	
	Model* model = GlobalModel::instance();

	if (model) {
		result = model->setSelectedTreeItems(_selectionData, _selectedModelItems, _selectedVisibleModelItems);
	}

	return result;
}

void ViewerAPI::executeAction(ot::UID _buttonID)
{
	if (GlobalModel::instance() == nullptr) return;

	GlobalModel::instance()->executeAction(_buttonID);
}

void ViewerAPI::setHoverTreeItem(ot::UID hoverItemID)
{
	if (GlobalModel::instance() == nullptr) return;

	GlobalModel::instance()->setHoverTreeItem(hoverItemID);
}

void ViewerAPI::setEntityName(unsigned long long _modelEntityID, const std::string& _newName)
{
	if (GlobalModel::instance() == nullptr) return;

	GlobalModel::instance()->setEntityName(_modelEntityID, _newName);
}

std::string ViewerAPI::getEntityName(ot::UID _modelEntityID) {
	if (GlobalModel::instance() == nullptr) {
		return "";
	}

	return GlobalModel::instance()->getEntityName(_modelEntityID);
}

ot::UID ViewerAPI::getEntityID(const std::string& _entityName) {
	if (GlobalModel::instance() == nullptr) {
		return ot::invalidUID;
	}

	return GlobalModel::instance()->getEntityID(_entityName);
}

void ViewerAPI::renameEntityPath(const std::string &oldPath, const std::string &newPath)
{
	if (GlobalModel::instance() == nullptr) return;

	GlobalModel::instance()->renameEntityPath(oldPath, newPath);
}

void ViewerAPI::setDataModel(ot::UID osgModelID, ot::UID dataModelID)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->setDataModel(dataModelID);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::getSelectedModelEntityIDs(std::list<unsigned long long> &selected)
{
	if (GlobalModel::instance() == nullptr) return;

	GlobalModel::instance()->getSelectedModelEntityIDs(selected);
}

void ViewerAPI::getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selected)
{
	if (GlobalModel::instance() == nullptr) return;

	GlobalModel::instance()->getSelectedVisibleModelEntityIDs(selected);
}

ot::UID ViewerAPI::getModelEntityIDFromTreeID(ot::UID uid)
{
	if (GlobalModel::instance() == nullptr) return 0;

	return GlobalModel::instance()->getModelEntityIDFromTreeID(uid);
}

ot::UID ViewerAPI::getTreeIDFromModelEntityID(ot::UID modelID)
{
	if (GlobalModel::instance() == nullptr) return 0;

	return GlobalModel::instance()->getTreeIDFromModelID(modelID);
}

void ViewerAPI::removeShapes(ot::UID osgModelID, std::list<unsigned long long> modelEntityID)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->removeShapes(modelEntityID);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::setShapeVisibility(ot::UID osgModelID, std::list<unsigned long long> visibleID, std::list<unsigned long long> hiddenID)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->setShapeVisibility(visibleID, hiddenID);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::hideEntities(ot::UID osgModelID, std::list<unsigned long long> hiddenID)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->hideEntities(hiddenID);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::showBranch(ot::UID osgModelID, const std::string &branchName)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->showBranch(branchName);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::hideBranch(ot::UID osgModelID, const std::string &branchName)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->hideBranch(branchName);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::enterEntitySelectionMode(ot::UID osgModelID, ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage,
									     std::list<std::string> &optionNames, std::list<std::string> &optionValues)
{
	getModelFromID(osgModelID)->enterEntitySelectionMode(replyTo, selectionType, allowMultipleSelection, selectionFilter, selectionAction, selectionMessage, optionNames, optionValues);
}

void ViewerAPI::setDataBaseConnection(const std::string &databaseURL, const std::string &userName, const std::string &password)
{
	
	DataBase::instance().setDataBaseServerURL(databaseURL);
	DataBase::instance().setUserCredentials(userName, password);
}

void ViewerAPI::prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	DataBase::instance().prefetchDocumentsFromStorage(prefetchIDs);
}

void ViewerAPI::setTabNames(ot::UID _viewerID, const std::string & _osgViewTabName, const std::string & _versionGraphTabName) {
	ot::ViewerView * v = intern::ViewerManager::uidToViewerMap()[_viewerID];
	if (v != nullptr) {
		v->getViewer()->setTabNames(_osgViewTabName, _versionGraphTabName);
	}
}

void ViewerAPI::viewerTabChanged(const ot::WidgetViewBase& _viewInfo) {
	if (GlobalModel::instance() != nullptr) {
		GlobalModel::instance()->viewerTabChanged(_viewInfo);
	}
}

void ViewerAPI::loadNextDataChunk(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex) {
	if (GlobalModel::instance() != nullptr) {
		GlobalModel::instance()->loadNextDataChunk(_entityName, _type, _nextChunkStartIndex);
	}
}

void ViewerAPI::loadRemainingData(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex) {
	if (GlobalModel::instance() != nullptr) {
		GlobalModel::instance()->loadRemainingData(_entityName, _type, _nextChunkStartIndex);
	}
}

void ViewerAPI::viewDataModifiedChanged(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, bool _isModified) {
	ViewerToolBar::instance().viewDataModifiedHasChanged(_type, _isModified);
}

void ViewerAPI::shortcutActivated(const std::string & _keySequence) {
	if (GlobalModel::instance() != nullptr) {

	}
}

void ViewerAPI::createRubberband(ot::UID _viewerID, ot::serviceID_t _senderId, std::string & _note, const std::string & _configurationJson) {
	auto viewer = intern::ViewerManager::uidToViewerMap().find(_viewerID);
	if (viewer == intern::ViewerManager::uidToViewerMap().end()) {
		assert(0);
		return;
	}

	Rubberband * rubberband = viewer->second->getViewer()->getRubberband();
	if (rubberband) {
		viewer->second->getViewer()->cancelRubberband();
	}

	viewer->second->getViewer()->createRubberband(_senderId, _note, _configurationJson);
}

void ViewerAPI::cancelAllRubberbands(ot::UID osgModelID)
{
	Model* model = intern::OsgModelManager::uidToModelMap()[osgModelID];
	if (model == nullptr) return;

	// Get all viewers and delete the rubberbands
	std::list<Viewer*> viewerList = model->getViewerList();

	for (auto viewer : viewerList)
	{
		viewer->cancelRubberband();
	}
}

void ViewerAPI::settingsItemChanged(ot::UID _viewerID, const ot::Property* _item) {
	auto viewer = intern::ViewerManager::uidToViewerMap().find(_viewerID);
	if (viewer == intern::ViewerManager::uidToViewerMap().end()) {
		assert(0);
		return;
	}
	viewer->second->getViewer()->settingsItemChanged(_item);
}

bool ViewerAPI::propertyGridValueChanged(ot::UID _viewerID, const ot::Property* _property)
{
	auto viewer = intern::ViewerManager::uidToViewerMap().find(_viewerID);
	if (viewer == intern::ViewerManager::uidToViewerMap().end()) {
		assert(0);
		return false;
	}
	return viewer->second->getViewer()->propertyGridValueChanged(_property);
}
