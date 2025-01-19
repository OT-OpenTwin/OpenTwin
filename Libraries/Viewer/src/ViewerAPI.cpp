#include "stdafx.h"

#include "ViewerAPI.h"
#include "Model.h"
#include "Viewer.h"
#include "ViewerView.h"
#include "FrontendAPI.h"
#include "DataBase.h"
#include "PlotManager.h"
#include "PlotManagerView.h"
#include "Rubberband.h"
#include "EntityBase.h"
#include "Factory.h"
#include "ViewerToolBar.h"

#include "OTCore/Logger.h"
#include "OTCore/VariableToStringConverter.h"

ot::UID modelCount = 0;
std::map<ot::UID, Model*> osgModelManager;

ot::UID viewerCount = 0;
std::map<ot::UID, ot::ViewerView*> viewerManager;

Model *globalActiveModel = nullptr;

std::string globalFontPath;

void ViewerAPI::setFontPath(const std::string &fontPath)
{
	globalFontPath = fontPath;
}

void ViewerAPI::setFrontendAPI(FrontendAPI* _api) {
	FrontendAPI::setInstance(_api);
}

ot::UID ViewerAPI::createModel(void)
{
	modelCount++;

	Model *model = new Model;
	osgModelManager[modelCount] = model;
	model->setID(modelCount);

	if (FrontendAPI::instance() != nullptr) {
		FrontendAPI::instance()->createTree();
	}

	return modelCount;
}

void ViewerAPI::activateModel(ot::UID osgModelID)
{
	Model *newActiveModel = osgModelManager[osgModelID];
	if (newActiveModel == globalActiveModel) return;

	if (globalActiveModel != nullptr) globalActiveModel->deactivateModel();
	newActiveModel->activateModel();

	globalActiveModel = newActiveModel;
}

void ViewerAPI::deactivateCurrentlyActiveModel(void)
{
	if (globalActiveModel != nullptr)
	{
		globalActiveModel->deactivateModel();
	}

	globalActiveModel = nullptr;
}

void ViewerAPI::deleteModel(ot::UID osgModelID)
{
	Model *model = osgModelManager[osgModelID];

	// Make sure that the to be deleted model is no longer active
	if (model == globalActiveModel) globalActiveModel = nullptr;

	// Get all viewers and delete them
	std::list<Viewer *> viewerList = model->getViewerList();

	for (auto viewer : viewerList)
	{
		assert(FrontendAPI::instance() != nullptr);
		if (FrontendAPI::instance() != nullptr) FrontendAPI::instance()->removeViewer(viewer->getViewerID());

		viewerManager.erase(viewer->getViewerID());
		viewer->detachFromModel();

		delete viewer;
		viewer = nullptr;
	}

	// Delete the model
	osgModelManager.erase(osgModelID);

	delete model;
	model = nullptr;
}

ot::UID ViewerAPI::getActiveDataModel(void)
{
	if (globalActiveModel == nullptr) return 0;

	return globalActiveModel->getDataModel();
}

ot::UID ViewerAPI::getActiveViewerModel(void)
{
	if (globalActiveModel == nullptr) return 0;

	return globalActiveModel->getID();
}

ot::UID ViewerAPI::createViewer(ot::UID osgModelID, double scaleWidth, double scaleHeight, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB)
{
	viewerCount++;

	ot::ViewerView* viewer = new ot::ViewerView(osgModelID, viewerCount, scaleWidth, scaleHeight, backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);
	viewerManager[viewerCount] = viewer;

	return viewerCount;
}

ot::WidgetView* ViewerAPI::getViewerWidget(ot::UID viewerID)
{
	try
	{
		ot::WidgetView* viewer = viewerManager.at(viewerID);

		return viewer;
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified viewer does not exist");
	}

	return nullptr;
}

void ViewerAPI::resetAllViews1D(ot::UID osgModelID)
{
	getModelFromID(osgModelID)->resetAllViews1D();
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

void ViewerAPI::addNodeFromFacetData(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
									 double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNameMap, std::string &errors,
									 bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);
		
		model->addNodeFromFacetData(treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons, backFaceCulling, offsetFactor, false, isEditable, nodes, triangles, edges, faceNameMap,
								    errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addNodeFromFacetDataBase(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, 
										 const std::string &textureType, bool reflective, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool backFaceCulling,
										 double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, unsigned long long entityID, unsigned long long version,
										 bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addNodeFromFacetDataBase(treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective, modelEntityID, treeIcons, backFaceCulling, offsetFactor, 
										isHidden, isEditable, projectName, entityID, version, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationContainerNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool editable)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationContainerNode(treeName, modelEntityID, treeIcons, editable);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}


void ViewerAPI::addVisualizationNode(ot::UID osgModelID, const std::string& treeName, unsigned long long modelEntityID, const OldTreeIcon& treeIcons, bool editable, ot::VisualisationTypes _visualisationTypes)
{
	try
	{
		Model* model = osgModelManager.at(osgModelID);

		model->addSceneNode(treeName, modelEntityID, treeIcons, editable,_visualisationTypes);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVTKNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVTKNode(treeName, modelEntityID, treeIcons, isHidden, editable, projectName, visualizationDataID, visualizationDataVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::updateVTKNode(ot::UID osgModelID, unsigned long long modelEntityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->updateVTKNode(modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationAnnotationNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, 
											   const OldTreeIcon &treeIcons, bool isHidden,
										       const double edgeColorRGB[3],
											   const std::vector<std::array<double, 3>> &points,
											   const std::vector<std::array<double, 3>> &points_rgb,
											   const std::vector<std::array<double, 3>> &triangle_p1,
											   const std::vector<std::array<double, 3>> &triangle_p2,
											   const std::vector<std::array<double, 3>> &triangle_p3,
											   const std::vector<std::array<double, 3>> &triangle_rgb)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationAnnotationNode(name, modelEntityID, treeIcons, isHidden, edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationAnnotationNodeDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationAnnotationNodeDataBase(name, modelEntityID, treeIcons, isHidden, projectName, entityID, version);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationMeshNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, unsigned long long entityID, unsigned long long version)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationMeshNodeFromFacetDataBase(name, modelEntityID, treeIcons, edgeColorRGB, displayTetEdges, projectName, entityID, version);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationCartesianMeshNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
												  const std::string &projectName, unsigned long long faceListEntityID, unsigned long long faceListEntityVersion, unsigned long long nodeListEntityID, unsigned long long nodeListEntityVersion)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationCartesianMeshNode(name, modelEntityID, treeIcons, isHidden, edgeColorRGB, meshLineColorRGB, showMeshLines, meshCoordsX, meshCoordsY, meshCoordsZ, projectName, faceListEntityID, faceListEntityVersion, nodeListEntityID, nodeListEntityVersion);
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
		Model *model = osgModelManager.at(osgModelID);

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
		Model *model = osgModelManager.at(osgModelID);

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
		if(osgModelID == -1)
		{ 
			model = globalActiveModel;
		}
		else
		{
			model = osgModelManager.at(osgModelID);
		}
		assert(model != nullptr);
		model->notifySceneNodeAboutViewChange(_sceneNodeName, _state, _viewType);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

std::list<std::string> ViewerAPI::getSelectedCurves(ot::UID osgModelID)
{
	Model* model = osgModelManager.at(osgModelID);
	
	std::list<std::string> curveDescriptions = model->getSelectedCurves();
	
	return curveDescriptions;
}

void ViewerAPI::removeSelectedCurveNodes(ot::UID osgModelID)
{
	Model* model = osgModelManager.at(osgModelID);

	model->removedSelectedCurveNodes();
}

void ViewerAPI::addVisualizationCartesianMeshItemNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3])
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationCartesianMeshItemNode(name, modelEntityID, treeIcons, isHidden, facesList, color);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationMeshItemNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version, long long tetEdgesID, long long tetEdgesVersion)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationMeshItemNodeFromFacetDataBase(name, modelEntityID, treeIcons, isHidden, projectName, entityID, version, tetEdgesID, tetEdgesVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationPlot1DNode(ot::UID _osgModelID, const ot::Plot1DDataBaseCfg& _config)
{
	try
	{
		Model *model = osgModelManager.at(_osgModelID);
		model->addVisualizationPlot1DNode(_config);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}



void ViewerAPI::visualizationResult1DPropertiesChanged(ot::UID _osgModelID, ot::UID _entityID, ot::UID _version)
{
	try
	{
		Model *model = osgModelManager.at(_osgModelID);

		model->visualizationResult1DPropertiesChanged(_entityID, _version);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::visualizationPlot1DPropertiesChanged(ot::UID osgModelID, const ot::Plot1DCfg& _config)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->visualizationPlot1DPropertiesChanged(_config);
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
		Model *model = osgModelManager.at(osgModelID);

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
		Model* model = osgModelManager.at(osgModelID);

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
		Model *model = osgModelManager.at(osgModelID);

		model->updateObjectFacetsFromDataBase(modelEntityID, entityID, entityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::setClearColor(ot::UID viewerID, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB) {
	ot::ViewerView * v = viewerManager[viewerID];
	if (v != nullptr) {
		v->getViewer()->setClearColorAutomatic(backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);
	}
}

Model *ViewerAPI::getModelFromID(ot::UID osgModelID)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);
		return model;
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}

	return nullptr;
}

void ViewerAPI::setSelectedTreeItems(const std::list<ot::UID>& _selectedTreeItems, std::list<unsigned long long>& _selectedModelItems, std::list<unsigned long long>& _selectedVisibleModelItems, bool _selectionFromTree)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->setSelectedTreeItems(_selectedTreeItems, _selectedModelItems, _selectedVisibleModelItems, _selectionFromTree);
}

void ViewerAPI::executeAction(unsigned long long buttonID)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->executeAction(buttonID);
}

void ViewerAPI::setHoverTreeItem(ot::UID hoverItemID)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->setHoverTreeItem(hoverItemID);
}

void ViewerAPI::setEntityName(unsigned long long _modelEntityID, const std::string& _newName)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->setEntityName(_modelEntityID, _newName);
}

void ViewerAPI::renameEntityPath(const std::string &oldPath, const std::string &newPath)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->renameEntityPath(oldPath, newPath);
}

void ViewerAPI::setDataModel(ot::UID osgModelID, ot::UID dataModelID)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->setDataModel(dataModelID);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::getSelectedModelEntityIDs(std::list<unsigned long long> &selected)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->getSelectedModelEntityIDs(selected);
}

void ViewerAPI::getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selected)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->getSelectedVisibleModelEntityIDs(selected);
}

ot::UID ViewerAPI::getModelEntityIDFromTreeID(ot::UID uid)
{
	if (globalActiveModel == nullptr) return 0;

	return globalActiveModel->getModelEntityIDFromTreeID(uid);
}

ot::UID ViewerAPI::getTreeIDFromModelEntityID(ot::UID modelID)
{
	if (globalActiveModel == nullptr) return 0;

	return globalActiveModel->getTreeIDFromModelID(modelID);
}

void ViewerAPI::removeShapes(ot::UID osgModelID, std::list<unsigned long long> modelEntityID)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

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
		Model *model = osgModelManager.at(osgModelID);

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
		Model *model = osgModelManager.at(osgModelID);

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
		Model *model = osgModelManager.at(osgModelID);

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
		Model *model = osgModelManager.at(osgModelID);

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
	
	DataBase::GetDataBase()->setDataBaseServerURL(databaseURL);
	DataBase::GetDataBase()->setUserCredentials(userName, password);
}

void ViewerAPI::prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIDs);
}

void ViewerAPI::setTabNames(ot::UID _viewerID, const std::string & _osgViewTabName, const std::string & _plotTabName, const std::string & _versionGraphTabName) {
	ot::ViewerView * v = viewerManager[_viewerID];
	if (v != nullptr) {
		v->getViewer()->setTabNames(_osgViewTabName, _plotTabName, _versionGraphTabName);
	}
}

ot::WidgetView* ViewerAPI::getPlotWidget(ot::UID _viewerID) {
	ot::ViewerView * v = viewerManager[_viewerID];
	if (v != nullptr) {
		return v->getViewer()->get1DPlot();
	}
	else {
		return nullptr;
	}
}

void ViewerAPI::viewerTabChanged(const std::string & _tabTitle, ot::WidgetViewBase::ViewType _type)
{
	if (globalActiveModel != nullptr)
	{
		globalActiveModel->viewerTabChanged(_tabTitle, _type);
	}
}

void ViewerAPI::viewDataModifiedChanged(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, bool _isModified) {
	ViewerToolBar::instance().viewDataModifiedHasChanged(_type, _isModified);
}

void ViewerAPI::shortcutActivated(const std::string & _keySequence) {
	if (globalActiveModel != nullptr) {

	}
}

void ViewerAPI::createRubberband(ot::UID _viewerID, ot::serviceID_t _senderId, std::string & _note, const std::string & _configurationJson) {
	auto viewer = viewerManager.find(_viewerID);
	if (viewer == viewerManager.end()) {
		assert(0);
		return;
	}

	Rubberband * rubberband = viewer->second->getViewer()->getRubberband();
	if (rubberband) {
		OT_LOG_W("A rubberband is already active. Created by service: " + std::to_string(rubberband->creator()));
		return;
	}

	viewer->second->getViewer()->createRubberband(_senderId, _note, _configurationJson);
}

void ViewerAPI::settingsItemChanged(ot::UID _viewerID, const ot::Property* _item) {
	auto viewer = viewerManager.find(_viewerID);
	if (viewer == viewerManager.end()) {
		assert(0);
		return;
	}
	viewer->second->getViewer()->settingsItemChanged(_item);
}

bool ViewerAPI::propertyGridValueChanged(ot::UID _viewerID, const ot::Property* _property)
{
	auto viewer = viewerManager.find(_viewerID);
	if (viewer == viewerManager.end()) {
		assert(0);
		return false;
	}
	return viewer->second->getViewer()->propertyGridValueChanged(_property);
}
