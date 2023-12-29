#include "stdafx.h"

#include "ViewerAPI.h"
#include "Model.h"
#include "Viewer.h"
#include "Notifier.h"
#include "DataBase.h"
#include "Plot.h"
#include "VersionGraph.h"
#include "Rubberband.h"
#include "EntityParameterizedDataTable.h"
#include "EntityTableSelectedRanges.h"
#include "EntityBase.h"
#include "Factory.h"

#include "TableViewer.h"

ot::UID modelCount = 0;
std::map<ot::UID, Model*> osgModelManager;

ot::UID viewerCount = 0;
std::map<ot::UID, Viewer*> viewerManager;

ViewerAPI::Notifier *globalNotifier = nullptr;

Model *globalActiveModel = nullptr;

std::string globalFontPath;

void ViewerAPI::setFontPath(const std::string &fontPath)
{
	globalFontPath = fontPath;
}

ot::UID ViewerAPI::createModel(void)
{
	modelCount++;

	Model *model = new Model;
	osgModelManager[modelCount] = model;
	model->setID(modelCount);

	assert(getNotifier() != nullptr);
	if (getNotifier() != nullptr) getNotifier()->createTree();

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
		assert(getNotifier() != nullptr);
		if (getNotifier() != nullptr) getNotifier()->removeViewer(viewer->getViewerID());

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

	Viewer *viewer = new Viewer(osgModelID, viewerCount, scaleWidth, scaleHeight, backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);
	viewerManager[viewerCount] = viewer;

	return viewerCount;
}

QWidget* ViewerAPI::getViewerWidget(ot::UID viewerID)
{
	try
	{
		Viewer *viewer = viewerManager.at(viewerID);

		return viewer;
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified viewer does not exist");
	}

	return nullptr;
}

void ViewerAPI::registerNotifier(ViewerAPI::Notifier *notifier)
{
	globalNotifier = notifier;
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

void ViewerAPI::addNodeFromFacetData(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
									 double offsetFactor, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors,
									 bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);
		
		model->addNodeFromFacetData(treeName, surfaceColorRGB, edgeColorRGB, modelEntityID, treeIcons, backFaceCulling, offsetFactor, false, isEditable, nodes, triangles, edges, 
								    errors, selectChildren, manageParentVisibility, manageChildVisibility, showWhenSelected);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addNodeFromFacetDataBase(ot::UID osgModelID, const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, 
										 const std::string &textureType, bool reflective, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling,
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

void ViewerAPI::addVisualizationContainerNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool editable)
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

void ViewerAPI::addVTKNode(ot::UID osgModelID, const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool editable, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion)
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
											   const TreeIcon &treeIcons, bool isHidden,
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

void ViewerAPI::addVisualizationAnnotationNodeDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version)
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

void ViewerAPI::addVisualizationMeshNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, unsigned long long entityID, unsigned long long version)
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

void ViewerAPI::addVisualizationCartesianMeshNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
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

void ViewerAPI::addVisualizationCartesianMeshItemNode(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3])
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

void ViewerAPI::addVisualizationMeshItemNodeFromFacetDataBase(ot::UID osgModelID, const std::string &name, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version, long long tetEdgesID, long long tetEdgesVersion)
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

void ViewerAPI::addVisualizationPlot1DNode(ot::UID osgModelID, const std::string &name, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden,
										   const std::string &projectName, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
										   bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax, std::list<ot::UID> &curvesID, std::list<ot::UID> &curvesVersions,
										   std::list<std::string> &curvesNames)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationPlot1DNode(name, modelEntityID, treeIcons, isHidden, projectName, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, curvesID, curvesVersions, curvesNames);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationTextNode(ot::UID osgModelID, const std::string &name, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName,
										 ot::UID textEntityID, ot::UID textEntityVersion)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationTextNode(name, modelEntityID, treeIcons, isHidden, projectName, textEntityID, textEntityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::addVisualizationTableNode(ot::UID osgModelID, const std::string &name, ot::UID modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName,
										  ot::UID tableEntityID, ot::UID tableEntityVersion)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->addVisualizationTableNode(name, modelEntityID, treeIcons, isHidden, projectName, tableEntityID, tableEntityVersion);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}


void ViewerAPI::visualizationResult1DPropertiesChanged(ot::UID osgModelID, unsigned long long entityID, unsigned long long version)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->visualizationResult1DPropertiesChanged(entityID, version);
	}
	catch (std::out_of_range)
	{
		throw std::exception("The specified model does not exist");
	}
}

void ViewerAPI::visualizationPlot1DPropertiesChanged(ot::UID osgModelID, ot::UID modelEntityID, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
												     bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax)
{
	try
	{
		Model *model = osgModelManager.at(osgModelID);

		model->visualizationPlot1DPropertiesChanged(modelEntityID, title, plotType, plotQuantity, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax);
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
	Viewer * v = viewerManager[viewerID];
	if (v != nullptr) {
		v->setClearColorAutomatic(backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);
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

void ViewerAPI::setSelectedTreeItems(std::list<ot::UID> &selected, std::list<unsigned long long> &selectedModelItems, std::list<unsigned long long> &selectedVisibleModelItems)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->setSelectedTreeItems(selected, selectedModelItems, selectedVisibleModelItems);
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

void ViewerAPI::setEntityName(unsigned long long modelEntityID, const std::string &newName)
{
	if (globalActiveModel == nullptr) return;

	globalActiveModel->setEntityName(modelEntityID, newName);
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

void ViewerAPI::setPlotStyles(const std::string & _plotBackgroundSheet, const std::string & _xyPlotStyleSheet, const std::string & _xyCanvasStyleSheet, const QColor & _polarPlotColor, const QPen & _zoomerPen) {
	for (auto itm : viewerManager) {
		if (itm.second != nullptr) {
			itm.second->get1DPlot()->SetStyleSheets(_plotBackgroundSheet, _xyPlotStyleSheet, _xyCanvasStyleSheet, _polarPlotColor, _zoomerPen);
		}
	}
}

void ViewerAPI::setPlotStyle(ot::UID _viewerID, const std::string & _plotBackgroundSheet, const std::string & _xyPlotStyleSheet, const std::string & _xyCanvasStyleSheet, const QColor & _polarPlotColor, const QPen & _zoomerPen) {
	auto itm = viewerManager.find(_viewerID);
	if (itm != viewerManager.end()) {
		if (itm->second != nullptr) {
			itm->second->get1DPlot()->SetStyleSheets(_plotBackgroundSheet, _xyPlotStyleSheet, _xyCanvasStyleSheet, _polarPlotColor, _zoomerPen);
		}
	}
}

void ViewerAPI::setVersionGraphStyles(const std::string & _plotBackgroundSheet, const QColor & _foregroundColor, const QColor & _boxColor, const QColor &_highlightBoxColor, const QColor &_highlightForegroundColor) {
	for (auto itm : viewerManager) {
		if (itm.second != nullptr) {
			itm.second->getVersionGraph()->SetStyleSheets(_plotBackgroundSheet, _foregroundColor, _boxColor, _highlightBoxColor, _highlightForegroundColor);
		}
	}
}

void ViewerAPI::setVersionGraphStyle(ot::UID _viewerID, const std::string & _plotBackgroundSheet, const QColor & _foregroundColor, const QColor & _boxColor, const QColor &_highlightBoxColor, const QColor &_highlightForegroundColor) {
	auto itm = viewerManager.find(_viewerID);
	if (itm != viewerManager.end()) {
		if (itm->second != nullptr) {
			itm->second->getVersionGraph()->SetStyleSheets(_plotBackgroundSheet, _foregroundColor, _boxColor, _highlightBoxColor, _highlightForegroundColor);
		}
	}
}

void ViewerAPI::setVersionGraph(ot::UID _viewerID, std::list<std::tuple<std::string, std::string, std::string>> &versionGraph, const std::string &activeVersion, const std::string &activeBranch)
{
	auto itm = viewerManager.find(_viewerID);
	if (itm != viewerManager.end()) {
		if (itm->second != nullptr) {
			itm->second->getVersionGraph()->setVersionGraph(versionGraph, activeVersion, activeBranch);
		}
	}
}

void ViewerAPI::setVersionGraphActive(ot::UID _viewerID, const std::string &activeVersion, const std::string &activeBranch)
{
	auto itm = viewerManager.find(_viewerID);
	if (itm != viewerManager.end()) {
		if (itm->second != nullptr) {
			itm->second->getVersionGraph()->selectVersion(activeVersion, activeBranch);
		}
	}
}

void ViewerAPI::removeVersionGraphVersions(ot::UID _viewerID, const std::list<std::string> &versions)
{
	auto itm = viewerManager.find(_viewerID);
	if (itm != viewerManager.end()) {
		if (itm->second != nullptr) {
			itm->second->getVersionGraph()->removeVersions(versions);
		}
	}
}

void ViewerAPI::addNewVersionGraphStateAndActivate(ot::UID _viewerID, const std::string &newVersion, const std::string &activeBranch, const std::string &parentVersion, const std::string &description)
{
	auto itm = viewerManager.find(_viewerID);
	if (itm != viewerManager.end()) {
		if (itm->second != nullptr) {
			itm->second->getVersionGraph()->addVersionAndActivate(newVersion, activeBranch, parentVersion, description);
		}
	}
}

void ViewerAPI::setTabNames(ot::UID _viewerID, const std::string & _osgViewTabName, const std::string & _plotTabName, const std::string & _versionGraphTabName) {
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr) {
		v->setTabNames(_osgViewTabName, _plotTabName, _versionGraphTabName);
	}
}

QWidget * ViewerAPI::getPlotWidget(ot::UID _viewerID) {
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr) {
		return v->get1DPlot()->widget();
	}
	else {
		return nullptr;
	}
}


QWidget * ViewerAPI::getTable(ot::UID _viewerID)
{
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr) {
		return v->getTableViewer()->getTable();
	}
	else {
		return nullptr;
	}
}

bool ViewerAPI::setTable(ot::UID _viewerID, ot::UID entityID, ot::UID entityVersion)
{
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr) 
	{
		DataBase* temp = DataBase::GetDataBase();
		auto entityBase = temp->GetEntityFromEntityIDandVersion(entityID, entityVersion, &Factory::getClassFactory());
		std::shared_ptr<EntityParameterizedDataTable> tableTopoEnt((dynamic_cast<EntityParameterizedDataTable*>(entityBase)));
		bool refresh = false;

		if (tableTopoEnt != nullptr)
		{
			refresh = v->getTableViewer()->CreateNewTable(tableTopoEnt, tableTopoEnt->getSelectedHeaderOrientation());
		}
		else
		{
			std::shared_ptr<EntityResultTable<std::string>> generalTableTopoEnt((dynamic_cast<EntityResultTable<std::string>*>(entityBase)));
			if (generalTableTopoEnt != nullptr)
			{
				refresh = v->getTableViewer()->CreateNewTable(generalTableTopoEnt);
			}
		}
		return refresh;
	}
	else
	{
		return false;
	}
}


void ViewerAPI::setTableSelection(ot::UID _viewerID, std::vector<ot::TableRange> ranges)
{
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr)
	{
		TableViewer* tViewer = v->getTableViewer();
	
		tViewer->SelectRanges(ranges);
	}
	else
	{
		throw std::runtime_error("Tableviewer is not connected with viewer.");
	}
}

std::vector<ot::TableRange> ViewerAPI::GetSelectedTableRange(ot::UID _viewerID)
{
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr)
	{
		TableViewer* tViewer = v->getTableViewer();
		return tViewer->GetSelectedRanges();
	}
	else
	{
		throw std::runtime_error("Tableviewer is not connected with viewer.");
	}
}

void ViewerAPI::ChangeColourOfSelection(ot::UID _viewerID, ot::Color backGroundColour)
{
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr)
	{
		TableViewer* tViewer = v->getTableViewer();

		tViewer->ChangeColorOfSelection(backGroundColour);
	}
}

std::pair<ot::UID, ot::UID> ViewerAPI::GetActiveTableIdentifyer(ot::UID _viewerID)
{
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr)
	{
		TableViewer* tViewer = v->getTableViewer();
		return std::make_pair<>(tViewer->getTableID(),	tViewer->getTableVersion());
	}
	return std::pair<ot::UID, ot::UID>();
}

std::string ViewerAPI::getTableName(ot::UID _viewerID)
{
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr) {
		return v->getTableViewer()->getTableName();
	}
	else {
		return "";
	}
}

void ViewerAPI::AddToSelectedTableRow(bool _insertAbove, ot::UID _viewerID)
{
	Viewer* v = viewerManager[_viewerID];
	if (v != nullptr) {
		v->getTableViewer()->AddRow(_insertAbove);
	}
}

void ViewerAPI::AddToSelectedTableColumn(bool _insertLeft, ot::UID _viewerID)
{
	Viewer* v = viewerManager[_viewerID];
	if (v != nullptr) {
		v->getTableViewer()->AddColumn(_insertLeft);
	}
}

void ViewerAPI::DeleteFromSelectedTableColumn(ot::UID _viewerID)
{
	Viewer* v = viewerManager[_viewerID];
	if (v != nullptr) {
		v->getTableViewer()->DeleteSelectedColumn();
	}
}

void ViewerAPI::DeleteFromSelectedTableRow(ot::UID _viewerID)
{
	Viewer* v = viewerManager[_viewerID];
	if (v != nullptr) {
		v->getTableViewer()->DeleteSelectedRow();
	}
}

QWidget * ViewerAPI::getVersionGraphWidget(ot::UID _viewerID) {
	Viewer * v = viewerManager[_viewerID];
	if (v != nullptr) {
		return v->getVersionGraph()->widget();
	}
	else {
		return nullptr;
	}
}

void ViewerAPI::viewerTabChanged(const std::string & _tabTitle) 
{
	if (globalActiveModel != nullptr)
	{
		globalActiveModel->viewerTabChanged(_tabTitle);
	}
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

	Rubberband * rubberband = viewer->second->getRubberband();
	if (rubberband) {
		std::string msg{ "[VIEW] [WARNING] A rubberband is already active. Created by service: " };
		msg.append(std::to_string(rubberband->creator())).append("\n");
		globalNotifier->displayText(msg);
		return;
	}

	viewer->second->createRubberband(_senderId, _note, _configurationJson);
}

void ViewerAPI::settingsItemChanged(ot::UID _viewerID, ot::AbstractSettingsItem * _item) {
	auto viewer = viewerManager.find(_viewerID);
	if (viewer == viewerManager.end()) {
		assert(0);
		return;
	}
	viewer->second->settingsItemChanged(_item);
}

void ViewerAPI::contextMenuItemClicked(ot::UID _viewerID, const std::string& _menuName, const std::string& _itemName) {
	auto viewer = viewerManager.find(_viewerID);
	if (viewer == viewerManager.end()) {
		assert(0);
		return;
	}
	viewer->second->contextMenuItemClicked(_menuName, _itemName);
}

void ViewerAPI::contextMenuItemCheckedChanged(ot::UID _viewerID, const std::string& _menuName, const std::string& _itemName, bool _isChecked) {
	auto viewer = viewerManager.find(_viewerID);
	if (viewer == viewerManager.end()) {
		assert(0);
		return;
	}
	viewer->second->contextMenuItemCheckedChanged(_menuName, _itemName, _isChecked);
}

bool ViewerAPI::propertyGridValueChanged(ot::UID _viewerID, int itemID)
{
	auto viewer = viewerManager.find(_viewerID);
	if (viewer == viewerManager.end()) {
		assert(0);
		return false;
	}
	return viewer->second->propertyGridValueChanged(itemID);
}
