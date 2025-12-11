// @otlicense
// File: Model.cpp
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

#include "Model.h"
#include "Viewer.h"
#include "ViewerView.h"
#include "FrontendAPI.h"
#include "ViewerToolBar.h"
#include "ViewerSettings.h"

#include "OTCore/String.h"

#include "OTWidgets/Table.h"
#include "OTWidgets/TableView.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/TextEditorView.h"

#include "OTCommunication/ActionTypes.h"

#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "OTCore/ReturnMessage.h"
#include "OTCore/LogDispatcher.h"

#include "SceneNodeBase.h"
#include "SceneNodeContainer.h"
#include "SceneNodeGeometry.h"
#include "SceneNodeAnnotation.h"
#include "SceneNodeMesh.h"
#include "SceneNodeMeshItem.h"
#include "SceneNodeCartesianMesh.h"
#include "SceneNodeCartesianMeshItem.h"
#include "SceneNodeVTK.h"
#include "SceneNodeMultiVisualisation.h"

#include "ManipulatorBase.h"
#include "TransformManipulator.h"

#include "DataBase.h"
#include "PlotManager.h"
#include "DocumentAPI.h"
#include "GridFSFileInfo.h"
#include "PlotManagerView.h"

#include "VisualiserHelper.h"

#include "IntersectionCapCalculator.h"

#include <QtWidgets/qheaderview.h>

#include <fstream>

Model::Model() :
	osgRootNode(nullptr),
	sceneNodesRoot(nullptr),
	isActive(false),
	wireFrameState(false),
	currentHoverItem(nullptr),
	dataModelID(0),
	currentSelectionMode(ENTITY), 
	currentSelectionMultiple(false),
	viewerModelID(0),
	singleItemSelected(false),
	treeStateRecording(false),
	currentManipulator(nullptr),
	m_hasModalMenu(false),
	m_currentCentralView(ot::WidgetViewBase::CustomView),
	currentSelectionReplyTo(ot::invalidServiceID)
{
	sceneNodesRoot = new SceneNodeContainer();
	osgRootNode = sceneNodesRoot->getShapeNode();
}

Model::~Model()
{
	// Delete all shapes in the shapes list
	delete 	sceneNodesRoot;
	sceneNodesRoot = nullptr;

	ViewerToolBar::instance().removeUIControls();
}

void Model::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) const {
	using namespace ot;
	JsonArray viewerArr;
	for (const auto& it : viewerList) {
		JsonObject viewerObj;
		viewerObj.AddMember("ID", it->getViewerID(), _allocator);
		viewerArr.PushBack(viewerObj, _allocator);
	}
	_object.AddMember("Viewers", viewerArr, _allocator);

	if (sceneNodesRoot) {
		JsonObject rootObj;
		sceneNodesRoot->getDebugInformation(rootObj, _allocator);
		_object.AddMember("SceneNodesRoot", rootObj, _allocator);
	}
	else {
		_object.AddMember("SceneNodesRoot", JsonNullValue(), _allocator);
	}

	_object.AddMember("IsActive", isActive, _allocator);
	_object.AddMember("WireFrameState", wireFrameState, _allocator);
	if (currentHoverItem) {
		JsonObject currentHoverItemObj;
		currentHoverItemObj.AddMember("Name", JsonString(currentHoverItem->getName(), _allocator), _allocator);
		currentHoverItemObj.AddMember("ModelID", currentHoverItem->getModelEntityID(), _allocator);
		currentHoverItemObj.AddMember("TreeItemID", currentHoverItem->getTreeItemID(), _allocator);
		_object.AddMember("CurrentHoverItem", currentHoverItemObj, _allocator);
	}
	else {
		_object.AddMember("CurrentHoverItem", JsonNullValue(), _allocator);
	}
	_object.AddMember("DataModelID", dataModelID, _allocator);

	switch (currentSelectionMode) {
	case Model::ENTITY:
		_object.AddMember("CurrentSelectionMode", JsonString("Entity", _allocator), _allocator);
		break;
	case Model::FACE:
		_object.AddMember("CurrentSelectionMode", JsonString("Face", _allocator), _allocator);
		break;
	case Model::SHAPE:
		_object.AddMember("CurrentSelectionMode", JsonString("Shape", _allocator), _allocator);
		break;
	case Model::EDGE:
		_object.AddMember("CurrentSelectionMode", JsonString("Edge", _allocator), _allocator);
		break;
	default:
		_object.AddMember("CurrentSelectionMode", JsonString("<Unknown>", _allocator), _allocator);
		break;
	}

	_object.AddMember("CurrentSelectionReplyTo", currentSelectionReplyTo, _allocator);
	_object.AddMember("CurrentSelectionAction", JsonString(currentSelectionAction, _allocator), _allocator);
	_object.AddMember("CurrentSelectionOptionNames", JsonArray(currentSelectionOptionNames, _allocator), _allocator);
	_object.AddMember("CurrentSelectionOptionValues", JsonArray(currentSelectionOptionValues, _allocator), _allocator);
	_object.AddMember("CurrentSelectionMultiple", currentSelectionMultiple, _allocator);

	JsonArray currentFaceSelectionArr;
	for (const FaceSelection& faceSelection : currentFaceSelection) {
		if (faceSelection.getSelectedItem()) {
			JsonObject faceSelectionObj;
			faceSelectionObj.AddMember("SelectedItem.Name", JsonString(currentHoverItem->getName(), _allocator), _allocator);
			faceSelectionObj.AddMember("SelectedItem.ModelID", currentHoverItem->getModelEntityID(), _allocator);
			faceSelectionObj.AddMember("SelectedItem.TreeItemID", currentHoverItem->getTreeItemID(), _allocator);
			currentFaceSelectionArr.PushBack(faceSelectionObj, _allocator);
		}
		else {
			currentFaceSelectionArr.PushBack(JsonNullValue(), _allocator);
		}
	}
	_object.AddMember("CurrentFaceSelection", currentFaceSelectionArr, _allocator);

	JsonArray currentEdgeSelectionArr;
	for (const EdgeSelection& edgeSelection : currentEdgeSelection) {
		if (edgeSelection.getSelectedItem()) {
			JsonObject edgeSelectionObj;
			edgeSelectionObj.AddMember("SelectedItem.Name", JsonString(edgeSelection.getSelectedItem()->getName(), _allocator), _allocator);
			edgeSelectionObj.AddMember("SelectedItem.ModelID", edgeSelection.getSelectedItem()->getModelEntityID(), _allocator);
			edgeSelectionObj.AddMember("SelectedItem.TreeItemID", edgeSelection.getSelectedItem()->getTreeItemID(), _allocator);
			currentEdgeSelectionArr.PushBack(edgeSelectionObj, _allocator);
		}
		else {
			currentEdgeSelectionArr.PushBack(JsonNullValue(), _allocator);
		}
	}
	_object.AddMember("CurrentEdgeSelection", currentEdgeSelectionArr, _allocator);

	_object.AddMember("ViewerModelID", viewerModelID, _allocator);
	_object.AddMember("SingleItemSelected", singleItemSelected, _allocator);
	_object.AddMember("TreeStateRecording", treeStateRecording, _allocator);

	_object.AddMember("HasModalMenu", m_hasModalMenu, _allocator);
	_object.AddMember("CurrentMenu", JsonString(m_currentMenu, _allocator), _allocator);
	_object.AddMember("PreviousMenu", JsonString(m_previousMenu, _allocator), _allocator);

	JsonObject currentCentralViewObj;
	m_currentCentralView.addToJsonObject(currentCentralViewObj, _allocator);
	_object.AddMember("CurrentCentralView", currentCentralViewObj, _allocator);
}

void Model::attachViewer(Viewer *viewer)
{
	assert(std::find(viewerList.begin(), viewerList.end(), viewer) == viewerList.end());    // Check that the item is not in the list yet

	viewerList.push_back(viewer);

	if (viewerList.size() == 1)
	{
		// The first viewer was registered. We assume that the 3D tab is active
		ViewerToolBar::instance().setupUIControls3D();
	}
}

void Model::detachViewer(Viewer *viewer)
{
	std::list<Viewer *>::iterator v = std::find(viewerList.begin(), viewerList.end(), viewer);
	assert(v != viewerList.end());    // Ensure that the item was found in the list

	viewerList.erase(v);

	if (viewerList.size() == 0)
	{
		// The last viewer was deregistered
		ViewerToolBar::instance().removeUIControls();
	}
}

void Model::deactivateModel(void)
{
	// Clear the tree entries
	if (FrontendAPI::instance() != nullptr)
	{
		FrontendAPI::instance()->clearTree();
	}

	if (isActive)
	{
		isActive = false;
	}
}

void Model::activateModel(void)
{
	if (!isActive)
	{
		isActive = true;
		fillTree();

		std::list<ot::UID> selectedTreeItemID;
		getSelectedTreeItemIDs(selectedTreeItemID);

		ViewerToolBar::instance().updateViewEnabledState(selectedTreeItemID);
	}
}

void Model::fillPropertyGrid(const ot::PropertyGridCfg& _configuration)
{
	FrontendAPI::instance()->fillPropertyGrid(_configuration);
}

void Model::clearModalPropertyGrid() {
	FrontendAPI::instance()->clearModalPropertyGrid();
}

void Model::setDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName, double value)
{
	FrontendAPI::instance()->setDoublePropertyValue(_groupName, _itemName, value);
}

double Model::getDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName)
{
	return FrontendAPI::instance()->getDoublePropertyValue(_groupName, _itemName);
}

bool Model::propertyGridValueChanged(const ot::Property* _property)
{
	if (currentManipulator != nullptr)
	{
		return currentManipulator->propertyGridValueChanged(_property);
	}

	return false;
}

void Model::lockSelectionAndModification(bool flag)
{
	FrontendAPI::instance()->lockSelectionAndModification(flag);
}

void Model::addSceneNodesToTree(SceneNodeBase* _root)
{
	// here we add the current item and all its children to the tree. 
	// We assume that all parent items already exist.

	if (isActive)
	{
		assert(FrontendAPI::instance() != nullptr);
		if (FrontendAPI::instance() != nullptr)
		{
			if (_root != sceneNodesRoot)  // The toplevel scene node is invisible and therefore not added to the tree.
			{
				ot::UID treeItemID = FrontendAPI::instance()->addTreeItem(_root->getTreeItem());
				FrontendAPI::instance()->setTreeItemIcon(treeItemID, _root->isVisible() ? _root->getTreeItem().getIcons().getVisibleIcon() : _root->getTreeItem().getIcons().getHiddenIcon());

				_root->setTreeItemID(treeItemID);

				// Now add the node to the map fro faster access
				treeItemToSceneNodesMap[treeItemID] = _root;

				// Now we check whether the current item is visible
				manageParentVisibility(_root);

				//if (root->isVisible())
				//{
				//	// We need to set the parent status to visible
				//	if (root->getParent() != nullptr)
				//	{
				//		setItemVisibleState(root->getParent(), true);
				//	}
				//}
			}

			if (_root->getParent() != nullptr)
			{
				if (_root->getParent()->isSelected())
				{
					FrontendAPI::instance()->selectTreeItem(_root->getTreeItemID());
				}
			}

			// Finally process all children of the current item
			for (auto child : _root->getChildren())
			{
				addSceneNodesToTree(child);
			}
		}
	}
}

void Model::fillTree(void)
{
	assert(FrontendAPI::instance() != nullptr);
	if (FrontendAPI::instance() == nullptr) return;

	FrontendAPI::instance()->clearTree();

	addSceneNodesToTree(sceneNodesRoot);
}

void Model::resetAllViews3D(void)
{
	for (auto viewer : viewerList)
	{
		viewer->reset();
		viewer->refresh();
	}
}

void Model::refreshAllViews(void)
{
	for (auto viewer : viewerList)
	{
		viewer->refresh();
	}
}

void Model::setCursorText(const std::string &text)
{
	for (auto viewer : viewerList)
	{
		viewer->setCursorText(text);
	}
}

void Model::clearSelection(void)
{
	selectSceneNode(nullptr, false);
}

void Model::refreshSelection(void)
{
	std::list<unsigned long long> selectedEntities;
	getSelectedModelEntityIDs(selectedEntities);

	for (auto viewer : viewerList)
	{
		viewer->freeze3DView(true);
	}

	clearSelection();
	for (auto item : selectedEntities)
	{
		SceneNodeBase *entity = modelItemToSceneNodesMap[item];
		assert(entity != nullptr);

		selectSceneNode(entity, true);
	}

	for (auto viewer : viewerList)
	{
		viewer->freeze3DView(false);
	}
}

void Model::selectObject(unsigned long long modelEntityID)
{
	SceneNodeBase *entity = nullptr;

	if (modelItemToSceneNodesMap.count(modelEntityID) != 0)
	{
		entity = modelItemToSceneNodesMap[modelEntityID];
	}

	selectSceneNode(entity, false);
}

void Model::setTreeStateRecording(bool flag)
{
	if (flag)
	{
		treeStateRecording = true;
		treeInfoMap.clear();
	}
	else
	{
		treeStateRecording = false;

		for (auto item : treeInfoMap)
		{
			if (item.second & ITEM_SELECTED)
			{
				if (m_nameToSceneNodesMap.count(item.first) != 0)
				{
					SceneNodeBase *sceneItem = m_nameToSceneNodesMap[item.first];
					assert(sceneItem != nullptr);

					selectSceneNode(sceneItem, true);
				}
			}

			if (item.second & ITEM_EXPANDED)
			{
				if (m_nameToSceneNodesMap.count(item.first) != 0)
				{
					SceneNodeBase *sceneItem = m_nameToSceneNodesMap[item.first];
					assert(sceneItem != nullptr);

					FrontendAPI::instance()->expandSingleTreeItem(sceneItem->getTreeItemID());
				}
			}
		}
	}
}

void Model::centerMouseCursor(void)
{
	for (auto viewer : viewerList)
	{
		QPoint centerPos(viewer->mapToGlobal(viewer->contentsRect().center()));
		QCursor::setPos(centerPos);
	}
}

void Model::addVisualizationContainerNode(const ot::EntityTreeItem& _treeItem, const ot::VisualisationTypes& _visualisationTypes)
{
	// Check whether we already have a container node
	if (m_nameToSceneNodesMap.count(_treeItem.getEntityName()) != 0)
	{
		if (dynamic_cast<SceneNodeContainer*>(m_nameToSceneNodesMap[_treeItem.getEntityName()]) != nullptr) {
			return;
		}
		assert(0); // This is not a container node -> overwrite
	}

	// Create the new container node

	SceneNodeBase *containerNode = new SceneNodeContainer;

	containerNode->setTreeItem(_treeItem);
	
	VisualiserHelper::addVisualizer(containerNode, _visualisationTypes);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());
	assert(parentNode != nullptr); // We assume that the parent node already exists

	if (parentNode == nullptr)
	{
		// If the model is corrupt, this might happen. We deal with this by ignoring the current item
		delete containerNode;
		return;
	}

	// Now add the current node as child to the parent
	parentNode->addChild(containerNode);

	// Now add the current nodes osg node to the parent's osg node
	parentNode->getShapeNode()->addChild(containerNode->getShapeNode());

	// Add the tree name to the tree
	addSceneNodesToTree(containerNode);

	// Add the node to the maps for faster access
	storeInMaps(containerNode);
}

void Model::addVisualizationAnnotationNode(const ot::EntityTreeItem& _treeItem,
	bool _isHidden, const double _edgeColorRGB[3],
	const std::vector<std::array<double, 3>>& _points,
	const std::vector<std::array<double, 3>>& _pointsRgb,
	const std::vector<std::array<double, 3>>& _triangleP1,
	const std::vector<std::array<double, 3>>& _triangleP2,
	const std::vector<std::array<double, 3>>& _triangleP3,
	const std::vector<std::array<double, 3>>& _triangleRgb)
{
	SceneNodeAnnotation *annotationNode = nullptr;
	bool nodeAlreadyExists = false;

	// Check whether we already have a container node
	if (m_nameToSceneNodesMap.count(_treeItem.getEntityName()) != 0)
	{
		annotationNode = dynamic_cast<SceneNodeAnnotation *>(m_nameToSceneNodesMap[_treeItem.getEntityName()]);
		if (annotationNode == nullptr)
		{
			assert(0); // There already exists a node with this name, but it is not an annotation node
			return;
		}

		nodeAlreadyExists = true;
	}
	else
	{
		annotationNode = new SceneNodeAnnotation;
	}

	annotationNode->setTreeItem(_treeItem);
	annotationNode->setWireframe(wireFrameState);

	annotationNode->initializeFromData(_edgeColorRGB, _points, _pointsRgb, _triangleP1, _triangleP2, _triangleP3, _triangleRgb);

	if (!nodeAlreadyExists)
	{
		// Get the parent scene node
		SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());
		assert(parentNode != nullptr); // We assume that the parent node already exists

		// Now add the current node as child to the parent
		parentNode->addChild(annotationNode);

		// Now add the current nodes osg node to the parent's osg node
		parentNode->getShapeNode()->addChild(annotationNode->getShapeNode());

		// Add the tree name to the tree
		addSceneNodesToTree(annotationNode);
	}

	// Make the node invisible, if needed
	if (_isHidden)
	{
		setItemVisibleState(annotationNode, false);
	}

	// Add the node to the maps for faster access
	storeInMaps(annotationNode);
}

void Model::addVisualizationAnnotationNodeDataBase(const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion)
{
	double edgeColorRGB[3] = { 0., 0., 0. };

	std::vector<std::array<double, 3>> points;
	std::vector<std::array<double, 3>> points_rgb;

	std::vector<std::array<double, 3>> triangle_p1;
	std::vector<std::array<double, 3>> triangle_p2;
	std::vector<std::array<double, 3>> triangle_p3;
	std::vector<std::array<double, 3>> triangle_rgb;

	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(_dataEntityID, _dataEntityVersion, doc))
	{
		assert(0);
		return;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityAnnotationData")
	{
		assert(0);
		return;
	}

	int schemaVersion = (int)DataBase::getIntFromView(doc_view, "SchemaVersion_EntityAnnotationData");
	if (schemaVersion != 1)
	{
		assert(0);
		return;
	}

	// Now read the data

	auto color = doc_view["EdgesColor"].get_array().value;

	auto col = color.begin();
	edgeColorRGB[0] = col->get_double(); col++;
	edgeColorRGB[1] = col->get_double(); col++;
	edgeColorRGB[2] = col->get_double(); col++;

	auto pointX = doc_view["PointX"].get_array().value;
	auto pointY = doc_view["PointY"].get_array().value;
	auto pointZ = doc_view["PointZ"].get_array().value;

	auto px = pointX.begin();
	auto py = pointY.begin();
	auto pz = pointZ.begin();

	size_t numberPoints = std::distance(pointX.begin(), pointX.end());
	assert(numberPoints == std::distance(pointY.begin(), pointY.end()));
	assert(numberPoints == std::distance(pointZ.begin(), pointZ.end()));

	points.resize(numberPoints);

	for (unsigned long index = 0; index < numberPoints; index++)
	{
		points[index][0] = px->get_double();
		points[index][1] = py->get_double();
		points[index][2] = pz->get_double();

		px++;
		py++;
		pz++;
	}

	auto pointR = doc_view["PointR"].get_array().value;
	auto pointG = doc_view["PointG"].get_array().value;
	auto pointB = doc_view["PointB"].get_array().value;

	auto pr = pointR.begin();
	auto pg = pointG.begin();
	auto pb = pointB.begin();

	assert(numberPoints == std::distance(pointR.begin(), pointR.end()));
	assert(numberPoints == std::distance(pointG.begin(), pointG.end()));
	assert(numberPoints == std::distance(pointB.begin(), pointB.end()));

	points_rgb.resize(numberPoints);

	for (unsigned long index = 0; index < numberPoints; index++)
	{
		points_rgb[index][0] = pr->get_double();
		points_rgb[index][1] = pg->get_double();
		points_rgb[index][2] = pb->get_double();

		pr++;
		pg++;
		pb++;
	}

	auto triangle1X = doc_view["Triangle1X"].get_array().value;
	auto triangle1Y = doc_view["Triangle1Y"].get_array().value;
	auto triangle1Z = doc_view["Triangle1Z"].get_array().value;

	auto t1x = triangle1X.begin();
	auto t1y = triangle1Y.begin();
	auto t1z = triangle1Z.begin();

	size_t numberTriangles = std::distance(triangle1X.begin(), triangle1X.end());
	assert(numberTriangles == std::distance(triangle1Y.begin(), triangle1Y.end()));
	assert(numberTriangles == std::distance(triangle1Z.begin(), triangle1Z.end()));

	triangle_p1.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_p1[index][0] = t1x->get_double();
		triangle_p1[index][1] = t1y->get_double();
		triangle_p1[index][2] = t1z->get_double();

		t1x++;
		t1y++;
		t1z++;
	}

	auto triangle2X = doc_view["Triangle2X"].get_array().value;
	auto triangle2Y = doc_view["Triangle2Y"].get_array().value;
	auto triangle2Z = doc_view["Triangle2Z"].get_array().value;

	auto t2x = triangle2X.begin();
	auto t2y = triangle2Y.begin();
	auto t2z = triangle2Z.begin();

	assert(numberTriangles == std::distance(triangle2X.begin(), triangle2X.end()));
	assert(numberTriangles == std::distance(triangle2Y.begin(), triangle2Y.end()));
	assert(numberTriangles == std::distance(triangle2Z.begin(), triangle2Z.end()));

	triangle_p2.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_p2[index][0] = t2x->get_double();
		triangle_p2[index][1] = t2y->get_double();
		triangle_p2[index][2] = t2z->get_double();

		t2x++;
		t2y++;
		t2z++;
	}

	auto triangle3X = doc_view["Triangle3X"].get_array().value;
	auto triangle3Y = doc_view["Triangle3Y"].get_array().value;
	auto triangle3Z = doc_view["Triangle3Z"].get_array().value;

	auto t3x = triangle3X.begin();
	auto t3y = triangle3Y.begin();
	auto t3z = triangle3Z.begin();

	assert(numberTriangles == std::distance(triangle3X.begin(), triangle3X.end()));
	assert(numberTriangles == std::distance(triangle3Y.begin(), triangle3Y.end()));
	assert(numberTriangles == std::distance(triangle3Z.begin(), triangle3Z.end()));

	triangle_p3.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_p3[index][0] = t3x->get_double();
		triangle_p3[index][1] = t3y->get_double();
		triangle_p3[index][2] = t3z->get_double();

		t3x++;
		t3y++;
		t3z++;
	}

	auto triangleR = doc_view["TriangleR"].get_array().value;
	auto triangleG = doc_view["TriangleG"].get_array().value;
	auto triangleB = doc_view["TriangleB"].get_array().value;

	auto tr = triangleR.begin();
	auto tg = triangleG.begin();
	auto tb = triangleB.begin();

	assert(numberTriangles == std::distance(triangleR.begin(), triangleR.end()));
	assert(numberTriangles == std::distance(triangleG.begin(), triangleG.end()));
	assert(numberTriangles == std::distance(triangleB.begin(), triangleB.end()));

	triangle_rgb.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_rgb[index][0] = tr->get_double();
		triangle_rgb[index][1] = tg->get_double();
		triangle_rgb[index][2] = tb->get_double();

		tr++;
		tg++;
		tb++;
	}

	// Finally create the entry

	addVisualizationAnnotationNode(_treeItem, _isHidden, edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
}


void Model::addNodeFromFacetDataBase(const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], const std::string& _materialType, const std::string& _textureType, bool _reflective, bool _backFaceCulling,
	double _offsetFactor, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion,
	bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected, std::vector<double>& _transformation)
{
	SceneNodeGeometry *geometryNode = createNewGeometryNode(_treeItem, _isHidden, _manageParentVisibility, _manageChildVisibility);

	if (geometryNode != nullptr)
	{
		geometryNode->setSurfaceColorRGB(_surfaceColorRGB);
		geometryNode->setEdgeColorRGB(_edgeColorRGB);
		geometryNode->setMaterialType(_materialType);
		geometryNode->setTextureType(_textureType, _reflective);
		geometryNode->setBackFaceCulling(_backFaceCulling);
		geometryNode->setOffsetFactor(_offsetFactor);
		geometryNode->setStorage(_projectName, _dataEntityID, _dataEntityVersion);
		geometryNode->setWireframe(wireFrameState);
		geometryNode->setShowWhenSelected(_showWhenSelected);
		geometryNode->setTransformation(_transformation);

		//if (isHidden)
		//{
		//	geometryNode->setNeedsInitialization();
		//}
		//else
		//{
		//	geometryNode->initializeFromDataStorage();
		//}
		geometryNode->initializeFromDataStorage(); // We want to have the hover preview function for hidden items as well

		if (_isHidden)
		{
			setItemVisibleState(geometryNode, false);
		}
	}
}

void Model::addSceneNode(const ot::EntityTreeItem& _treeItem, ot::VisualisationTypes _visualisationTypes) 
{
	// Check whether we already have a node with this name
	auto existingSceneNode = m_nameToSceneNodesMap.find(_treeItem.getEntityName());
	if (existingSceneNode != m_nameToSceneNodesMap.end())
	{
		if (dynamic_cast<SceneNodeMultiVisualisation*>(existingSceneNode->second) != nullptr)
		{
			return;
		}
		assert(0); // This is not a container node -> overwrite
	}

	// Create the new container node
	SceneNodeBase* sceneNode = new SceneNodeMultiVisualisation();
	sceneNode->setTreeItem(_treeItem);
	
	VisualiserHelper::addVisualizer(sceneNode, _visualisationTypes);

	// Get the parent scene node
	SceneNodeBase* parentNode = getParentNode(_treeItem.getEntityName());
	
	if (parentNode == nullptr)
	{
		OTAssert(0, "Corrupt model: parent node not found");

		// If the model is corrupt, this might happen. We deal with this by ignoring the current item
		delete sceneNode;
		return;
	}

	// Now add the current node as child to the parent
	parentNode->addChild(sceneNode);

	// Now add the current nodes osg node to the parent's osg node
	parentNode->getShapeNode()->addChild(sceneNode->getShapeNode());

	// Add the tree name to the tree
	addSceneNodesToTree(sceneNode);

	// Add the node to the maps for faster access
	storeInMaps(sceneNode);

}

SceneNodeGeometry *Model::createNewGeometryNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, bool _manageParentVisibility, bool _manageChildVisibility)
{
	// Check whether the item already exists
	SceneNodeBase *item = modelItemToSceneNodesMap[_treeItem.getEntityID()];

	SceneNodeGeometry *geometryNode = nullptr;

	if (item != nullptr)
	{
		// The item exists and needs to be updated

		geometryNode = dynamic_cast<SceneNodeGeometry *>(item);

		if (geometryNode == nullptr)
		{
			OTAssertNullptr("The existing item is not a geometry node");
			return nullptr;
		}
	}
	else
	{
		// The item is new and needs to be created

		geometryNode = new SceneNodeGeometry;
		geometryNode->setTreeItem(_treeItem);
		geometryNode->setWireframe(wireFrameState);
		geometryNode->setVisible(!_isHidden);
		geometryNode->setManageVisibilityOfParent(_manageParentVisibility);
		geometryNode->setManageVisibilityOfChildren(_manageChildVisibility);

		// Get the parent scene node
		SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());

		// Deal with potentially corrupt models
		if (parentNode == nullptr)
		{
			OTAssert(0, "Corrupt model: parent node not found");

			delete geometryNode;
			return nullptr;
		}

		// Now add the current node as child to the parent
		parentNode->addChild(geometryNode);

		// Add the tree name to the tree
		addSceneNodesToTree(geometryNode);

		// Make the node invisible, if needed
		if (_isHidden)
		{
			setItemVisibleState(geometryNode, false);
		}

		geometryNode->setModel(this);

		// Add the node to the maps for faster access
		storeInMaps(geometryNode);
	}

	return geometryNode;
}

void Model::setEntityName(ot::UID _modelEntityID, const std::string& _newName) {
	SceneNodeBase* item = modelItemToSceneNodesMap[_modelEntityID];
	if (item == nullptr) {
		OT_LOG_W("SceneNode not found { \"EntityID\": " + std::to_string(_modelEntityID) + " }");
		return;
	}

	std::string newText;
	size_t index = _newName.rfind("/");
	if (index == std::string::npos) {
		newText = _newName;
	}
	else {
		newText = _newName.substr(index + 1);
	}

	m_nameToSceneNodesMap[_newName] = item;
	m_nameToSceneNodesMap.erase(item->getName());

	item->setName(_newName);

	FrontendAPI::instance()->setTreeItemText(item->getTreeItemID(), newText);
}

std::string Model::getEntityName(unsigned long long modelEntityID) const {
	const auto it = modelItemToSceneNodesMap.find(modelEntityID);
	if (it != modelItemToSceneNodesMap.end()) {
		return it->second->getName();
	}
	else {
		return "";
	}
}

ot::UID Model::getEntityID(const std::string& _entityName) const {
	const auto it = m_nameToSceneNodesMap.find(_entityName);
	if (it != m_nameToSceneNodesMap.end()) {
		return it->second->getModelEntityID();
	}
	else {
		return ot::invalidUID;
	}
}

void Model::renameEntityPath(const std::string &oldPath, const std::string &newPath)
{
	std::map<std::string, SceneNodeBase *> entityMap = m_nameToSceneNodesMap;
	std::string filter = oldPath + "/";

	for (auto item : entityMap)
	{
		std::string name = item.first;

		if (name.size() == oldPath.size())
		{
			if (name == oldPath)
			{
				// This is the item which needs to be renamed (item + tree item)

				item.second->setName(newPath);
				m_nameToSceneNodesMap[newPath] = item.second;
				m_nameToSceneNodesMap.erase(name);

				std::string newText;

				size_t index = newPath.rfind("/");
				if (index == std::string::npos)
				{
					newText = newPath;
				}
				else
				{
					newText = newPath.substr(index + 1);
				}

				FrontendAPI::instance()->setTreeItemText(item.second->getTreeItemID(), newText);
			}
		}
		else if (name.size() > oldPath.size())
		{
			if (name.substr(0, filter.size()) == filter)
			{
				std::string newName = newPath + name.substr(filter.size() - 1);

				// Here we just need to rename the item itself and don't need to update the tree
				item.second->setName(newName);
				m_nameToSceneNodesMap[newName] = item.second;
				m_nameToSceneNodesMap.erase(name);
			}
		}
	}
}

void Model::addNodeFromFacetData(const ot::EntityTreeItem& _treeItem, bool _isHidden, double _surfaceColorRGB[3], double _edgeColorRGB[3], bool _backFaceCulling,
	double _offsetFactor, std::vector<Geometry::Node>& _nodes, std::list<Geometry::Triangle>& _triangles, std::list<Geometry::Edge>& _edges, std::map<ot::UID, std::string>& _faceNameMap, std::string& _errors,
	bool _manageParentVisibility, bool _manageChildVisibility, bool _showWhenSelected)
{
	SceneNodeGeometry *geometryNode = createNewGeometryNode(_treeItem, _isHidden, _manageParentVisibility, _manageChildVisibility);

	if (geometryNode != nullptr)
	{
		geometryNode->setErrors(_errors);

		geometryNode->setSurfaceColorRGB(_surfaceColorRGB);
		geometryNode->setEdgeColorRGB(_edgeColorRGB);
		geometryNode->setBackFaceCulling(_backFaceCulling);
		geometryNode->setOffsetFactor(_offsetFactor);
		geometryNode->setWireframe(wireFrameState);
		geometryNode->setShowWhenSelected(_showWhenSelected);

		geometryNode->initializeFromFacetData(_nodes, _triangles, _edges, _faceNameMap);
	}
}

void Model::updateObjectColor(unsigned long long modelEntityID, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, const std::string &textureType, bool reflective)
{
	// First get the node
	SceneNodeBase *item = modelItemToSceneNodesMap[modelEntityID];
	assert(item != nullptr);
	if (item == nullptr) return;

	// Now check whether it is a geometry item
	SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry*>(item);
	assert(geometryItem != nullptr);
	if (geometryItem == nullptr) return;

	geometryItem->setSurfaceColorRGB(surfaceColorRGB);
	geometryItem->setEdgeColorRGB(edgeColorRGB);
	geometryItem->setMaterialType(materialType);
	geometryItem->setTextureType(textureType, reflective);
	geometryItem->updateObjectColor(surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective);
}

void Model::updateMeshColor(unsigned long long modelEntityID, double colorRGB[3])
{
	// First get the node
	SceneNodeBase* item = modelItemToSceneNodesMap[modelEntityID];
	assert(item != nullptr);
	if (item == nullptr) return;

	// Now check whether it is a geometry item
	SceneNodeMeshItem *meshItem = dynamic_cast<SceneNodeMeshItem*>(item);
	assert(meshItem != nullptr);
	if (meshItem == nullptr) return;

	meshItem->updateObjectColor(colorRGB[0], colorRGB[1], colorRGB[2]);
}

void Model::updateObjectFacetsFromDataBase(unsigned long long modelEntityID, unsigned long long entityID, unsigned long long entityVersion)
{
	// First get the node
	SceneNodeBase *item = modelItemToSceneNodesMap[modelEntityID];
	assert(item != nullptr);
	if (item == nullptr) return;

	// Now check whether it is a geometry item
	SceneNodeGeometry *geometryItem = dynamic_cast<SceneNodeGeometry*>(item);
	assert(geometryItem != nullptr);
	if (geometryItem == nullptr) return;

	geometryItem->updateObjectFacetsFromDataBase(entityID, entityVersion);
}

SceneNodeBase *Model::getParentNode(const std::string &treeName)
{
	// Fist get the tree name of the parent item from the tree name of the current item

	const char separator = '/';
	std::string::size_type pos;

	if ((pos = treeName.rfind(separator)) == std::string::npos)
	{
		// The item is a top level item. Therefore, the parent is the root node.
		return sceneNodesRoot;
	}

	std::string parentName = treeName.substr(0, pos);

	// Now get the parent item from the map
	SceneNodeBase *parent = m_nameToSceneNodesMap[parentName];
	assert(parent != nullptr);

	return parent;
}

void Model::resetSelection(SceneNodeBase *root)
{
	root->setSelected(false, ot::SelectionData(), isSingleItemSelected(), {});
	root->setSelectionHandled(false);

	for (auto child : root->getChildren())
	{
		resetSelection(child);
	}
}

void Model::setAllShapesOpaque(SceneNodeBase *root)
{
	root->setTransparent(false);

	for (auto child : root->getChildren())
	{
		setAllShapesOpaque(child);
	}
}

void Model::setSelectedShapesOpaqueAndOthersTransparent(SceneNodeBase *root)
{
	root->setTransparent(!root->isSelected());

	for (auto child : root->getChildren())
	{
		setSelectedShapesOpaqueAndOthersTransparent(child);
	}
}

ot::SelectionHandlingResult Model::setSelectedTreeItems(const ot::SelectionData& _selectionData, std::list<unsigned long long>& _selectedModelItems, std::list<unsigned long long>& _selectedVisibleModelItems) {
	ot::SelectionHandlingResult result;

	_selectedModelItems.clear();

	// Set the selection and selection handled flags for all nodes to false
	resetSelection(sceneNodesRoot);

	if (_selectionData.getSelectedTreeItems().empty()) {
		// No shape selected -> Draw all shapes opaque
		setAllShapesOpaque(sceneNodesRoot);

		// Update the working plane transformation 
		updateWorkingPlaneTransform();
		
		ViewerToolBar::instance().updateViewEnabledState(_selectionData.getSelectedTreeItems());
		refreshAllViews();

		// Clear visualizing entities for last central view
		ot::WidgetView* view = FrontendAPI::instance()->getLastFocusedCentralView();
		if (view) {
			ot::WidgetViewBase::ViewType viewType = view->getViewData().getViewType();
			if (viewType == ot::WidgetViewBase::View3D) {
				view->clearVisualizingItems();
			}
		}

		return result;
	}

	singleItemSelected = (_selectionData.getSelectedTreeItems().size() == 1);

	// Now at least one shape is selected
	// -> selected shapes are drawn opaque and all others are drawn transparent

	bool isItem3DSelected = false;

	// First gather information about all selected nodes
	std::list<SceneNodeBase*> selectedNodes;
	
	for (ot::UID item : _selectionData.getSelectedTreeItems()) {
		SceneNodeBase* node = treeItemToSceneNodesMap[item];
		if (node != nullptr) {
			selectedNodes.push_back(node);
		}
	}

	// Now set the selected state for all selected nodes
	
	for (SceneNodeBase* node : selectedNodes) {
		assert(node != nullptr);

		if (node->isItem3D()) {
			if (!isItem3DSelected) {
				FrontendAPI::instance()->clearVisualizingEntitesFromView("3D", ot::WidgetViewBase::View3D);
				isItem3DSelected = true;
			}
			
			FrontendAPI::instance()->addVisualizingEntityToView(node->getTreeItemID(), "3D", ot::WidgetViewBase::View3D);
		}
		
		result |= node->setSelected(true, _selectionData, isSingleItemSelected(), selectedNodes);
		_selectedModelItems.push_back(node->getModelEntityID());

		if (node->isVisible()) {
			_selectedVisibleModelItems.push_back(node->getModelEntityID());
		}
	}

	// Now update the transparent / opaque mode acoording to the selection
	setSelectedShapesOpaqueAndOthersTransparent(sceneNodesRoot);
	

	if (isItem3DSelected) {
		// Ensure that we have the 3D view active
		if (ensure3DView()) {
			result |= ot::SelectionHandlingEvent::ActiveViewChanged;
		}
	}

	// Update the UI state and the view
	ViewerToolBar::instance().updateViewEnabledState(_selectionData.getSelectedTreeItems());
	refreshAllViews();

	// Update the working plane transformation 
	updateWorkingPlaneTransform();

	return result;
}

bool Model::ensure3DView(void) {
	if (FrontendAPI::instance()->getCurrentVisualizationTabTitle() != "Versions") {
		FrontendAPI::instance()->setCurrentVisualizationTabFromTitle("3D");
		return true;
	}
	else {
		return false;
	}
}

void Model::manageParentVisibility(SceneNodeBase *item)
{
	assert(item != nullptr);

	if (item->getManageVisibilityOfParent())
	{
		SceneNodeBase *parent = item->getParent();

		if (parent != nullptr)
		{
			if (item->isVisible())
			{
				// If we become visible, we set the parent visibility to visible as well
				setItemVisibleState(parent, true);
			}
			else
			{
				// If we become invisible, we check whether the parent has any visible items. If not, we set the parent to invisible as well
				bool anyVisible = false;
				for (auto sister : parent->getChildren())
				{
					if (sister->isVisible()) anyVisible = true;
				}

				if (!anyVisible)
				{
					setItemVisibleState(parent, false);
				}
			}
		}
	}
}

void Model::setItemVisibleState(SceneNodeBase* _item, bool _visible)
{
	if (_visible != _item->isVisible())
	{
		_item->setVisible(_visible);
		FrontendAPI::instance()->setTreeItemIcon(_item->getTreeItemID(), _item->isVisible() ? _item->getTreeItem().getIcons().getVisibleIcon() : _item->getTreeItem().getIcons().getHiddenIcon());

		manageParentVisibility(_item);
	}
}

void Model::getPrefetchForAllSceneNodes(SceneNodeBase *root, std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	root->getPrefetch(projectName, prefetchIDs);

	for (auto child : root->getChildren())
	{
		getPrefetchForAllSceneNodes(child, projectName, prefetchIDs);
	}
}

void Model::showAllSceneNodes(SceneNodeBase *root)
{
	setItemVisibleState(root, true);

	if (root->getManageVisibilityOfChildren())
	{
		for (auto child : root->getChildren())
		{
			showAllSceneNodes(child);
		}
	}
}

void Model::hideAllSceneNodes(SceneNodeBase *root)
{
	setItemVisibleState(root, false);

	if (root->getManageVisibilityOfChildren())
	{
		for (auto child : root->getChildren())
		{
			hideAllSceneNodes(child);
		}
	}
}

void Model::getPrefetchForSelectedSceneNodes(SceneNodeBase *root, std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	if (root->isSelected())
	{
		root->getPrefetch(projectName, prefetchIDs);
	}

	for (auto child : root->getChildren())
	{
		getPrefetchForSelectedSceneNodes(child, projectName, prefetchIDs);
	}
}

void Model::showSelectedSceneNodes(SceneNodeBase *root)
{
	if (root->isSelected()) setItemVisibleState(root, true);

	for (auto child : root->getChildren())
	{
		showSelectedSceneNodes(child);
	}
}

void Model::setSceneNodeVisibility(SceneNodeBase *root, std::map<unsigned long long, bool> nodeVisibility)
{
	if (nodeVisibility.count(root->getModelEntityID()) > 0)
	{
		setItemVisibleState(root, nodeVisibility[root->getModelEntityID()]);
	}

	for (auto child : root->getChildren())
	{
		setSceneNodeVisibility(child, nodeVisibility);
	}
}

void Model::hideSelectedSceneNodes(SceneNodeBase *root)
{
	if (root->isSelected()) setItemVisibleState(root, false);

	for (auto child : root->getChildren())
	{
		hideSelectedSceneNodes(child);
	}
}

void Model::hideUnselectedSceneNodes(SceneNodeBase *root)
{
	if (!root->isSelected()) setItemVisibleState(root, false);

	for (auto child : root->getChildren())
	{
		if (child->isSelected())
		{
			// Here we need to set the visible state of all parents to true
			setItemVisibleState(root, true);
		}

		hideUnselectedSceneNodes(child);
	}
}

void Model::hideEntities(std::list<unsigned long long> hiddenID)
{
	for (auto entity : hiddenID)
	{
		SceneNodeBase *node = modelItemToSceneNodesMap[entity];
		hideSceneNodeAndChilds(node);
	}

	refreshAllViews();
}

void Model::showBranch(const std::string &branchName)
{
	if (m_nameToSceneNodesMap.count(branchName) > 0)
	{
		SceneNodeBase *rootNode = m_nameToSceneNodesMap[branchName];

		// Now show the entities of the selected branch
		std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
		std::string projectName;

		getPrefetchForAllSceneNodes(rootNode, projectName, prefetchIDs);

		if (!prefetchIDs.empty())
		{
			DataBase::instance().prefetchDocumentsFromStorage(prefetchIDs);
		}

		showAllSceneNodes(rootNode);
	}

	refreshAllViews();
}

void Model::hideBranch(const std::string &branchName)
{
	if (m_nameToSceneNodesMap.count(branchName) > 0)
	{
		SceneNodeBase *rootNode = m_nameToSceneNodesMap[branchName];

		hideAllSceneNodes(rootNode);
	}

	refreshAllViews();
}

void Model::hideSceneNodeAndChilds(SceneNodeBase *node)
{
	setItemVisibleState(node, false);

	for (auto child : node->getChildren())
	{
		hideSceneNodeAndChilds(child);
	}
}

void Model::setWireframeStateForAllSceneNodes(SceneNodeBase *root, bool wireframe)
{
	root->setWireframe(wireframe);

	for (auto child : root->getChildren())
	{
		setWireframeStateForAllSceneNodes(child, wireframe);
	}
}

void Model::toggleWireframeView(void)
{
	wireFrameState = !wireFrameState;

	setWireframeStateForAllSceneNodes(sceneNodesRoot, wireFrameState);
}

void Model::toggleWorkingPlane(void) 
{
	for (auto v : viewerList) 
	{
		v->toggleWorkingPlane();
	}
}

bool Model::compareTransformations(osg::Matrix &matrix1, osg::Matrix &matrix2)
{
	for (int i = 0; i < 16; i++)
	{
		if (fabs(matrix1.ptr()[i] - matrix2.ptr()[i]) > 1e-6)
		{
			return false; // The transformations are not the same
		}
	}

	return true;  // The transformations are the same (within the selected tolerance)
}

bool Model::getTransformationOfSelectedShapes(SceneNodeBase *root, bool &first, osg::Matrix &matrix)
{
	if (root->isSelected())
	{
		SceneNodeGeometry *geometryNode = dynamic_cast<SceneNodeGeometry *>(root);

		if (geometryNode != nullptr)
		{
			if (first)
			{
				matrix = geometryNode->getTransformation();
				first = false;
			}
			else
			{
				osg::Matrix localMatrix = geometryNode->getTransformation();

				if (!compareTransformations(matrix, localMatrix))
				{
					return false;
				}
			}
		}
	}

	for (auto child : root->getChildren())
	{
		if (!getTransformationOfSelectedShapes(child, first, matrix))
		{
			return false;
		}
	}

	return true;
}

void Model::updateWorkingPlaneTransform(void) 
{
	osg::Matrix matrix;
	bool first = true;

	if (!getTransformationOfSelectedShapes(sceneNodesRoot, first, matrix))
	{
		// The transformations are not the same for all selected scene nodes -> we use as identity transform
		matrix = osg::Matrix::identity();
	}

	matrix.transpose(matrix); // We need to transpose the matrix, since the working plane transform needs to be transposed to match the shape transform

	for (auto v : viewerList) {
		v->setWorkingPlaneTransform(matrix);
	}

	currentWorkingplaneTransform = matrix;


	matrix.transpose(matrix);
	currentWorkingplaneTransformTransposedInverse = currentWorkingplaneTransformTransposedInverse.inverse(matrix);
}

void Model::toggleAxisCross(void) {
	for (auto v : viewerList) {
		v->toggleAxisCross();
	}
}

void Model::toggleCenterAxisCross(void) {
	for (auto v : viewerList) {
		v->toggleCenterAxisCross();
	}
}

void Model::toggleCutplane(void) {
	for (auto v : viewerList) {
		v->toggleCutplane();
	}
}

void Model::saveTextEditor(void) {
	if (FrontendAPI::instance()) {
		FrontendAPI::instance()->requestSaveForCurrentVisualizationTab();
	}
}

void Model::exportTextEditor(void) {
	FrontendAPI* api = FrontendAPI::instance();
	
	if (!api) {
		return;
	}

	ot::TextEditorView* view = dynamic_cast<ot::TextEditorView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	ot::TextEditor* edit = view->getTextEditor();

	if (view->getViewContentModified()) {
		edit->slotSaveRequested();

		if (view->getViewContentModified()) {
			OT_LOG_E("Could not export text editor content because saving failed");
			return;
		}
	}
	
	std::string filePath = api->getSaveFileName(
		"Export To File", 
		"", 
		edit->getFileExtensionFilter()
	);

	if (!filePath.empty()) {
		if (edit->hasMoreToLoad()) {
			FrontendAPI::instance()->lockSelectionAndModification(true);
			FrontendAPI::instance()->setProgressBarVisibility("Export text", true, true);
			std::thread exportThread(&Model::exportTextWorker, this, filePath, view->getViewData().getEntityName());
			exportThread.detach();
		}
		else {
			std::ofstream outFile(filePath);
			if (outFile.is_open()) {
				outFile << edit->toPlainText().toStdString();
				outFile.close();
				FrontendAPI::instance()->displayText("File exported successfully: " + filePath + "\n");
			}
			else {
				OT_LOG_E("Could not open file for writing: " + filePath);
			}
		}
	}
}

void Model::saveTable(void) {
	if (FrontendAPI::instance()) {
		FrontendAPI::instance()->requestSaveForCurrentVisualizationTab();
	}
}

void Model::exportTableAsCSV(void) {
	FrontendAPI* api = FrontendAPI::instance();

	if (!api) {
		return;
	}

	ot::TableView* view = dynamic_cast<ot::TableView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	ot::Table* table = view->getTable();

	if (view->getViewContentModified()) {
		table->slotSaveRequested();
	}

	std::string filePath = api->getSaveFileName("Export To File", "", ot::FileExtension::toFilterString({ ot::FileExtension::CSV, ot::FileExtension::AllFiles }));
	if (!filePath.empty()) {
		// todo: export...
	}
}

void Model::addTableRowBefore(void) {
	FrontendAPI* api = FrontendAPI::instance();

	if (!api) {
		return;
	}

	ot::TableView* view = dynamic_cast<ot::TableView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	QRect selectionRect = view->getTable()->getSelectionBoundingRect();
	int ix = 0;
	if (selectionRect.isValid()) {
		ix = selectionRect.top();
	}

	bool isHeader = false;
	if (ix < view->getTable()->rowCount()) {
		isHeader = (view->getTable()->verticalHeaderItem(ix) != nullptr);
	}

	view->getTable()->insertRow(ix);

	if (isHeader) {
		view->getTable()->setVerticalHeaderItem(ix, new QTableWidgetItem);
	}

	view->getTable()->setContentChanged(true);
}

void Model::addTableRowAfter(void) {
	FrontendAPI* api = FrontendAPI::instance();

	if (!api) {
		return;
	}

	ot::TableView* view = dynamic_cast<ot::TableView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	QRect selectionRect = view->getTable()->getSelectionBoundingRect();
	int ix = view->getTable()->rowCount();
	if (selectionRect.isValid()) {
		ix = selectionRect.bottom() + 1;
	}

	bool isHeader = false;
	if (ix <= view->getTable()->rowCount() && ix > 0) {
		isHeader = (view->getTable()->verticalHeaderItem(ix - 1) != nullptr);
	}

	view->getTable()->insertRow(ix);

	if (isHeader) {
		view->getTable()->setVerticalHeaderItem(ix, new QTableWidgetItem);
	}

	view->getTable()->setContentChanged(true);
}

void Model::removeTableRow(void) {
	FrontendAPI* api = FrontendAPI::instance();

	if (!api) {
		return;
	}

	ot::TableView* view = dynamic_cast<ot::TableView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	QRect selectionRect = view->getTable()->getSelectionBoundingRect();
	if (selectionRect.isValid()) {
		int ix = selectionRect.top();
		for (int i = ix; i <= selectionRect.bottom(); i++) {
			view->getTable()->removeRow(ix);
		}
		view->getTable()->setContentChanged(true);
	}
}

void Model::addTableColumnBefore(void) {
	FrontendAPI* api = FrontendAPI::instance();

	if (!api) {
		return;
	}

	ot::TableView* view = dynamic_cast<ot::TableView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	QRect selectionRect = view->getTable()->getSelectionBoundingRect();
	int ix = 0;
	if (selectionRect.isValid()) {
		ix = selectionRect.left();
	}

	bool isHeader = false;
	if (ix < view->getTable()->columnCount()) {
		isHeader = (view->getTable()->horizontalHeaderItem(ix) != nullptr);
	}

	view->getTable()->insertColumn(ix);

	if (isHeader) {
		view->getTable()->setHorizontalHeaderItem(ix, new QTableWidgetItem);
	}

	view->getTable()->setContentChanged(true);
}

void Model::addTableColumnAfter(void) {
	FrontendAPI* api = FrontendAPI::instance();

	if (!api) {
		return;
	}

	ot::TableView* view = dynamic_cast<ot::TableView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	QRect selectionRect = view->getTable()->getSelectionBoundingRect();
	int ix = view->getTable()->columnCount();
	if (selectionRect.isValid()) {
		ix = selectionRect.bottom() + 1;
	}

	bool isHeader = false;
	if (ix <= view->getTable()->columnCount() && ix > 0) {
		isHeader = (view->getTable()->horizontalHeaderItem(ix - 1) != nullptr);
	}

	view->getTable()->insertColumn(ix);

	if (isHeader) {
		view->getTable()->setHorizontalHeaderItem(ix, new QTableWidgetItem);
	}
	view->getTable()->setContentChanged(true);
}

void Model::removeTableColumn(void) {
	FrontendAPI* api = FrontendAPI::instance();

	if (!api) {
		return;
	}

	ot::TableView* view = dynamic_cast<ot::TableView*>(api->getCurrentView());

	if (!view) {
		OT_LOG_E("Current view is not a table");
		return;
	}

	QRect selectionRect = view->getTable()->getSelectionBoundingRect();
	if (selectionRect.isValid()) {
		int ix = selectionRect.left();
		for (int i = ix; i <= selectionRect.right(); i++) {
			view->getTable()->removeColumn(ix);
		}
		view->getTable()->setContentChanged(true);
	}
}

void Model::viewerTabChangedToCentral(const ot::WidgetViewBase& _viewInfo) {
	// If we switch Main->Navigation->Main we don't want the tool bar to change
	if (_viewInfo.getTitle() == m_currentCentralView.getTitle() && _viewInfo.getViewType() == m_currentCentralView.getViewType()) {
		return;
	}

	ViewerToolBar::instance().removeUIControls();

	m_hasModalMenu = false;

	switch (_viewInfo.getViewType()) {
	case ot::WidgetViewBase::View3D:
		ViewerToolBar::instance().setupUIControls3D();
		break;

	case ot::WidgetViewBase::ViewText:
		m_hasModalMenu = true;
		m_previousMenu = FrontendAPI::instance()->getCurrentMenuPage();

		ViewerToolBar::instance().setupUIControlsText();

		m_currentMenu = FrontendAPI::instance()->getCurrentMenuPage();
		break;

	case ot::WidgetViewBase::ViewTable:
		m_hasModalMenu = true;
		m_previousMenu = FrontendAPI::instance()->getCurrentMenuPage();

		ViewerToolBar::instance().setupUIControlsTable();

		m_currentMenu = FrontendAPI::instance()->getCurrentMenuPage();
		break;

	default:
		break;
	}

	m_currentCentralView = _viewInfo;
}

void Model::executeAction(ot::UID _buttonID) {
	ViewerToolBar::ButtonType button = ViewerToolBar::instance().getButtonTypeFromUID(_buttonID);
	switch (button) {
	case ViewerToolBar::Reset3DViewButton: resetAllViews3D(); break;
	case ViewerToolBar::ShowAllButton: showAllSceneNodesAction(); break;
	case ViewerToolBar::ShowSelectedButton: showSelectedSceneNodesAction(); break;
	case ViewerToolBar::HideSelectedButton: hideSelectedSceneNodesAction(); break;
	case ViewerToolBar::HideUnselectedButton: hideUnselectedSceneNodesAction(); break;
	case ViewerToolBar::WireframeButton: toggleWireframeView(); break;
	case ViewerToolBar::WorkingPlaneButton: toggleWorkingPlane(); break;
	case ViewerToolBar::AxisCrossButton: toggleAxisCross(); break;
	case ViewerToolBar::CenterAxisCrossButton: toggleCenterAxisCross(); break;
	case ViewerToolBar::CutplaneButton: toggleCutplane(); break;
	case ViewerToolBar::TextEditorSaveButton: saveTextEditor(); break;
	case ViewerToolBar::TextEditorExportButton: exportTextEditor(); break;
	case ViewerToolBar::TableSaveButton: saveTable(); break;
	case ViewerToolBar::TableExportCSVButton: exportTableAsCSV(); break;
	case ViewerToolBar::TableAddRowBefore: addTableRowBefore(); break;
	case ViewerToolBar::TableAddRowAfter: addTableRowAfter(); break;
	case ViewerToolBar::TableRemoveRow: removeTableRow(); break;
	case ViewerToolBar::TableAddColumnBefore: addTableColumnBefore(); break;
	case ViewerToolBar::TabbleAddColumnAfter: addTableColumnAfter(); break;
	case ViewerToolBar::TableRemoveColumn: removeTableColumn(); break;
	case ViewerToolBar::NoButton: break;
	default:
		OT_LOG_W("Unknown button (" + std::to_string(_buttonID) + ")");
		break;
	}

	refreshAllViews();
}

void Model::showAllSceneNodesAction(void)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
	std::string projectName;

	getPrefetchForAllSceneNodes(sceneNodesRoot, projectName, prefetchIDs);

	if (!prefetchIDs.empty())
	{
		DataBase::instance().prefetchDocumentsFromStorage(prefetchIDs);
	}

	showAllSceneNodes(sceneNodesRoot);
	if (FrontendAPI::instance() != nullptr) FrontendAPI::instance()->refreshSelection();
}

void Model::showSelectedSceneNodesAction(void)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
	std::string projectName;

	getPrefetchForSelectedSceneNodes(sceneNodesRoot, projectName, prefetchIDs);

	if (!prefetchIDs.empty())
	{
		DataBase::instance().prefetchDocumentsFromStorage(prefetchIDs);
	}

	showSelectedSceneNodes(sceneNodesRoot);
	if (FrontendAPI::instance() != nullptr) FrontendAPI::instance()->refreshSelection();
}

void Model::hideSelectedSceneNodesAction(void)
{
	hideSelectedSceneNodes(sceneNodesRoot);

	if (FrontendAPI::instance() != nullptr) FrontendAPI::instance()->refreshSelection();
}

void Model::hideUnselectedSceneNodesAction(void)
{
	hideUnselectedSceneNodes(sceneNodesRoot);

	if (FrontendAPI::instance() != nullptr) FrontendAPI::instance()->refreshSelection();
}

SceneNodeBase *Model::findSelectedItemByLineSegment(osgUtil::Intersector *intersector, double sceneRadius, osg::Vec3d &intersectionPoint, unsigned long long &hitIndex)
{
	osgUtil::LineSegmentIntersector *lineIntersector = dynamic_cast<osgUtil::LineSegmentIntersector*>(intersector);
	if (lineIntersector == nullptr) return nullptr;

	if (!intersector->containsIntersections()) return nullptr;

	std::list<osg::NodePath> hitItemList;
	std::vector<unsigned long long> hitIndexList;

	const osgUtil::LineSegmentIntersector::Intersection &firstHit = *(lineIntersector->getIntersections().begin());
	intersectionPoint = firstHit.getWorldIntersectPoint();

	double tolerance = 1e-6 * sceneRadius;

	for (auto i : lineIntersector->getIntersections())
	{
		osg::Vec3d point = i.getWorldIntersectPoint();
		osg::Vec3d delta = intersectionPoint - point;

		if (delta.length() < tolerance)
		{
			hitItemList.push_back(i.nodePath);
			hitIndexList.push_back(i.primitiveIndex);
		}
	}

	// If we have multiple items at the same position, we need to find the one with the lowest offset value
	// In a first step, we search for the first switch item in the node path of each hit

	double minOffset = 1e10;
	SceneNodeBase *closestItem = nullptr;

	unsigned long long index = 0;
	for (auto hitItem : hitItemList)
	{
		int nNodes = hitItem.size();

		if (nNodes > 0)
		{
			for (int i = nNodes - 1; i >= 0; i--)
			{
				osg::Node *node = hitItem[i];
				if (osgNodetoSceneNodesMap.count(node) > 0)
				{
					SceneNodeBase *item = osgNodetoSceneNodesMap[node];

					if (item->getOffset() < minOffset)
					{
						minOffset = item->getOffset();
						closestItem = item;
						hitIndex = hitIndexList[index];
					}

					break;
				}
			}
		}

		index++;
	}

	return closestItem;
}

SceneNodeBase* Model::findSelectedItemByPolytope(osgUtil::Intersector* intersector, double sceneRadius, osg::Vec3d& intersectionPoint, ot::UID& faceId1, ot::UID& faceId2)
{
	osgUtil::PolytopeIntersector* polyIntersector = dynamic_cast<osgUtil::PolytopeIntersector*>(intersector);
	if (polyIntersector == nullptr) return nullptr;

	if (!intersector->containsIntersections()) return nullptr;

	std::list<osg::NodePath> hitItemList;
	std::vector<unsigned long long> hitIndexList;

	const osgUtil::PolytopeIntersector::Intersection& firstHit = *(polyIntersector->getIntersections().begin());

	intersectionPoint = firstHit.localIntersectionPoint;
	double maxDist = firstHit.maxDistance + (firstHit.maxDistance - firstHit.distance); // Add some tolerance to the max. distance

	double tolerance = 1e-6 * sceneRadius;

	for (auto i : polyIntersector->getIntersections())
	{
		if (isLineDrawable(i.drawable.get()))
		{
			osg::Vec3d point = i.localIntersectionPoint;
			osg::Vec3d delta = intersectionPoint - point;

			if (i.distance <= maxDist)
			{
				hitItemList.push_back(i.nodePath);
				hitIndexList.push_back(i.primitiveIndex);
			}
		}
	}

	// Since we are picking an edge of a solid here, we can have multiple edge hits at the same location.
	// These hits should belong to the edges of two adjacent faces.
	// We need to determine both edge indices and the corresponding geometry scene node item.

	SceneNodeBase* item = nullptr;

	bool faceId1Set = false;
	bool faceId2Set = false;

	auto hitIndex = hitIndexList.begin();

	for (auto hitItem : hitItemList)
	{
		int nNodes = hitItem.size();

		if (nNodes > 0)
		{
			for (int i = nNodes - 1; i >= 0; i--)
			{
				osg::Node* node = hitItemList.front()[i];
				if (osgNodetoSceneNodesMap.count(node) > 0)
				{
					if (item == nullptr)
					{
						item = osgNodetoSceneNodesMap[node];
					}
					else if (item != osgNodetoSceneNodesMap[node])
					{
						// The found items belong to two different shapes
						return nullptr;
					}

					ot::UID faceId = -1;
					
					SceneNodeGeometry* geomItem = dynamic_cast<SceneNodeGeometry*>(item);

					if (geomItem != nullptr)
					{
						faceId = geomItem->getFaceIdFromEdgePrimitiveIndex(*hitIndex);

						if (!faceId1Set)
						{
							faceId1 = faceId;
							faceId1Set = true;
						}
						else
						{
							if (faceId != faceId1)
							{
								if (!faceId2Set)
								{
									faceId2 = faceId;
									faceId2Set = true;
								}
								else
								{
									if (faceId != faceId2)
									{
										// This is a different item
										return nullptr;
									}
								}
							}
						}
					}

					break;
				}
			}
		}

		hitIndex++;
	}

	return item;
}

bool Model::isLineDrawable(osg::Drawable *drawable)
{
	osg::Geometry* geom = dynamic_cast<osg::Geometry*>(drawable);
	if (geom == nullptr) return false;

	for (auto prim : geom->getPrimitiveSetList())
	{
		if (prim->getMode() != osg::PrimitiveSet::LINES)
		{
			return false;
		}
	}

	return true;
}

void Model::selectSceneNode(SceneNodeBase *selectedItem, bool bCtrlKeyPressed)
{
	if (selectedItem == nullptr)
	{
		// Nothing was selected
		if (FrontendAPI::instance() != nullptr)
		{
			if (!bCtrlKeyPressed)
			{
				FrontendAPI::instance()->clearTreeSelection();
			}
		}
	}
	else
	{
		// Select the corresponding tree item
		if (FrontendAPI::instance() != nullptr)
		{
			if (!bCtrlKeyPressed)
			{
				FrontendAPI::instance()->selectSingleTreeItem(selectedItem->getTreeItemID());
			}
			else
			{
				FrontendAPI::instance()->toggleTreeItemSelection(selectedItem->getTreeItemID(), true);
			}
		}
	}
}

void Model::setHoverTreeItem(ot::UID hoverTreeItemID)
{
	if (currentSelectionMode != ENTITY) return;

	if (hoverTreeItemID == 0)
	{
		// No hover item selected

		if (currentHoverItem != nullptr)
		{
			currentHoverItem->setHighlighted(false);
			currentHoverItem = nullptr;

			refreshAllViews();
		}
	}
	else
	{
		// A hover item is selected
		SceneNodeBase *newHoverItem = treeItemToSceneNodesMap[hoverTreeItemID];
		assert(newHoverItem != nullptr);

		if (newHoverItem != currentHoverItem)
		{
			// A new item is selected as hover item

			// First reset the current hover item
			if (currentHoverItem != nullptr)
			{
				currentHoverItem->setHighlighted(false);
				currentHoverItem = nullptr;
			}

			// Now set the new hover item
			if (newHoverItem != nullptr)
			{
				newHoverItem->setHighlighted(true);
				currentHoverItem = newHoverItem;
			}

			refreshAllViews();
		}
	}
}

unsigned long long Model::getModelEntityIDFromTreeID(ot::UID treeItem)
{
	SceneNodeBase *item = treeItemToSceneNodesMap[treeItem];
	assert(item != nullptr);

	if (item == nullptr) return 0;

	return item->getModelEntityID();
}

unsigned long long Model::getTreeIDFromModelID(ot::UID modelID)
{
	for (const auto& treeItem : treeItemToSceneNodesMap)
	{
		if (treeItem.second->getModelEntityID() == modelID)
		{
			return treeItem.first;
		}
	}
	throw std::exception("Could not find tree item associated with modelID");
}

void Model::setHoverView(SceneNodeBase *selectedItem)
{
	if (selectedItem != currentHoverItem)
	{
		// A new item is selected as hover item

		// First reset the current hover item
		if (currentHoverItem != nullptr)
		{
			currentHoverItem->setHighlighted(false);
			currentHoverItem = nullptr;
		}

		// Now set the new hover item
		if (selectedItem != nullptr)
		{
			selectedItem->setHighlighted(true);
			currentHoverItem = selectedItem;
		}

		refreshAllViews();
	}
}

void Model::clearHoverView(void)
{
	if (currentHoverItem != nullptr)
	{
		currentHoverItem->setHighlighted(false);

		if (dynamic_cast<SceneNodeGeometry*>(currentHoverItem))
		{ 
			dynamic_cast<SceneNodeGeometry*>(currentHoverItem)->setHighlightNode(nullptr);

			if (dynamic_cast<SceneNodeGeometry*>(currentHoverItem)->getShapeNode() != nullptr)
			{
				dynamic_cast<SceneNodeGeometry*>(currentHoverItem)->getFaceEdgesHighlight()->setAllChildrenOff();
			}
		}

		currentHoverItem = nullptr;

		setCursorText("");
		updateSelectedFacesHighlight();
		refreshAllViews();
	}
}

void Model::addSelectedModelEntityIDToList(SceneNodeBase *root, std::list<unsigned long long> &selectedModelEntityID)
{
	if (root->isSelected()) selectedModelEntityID.push_back(root->getModelEntityID());

	for (auto child : root->getChildren())
	{
		addSelectedModelEntityIDToList(child, selectedModelEntityID);
	}
}

void Model::getSelectedModelEntityIDs(std::list<unsigned long long> &selectedModelEntityID)
{
	selectedModelEntityID.clear();
	addSelectedModelEntityIDToList(sceneNodesRoot, selectedModelEntityID);
}

void Model::addSelectedVisibleModelEntityIDToList(SceneNodeBase *root, std::list<unsigned long long> &selectedVisbleModelEntityID)
{
	if (root->isSelected() && root->isVisible()) selectedVisbleModelEntityID.push_back(root->getModelEntityID());

	for (auto child : root->getChildren())
	{
		addSelectedVisibleModelEntityIDToList(child, selectedVisbleModelEntityID);
	}
}

void Model::getSelectedVisibleModelEntityIDs(std::list<unsigned long long> &selectedVisbleModelEntityID)
{
	selectedVisbleModelEntityID.clear();
	addSelectedVisibleModelEntityIDToList(sceneNodesRoot, selectedVisbleModelEntityID);
}

void Model::addSelectedTreeItemIDToList(SceneNodeBase *root, std::list<ot::UID> &selectedTreeItemID)
{
	if (root->isSelected()) selectedTreeItemID.push_back(root->getTreeItemID());

	for (auto child : root->getChildren())
	{
		addSelectedTreeItemIDToList(child, selectedTreeItemID);
	}
}

void Model::getSelectedTreeItemIDs(std::list<ot::UID> &selectedTreeItemID)
{
	selectedTreeItemID.clear();
	addSelectedTreeItemIDToList(sceneNodesRoot, selectedTreeItemID);
}

void Model::removeSceneNodeAndChildren(SceneNodeBase *node, std::list<ot::UID> &treeItemDeleteList)
{
	// Remove all children of the current node
	std::list<SceneNodeBase*> allChildren = node->getChildren();
	for (auto child : allChildren)
	{
		removeSceneNodeAndChildren(child, treeItemDeleteList);
	}

	// Remove the shape from the tree
	treeItemDeleteList.push_back(node->getTreeItemID());
	
	//Deletes graphics item if one is associated
	FrontendAPI::instance()->removeGraphicsElements(node->getModelEntityID());

	// Ensure that we are not deleting the current hover item
	if (currentHoverItem == node)
	{
		currentHoverItem = nullptr; // We don't need to reset any display information, since the object will be deleted anyway.
	}

	// Store the current status of the item, if tree state recording is turned on
	if (treeStateRecording)
	{
		bool isExpanded = FrontendAPI::instance()->isTreeItemExpanded(node->getTreeItemID());
		treeInfoMap[node->getName()] = (node->isSelected() ? ITEM_SELECTED : 0) | (isExpanded ? ITEM_EXPANDED : 0);
	}

	// Remove the current scene node from the maps
	removeFromMaps(node);

	// Delete the scene Node
	// (This will also remove the node from the parents child list and delete all its children. Furthermore it will delete the corresponding osg Items if any)
	// Remove display information for the shape
	delete node;
	node = nullptr;
}

void Model::removeShapes(std::list<unsigned long long> modelEntityIDList)
{
	// We have to remove a list of shapes here. It may be that some of the shapes are 
	// child items of other shapes. We will loop through the list of shapes and remove them one by one. 
	// For each item we therefore need to check whether the item still exists

	std::list<ot::UID> treeItemDeleteList;  // We group the tree item deletions for better performance

	for (auto modelEntityID : modelEntityIDList)
	{
		if (modelItemToSceneNodesMap.count(modelEntityID) > 0)
		{
			// The item still exists
			SceneNodeBase *sceneNode = modelItemToSceneNodesMap[modelEntityID];
			assert(sceneNode != nullptr);

			if (sceneNode != nullptr)
			{
				removeSceneNodeAndChildren(sceneNode, treeItemDeleteList);
				sceneNode = nullptr;
			}
		}
	}

	// Remove the shapes from the tree
	FrontendAPI::instance()->removeTreeItems(treeItemDeleteList);
	
	// Refresh the views 
	refreshAllViews();
}

void Model::setShapeVisibility(std::list<unsigned long long> visibleID, std::list<unsigned long long> hiddenID)
{
	std::map<unsigned long long, bool> nodeVisibility;
	for (auto entity : visibleID)
	{
		nodeVisibility[entity] = true;
	}

	for (auto entity : hiddenID)
	{
		nodeVisibility[entity] = false;
	}

	setSceneNodeVisibility(sceneNodesRoot, nodeVisibility);

	// Refresh the views 
	refreshAllViews();
}

void Model::enterEntitySelectionMode(ot::serviceID_t replyTo, const std::string &selectionType, bool allowMultipleSelection, const std::string &selectionFilter, const std::string &selectionAction, const std::string &selectionMessage,
									 std::list<std::string> &optionNames, std::list<std::string> &optionValues)
{
	if (selectionType == "FACE")
	{
		ensure3DView();
		// Turn off wireframe
		if (isWireFrameMode()) toggleWireframeView();

		for (auto viewer : viewerList)
		{
			std::string firstText = allowMultipleSelection ? "Double-click to select faces to " : "Double-click to select a face to ";
			std::string secondText = allowMultipleSelection ? " (press RETURN to complete or ESC to cancel)" : " (press ESC to cancel)";

			viewer->setOverlayText(firstText + selectionMessage + secondText);
			viewer->setFreezeWorkingPlane(true);
		}

		if (!viewerList.empty())
		{
			viewerList.front()->setFocus();
		}

		currentSelectionMode         = FACE;
		currentSelectionReplyTo      = replyTo;
		currentSelectionAction       = selectionAction;
		currentSelectionMultiple     = allowMultipleSelection;
		currentSelectionOptionNames  = optionNames;
		currentSelectionOptionValues = optionValues;
	}
	else if (selectionType == "EDGE")
	{
		ensure3DView();

		for (auto viewer : viewerList)
		{
			std::string firstText = allowMultipleSelection ? "Double-click to select edges to " : "Double-click to select an edge to ";
			std::string secondText = allowMultipleSelection ? " (press RETURN to complete or ESC to cancel)" : " (press ESC to cancel)";

			viewer->setOverlayText(firstText + selectionMessage + secondText);
			viewer->setFreezeWorkingPlane(true);
		}

		if (!viewerList.empty())
		{
			viewerList.front()->setFocus();
		}

		currentSelectionMode = EDGE;
		currentSelectionReplyTo = replyTo;
		currentSelectionAction = selectionAction;
		currentSelectionMultiple = allowMultipleSelection;
		currentSelectionOptionNames = optionNames;
		currentSelectionOptionValues = optionValues;
	}
	else if (selectionType == "SHAPE")
	{
		ensure3DView();

		for (auto viewer : viewerList)
		{
			std::string firstText = allowMultipleSelection ? "Double-click to select shapes to " : "Double-click to select a shape to ";
			std::string secondText = allowMultipleSelection ? " (press RETURN to complete or ESC to cancel)" : " (press ESC to cancel)";

			viewer->setOverlayText(firstText + selectionMessage + secondText);
			viewer->setFreezeWorkingPlane(true);
		}

		if (!viewerList.empty())
		{
			viewerList.front()->setFocus();
		}

		currentSelectionMode         = SHAPE;
		currentSelectionReplyTo      = replyTo;
		currentSelectionAction       = selectionAction;
		currentSelectionMultiple     = allowMultipleSelection;
		currentSelectionOptionNames  = optionNames;
		currentSelectionOptionValues = optionValues;
	}
	else if (selectionType == "TRANSFORM")
	{
		ensure3DView();

		for (auto viewer : viewerList)
		{
			viewer->setOverlayText("Move or rotate selected shapes (press RETURN to complete or ESC to cancel)");
		}

		if (!viewerList.empty())
		{
			viewerList.front()->setFocus();

			std::list<SceneNodeBase *> objects;
			for (auto item : optionValues)
			{
				if (m_nameToSceneNodesMap.count(item) != 0)
				{
					objects.push_back(m_nameToSceneNodesMap[item]);
				}
				else
				{
					assert(0); // This item is not present in the view
				}
			}

			TransformManipulator *transformManipulator = new TransformManipulator(viewerList.front(), objects);
			transformManipulator->setReplyTo(replyTo);
			transformManipulator->setAction(selectionAction);
			transformManipulator->setOptionNames(optionNames);
			transformManipulator->setOptionValues(optionValues);

			currentManipulator = transformManipulator;
		}
	}
	else
	{
		assert(0); // Unknown selection type
	}
}

void Model::freeze3DView(bool flag)
{
	for (auto viewer : viewerList)
	{
		viewer->freeze3DView(flag);
	}
}

void Model::processCurrentSelectionMode(osgUtil::Intersector *intersector, double sceneRadius, bool bCtrlKeyPressed)
{
	switch (currentSelectionMode)
	{
	case ENTITY:
	{
		osg::Vec3d intersectionPoint;
		unsigned long long hitIndex = 0;
		SceneNodeBase *selectedItem = findSelectedItemByLineSegment(intersector, sceneRadius, intersectionPoint, hitIndex);
		selectSceneNode(selectedItem, bCtrlKeyPressed);
		break;
	}
	case FACE:
	{
		clearHoverView();

		osg::Vec3d intersectionPoint;
		unsigned long long hitIndex = 0;
		SceneNodeGeometry *selectedItem = dynamic_cast<SceneNodeGeometry *>(findSelectedItemByLineSegment(intersector, sceneRadius, intersectionPoint, hitIndex));
		if (selectedItem != nullptr)
		{
			unsigned long long faceId = selectedItem->getFaceIdFromTriangleIndex(hitIndex);

			faceSelected(selectedItem->getModelEntityID(), selectedItem, faceId);
		}
		break;
	}
	case EDGE:
	{
		clearHoverView();

		osg::Vec3d intersectionPoint;
		unsigned long long faceId1 = 0, faceId2 = 0;
		SceneNodeGeometry* selectedItem = dynamic_cast<SceneNodeGeometry*> (findSelectedItemByPolytope(intersector, sceneRadius, intersectionPoint, faceId1, faceId2));
		if (selectedItem != nullptr)
		{
			edgeSelected(selectedItem->getModelEntityID(), selectedItem, faceId1, faceId2);
		}
		break;
	}
	case SHAPE:
	{
		osg::Vec3d intersectionPoint;
		unsigned long long hitIndex = 0;
		SceneNodeBase *selectedItem = findSelectedItemByLineSegment(intersector, sceneRadius, intersectionPoint, hitIndex);
		selectSceneNode(selectedItem, bCtrlKeyPressed);
		break;
	}
	default:
		assert(0); // Unknown selection mode
	}
}

void Model::clearEdgeSelection(void)
{
	for (auto edge : currentEdgeSelection)
	{
		edge.getSelectedItem()->removeSelectedEdge(edge.getNode());
	}
	currentEdgeSelection.clear();
}

void Model::escapeKeyPressed(void)
{
	currentFaceSelection.clear();

	clearEdgeSelection();

	endCurrentSelectionMode(true);

	if (currentManipulator != nullptr)
	{
		for (auto viewer : viewerList)
		{
			viewer->removeOverlay();
		}

		currentManipulator->cancelOperation();

		delete currentManipulator;
		currentManipulator = nullptr;
	}
}

void Model::returnKeyPressed(void)
{
	if (currentManipulator != nullptr)
	{
		for (auto viewer : viewerList)
		{
			viewer->removeOverlay();
		}

		currentManipulator->performOperation();

		delete currentManipulator;
		currentManipulator = nullptr;
	}
	else
	{
		endCurrentSelectionMode(false);
	}
}

void Model::endCurrentSelectionMode(bool cancelled)
{
	if (currentSelectionMode != ENTITY)
	{
		for (auto viewer : viewerList)
		{
			viewer->removeOverlay();
			viewer->setFreezeWorkingPlane(false);
		}
	}

	clearSelectedFacesHighlight();
	clearHoverView();

	if (cancelled)
	{
		currentSelectionMode = ENTITY;  // Switch back to the standard selection mode
		return;
	}

	if (currentSelectionMode == FACE)
	{
		currentSelectionMode = ENTITY;  // Switch back to the standard selection mode

		// Process selected faces

		if (!currentFaceSelection.empty())
		{
			rapidjson::Document newDoc;
			newDoc.SetObject();

			rapidjson::Value modelID(rapidjson::kArrayType);
			rapidjson::Value faceName(rapidjson::kArrayType);

			for (auto selection : currentFaceSelection)
			{
				modelID.PushBack(rapidjson::Value(selection.getModelID()), newDoc.GetAllocator());
				faceName.PushBack(ot::JsonString(selection.getFaceName().c_str(), newDoc.GetAllocator()), newDoc.GetAllocator());
			}

			newDoc.AddMember("modelID", modelID, newDoc.GetAllocator());
			newDoc.AddMember("faceName", faceName, newDoc.GetAllocator());

			rapidjson::StringBuffer buffer;
			buffer.Clear();

			// Setup the Writer
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			newDoc.Accept(writer);

			std::string selectionInfo = buffer.GetString();

			// Send the selection message to the model
			FrontendAPI::instance()->entitiesSelected(currentSelectionReplyTo, currentSelectionAction, selectionInfo, currentSelectionOptionNames, currentSelectionOptionValues);

			currentFaceSelection.clear();
		}
	}
	else if (currentSelectionMode == EDGE)
	{
		currentSelectionMode = ENTITY;  // Switch back to the standard selection mode

		// Process selected edges

		if (!currentEdgeSelection.empty())
		{
			rapidjson::Document newDoc;
			newDoc.SetObject();

			rapidjson::Value modelID(rapidjson::kArrayType);
			rapidjson::Value edgeName(rapidjson::kArrayType);

			for (auto selection : currentEdgeSelection)
			{
				modelID.PushBack(rapidjson::Value(selection.getModelID()), newDoc.GetAllocator());
				edgeName.PushBack(ot::JsonString(selection.getEdgeName().c_str(), newDoc.GetAllocator()), newDoc.GetAllocator());
			}

			newDoc.AddMember("modelID", modelID, newDoc.GetAllocator());
			newDoc.AddMember("edgeName", edgeName, newDoc.GetAllocator());

			rapidjson::StringBuffer buffer;
			buffer.Clear();

			// Setup the Writer
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			newDoc.Accept(writer);

			std::string selectionInfo = buffer.GetString();

			// Send the selection message to the model
			FrontendAPI::instance()->entitiesSelected(currentSelectionReplyTo, currentSelectionAction, selectionInfo, currentSelectionOptionNames, currentSelectionOptionValues);

			clearEdgeSelection();
		}
	}
	else if (currentSelectionMode == SHAPE)
	{
		currentSelectionMode = ENTITY;  // Switch back to the standard selection mode

		// Process selected entities
		std::list<unsigned long long> selectedModelEntityID;
		getSelectedModelEntityIDs(selectedModelEntityID);

		if (!selectedModelEntityID.empty())
		{
			rapidjson::Document newDoc;
			newDoc.SetObject();

			rapidjson::Value modelID(rapidjson::kArrayType);

			for (auto entityID : selectedModelEntityID)
			{
				modelID.PushBack(rapidjson::Value(entityID), newDoc.GetAllocator());
			}

			newDoc.AddMember("modelID", modelID, newDoc.GetAllocator());

			rapidjson::StringBuffer buffer;
			buffer.Clear();

			// Setup the Writer
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			newDoc.Accept(writer);

			std::string selectionInfo = buffer.GetString();

			// Send the selection message to the model
			FrontendAPI::instance()->entitiesSelected(currentSelectionReplyTo, currentSelectionAction, selectionInfo, currentSelectionOptionNames, currentSelectionOptionValues);
		}
	}
	else
	{
		assert(0); // Unknown selection mode
	}
}

void Model::faceSelected(unsigned long long modelID, SceneNodeGeometry *selectedItem, unsigned long long faceId)
{
	std::string faceName = selectedItem->getFaceNameFromId(faceId);
	if (faceName.empty()) return;

	// Check whether the face is already in the selected list
	bool addFace = true;
	for (auto face : currentFaceSelection)
	{
		if (face.getSelectedItem() == selectedItem && face.getFaceId() == faceId)
		{
			addFace = false;
			currentFaceSelection.remove(face);

			setSelectedFacesHighlight(selectedItem, faceId, false);
			updateSelectedFacesHighlight();
			refreshAllViews();
			break;
		}
	}

	if (addFace && selectedItem != nullptr)
	{
		// Store face selection in list
		FaceSelection selection;
		selection.setData(modelID);
		selection.setSelectedItem(selectedItem);
		selection.setFaceId(faceId);
		selection.setFaceName(faceName);

		currentFaceSelection.push_back(selection);

		setSelectedFacesHighlight(selectedItem, faceId, true);
	}

	// If multiselection is not active, we end the selection mode now
	if (!currentSelectionMultiple) endCurrentSelectionMode(false);
}

void Model::edgeSelected(unsigned long long modelID, SceneNodeGeometry* selectedItem, unsigned long long faceId1, unsigned long long faceId2)
{
	if (selectedItem == nullptr) return;
	
	std::string edgeName = selectedItem->getEdgeNameFromFaceIds(faceId1, faceId2);
	if (edgeName.empty()) return;

	// Check whether the face is already in the selected list
	bool addEdge = true;
	for (auto edge : currentEdgeSelection)
	{
		if (edge.getSelectedItem() == selectedItem && (   faceId1 == edge.getFaceId1() && faceId2 == edge.getFaceId2()
													   || faceId1 == edge.getFaceId2() && faceId2 == edge.getFaceId1()))
		{
			addEdge = false;
			edge.getSelectedItem()->removeSelectedEdge(edge.getNode());

			currentEdgeSelection.remove(edge);

			refreshAllViews();
			break;
		}
	}

	if (addEdge && selectedItem != nullptr)
	{
		// Store face selection in list
		EdgeSelection selection;
		selection.setData(modelID);
		selection.setSelectedItem(selectedItem);
		selection.setFaceIds(faceId1, faceId2);
		selection.setNode(selectedItem->addSelectedEdge(faceId1, faceId2));
		selection.setEdgeName(edgeName);

		currentEdgeSelection.push_back(selection);

		refreshAllViews();
	}
	
	// If multiselection is not active, we end the selection mode now
	if (!currentSelectionMultiple) endCurrentSelectionMode(false);
}

void Model::setSelectedFacesHighlight(SceneNodeGeometry *selectedItem, unsigned long long faceId, bool highlight)
{
	if (highlight)
	{
		selectedFacesList[selectedItem].remove(faceId);
		selectedFacesList[selectedItem].push_back(faceId);
	}
	else
	{
		selectedFacesList[selectedItem].remove(faceId);
	}

}

void Model::clearSelectedFacesHighlight(void)
{
	for (auto entity : selectedFacesList)
	{
		for (auto face : entity.second)
		{
			entity.first->setEdgeHighlight(face, false, 1.0);
		}
	}

	selectedFacesList.clear();
}

void Model::updateSelectedFacesHighlight(void)
{
	for (auto entity : selectedFacesList)
	{
		for (auto face : entity.second)
		{
			entity.first->setEdgeHighlight(face, true, 3.0);
		}
	}
}

bool Model::isFaceSelected(SceneNodeGeometry *selectedItem, unsigned long long faceId)
{
	for (auto face : currentFaceSelection)
	{
		if (face.getSelectedItem() == selectedItem && face.getFaceId() == faceId)
		{
			return true;
		}
	}

	return false;
}

unsigned int Model::getFaceSelectionTraversalMask(void)
{
	return 1;   // The last bit is not set if the shape is transparent. Therefore, we can use the last bit mask to exclude transparent objects
				// The transparent flag is also not set for helper geometry, so this one will not be picked as well.
}

unsigned int Model::getEdgeSelectionTraversalMask(void)
{
	return 1;   // The last bit is not set if the shape is transparent. Therefore, we can use the last bit mask to exclude transparent objects
				// The transparent flag is also not set for helper geometry, so this one will not be picked as well.
}

unsigned int Model::getCurrentTraversalMask(void)
{
	// The second bit is set if the shape shall be excluded from selection

	switch (currentSelectionMode)
	{
		case ENTITY:
			return 2;   // We do not pick objects for which the second bit is not set.
			break;
		case FACE:
			return getFaceSelectionTraversalMask();
			break;
		case EDGE:
			return getEdgeSelectionTraversalMask();
			break;
		case SHAPE:
			return 2;   // We do not pick objects for which the second bit is not set.
			break;
		default:
			assert(0); // Unknown selection type
	}

	return ~0;
}

void Model::processHoverView(osgUtil::Intersector *intersector, double sceneRadius)
{
	switch (currentSelectionMode)
	{
	case ENTITY:
		if (intersector->containsIntersections())
		{
			osg::Vec3d intersectionPoint;
			unsigned long long hitIndex = 0;
			SceneNodeBase *selectedItem = findSelectedItemByLineSegment(intersector, sceneRadius, intersectionPoint, hitIndex);
			setHoverView(selectedItem);
		}
		else
		{
			clearHoverView();
		}
		break;
	case FACE:
		if (intersector->containsIntersections())
		{
			osg::Vec3d intersectionPoint;
			unsigned long long hitIndex = 0;
			SceneNodeGeometry *selectedItem = dynamic_cast<SceneNodeGeometry *> (findSelectedItemByLineSegment(intersector, sceneRadius, intersectionPoint, hitIndex));
			if (selectedItem != nullptr)
			{
				clearHoverView();

				unsigned long long faceId = selectedItem->getFaceIdFromTriangleIndex(hitIndex);
				
				std::string faceName = selectedItem->getFaceNameFromId(faceId);

				if (!faceName.empty())
				{
					setCursorText(faceName);

					if (isFaceSelected(selectedItem, faceId))
					{
						selectedItem->setEdgeHighlight(faceId, true, ViewerSettings::instance()->geometryHighlightLineWidth);
					}
					else
					{
						selectedItem->setEdgeHighlight(faceId, true, ViewerSettings::instance()->geometryHighlightLineWidth);
					}
					currentHoverItem = selectedItem;
				}
			}
			else
			{
				clearHoverView();
			}
		}
		else
		{
			clearHoverView();
		}
		break;
	case EDGE:
		if (intersector->containsIntersections())
		{
			osg::Vec3d intersectionPoint;
			unsigned long long faceId1 = 0, faceId2 = 0;
			SceneNodeGeometry* selectedItem = dynamic_cast<SceneNodeGeometry*> (findSelectedItemByPolytope(intersector, sceneRadius, intersectionPoint, faceId1, faceId2));
			if (selectedItem != nullptr)
			{
				if (currentHoverItem != selectedItem)
				{
					clearHoverView();
				}

				std::string edgeName = selectedItem->getEdgeNameFromFaceIds(faceId1, faceId2);
				if (!edgeName.empty())
				{
					selectedItem->setEdgeHighlightNode(faceId1, faceId2);
					setCursorText(edgeName);

					currentHoverItem = selectedItem;
				}
			}
			else
			{
				clearHoverView();
			}
		}
		else
		{
			clearHoverView();
		}
		break;
	case SHAPE:
		if (intersector->containsIntersections())
		{
			osg::Vec3d intersectionPoint;
			unsigned long long hitIndex = 0;
			SceneNodeBase *selectedItem = findSelectedItemByLineSegment(intersector, sceneRadius, intersectionPoint, hitIndex);
			setHoverView(selectedItem);
		}
		else
		{
			clearHoverView();
		}
		break;
	default:
		assert(0); // Unknown selection mode
	}
}

void Model::storeShapeNode(SceneNodeBase *node)
{
	if (node->getShapeNode()) {
		osgNodetoSceneNodesMap[node->getShapeNode()] = node; 
	}
}

void Model::storeShapeNode(osg::Node *node, SceneNodeBase *sceneNode)
{
	osgNodetoSceneNodesMap[node] = sceneNode;
}

void Model::forgetShapeNode(osg::Node *node)
{
	osgNodetoSceneNodesMap.erase(node);
}

void Model::notifySceneNodeAboutViewChange(const std::string& _sceneNodeName, const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType)
{
	auto sceneNodeIt = m_nameToSceneNodesMap.find(_sceneNodeName);
	if (sceneNodeIt == m_nameToSceneNodesMap.end()) {
		return;
	}
	
	sceneNodeIt->second->setViewChange(_state, _viewType);
}

void Model::addVisualizationMeshNodeFromFacetDataBase(const ot::EntityTreeItem& _treeItem, double _edgeColorRGB[3], bool _displayTetEdges, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion)
{
	SceneNodeMesh *meshNode = new SceneNodeMesh;

	meshNode->setTreeItem(_treeItem);
	meshNode->setDisplayTetEdges(_displayTetEdges);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	storeInMaps(meshNode);

	meshNode->setStorage(_projectName, _dataEntityID, _dataEntityVersion);
	meshNode->setEdgeColor(_edgeColorRGB);

	meshNode->setModel(this);
	meshNode->setNeedsInitialization();
}

void Model::addVisualizationMeshItemNodeFromFacetDataBase(const ot::EntityTreeItem& _treeItem, bool isHidden, const std::string& _projectName, ot::UID _tetEdgesID, ot::UID _tetEdgesVersion)
{
	SceneNodeMeshItem *meshNode = new SceneNodeMeshItem;

	meshNode->setTreeItem(_treeItem);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	storeInMaps(meshNode);

	meshNode->setStorage(_projectName, _treeItem.getEntityID(), _treeItem.getEntityVersion(), _tetEdgesID, _tetEdgesVersion);

	// Now find the owning mesh
	SceneNodeBase *masterMeshNode = parentNode;
	while (dynamic_cast<SceneNodeMesh*>(masterMeshNode) == nullptr)
	{
		masterMeshNode = masterMeshNode->getParent();
		if (masterMeshNode == nullptr) break;
	}

	meshNode->setMesh(dynamic_cast<SceneNodeMesh*>(masterMeshNode));

	meshNode->setModel(this);

	meshNode->setNeedsInitialization();

	if (!isHidden)
	{
		meshNode->ensureDataLoaded();
	}
	else
	{
		setItemVisibleState(meshNode, false);
	}
}

void Model::addVisualizationCartesianMeshNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, double _edgeColorRGB[3], double _meshLineColorRGB[3], bool _showMeshLines, const std::vector<double>& _meshCoordsX, const std::vector<double>& _meshCoordsY, const std::vector<double>& _meshCoordsZ,
	const std::string& _projectName, ot::UID _faceListEntityID, ot::UID _faceListEntityVersion, ot::UID _nodeListEntityID, ot::UID _nodeListEntityVersion)
{
	SceneNodeCartesianMesh *meshNode = new SceneNodeCartesianMesh;

	meshNode->setTreeItem(_treeItem);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Now add the current nodes osg node to the parent's osg node
	parentNode->getShapeNode()->addChild(meshNode->getShapeNode());

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	storeInMaps(meshNode);

	meshNode->setEdgeColor(_edgeColorRGB);
	meshNode->setMeshLineColor(_meshLineColorRGB);
	meshNode->setMeshLines(_meshCoordsX, _meshCoordsY, _meshCoordsZ);
	meshNode->setFaceListStorage(_projectName, _faceListEntityID, _faceListEntityVersion);
	meshNode->setNodeListStorage(_nodeListEntityID, _nodeListEntityVersion);
	meshNode->setNeedsInitialization();

	meshNode->setModel(this);

	if (!_isHidden)
	{
		meshNode->ensureDataLoaded();
	}
	else
	{
		setItemVisibleState(meshNode, false);
	}

	meshNode->showMeshLines(_showMeshLines);
}

void Model::visualizationCartesianMeshNodeShowLines(unsigned long long modelEntityID, bool showMeshLines)
{
	SceneNodeCartesianMesh *meshNode = dynamic_cast<SceneNodeCartesianMesh *>(modelItemToSceneNodesMap[modelEntityID]);

	if (meshNode != nullptr)
	{
		meshNode->showMeshLines(showMeshLines);
	}

	refreshAllViews();
}

void Model::visualizationTetMeshNodeTetEdges(unsigned long long modelEntityID, bool displayTetEdges)
{
	SceneNodeMesh *meshNode = dynamic_cast<SceneNodeMesh *>(modelItemToSceneNodesMap[modelEntityID]);

	if (meshNode != nullptr)
	{
		meshNode->setDisplayTetEdges(displayTetEdges);
	}

	refreshAllViews();
}

void Model::addVisualizationCartesianMeshItemNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::vector<int>& _facesList, double _color[3])
{
	SceneNodeCartesianMeshItem *meshNode = new SceneNodeCartesianMeshItem;

	meshNode->setTreeItem(_treeItem);
	meshNode->setModel(this);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	storeInMaps(meshNode);

	// Now find the owning mesh
	SceneNodeBase *masterMeshNode = parentNode;
	while (dynamic_cast<SceneNodeCartesianMesh*>(masterMeshNode) == nullptr)
	{
		masterMeshNode = masterMeshNode->getParent();
		if (masterMeshNode == nullptr) break;
	}

	meshNode->setMesh(dynamic_cast<SceneNodeCartesianMesh*>(masterMeshNode));

	// Set the data
	meshNode->setColor(_color[0], _color[1], _color[2]);
	meshNode->setFacesList(_facesList);

	if (_isHidden)
	{
		setItemVisibleState(meshNode, false);
	}
}

void Model::viewerTabChanged(const ot::WidgetViewBase& _viewInfo) {
	if (m_hasModalMenu) {
		if (m_currentMenu == FrontendAPI::instance()->getCurrentMenuPage()) {
			FrontendAPI::instance()->setCurrentMenuPage(m_previousMenu);
		}
	}

	if (_viewInfo.getViewFlags() & ot::WidgetViewBase::ViewIsCentral) {
		this->viewerTabChangedToCentral(_viewInfo);
	}
}

void Model::loadNextDataChunk(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex) {
	auto nodeIt = m_nameToSceneNodesMap.find(_entityName);
	if (nodeIt == m_nameToSceneNodesMap.end()) {
		OT_LOG_E("Could not find entity \"" + _entityName + "\"");
		return;
	}

	for (Visualiser* vis : nodeIt->second->getVisualiser()) {
		if (vis->getViewType() == _type) {
			if (!vis->requestNextDataChunk(_nextChunkStartIndex)) {
				OT_LOG_E("Failed to request next data chunk { \"EntityName\": \"" + _entityName + "\", \"ViewType\": \"" + ot::WidgetViewBase::toString(_type) + "\", \"NextChunkStartIndex\": " + std::to_string(_nextChunkStartIndex) + " }");
			}
		}
	}
}

void Model::loadRemainingData(const std::string& _entityName, ot::WidgetViewBase::ViewType _type, size_t _nextChunkStartIndex) {
	auto nodeIt = m_nameToSceneNodesMap.find(_entityName);
	if (nodeIt == m_nameToSceneNodesMap.end()) {
		OT_LOG_E("Could not find entity \"" + _entityName + "\"");
		return;
	}

	for (Visualiser* vis : nodeIt->second->getVisualiser()) {
		if (vis->getViewType() == _type) {
			if (!vis->requestRemainingData(_nextChunkStartIndex)) {
				OT_LOG_E("Failed to request remaining data { \"EntityName\": \"" + _entityName + "\", \"ViewType\": \"" + ot::WidgetViewBase::toString(_type) + "\", \"NextChunkStartIndex\": " + std::to_string(_nextChunkStartIndex) + " }");
			}
		}
	}
}

void Model::addVTKNode(const ot::EntityTreeItem& _treeItem, bool _isHidden, const std::string& _projectName, ot::UID _dataEntityID, ot::UID _dataEntityVersion)
{
	// Check whether the item already exists
	SceneNodeBase *item = modelItemToSceneNodesMap[_treeItem.getEntityID()];

	SceneNodeVTK *vtkNode = nullptr;

	if (item != nullptr)
	{
		// The item exists and needs to be updated

		vtkNode = dynamic_cast<SceneNodeVTK *>(item);
		assert(vtkNode != nullptr);

		if (vtkNode == nullptr)
		{
			assert(0);
			return;
		}
	}
	else
	{
		// The item is new and needs to be created

		vtkNode = new SceneNodeVTK;

		vtkNode->setTreeItem(_treeItem);

		// Get the parent scene node
		SceneNodeBase *parentNode = getParentNode(_treeItem.getEntityName());
		assert(parentNode != nullptr); // We assume that the parent node already exists

									   // Deal with potentially corrupt models
		if (parentNode == nullptr)
		{
			delete vtkNode;
			vtkNode = nullptr;
			return;
		}

		// Now add the current node as child to the parent
		parentNode->addChild(vtkNode);

		// Add the tree name to the tree
		addSceneNodesToTree(vtkNode);

		// Make the node invisible, if needed
		if (_isHidden)
		{
			setItemVisibleState(vtkNode, false);
		}

		vtkNode->setModel(this);

		// Now update the vtk node
		vtkNode->updateVTKNode(_projectName, _dataEntityID, _dataEntityVersion);

		// Add the node to the maps for faster access
		storeInMaps(vtkNode);
	}
}

void Model::updateVTKNode(ot::UID _entityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion)
{
	SceneNodeVTK *vtkNode = dynamic_cast<SceneNodeVTK *>(modelItemToSceneNodesMap[_entityID]);
	assert(vtkNode != nullptr);
	if (vtkNode == nullptr) return;

	vtkNode->updateVTKNode(projectName, visualizationDataID, visualizationDataVersion);
}

void Model::updateCapGeometry(osg::Vec3d normal, osg::Vec3d point, double radius)
{
	// This function gets called whenever the orientation or position of the cutplane is changed and the cap geometry therefore needs to be updated

	// First, we traverse the tree and find all geometry nodes
	updateCapGeometryForSceneNodes(sceneNodesRoot, normal, point, radius);
}

void Model::deleteCapGeometry()
{
	// This function gets called whenever the orientation or position of the cutplane is changed and the cap geometry therefore needs to be updated

	// First, we traverse the tree and find all geometry nodes
	deleteCapGeometryForSceneNodes(sceneNodesRoot);
}

void Model::updateCapGeometryForSceneNodes(SceneNodeBase *root, const osg::Vec3d &normal, const osg::Vec3d &point, double radius)
{
	OTAssertNullptr(root);
	if (root->isVisible() && dynamic_cast<SceneNodeGeometry*>(root) != nullptr)
	{
		// We have a SceneNodeGeometry item -> process it
		updateCapGeometryForGeometryItem(dynamic_cast<SceneNodeGeometry*>(root), normal, point, radius);
	}

	for (auto child : root->getChildren())
	{
		updateCapGeometryForSceneNodes(child, normal, point, radius);
	}
}

void Model::deleteCapGeometryForSceneNodes(SceneNodeBase* root)
{
	OTAssertNullptr(root);
	if (dynamic_cast<SceneNodeGeometry*>(root) != nullptr)
	{
		// We have a SceneNodeGeometry item -> process it
		dynamic_cast<SceneNodeGeometry*>(root)->deleteCutCapGeometryTriangles();
		dynamic_cast<SceneNodeGeometry*>(root)->deleteCutCapGeometryEdges();
	}

	for (auto child : root->getChildren())
	{
		deleteCapGeometryForSceneNodes(child);
	}
}

void Model::updateCapGeometryForGeometryItem(SceneNodeGeometry *item, const osg::Vec3d &normal, const osg::Vec3d &point, double radius)
{
	// Here we need to check whether the geometry item intersecs with the plane and update the geometry of the cap accordingly

	if (!item->getShapeNode()->getBound().valid()) return;

	osg::Vec3d boundingSphereCenter = item->getShapeNode()->getBound().center();
	double     boundingSphereRadius = item->getShapeNode()->getBound().radius();

	osg::Vec3d normalizedNormal = normal;
	normalizedNormal.normalize();

	// Distance is the positive dot product of the normalized normal and the point minus the center 
	double dist = abs(normalizedNormal.operator*(point.operator-(boundingSphereCenter)));

	if (dist > boundingSphereRadius) {
		item->deleteCutCapGeometryTriangles();
		item->deleteCutCapGeometryEdges();
	}
	else {
		IntersectionCapCalculator capping;
		capping.generateCapGeometryAndVisualization(item, normalizedNormal, point, radius);
	}
}

void Model::storeInMaps(SceneNodeBase* _node) {
	m_nameToSceneNodesMap[_node->getName()] = _node;
	if (_node->getShapeNode()) {
		osgNodetoSceneNodesMap[_node->getShapeNode()] = _node;
	}
	treeItemToSceneNodesMap[_node->getTreeItemID()] = _node;
	modelItemToSceneNodesMap[_node->getModelEntityID()] = _node;
}

void Model::removeFromMaps(const SceneNodeBase* _node) {
	m_nameToSceneNodesMap.erase(_node->getName());
	if (_node->getShapeNode()) {
		osgNodetoSceneNodesMap.erase(_node->getShapeNode());
	}
	treeItemToSceneNodesMap.erase(_node->getTreeItemID());
	modelItemToSceneNodesMap.erase(_node->getModelEntityID());
}

void Model::exportTextWorker(std::string _filePath, std::string _entityName) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_RequestTextData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(_entityName, doc.GetAllocator()), doc.GetAllocator());

	ot::ReturnMessage retMsg = ot::ReturnMessage::fromJson(FrontendAPI::instance()->messageModelService(doc.toJson()));
	if (!retMsg.isOk()) {
		OT_LOG_E("Could not get text data from model: " + retMsg.getWhat());
	} 
	else if (!retMsg.getWhat().empty()) {
		ot::JsonDocument doc;

		// Parse the returned GridFS info
		if (!doc.fromJson(retMsg.getWhat())) {
			OT_LOG_E("Could not parse grid fs info");
		}
		else {
			ot::GridFSFileInfo fileInfo(doc.getConstObject());

			// Now get the actual data from GridFS
			DataStorageAPI::DocumentAPI api;
			uint8_t* dataBuffer = nullptr;
			size_t length = 0;

			bsoncxx::oid oid_obj{ fileInfo.getDocumentId() };
			bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

			api.GetDocumentUsingGridFs(id, dataBuffer, length, fileInfo.getFileName());
			api.DeleteGridFSData(id, fileInfo.getFileName());

			std::string stringData(reinterpret_cast<char*>(dataBuffer), length);

			// Decompress if needed
			if (fileInfo.isFileCompressed()) {
				size_t decompressedSize = fileInfo.getUncompressedSize();
				uint8_t* decompr = ot::String::decompressBase64(stringData.c_str(), decompressedSize);
				stringData = std::string(reinterpret_cast<char*>(decompr), decompressedSize);
				delete[] decompr;
			}

			// Now write the data to file
			std::ofstream outFile(_filePath, std::ios_base::binary);
			if (outFile.is_open()) {
				outFile << stringData;
				outFile.close();
				FrontendAPI::instance()->displayText("File exported successfully: " + _filePath + "\n");
			}
			else {
				OT_LOG_E("Could not open file for writing: " + _filePath);
			}

			delete[] dataBuffer;
		}
	}

	FrontendAPI::instance()->lockSelectionAndModification(false);
	FrontendAPI::instance()->setProgressBarVisibility("Export text", false, true);
}

void Model::setTransparency(double transparencyValue)
{
	auto setTransparencyRecursive = [&](SceneNodeBase* node, auto&& setTransparencyRecursiveRef) -> void
	{
		node->setTransparency(transparencyValue);
		for (auto child : node->getChildren())
		{
			setTransparencyRecursiveRef(child, setTransparencyRecursiveRef);
		}
	};

	setTransparencyRecursive(sceneNodesRoot, setTransparencyRecursive);
}

void Model::setHighlightColor(const ot::Color& colorValue)
{
	auto setHighlightColorRecursive = [&](SceneNodeBase* node, auto&& setHighlightColorRecursiveRef) -> void
	{
		SceneNodeGeometry* geometryNode = dynamic_cast<SceneNodeGeometry*>(node);
		if (geometryNode != nullptr)
		{
			geometryNode->setHighlightColor(colorValue);
		}

		SceneNodeAnnotation* annotationNode = dynamic_cast<SceneNodeAnnotation*>(node);
		if (annotationNode != nullptr)
		{
			annotationNode->setHighlightColor(colorValue);
		}

		for (auto child : node->getChildren())
		{
			setHighlightColorRecursiveRef(child, setHighlightColorRecursiveRef);
		}
	};

	setHighlightColorRecursive(sceneNodesRoot, setHighlightColorRecursive);
}

void Model::setHighlightLineWidth(double lineWidthValue)
{
	auto setHighlightLineWidthRecursive = [&](SceneNodeBase* node, auto&& setHighlightLineWidthRecursiveRef) -> void
	{
		SceneNodeGeometry* geometryNode = dynamic_cast<SceneNodeGeometry*>(node);
		if (geometryNode != nullptr)
		{
			geometryNode->setHighlightLineWidth(lineWidthValue);
		}

		SceneNodeAnnotation* annotationNode = dynamic_cast<SceneNodeAnnotation*>(node);
		if (annotationNode != nullptr)
		{
			annotationNode->setHighlightLineWidth(lineWidthValue);
		}

		for (auto child : node->getChildren())
		{
			setHighlightLineWidthRecursiveRef(child, setHighlightLineWidthRecursiveRef);
		}
	};

	setHighlightLineWidthRecursive(sceneNodesRoot, setHighlightLineWidthRecursive);
}

void Model::updateEdgeColorMode()
{
	auto setUpdateEdgeColorModeRecursive = [&](SceneNodeBase* node, auto&& setUpdateEdgeColorModeRecursiveRef) -> void
		{
			SceneNodeGeometry* geometryNode = dynamic_cast<SceneNodeGeometry*>(node);
			if (geometryNode != nullptr)
			{
				geometryNode->updateEdgeColorMode();
			}

			for (auto child : node->getChildren())
			{
				setUpdateEdgeColorModeRecursiveRef(child, setUpdateEdgeColorModeRecursiveRef);
			}
		};

	setUpdateEdgeColorModeRecursive(sceneNodesRoot, setUpdateEdgeColorModeRecursive);
}

void Model::updateMeshEdgeColor()
{
	auto setupdateMeshEdgeColorModeRecursive = [&](SceneNodeBase* node, auto&& setupdateMeshEdgeColorModeRecursiveRef) -> void
		{
			SceneNodeMesh* meshNode = dynamic_cast<SceneNodeMesh*>(node);
			if (meshNode != nullptr)
			{
				meshNode->updateEdgeColor();
			}

			SceneNodeCartesianMesh* cartesianMeshNode = dynamic_cast<SceneNodeCartesianMesh*>(node);
			if (cartesianMeshNode != nullptr)
			{
				cartesianMeshNode->updateEdgeColor();
			}

			for (auto child : node->getChildren())
			{
				setupdateMeshEdgeColorModeRecursiveRef(child, setupdateMeshEdgeColorModeRecursiveRef);
			}
		};

	setupdateMeshEdgeColorModeRecursive(sceneNodesRoot, setupdateMeshEdgeColorModeRecursive);
}
