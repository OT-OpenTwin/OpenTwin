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
#include "Factory.h"
#include "ViewerToolBar.h"
#include "GlobalFontPath.h"

#include "OTCore/Logger.h"
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

ot::UID ViewerAPI::createViewer(ot::UID osgModelID, double scaleWidth, double scaleHeight, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB)
{
	intern::ViewerManager::viewerCount()++;

	ot::ViewerView* viewer = new ot::ViewerView(osgModelID, intern::ViewerManager::viewerCount(), scaleWidth, scaleHeight, backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);
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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);
		
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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->addNodeFromFacetDataBase(treeName, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective, modelEntityID, treeIcons, backFaceCulling, offsetFactor, 
										isHidden, isEditable, projectName, entityID, version, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationContainerNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool editable, const ot::VisualisationTypes& _visualisationTypes)
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->addVisualizationContainerNode(treeName, modelEntityID, treeIcons, editable, _visualisationTypes);
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
		Model* model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		if(osgModelID == -1)
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

void ViewerAPI::addVisualizationCartesianMeshItemNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const OldTreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3])
{
	try
	{
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

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
		Model *model = intern::OsgModelManager::uidToModelMap().at(osgModelID);

		model->addVisualizationMeshItemNodeFromFacetDataBase(name, modelEntityID, treeIcons, isHidden, projectName, entityID, version, tetEdgesID, tetEdgesVersion);
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

ot::SelectionHandlingResult ViewerAPI::setSelectedTreeItems(const std::list<ot::UID>& _selectedTreeItems, std::list<unsigned long long>& _selectedModelItems, std::list<unsigned long long>& _selectedVisibleModelItems, ot::SelectionOrigin _selectionOrigin)
{
	ot::SelectionHandlingResult result;
	
	Model* model = GlobalModel::instance();

	if (model) {
		result = model->setSelectedTreeItems(_selectedTreeItems, _selectedModelItems, _selectedVisibleModelItems, _selectionOrigin);
	}

	return result;
}

void ViewerAPI::executeAction(unsigned long long buttonID)
{
	if (GlobalModel::instance() == nullptr) return;

	GlobalModel::instance()->executeAction(buttonID);
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
	
	DataBase::GetDataBase()->setDataBaseServerURL(databaseURL);
	DataBase::GetDataBase()->setUserCredentials(userName, password);
}

void ViewerAPI::prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIDs);
}

void ViewerAPI::setTabNames(ot::UID _viewerID, const std::string & _osgViewTabName, const std::string & _plotTabName, const std::string & _versionGraphTabName) {
	ot::ViewerView * v = intern::ViewerManager::uidToViewerMap()[_viewerID];
	if (v != nullptr) {
		v->getViewer()->setTabNames(_osgViewTabName, _plotTabName, _versionGraphTabName);
	}
}

ot::WidgetView* ViewerAPI::getPlotWidget(ot::UID _viewerID) {
	ot::ViewerView * v = intern::ViewerManager::uidToViewerMap()[_viewerID];
	if (v != nullptr) {
		return v->getViewer()->get1DPlot();
	}
	else {
		return nullptr;
	}
}

void ViewerAPI::viewerTabChanged(const ot::WidgetViewBase& _viewInfo) {
	if (GlobalModel::instance() != nullptr) {
		GlobalModel::instance()->viewerTabChanged(_viewInfo);
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
		OT_LOG_W("A rubberband is already active. Created by service: " + std::to_string(rubberband->creator()));
		return;
	}

	viewer->second->getViewer()->createRubberband(_senderId, _note, _configurationJson);
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
