#include "stdafx.h"

#include "Model.h"
#include "Viewer.h"
#include "Notifier.h"

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

#include "OTCore/Logger.h"
#include "SceneNodeBase.h"
#include "SceneNodeContainer.h"
#include "SceneNodeGeometry.h"
#include "SceneNodeAnnotation.h"
#include "SceneNodeMesh.h"
#include "SceneNodeMeshItem.h"
#include "SceneNodeCartesianMesh.h"
#include "SceneNodeCartesianMeshItem.h"
#include "SceneNodePlot1D.h"
#include "SceneNodePlot1DCurve.h"
#include "SceneNodeTextItem.h"
#include "SceneNodeTableItem.h"
#include "SceneNodeVTK.h"

#include "ManipulatorBase.h"
#include "TransformManipulator.h"

#include "DataBase.h"
#include "PlotViewOld.h"

bool operator==(const FaceSelection& left, const FaceSelection& right) 
{ 
	return (left.getSelectedItem() == right.getSelectedItem() && left.getFaceId() == right.getFaceId()); 
}

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
	currentManipulator(nullptr)
{
	sceneNodesRoot = new SceneNodeContainer();
	osgRootNode = sceneNodesRoot->getShapeNode();
}

Model::~Model()
{
	// Delete all shapes in the shapes list
	delete 	sceneNodesRoot;
	sceneNodesRoot = nullptr;

	removeUIControls();
}

void Model::attachViewer(Viewer *viewer)
{
	assert(std::find(viewerList.begin(), viewerList.end(), viewer) == viewerList.end());    // Check that the item is not in the list yet

	viewerList.push_back(viewer);

	if (viewerList.size() == 1)
	{
		// The first viewer was registered. We assume that the 3D tab is active
		setupUIControls3D();
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
		removeUIControls();
	}
}

void Model::deactivateModel(void)
{
	// Clear the tree entries
	if (getNotifier() != nullptr)
	{
		getNotifier()->clearTree();
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

		updateUIControlState(selectedTreeItemID);
	}
}

void Model::fillPropertyGrid(const ot::PropertyGridCfg& _configuration)
{
	getNotifier()->fillPropertyGrid(_configuration);
}

void Model::setDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName, double value)
{
	getNotifier()->setDoublePropertyValue(_groupName, _itemName, value);
}

double Model::getDoublePropertyGridValue(const std::string& _groupName, const std::string& _itemName)
{
	return getNotifier()->getDoublePropertyValue(_groupName, _itemName);
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
	getNotifier()->lockSelectionAndModification(flag);
}

void Model::addSceneNodesToTree(SceneNodeBase *root)
{
	// here we add the current item and all its children to the tree. 
	// We assume that all parent items already exist.

	if (isActive)
	{
		assert(getNotifier() != nullptr);
		if (getNotifier() != nullptr)
		{
			if (root != sceneNodesRoot)  // The toplevel scene node is invisible and therefore not added to the tree.
			{
				ot::UID treeItemID = getNotifier()->addTreeItem(root->getName(), root->isEditable(), root->getSelectChildren());
				getNotifier()->setTreeItemIcon(treeItemID, root->getTreeIcons().size, root->isVisible() ? root->getTreeIcons().visibleIcon : root->getTreeIcons().hiddenIcon);

				root->setTreeItemID(treeItemID);

				// Now add the node to the map fro faster access
				treeItemToSceneNodesMap[treeItemID] = root;

				// Now we check whether the current item is visible
				manageParentVisibility(root);

				//if (root->isVisible())
				//{
				//	// We need to set the parent status to visible
				//	if (root->getParent() != nullptr)
				//	{
				//		setItemVisibleState(root->getParent(), true);
				//	}
				//}
			}

			if (root->getParent() != nullptr)
			{
				if (root->getParent()->isSelected())
				{
					getNotifier()->selectTreeItem(root->getTreeItemID());
				}
			}

			// Finally process all children of the current item
			for (auto child : root->getChildren())
			{
				addSceneNodesToTree(child);
			}
		}
	}
}

void Model::fillTree(void)
{
	assert(getNotifier() != nullptr);
	if (getNotifier() == nullptr) return;

	getNotifier()->clearTree();

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

void Model::resetAllViews1D(void)
{
	for (auto viewer : viewerList)
	{
		viewer->get1DPlot()->resetView();
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
				if (nameToSceneNodesMap.count(item.first) != 0)
				{
					SceneNodeBase *sceneItem = nameToSceneNodesMap[item.first];
					assert(sceneItem != nullptr);

					selectSceneNode(sceneItem, true);
				}
			}

			if (item.second & ITEM_EXPANDED)
			{
				if (nameToSceneNodesMap.count(item.first) != 0)
				{
					SceneNodeBase *sceneItem = nameToSceneNodesMap[item.first];
					assert(sceneItem != nullptr);

					getNotifier()->expandSingleTreeItem(sceneItem->getTreeItemID());
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

void  Model::addVisualizationContainerNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool editable)
{
	// Check whether we already have a container node
	if (nameToSceneNodesMap.count(treeName) != 0)
	{
		if (dynamic_cast<SceneNodeContainer *>(nameToSceneNodesMap[treeName]) != nullptr) return;
		assert(0); // This is not a container node -> overwrite
	}

	// Create the new container node

	SceneNodeBase *containerNode = new SceneNodeContainer;

	containerNode->setName(treeName);
	containerNode->setEditable(editable);
	containerNode->setModelEntityID(modelEntityID);
	containerNode->setTreeIcons(treeIcons);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
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
	nameToSceneNodesMap[treeName] = containerNode;
	osgNodetoSceneNodesMap[containerNode->getShapeNode()] = containerNode;
	treeItemToSceneNodesMap[containerNode->getTreeItemID()] = containerNode;
	modelItemToSceneNodesMap[modelEntityID] = containerNode;
}

void Model::addVisualizationAnnotationNode(const std::string &treeName, unsigned long long modelEntityID, 
										   const TreeIcon &treeIcons, bool isHidden,
										   const double edgeColorRGB[3],
										   const std::vector<std::array<double, 3>> &points,
										   const std::vector<std::array<double, 3>> &points_rgb,
										   const std::vector<std::array<double, 3>> &triangle_p1,
										   const std::vector<std::array<double, 3>> &triangle_p2,
										   const std::vector<std::array<double, 3>> &triangle_p3,
										   const std::vector<std::array<double, 3>> &triangle_rgb)
{
	SceneNodeAnnotation *annotationNode = nullptr;
	bool nodeAlreadyExists = false;

	// Check whether we already have a container node
	if (nameToSceneNodesMap.count(treeName) != 0)
	{
		annotationNode = dynamic_cast<SceneNodeAnnotation *>(nameToSceneNodesMap[treeName]);
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

	annotationNode->setName(treeName);
	annotationNode->setModelEntityID(modelEntityID);
	annotationNode->setTreeIcons(treeIcons);	
	annotationNode->setWireframe(wireFrameState);

	annotationNode->initializeFromData(edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);

	if (!nodeAlreadyExists)
	{
		// Get the parent scene node
		SceneNodeBase *parentNode = getParentNode(treeName);
		assert(parentNode != nullptr); // We assume that the parent node already exists

		// Now add the current node as child to the parent
		parentNode->addChild(annotationNode);

		// Now add the current nodes osg node to the parent's osg node
		parentNode->getShapeNode()->addChild(annotationNode->getShapeNode());

		// Add the tree name to the tree
		addSceneNodesToTree(annotationNode);
	}

	// Make the node invisible, if needed
	if (isHidden)
	{
		setItemVisibleState(annotationNode, false);
	}

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = annotationNode;
	osgNodetoSceneNodesMap[annotationNode->getShapeNode()] = annotationNode;
	treeItemToSceneNodesMap[annotationNode->getTreeItemID()] = annotationNode;
	modelItemToSceneNodesMap[modelEntityID] = annotationNode;
}

void Model::addVisualizationAnnotationNodeDataBase(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long entityID, unsigned long long version)
{
	double edgeColorRGB[3];

	std::vector<std::array<double, 3>> points;
	std::vector<std::array<double, 3>> points_rgb;

	std::vector<std::array<double, 3>> triangle_p1;
	std::vector<std::array<double, 3>> triangle_p2;
	std::vector<std::array<double, 3>> triangle_p3;
	std::vector<std::array<double, 3>> triangle_rgb;

	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(entityID, version, doc))
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

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, "SchemaVersion_EntityAnnotationData");
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

	addVisualizationAnnotationNode(treeName, modelEntityID, treeIcons, isHidden, edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
}


void Model::addNodeFromFacetDataBase(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], const std::string &materialType, 
									 const std::string &textureType, bool reflective, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, 
									 double offsetFactor, bool isHidden, bool isEditable, const std::string &projectName, unsigned long long entityID, unsigned long long version,
									 bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected, std::vector<double> &transformation)
{
	SceneNodeGeometry *geometryNode = createNewGeometryNode(treeName, modelEntityID, treeIcons, isHidden, isEditable, selectChildren, manageParentVisibility, manageChildVisibility);

	if (geometryNode != nullptr)
	{
		geometryNode->setSurfaceColorRGB(surfaceColorRGB);
		geometryNode->setEdgeColorRGB(edgeColorRGB);
		geometryNode->setMaterialType(materialType);
		geometryNode->setTextureType(textureType, reflective);
		geometryNode->setBackFaceCulling(backFaceCulling);
		geometryNode->setOffsetFactor(offsetFactor);
		geometryNode->setStorage(projectName, entityID, version);
		geometryNode->setTreeIcons(treeIcons);
		geometryNode->setWireframe(wireFrameState);
		geometryNode->setShowWhenSelected(showWhenSelected);
		geometryNode->setTransformation(transformation);

		//if (isHidden)
		//{
		//	geometryNode->setNeedsInitialization();
		//}
		//else
		//{
		//	geometryNode->initializeFromDataStorage();
		//}
		geometryNode->initializeFromDataStorage(); // We want to have the hover preview function for hidden items as well

		if (isHidden)
		{
			setItemVisibleState(geometryNode, false);
		}
	}
}

void Model::addSceneNodeText(const std::string& treeName, unsigned long long modelEntityID, const TreeIcon& treeIcons, bool editable)
{
	// Check whether we already have a container node
	if (nameToSceneNodesMap.count(treeName) != 0)
	{
		if (dynamic_cast<SceneNodeTextItem*>(nameToSceneNodesMap[treeName]) != nullptr) return;
		assert(0); // This is not a container node -> overwrite
	}

	// Create the new container node

	SceneNodeTextItem* textNode = new SceneNodeTextItem;

	textNode ->setName(treeName);
	textNode ->setEditable(editable);
	textNode ->setModelEntityID(modelEntityID);
	textNode ->setTreeIcons(treeIcons);
	textNode->setModel(this);
	textNode->addVisualiserText();

	// Get the parent scene node
	SceneNodeBase* parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	if (parentNode == nullptr)
	{
		// If the model is corrupt, this might happen. We deal with this by ignoring the current item
		delete textNode;
		return;
	}

	// Now add the current node as child to the parent
	parentNode->addChild(textNode);

	// Now add the current nodes osg node to the parent's osg node
	parentNode->getShapeNode()->addChild(textNode->getShapeNode());

	// Add the tree name to the tree
	addSceneNodesToTree(textNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = textNode;
	osgNodetoSceneNodesMap[textNode->getShapeNode()] = textNode;
	treeItemToSceneNodesMap[textNode->getTreeItemID()] = textNode;
	modelItemToSceneNodesMap[modelEntityID] = textNode;
}

SceneNodeGeometry *Model::createNewGeometryNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, 
												bool isHidden, bool isEditable, bool selectChildren, bool manageParentVisibility, bool manageChildVisibility)
{
	// Check whether the item already exists
	SceneNodeBase *item = modelItemToSceneNodesMap[modelEntityID];

	SceneNodeGeometry *geometryNode = nullptr;

	if (item != nullptr)
	{
		// The item exists and needs to be updated

		geometryNode = dynamic_cast<SceneNodeGeometry *>(item);
		assert(geometryNode != nullptr);

		if (geometryNode == nullptr)
		{
			assert(0);
			return nullptr;
		}
	}
	else
	{
		// The item is new and needs to be created

		geometryNode = new SceneNodeGeometry;

		geometryNode->setName(treeName);
		geometryNode->setModelEntityID(modelEntityID);
		geometryNode->setTreeIcons(treeIcons);
		geometryNode->setEditable(isEditable);
		geometryNode->setWireframe(wireFrameState);
		geometryNode->setVisible(!isHidden);
		geometryNode->setSelectChildren(selectChildren);
		geometryNode->setManageVisibilityOfParent(manageParentVisibility);
		geometryNode->setManageVisibilityOfChildren(manageChildVisibility);

		// Get the parent scene node
		SceneNodeBase *parentNode = getParentNode(treeName);
		assert(parentNode != nullptr); // We assume that the parent node already exists

		// Deal with potentially corrupt models
		if (parentNode == nullptr)
		{
			delete geometryNode;
			return nullptr;
		}

		// Now add the current node as child to the parent
		parentNode->addChild(geometryNode);

		// Add the tree name to the tree
		addSceneNodesToTree(geometryNode);

		// Make the node invisible, if needed
		if (isHidden)
		{
			setItemVisibleState(geometryNode, false);
		}

		geometryNode->setModel(this);

		// Add the node to the maps for faster access
		nameToSceneNodesMap[treeName] = geometryNode;
		treeItemToSceneNodesMap[geometryNode->getTreeItemID()] = geometryNode;
		modelItemToSceneNodesMap[modelEntityID] = geometryNode;
	}

	return geometryNode;
}

void Model::setEntityName(unsigned long long modelEntityID, const std::string &newName)
{
	SceneNodeBase *item = modelItemToSceneNodesMap[modelEntityID];
	assert(item != nullptr);

	if (item != nullptr)
	{
		std::string newText;

		size_t index = newName.rfind("/");
		if (index == std::string::npos)
		{
			newText = newName;
		}
		else
		{
			newText = newName.substr(index + 1);
		}

		nameToSceneNodesMap[newName] = item;
		nameToSceneNodesMap.erase(item->getName());

		item->setName(newName);

		getNotifier()->setTreeItemText(item->getTreeItemID(), newText);
	}
}

void Model::renameEntityPath(const std::string &oldPath, const std::string &newPath)
{
	std::map<std::string, SceneNodeBase *> entityMap = nameToSceneNodesMap;
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
				nameToSceneNodesMap[newPath] = item.second;
				nameToSceneNodesMap.erase(name);

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

				getNotifier()->setTreeItemText(item.second->getTreeItemID(), newText);
			}
		}
		else if (name.size() > oldPath.size())
		{
			if (name.substr(0, filter.size()) == filter)
			{
				std::string newName = newPath + name.substr(filter.size() - 1);

				// Here we just need to rename the item itself and don't need to update the tree
				item.second->setName(newName);
				nameToSceneNodesMap[newName] = item.second;
				nameToSceneNodesMap.erase(name);
			}
		}
	}
}

void Model::addNodeFromFacetData(const std::string &treeName, double surfaceColorRGB[3], double edgeColorRGB[3], unsigned long long modelEntityID, const TreeIcon &treeIcons, bool backFaceCulling, double offsetFactor, bool isHidden, bool isEditable, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNameMap, std::string &errors,
								 bool selectChildren, bool manageParentVisibility, bool manageChildVisibility, bool showWhenSelected)
{
	SceneNodeGeometry *geometryNode = createNewGeometryNode(treeName, modelEntityID, treeIcons, isHidden, isEditable, selectChildren, manageParentVisibility, manageChildVisibility);

	if (geometryNode != nullptr)
	{
		geometryNode->setErrors(errors);

		geometryNode->setSurfaceColorRGB(surfaceColorRGB);
		geometryNode->setEdgeColorRGB(edgeColorRGB);
		geometryNode->setBackFaceCulling(backFaceCulling);
		geometryNode->setOffsetFactor(offsetFactor);
		geometryNode->setTreeIcons(treeIcons);
		geometryNode->setWireframe(wireFrameState);
		geometryNode->setShowWhenSelected(showWhenSelected);

		geometryNode->initializeFromFacetData(nodes, triangles, edges, faceNameMap);
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
	SceneNodeBase *parent = nameToSceneNodesMap[parentName];
	assert(parent != nullptr);

	return parent;
}

void Model::resetSelection(SceneNodeBase *root)
{
	root->setSelected(false);

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

void Model::setSelectedTreeItems(std::list<ot::UID> &selectedTreeItemID, std::list<unsigned long long> &selectedModelItems, std::list<unsigned long long> &selectedVisibleModelItems)
{
	selectedModelItems.clear();

	// Set the selection flag for all nodes to false
	resetSelection(sceneNodesRoot);

	if (selectedTreeItemID.empty())
	{
		// No shape selected -> Draw all shapes opaque
		setAllShapesOpaque(sceneNodesRoot);

		// Update the working plane transformation 
		updateWorkingPlaneTransform();
		
		updateUIControlState(selectedTreeItemID);
		clear1DPlot();
		refreshAllViews();
		return;
	}

	singleItemSelected = (selectedTreeItemID.size() == 1);

	// Now at least one shape is selected
	// -> selected shapes are drawn opaque and all others are drawn transparent

	bool isItem1DSelected = false;
	bool isItem3DSelected = false;

	// First set the selected state for all selected nodes
	for (auto item : selectedTreeItemID)
	{
		SceneNodeBase *sceneNode = treeItemToSceneNodesMap[item];

		if (sceneNode != nullptr)
		{
			isItem1DSelected |= sceneNode->isItem1D();
			isItem3DSelected |= sceneNode->isItem3D();

			assert(sceneNode != nullptr);
			sceneNode->setSelected(true);
			selectedModelItems.push_back(sceneNode->getModelEntityID());

			if (sceneNode->isVisible())
			{
				selectedVisibleModelItems.push_back(sceneNode->getModelEntityID());
			}
		}
	}

	// Now update the transparent / opaque mode acoording to the selection
	setSelectedShapesOpaqueAndOthersTransparent(sceneNodesRoot);
	
	// Update the UI state and the view
	updateUIControlState(selectedTreeItemID);
	refreshAllViews();

	// reset the 1d view, if necessary
	if (!isItem1DSelected)
	{
		clear1DPlot();
	}
	else
	{
		update1DPlot(sceneNodesRoot);
	}

	if (isItem1DSelected && !isItem3DSelected)
	{
		// Ensure that we have the 1D view active
		ensure1DView();
	}

	if (isItem3DSelected && !isItem1DSelected)
	{
		// Ensure that we have the 3D view active
		ensure3DView();
	}

	// Update the working plane transformation 
	updateWorkingPlaneTransform();
}

void Model::ensure1DView(void)
{
	if (getNotifier()->getCurrentVisualizationTab() != "Versions")
	{
		getNotifier()->setCurrentVisualizationTab("1D");
	}
}

void Model::ensure3DView(void)
{
	if (getNotifier()->getCurrentVisualizationTab() != "Versions")
	{
		getNotifier()->setCurrentVisualizationTab("3D");
	}
}

void Model::set1DPlotIncompatibleData(void)
{
	for (auto viewer : viewerList)
	{
		if (viewer->get1DPlot() != nullptr)
		{
			viewer->get1DPlot()->setIncompatibleData();
		}
	}
}

void Model::remove1DPlotErrorState(void) {
	for (auto viewer : viewerList)
	{
		if (viewer->get1DPlot() != nullptr)
		{
			viewer->get1DPlot()->setErrorState(false);
		}
	}
}

void Model::clear1DPlot(void)
{
	for (auto viewer : viewerList)
	{
		if (viewer->get1DPlot() != nullptr)
		{
			viewer->get1DPlot()->clear(false);
		}
	}
}

void Model::update1DPlot(SceneNodeBase *root)
{
	std::list<PlotCurveItem> selectedCurves;
	AbstractPlot::PlotType plotType;
	PlotDataset::axisQuantity yAxisQuantity;
	std::string title;
	std::string projectName;
	bool grid = false;
	int gridColor[3] = { 0, 0, 0 };
	bool legend = false;
	bool logscaleX = false;
	bool logscaleY = false;
	bool autoscaleX = false;
	bool autoscaleY = false;
	double xmin = 0.0;
	double xmax = 0.0;
	double ymin = 0.0;
	double ymax = 0.0;
	SceneNodePlot1D*commonPlot = nullptr;
	bool firstCurve = true;
	bool compatible = true;

	// Determine the selected items
	add1DPlotItems(root, firstCurve, commonPlot, plotType, yAxisQuantity, title, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, projectName, selectedCurves, compatible);

	if (!compatible)
	{
		// Here we need to show a message that the data is incompatible
		set1DPlotIncompatibleData();
		return;
	}

	// Here we remove the error state if there was one set before
	remove1DPlotErrorState();

	// If all selected curves belong to a single plot, add all unselected curces as dimmed
	if (commonPlot != nullptr)
	{
		addCompatibleDimmedPlotItems(commonPlot, plotType, yAxisQuantity, title, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, projectName, selectedCurves);
	}

	for (auto viewer : viewerList)
	{
		if (viewer->get1DPlot() != nullptr)
		{
			viewer->get1DPlot()->setFromDataBase(plotType, yAxisQuantity, title, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, projectName, selectedCurves);
		}
	}
}

void Model::add1DPlotItems(SceneNodeBase *root, bool &firstCurve, SceneNodePlot1D*& commonPlot, AbstractPlot::PlotType &plotType, PlotDataset::axisQuantity &yAxisQuantity, std::string &title, bool &grid, int gridColor[], bool &legend, bool &logscaleX, bool &logscaleY,
						   bool &autoscaleX, bool &autoscaleY, double &xmin, double &xmax, double &ymin, double &ymax, std::string &projectName, std::list<PlotCurveItem> &selectedCurves, bool &compatible)
{
	if (root->isSelected() && root->isVisible())
	{
		SceneNodePlot1DCurve *curve = dynamic_cast<SceneNodePlot1DCurve*>(root);

		if (curve != nullptr)
		{
			SceneNodePlot1D* plot = getPlotFromCurve(curve);
			if (!plot) {
				OT_LOG_EA("Could not find plot from curve");
				return;
			}

			if (firstCurve)
			{
				title = plot->getTitle();
				plotType = plot->getPlotType();
				yAxisQuantity = plot->getPlotQuantity();
				grid = plot->getGrid();
				gridColor[0] = plot->getGridColorR();
				gridColor[1] = plot->getGridColorG();
				gridColor[2] = plot->getGridColorB();
				legend = plot->getLegend();
				logscaleX = plot->getLogscaleX();
				logscaleY = plot->getLogscaleY();
				autoscaleX = plot->getAutoscaleX();
				autoscaleY = plot->getAutoscaleY();
				xmin = plot->getXmin();
				xmax = plot->getXmax();
				ymin = plot->getYmin();
				ymax = plot->getYmax();

				projectName = curve->getProjectName();

				commonPlot = plot;
				firstCurve = false;
			}
			else
			{
				if (projectName != curve->getProjectName() || title != plot->getTitle() || legend != plot->getLegend() ||
					!gridCompatible(grid, gridColor[0], gridColor[1], gridColor[2], plot->getGrid(), plot->getGridColorR(), plot->getGridColorG(), plot->getGridColorB()) ||
					!axisCompatible(logscaleX, autoscaleX, xmin, xmax, plot->getLogscaleX(), plot->getAutoscaleX(), plot->getXmin(), plot->getXmax()) ||
					!axisCompatible(logscaleY, autoscaleY, ymin, ymax, plot->getLogscaleY(), plot->getAutoscaleY(), plot->getYmin(), plot->getYmax()))
				{
					compatible = false;
					return;
				}

				if (plot != commonPlot) commonPlot = nullptr;
			}

			PlotCurveItem curveItem;
			curveItem.setModelEntity(curve->getModelEntityID(), curve->getModelEntityVersion());
			curveItem.setTreeID(curve->getTreeItemID());

			std::string parentName = curve->getParent()->getName();
			std::string curveName = curve->getName().substr(parentName.length()+1);
			curveItem.setName(curveName);

			selectedCurves.push_back(curveItem);
		}
	}

	for (auto child : root->getChildren())
	{
		add1DPlotItems(child, firstCurve, commonPlot, plotType, yAxisQuantity, title, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, projectName, selectedCurves, compatible);
		if (!compatible) break;
	}
}

void Model::addCompatibleDimmedPlotItems(SceneNodeBase *root, AbstractPlot::PlotType &plotType, PlotDataset::axisQuantity &yAxisQuantity, std::string &title, bool &grid, int gridColor[], bool &legend, bool &logscaleX, bool &logscaleY,
										 bool &autoscaleX, bool &autoscaleY, double &xmin, double &xmax, double &ymin, double &ymax, std::string &projectName, std::list<PlotCurveItem> &selectedCurves)
{
	if (!root->isSelected() && root->isVisible())
	{
		SceneNodePlot1DCurve *curve = dynamic_cast<SceneNodePlot1DCurve*>(root);

		if (curve != nullptr)
		{
			SceneNodePlot1D* plot = getPlotFromCurve(curve);
			
			if (!plot) {
				OT_LOG_EA("Could not find plot from curve");
				return;
			}

			if (projectName == curve->getProjectName() && title == plot->getTitle() && legend == plot->getLegend() &&
				gridCompatible(grid, gridColor[0], gridColor[1], gridColor[2], plot->getGrid(), plot->getGridColorR(), plot->getGridColorG(), plot->getGridColorB()) &&
				axisCompatible(logscaleX, autoscaleX, xmin, xmax, plot->getLogscaleX(), plot->getAutoscaleX(), plot->getXmin(), plot->getXmax()) &&
				axisCompatible(logscaleY, autoscaleY, ymin, ymax, plot->getLogscaleY(), plot->getAutoscaleY(), plot->getYmin(), plot->getYmax()))
			{
				PlotCurveItem curveItem;
				curveItem.setModelEntity(curve->getModelEntityID(), curve->getModelEntityVersion());
				curveItem.setTreeID(curve->getTreeItemID());
				curveItem.setDimmed(true);

				std::string parentName = curve->getParent()->getName();
				std::string curveName = curve->getName().substr(parentName.length() + 1);
				curveItem.setName(curveName);

				selectedCurves.push_back(curveItem);
			}
		}
	}

	for (auto child : root->getChildren())
	{
		addCompatibleDimmedPlotItems(child, plotType, yAxisQuantity, title, grid, gridColor, legend, logscaleX, logscaleY, autoscaleX, autoscaleY, xmin, xmax, ymin, ymax, projectName, selectedCurves);
	}
}


bool Model::gridCompatible(bool grid1, int r1, int g1, int b1, bool grid2, int r2, int g2, int b2)
{
	if (grid1 != grid2) return false;

	if (grid1)
	{
		if (r1 != r2 || g1 != g2 || b1 != b2) return false;
	}

	return true;
}

bool Model::axisCompatible(bool logscale1, bool autoscale1, double min1, double max1, bool logscale2, bool autoscale2, double min2, double max2)
{
	if (logscale1 != logscale2) return false;
	if (autoscale1 != autoscale2) return false;

	if (!autoscale1)
	{
		if (fabs(min1 - min2) > 1e-10 || fabs(max1 - max2) > 1e-10) return false;
	}

	return true;
}

bool Model::plotCompatible(AbstractPlot::PlotType type1, PlotDataset::axisQuantity quantity1, AbstractPlot::PlotType type2, PlotDataset::axisQuantity quantity2) {
	if (type1 != type2) { return false; }
	if (type1 == AbstractPlot::Cartesian) {
		if (quantity1 != quantity2) { return false; }
	}
	return true;
}

SceneNodePlot1D* Model::getPlotFromCurve(SceneNodePlot1DCurve* curve)
{
	SceneNodePlot1D* plotItem = nullptr;

	SceneNodeBase *parent = curve->getParent();

	do 
	{
		plotItem = dynamic_cast<SceneNodePlot1D*>(parent);

		parent = parent->getParent();

	} while (parent != nullptr && plotItem == nullptr);

	return plotItem;
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

void Model::setItemVisibleState(SceneNodeBase *item, bool visible)
{
	if (visible != item->isVisible())
	{
		item->setVisible(visible);
		getNotifier()->setTreeItemIcon(item->getTreeItemID(), item->getTreeIcons().size, item->isVisible() ? item->getTreeIcons().visibleIcon : item->getTreeIcons().hiddenIcon);

		manageParentVisibility(item);
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
	if (nameToSceneNodesMap.count(branchName) > 0)
	{
		SceneNodeBase *rootNode = nameToSceneNodesMap[branchName];

		// Now show the entities of the selected branch
		std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
		std::string projectName;

		getPrefetchForAllSceneNodes(rootNode, projectName, prefetchIDs);

		if (!prefetchIDs.empty())
		{
			DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIDs);
		}

		showAllSceneNodes(rootNode);
	}

	refreshAllViews();
}

void Model::hideBranch(const std::string &branchName)
{
	if (nameToSceneNodesMap.count(branchName) > 0)
	{
		SceneNodeBase *rootNode = nameToSceneNodesMap[branchName];

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

void Model::setupUIControls3D(void)
{
	assert(getNotifier() != nullptr);
	if (getNotifier() == nullptr) return;
	if (!removeItemIDList.empty()) return;

	removeItemIDList.push_front(uiControls.viewPageID					= getNotifier()->addMenuPage("View"));

	removeItemIDList.push_front(uiControls.operationsGroupID			= getNotifier()->addMenuGroup(uiControls.viewPageID, "Operations"));
	removeItemIDList.push_front(uiControls.visiblityGroupID				= getNotifier()->addMenuGroup(uiControls.viewPageID, "Visibility"));
	removeItemIDList.push_front(uiControls.styleGroupID					= getNotifier()->addMenuGroup(uiControls.viewPageID, "Display Style"));

	removeItemIDList.push_front(uiControls.resetView3DButtonID			= getNotifier()->addMenuPushButton(uiControls.operationsGroupID, "Reset View", "ResetView", "Space"));

	removeItemIDList.push_front(uiControls.showAllButtonID				= getNotifier()->addMenuPushButton(uiControls.visiblityGroupID, "Show All", "ShowAll"));
	removeItemIDList.push_front(uiControls.showSelectedButtonID			= getNotifier()->addMenuPushButton(uiControls.visiblityGroupID, "Show Selected", "ShowSelected", "Ctrl+S"));
	removeItemIDList.push_front(uiControls.hideSelectedButtonID			= getNotifier()->addMenuPushButton(uiControls.visiblityGroupID, "Hide Selected", "HideSelected", "Ctrl+H"));
	removeItemIDList.push_front(uiControls.hideUnselectedButtonID		= getNotifier()->addMenuPushButton(uiControls.visiblityGroupID, "Hide Unselected", "HideUnselected"));

	removeItemIDList.push_front(uiControls.stateWireframeButtonID		= getNotifier()->addMenuPushButton(uiControls.styleGroupID, "Wireframe", "Wireframe"));
	removeItemIDList.push_front(uiControls.stateWorkingPlaneButtonID	= getNotifier()->addMenuPushButton(uiControls.styleGroupID, "Working plane", "WorkingPlane",
		                                                                      ot::ContextMenu("WorkingPlaneContext") << ot::ContextMenuItem("Settings", "Settings", "Settings", ot::ContextMenuItemRole(ot::ContextMenuItemRole::SettingsReference, "Viewer:WorkingPlane"))));
	removeItemIDList.push_front(uiControls.stateAxisCrossButtonID       = getNotifier()->addMenuPushButton(uiControls.styleGroupID, "Axis cross", "AxisCross",
		                                                                      ot::ContextMenu("AxisCrossContext") << ot::ContextMenuItem("Settings", "Settings", "Settings", ot::ContextMenuItemRole(ot::ContextMenuItemRole::SettingsReference, "Viewer:AxisCross"))));
	removeItemIDList.push_front(uiControls.stateCenterAxisCrossButtonID = getNotifier()->addMenuPushButton(uiControls.styleGroupID, "Center axis cross", "CenterAxisCross",	ot::ContextMenu("CenterAxisCrossContext") << ot::ContextMenuItem("Settings", "Settings", "Settings", ot::ContextMenuItemRole(ot::ContextMenuItemRole::SettingsReference, "Viewer:AxisCross"))));
	removeItemIDList.push_front(uiControls.cutplaneButtonID             = getNotifier()->addMenuPushButton(uiControls.styleGroupID, "Cutplane", "Cutplane",	ot::ContextMenu("CutplaneContext") << ot::ContextMenuItem("Settings", "Settings", "Settings", ot::ContextMenuItemRole(ot::ContextMenuItemRole::SettingsReference, "Viewer:Cutplane"))));

	// Send an initial notification to properly set the state of the new controls
	std::list<ot::UID> selectedTreeItemID;
	updateUIControlState(selectedTreeItemID);
}

void Model::setupUIControls1D(void)
{
	assert(getNotifier() != nullptr);
	if (getNotifier() == nullptr) return;
	if (!removeItemIDList.empty()) return;

	removeItemIDList.push_front(uiControls.viewPageID = getNotifier()->addMenuPage("View"));

	removeItemIDList.push_front(uiControls.operationsGroupID = getNotifier()->addMenuGroup(uiControls.viewPageID, "Operations"));
	removeItemIDList.push_front(uiControls.visiblityGroupID = getNotifier()->addMenuGroup(uiControls.viewPageID, "Visibility"));

	removeItemIDList.push_front(uiControls.resetView1DButtonID = getNotifier()->addMenuPushButton(uiControls.operationsGroupID, "Reset View", "ResetView", "Space"));

	removeItemIDList.push_front(uiControls.showSelectedButtonID = getNotifier()->addMenuPushButton(uiControls.visiblityGroupID, "Show Selected", "ShowSelected", "Ctrl+S"));
	removeItemIDList.push_front(uiControls.hideSelectedButtonID = getNotifier()->addMenuPushButton(uiControls.visiblityGroupID, "Hide Selected", "HideSelected", "Ctrl+H"));

	// Send an initial notification to properly set the state of the new controls
	std::list<ot::UID> selectedTreeItemID;
	updateUIControlState(selectedTreeItemID);
}

void Model::removeUIControls(void)
{
	if (removeItemIDList.empty()) return;

	assert(getNotifier() != nullptr);
	if (getNotifier() == nullptr) return;

	getNotifier()->removeUIElements(removeItemIDList);
	removeItemIDList.clear();
}

void Model::executeAction(unsigned long long buttonID)
{
	     if (buttonID == uiControls.resetView3DButtonID)   	         resetAllViews3D();
	else if (buttonID == uiControls.resetView1DButtonID)	         resetAllViews1D();
	else if (buttonID == uiControls.showAllButtonID)                 showAllSceneNodesAction();
	else if (buttonID == uiControls.showSelectedButtonID)	         showSelectedSceneNodesAction();
	else if (buttonID == uiControls.hideSelectedButtonID)            hideSelectedSceneNodesAction();
	else if (buttonID == uiControls.hideUnselectedButtonID)          hideUnselectedSceneNodesAction();
	else if (buttonID == uiControls.stateWireframeButtonID)          toggleWireframeView();
	else if (buttonID == uiControls.stateWorkingPlaneButtonID)       toggleWorkingPlane();
	else if (buttonID == uiControls.stateAxisCrossButtonID)          toggleAxisCross();
	else if (buttonID == uiControls.stateCenterAxisCrossButtonID)    toggleCenterAxisCross();
	else if (buttonID == uiControls.cutplaneButtonID)				 toggleCutplane();
	else assert(0); // Unhandled button action

	refreshAllViews();
}

void Model::showAllSceneNodesAction(void)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
	std::string projectName;

	getPrefetchForAllSceneNodes(sceneNodesRoot, projectName, prefetchIDs);

	if (!prefetchIDs.empty())
	{
		DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIDs);
	}

	showAllSceneNodes(sceneNodesRoot);
	if (getNotifier() != nullptr) getNotifier()->refreshSelection();
}

void Model::showSelectedSceneNodesAction(void)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
	std::string projectName;

	getPrefetchForSelectedSceneNodes(sceneNodesRoot, projectName, prefetchIDs);

	if (!prefetchIDs.empty())
	{
		DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIDs);
	}

	showSelectedSceneNodes(sceneNodesRoot);
	if (getNotifier() != nullptr) getNotifier()->refreshSelection();
}

void Model::hideSelectedSceneNodesAction(void)
{
	hideSelectedSceneNodes(sceneNodesRoot);

	if (getNotifier() != nullptr) getNotifier()->refreshSelection();
}

void Model::hideUnselectedSceneNodesAction(void)
{
	hideUnselectedSceneNodes(sceneNodesRoot);

	if (getNotifier() != nullptr) getNotifier()->refreshSelection();
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
		if (getNotifier() != nullptr)
		{
			if (!bCtrlKeyPressed)
			{
				getNotifier()->clearTreeSelection();
			}
		}
	}
	else
	{
		// Select the corresponding tree item
		if (getNotifier() != nullptr)
		{
			if (!bCtrlKeyPressed)
			{
				getNotifier()->selectSingleTreeItem(selectedItem->getTreeItemID());
			}
			else
			{
				getNotifier()->toggleTreeItemSelection(selectedItem->getTreeItemID(), true);
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

void Model::updateUIControlState(std::list<ot::UID> &selectedTreeItemID)
{
	if (!removeItemIDList.empty())
	{
		std::list<unsigned long long> enabled;
		std::list<unsigned long long> disabled;

		if (selectedTreeItemID.size() > 0)
		{
			enabled.push_back(uiControls.showSelectedButtonID);
			enabled.push_back(uiControls.hideSelectedButtonID);
			enabled.push_back(uiControls.hideUnselectedButtonID);
		}
		else
		{
			disabled.push_back(uiControls.showSelectedButtonID);
			disabled.push_back(uiControls.hideSelectedButtonID);
			disabled.push_back(uiControls.hideUnselectedButtonID);
		}

		getNotifier()->enableDisableControls(enabled, disabled);
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

	// Ensure that we are not deleting the current hover item
	if (currentHoverItem == node)
	{
		currentHoverItem = nullptr; // We don't need to reset any display information, since the object will be deleted anyway.
	}

	// Store the current status of the item, if tree state recording is turned on
	if (treeStateRecording)
	{
		bool isExpanded = getNotifier()->isTreeItemExpanded(node->getTreeItemID());
		treeInfoMap[node->getName()] = (node->isSelected() ? ITEM_SELECTED : 0) | (isExpanded ? ITEM_EXPANDED : 0);
	}

	// Remove the current scene node from the maps  
	nameToSceneNodesMap.erase(node->getName());
	osgNodetoSceneNodesMap.erase(node->getShapeNode());
	treeItemToSceneNodesMap.erase(node->getTreeItemID());
	modelItemToSceneNodesMap.erase(node->getModelEntityID());

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
	getNotifier()->removeTreeItems(treeItemDeleteList);

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
				if (nameToSceneNodesMap.count(item) != 0)
				{
					objects.push_back(nameToSceneNodesMap[item]);
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
			getNotifier()->entitiesSelected(currentSelectionReplyTo, currentSelectionAction, selectionInfo, currentSelectionOptionNames, currentSelectionOptionValues);

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
			getNotifier()->entitiesSelected(currentSelectionReplyTo, currentSelectionAction, selectionInfo, currentSelectionOptionNames, currentSelectionOptionValues);

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
			getNotifier()->entitiesSelected(currentSelectionReplyTo, currentSelectionAction, selectionInfo, currentSelectionOptionNames, currentSelectionOptionValues);
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
						selectedItem->setEdgeHighlight(faceId, true, 3.0);
					}
					else
					{
						selectedItem->setEdgeHighlight(faceId, true, 1.0);
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

void Model::addSceneNode(SceneNodeBase *node) 
{ 
	osgNodetoSceneNodesMap[node->getShapeNode()] = node; 
}

void Model::setSceneNode(osg::Node *node, SceneNodeBase *sceneNode)
{
	osgNodetoSceneNodesMap[node] = sceneNode;
}

void Model::removeSceneNode(osg::Node *node)
{
	osgNodetoSceneNodesMap.erase(node);
}

void Model::addVisualizationMeshNodeFromFacetDataBase(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, double edgeColorRGB[3], bool displayTetEdges, const std::string &projectName, unsigned long long entityID, unsigned long long version)
{
	SceneNodeMesh *meshNode = new SceneNodeMesh;

	meshNode->setName(treeName);
	meshNode->setModelEntityID(modelEntityID);
	meshNode->setTreeIcons(treeIcons);
	meshNode->setDisplayTetEdges(displayTetEdges);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = meshNode;
	treeItemToSceneNodesMap[meshNode->getTreeItemID()] = meshNode;
	modelItemToSceneNodesMap[modelEntityID] = meshNode;

	meshNode->setStorage(projectName, entityID, version);
	meshNode->setEdgeColor(edgeColorRGB);

	meshNode->setModel(this);
	meshNode->setNeedsInitialization();
}

void Model::addVisualizationMeshItemNodeFromFacetDataBase(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, 
														  unsigned long long entityID, unsigned long long version, long long tetEdgesID, long long tetEdgesVersion)
{
	SceneNodeMeshItem *meshNode = new SceneNodeMeshItem;

	meshNode->setName(treeName);
	meshNode->setModelEntityID(modelEntityID);
	meshNode->setTreeIcons(treeIcons);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = meshNode;
	treeItemToSceneNodesMap[meshNode->getTreeItemID()] = meshNode;
	modelItemToSceneNodesMap[modelEntityID] = meshNode;

	meshNode->setStorage(projectName, entityID, version, tetEdgesID, tetEdgesVersion);

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

void Model::addVisualizationCartesianMeshNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, double edgeColorRGB[3], double meshLineColorRGB[3], bool showMeshLines, const std::vector<double> &meshCoordsX, const std::vector<double> &meshCoordsY, const std::vector<double> &meshCoordsZ,
											  const std::string &projectName, unsigned long long faceListEntityID, unsigned long long faceListEntityVersion, unsigned long long nodeListEntityID, unsigned long long nodeListEntityVersion)
{
	SceneNodeCartesianMesh *meshNode = new SceneNodeCartesianMesh;

	meshNode->setName(treeName);
	meshNode->setModelEntityID(modelEntityID);
	meshNode->setTreeIcons(treeIcons);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Now add the current nodes osg node to the parent's osg node
	parentNode->getShapeNode()->addChild(meshNode->getShapeNode());

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = meshNode;
	osgNodetoSceneNodesMap[meshNode->getShapeNode()] = meshNode;
	treeItemToSceneNodesMap[meshNode->getTreeItemID()] = meshNode;
	modelItemToSceneNodesMap[modelEntityID] = meshNode;

	meshNode->setEdgeColor(edgeColorRGB);
	meshNode->setMeshLineColor(meshLineColorRGB);
	meshNode->setMeshLines(meshCoordsX, meshCoordsY, meshCoordsZ);
	meshNode->setFaceListStorage(projectName, faceListEntityID, faceListEntityVersion);
	meshNode->setNodeListStorage(nodeListEntityID, nodeListEntityVersion);
	meshNode->setNeedsInitialization();

	meshNode->setModel(this);

	if (!isHidden)
	{
		meshNode->ensureDataLoaded();
	}
	else
	{
		setItemVisibleState(meshNode, false);
	}

	meshNode->showMeshLines(showMeshLines);
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

void Model::addVisualizationCartesianMeshItemNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, std::vector<int> &facesList, double color[3])
{
	SceneNodeCartesianMeshItem *meshNode = new SceneNodeCartesianMeshItem;

	meshNode->setName(treeName);
	meshNode->setModelEntityID(modelEntityID);
	meshNode->setTreeIcons(treeIcons);
	meshNode->setModel(this);
	meshNode->setWireframe(wireFrameState);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(meshNode);

	// Add the tree name to the tree
	addSceneNodesToTree(meshNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = meshNode;
	treeItemToSceneNodesMap[meshNode->getTreeItemID()] = meshNode;
	modelItemToSceneNodesMap[modelEntityID] = meshNode;

	// Now find the owning mesh
	SceneNodeBase *masterMeshNode = parentNode;
	while (dynamic_cast<SceneNodeCartesianMesh*>(masterMeshNode) == nullptr)
	{
		masterMeshNode = masterMeshNode->getParent();
		if (masterMeshNode == nullptr) break;
	}

	meshNode->setMesh(dynamic_cast<SceneNodeCartesianMesh*>(masterMeshNode));

	// Set the data
	meshNode->setColor(color[0], color[1], color[2]);
	meshNode->setFacesList(facesList);

	if (isHidden)
	{
		setItemVisibleState(meshNode, false);
	}
}

void Model::addVisualizationPlot1DNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden,
									   const std::string &projectName, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
									   bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax, std::list<unsigned long long> &curvesID, std::list<unsigned long long> &curvesVersions,
									   std::list<std::string> &curvesNames)
{
	SceneNodePlot1D* plotNode = new SceneNodePlot1D;

	plotNode->setName(treeName);
	plotNode->setModelEntityID(modelEntityID);
	plotNode->setTreeIcons(treeIcons);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(plotNode);

	// Add the tree name to the tree
	addSceneNodesToTree(plotNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = plotNode;
	treeItemToSceneNodesMap[plotNode->getTreeItemID()] = plotNode;
	modelItemToSceneNodesMap[modelEntityID] = plotNode;

	plotNode->setProjectName(projectName);
	plotNode->setTitle(title);

	bool isComplex;
	plotNode->setPlotType(AbstractPlot::plotTypeFromString(plotType, isComplex));
	if (isComplex) { plotNode->setPlotQuantity(PlotDataset::Complex); }
	else { plotNode->setPlotQuantity(PlotDataset::plotQuantityFromString(plotQuantity)); }
	
	plotNode->setCurves(curvesID, curvesVersions, curvesNames);
	plotNode->setGrid(grid);
	plotNode->setGridColor(gridColor);
	plotNode->setLegend(legend);
	plotNode->setLogscaleX(logscaleX);
	plotNode->setLogscaleY(logscaleY);
	plotNode->setAutoscaleX(autoscaleX);
	plotNode->setAutoscaleY(autoscaleY);
	plotNode->setXmin(xmin);
	plotNode->setXmax(xmax);
	plotNode->setYmin(ymin);
	plotNode->setYmax(ymax);

	plotNode->setModel(this);

	if (isHidden)
	{
		setItemVisibleState(plotNode, false);
	}

	// Create the nodes for the curve

	TreeIcon curveTreeIcons;
	curveTreeIcons.size = 32;
	curveTreeIcons.visibleIcon = "Result1DVisible";
	curveTreeIcons.hiddenIcon = "Result1DHidden";

	for (unsigned long long n = 0; n < plotNode->getNumberOfCurves(); n++)
	{
		unsigned long long curveID = plotNode->getCurveID(n);
		unsigned long long curveVersion = plotNode->getCurveVersion(n);
		std::string curveName = plotNode->getCurveName(n);

		addVisualizationResult1DNode(treeName + "/" + curveName, curveID, curveVersion, curveTreeIcons, isHidden, projectName);
	}
}

void Model::addVisualizationResult1DNode(const std::string &treeName, unsigned long long curveEntityID, unsigned long long curveEntityVersion, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName)
{
	SceneNodePlot1DCurve* curveNode = new SceneNodePlot1DCurve;

	curveNode->setName(treeName);
	curveNode->setModelEntityID(curveEntityID);
	curveNode->setModelEntityVersion(curveEntityVersion);
	curveNode->setTreeIcons(treeIcons);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(curveNode);

	// Add the tree name to the tree
	addSceneNodesToTree(curveNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = curveNode;
	treeItemToSceneNodesMap[curveNode->getTreeItemID()] = curveNode;
	modelItemToSceneNodesMap[curveEntityID] = curveNode;

	curveNode->setStorage(projectName);

	curveNode->setModel(this);

	if (isHidden)
	{
		setItemVisibleState(curveNode, false);
	}
}

void Model::addVisualizationTextNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long textEntityID, unsigned long long textEntityVersion)
{
	SceneNodeTextItem* textNode = new SceneNodeTextItem;

	textNode->setName(treeName);
	textNode->setModelEntityID(modelEntityID);
	textNode->setTreeIcons(treeIcons);
	textNode->addVisualiserText();

	// Get the parent scene node
	SceneNodeBase* parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

	// Now add the current node as child to the parent
	parentNode->addChild(textNode);

	// Add the tree name to the tree
	addSceneNodesToTree(textNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = textNode;
	treeItemToSceneNodesMap[textNode->getTreeItemID()] = textNode;
	modelItemToSceneNodesMap[modelEntityID] = textNode;

	textNode->setStorage(projectName);

	textNode->setModel(this);

	if (isHidden)
	{
		setItemVisibleState(textNode, false);
	}
}

void Model::addVisualizationTableNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, const std::string &projectName, unsigned long long tableEntityID, unsigned long long tableEntityVersion)
{
	SceneNodeTableItem *tableNode = new SceneNodeTableItem;

	tableNode->setName(treeName);
	tableNode->setModelEntityID(modelEntityID);

	tableNode->setTableEntityID(tableEntityID);
	tableNode->setTableEntityVersion(tableEntityVersion);
	tableNode->setTreeIcons(treeIcons);

	// Get the parent scene node
	SceneNodeBase *parentNode = getParentNode(treeName);
	assert(parentNode != nullptr); // We assume that the parent node already exists

								   // Now add the current node as child to the parent
	parentNode->addChild(tableNode);

	// Add the tree name to the tree
	addSceneNodesToTree(tableNode);

	// Add the node to the maps for faster access
	nameToSceneNodesMap[treeName] = tableNode;
	treeItemToSceneNodesMap[tableNode->getTreeItemID()] = tableNode;
	modelItemToSceneNodesMap[modelEntityID] = tableNode;

	tableNode->setStorage(projectName);

	tableNode->setModel(this);

	if (isHidden)
	{
		setItemVisibleState(tableNode, false);
	}
}

void Model::visualizationResult1DPropertiesChanged(unsigned long long entityID, unsigned long long version)
{
	bool needsRedraw = changeResult1DEntityVersion(sceneNodesRoot, entityID, version);

	if (needsRedraw)
	{
		update1DPlot(sceneNodesRoot);
	}
}

bool Model::changeResult1DEntityVersion(SceneNodeBase *root, unsigned long long entityID, unsigned long long version)
{
	SceneNodePlot1D *plot = dynamic_cast<SceneNodePlot1D*>(root);
	SceneNodePlot1DCurve* curve = dynamic_cast<SceneNodePlot1DCurve*>(root);

	bool needsRedraw = false;

	if (plot != nullptr)
	{
		needsRedraw |= plot->changeResult1DEntityVersion(entityID, version);
	}

	if (curve != nullptr)
	{
		if (curve->getModelEntityID() == entityID)
		{
			if (curve->getModelEntityVersion() != version)
			{
				curve->setModelEntityVersion(version);
				needsRedraw = true;
			}
		}
	}

	for (auto child : root->getChildren())
	{
		needsRedraw |= changeResult1DEntityVersion(child, entityID, version);
	}

	return needsRedraw;
}

void Model::visualizationPlot1DPropertiesChanged(unsigned long long modelEntityID, const std::string &title, const std::string &plotType, const std::string &plotQuantity, bool grid, int gridColor[], bool legend, bool logscaleX, bool logscaleY,
												 bool autoscaleX, bool autoscaleY, double xmin, double xmax, double ymin, double ymax)
{
	SceneNodePlot1D* plotNode = dynamic_cast<SceneNodePlot1D*>(modelItemToSceneNodesMap[modelEntityID]);
	assert(plotNode != nullptr);

	if (plotNode == nullptr) return;

	plotNode->setTitle(title);

	bool isComplex;
	plotNode->setPlotType(AbstractPlot::plotTypeFromString(plotType, isComplex));
	if (isComplex) { plotNode->setPlotQuantity(PlotDataset::Complex); }
	else { plotNode->setPlotQuantity(PlotDataset::plotQuantityFromString(plotQuantity)); }
	
	plotNode->setGrid(grid);
	plotNode->setGridColor(gridColor);
	plotNode->setLegend(legend);
	plotNode->setLogscaleX(logscaleX);
	plotNode->setLogscaleY(logscaleY);
	plotNode->setAutoscaleX(autoscaleX);
	plotNode->setAutoscaleY(autoscaleY);
	plotNode->setXmin(xmin);
	plotNode->setXmax(xmax);
	plotNode->setYmin(ymin);
	plotNode->setYmax(ymax);

	update1DPlot(sceneNodesRoot);
}

void Model::viewerTabChanged(const std::string& _tabTitle, ot::WidgetViewBase::ViewType _type)
{
	removeUIControls();

	if (_type == ot::WidgetViewBase::View3D)
	{
		setupUIControls3D();
	}
	else if(_type == ot::WidgetViewBase::View1D)
	{
		setupUIControls1D();
	}
	else
	{
		// Now there may be other tabs in the UI, created by a ui plugin
		//assert(0); // Unknown tab title
	}
}

void Model::set1DPlotItemSelected(unsigned long long treeItemID, bool ctrlPressed)
{
	if (!ctrlPressed)
	{
		getNotifier()->selectSingleTreeItem(treeItemID);
	}
	else
	{
		SceneNodeBase *treeItem = treeItemToSceneNodesMap[treeItemID];
		assert(treeItem != nullptr);

		if (treeItem != nullptr)
		{
			// Check, if the corresponding plot is selected and the current child will be deselected. If so, toggle the selection of this item
			if (treeItem->isSelected())
			{
				SceneNodePlot1DCurve* curve = dynamic_cast<SceneNodePlot1DCurve*>(treeItem);

				if (curve != nullptr)
				{
					SceneNodePlot1D* plot = getPlotFromCurve(curve);
					if (plot != nullptr)
					{
						if (plot->isSelected())
						{
							getNotifier()->toggleTreeItemSelection(plot->getTreeItemID(), false);
						}
					}
				}
			}

			getNotifier()->toggleTreeItemSelection(treeItemID, false);
		}
	}
}

void Model::reset1DPlotItemSelection(void)
{
	getNotifier()->clearTreeSelection();
}

void Model::addVTKNode(const std::string &treeName, unsigned long long modelEntityID, const TreeIcon &treeIcons, bool isHidden, bool isEditable, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion)
{
	// Check whether the item already exists
	SceneNodeBase *item = modelItemToSceneNodesMap[modelEntityID];

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

		vtkNode->setName(treeName);
		vtkNode->setModelEntityID(modelEntityID);
		vtkNode->setTreeIcons(treeIcons);
		vtkNode->setEditable(isEditable);

		// Get the parent scene node
		SceneNodeBase *parentNode = getParentNode(treeName);
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
		if (isHidden)
		{
			setItemVisibleState(vtkNode, false);
		}

		vtkNode->setModel(this);

		// Now update the vtk node
		vtkNode->updateVTKNode(projectName, visualizationDataID, visualizationDataVersion);

		// Add the node to the maps for faster access
		nameToSceneNodesMap[treeName] = vtkNode;
		treeItemToSceneNodesMap[vtkNode->getTreeItemID()] = vtkNode;
		modelItemToSceneNodesMap[modelEntityID] = vtkNode;
	}
}

void Model::updateVTKNode(unsigned long long modelEntityID, const std::string &projectName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion)
{
	SceneNodeVTK *vtkNode = dynamic_cast<SceneNodeVTK *>(modelItemToSceneNodesMap[modelEntityID]);
	assert(vtkNode != nullptr);
	if (vtkNode == nullptr) return;

	vtkNode->updateVTKNode(projectName, visualizationDataID, visualizationDataVersion);
}

void Model::updateCapGeometry(osg::Vec3d normal, osg::Vec3d point)
{
	// This function gets called whenever the orientation or position of the cutplane is changed and the cap geometry therefore needs to be updated

	// First, we traverse the tree and find all geometry nodes
	updateCapGeometryForSceneNodes(sceneNodesRoot, normal, point);
}

std::list<std::string> Model::getSelectedCurves()
{
	ot::UIDList treeItemIDs;
	getSelectedTreeItemIDs(treeItemIDs);
	std::list<std::string> curveDescriptions;
	for (ot::UID treeID : treeItemIDs)
	{
		auto sceneNodeBaseByUID = treeItemToSceneNodesMap.find(treeID);
		assert(sceneNodeBaseByUID != treeItemToSceneNodesMap.end());
		auto sceneNodeBase = sceneNodeBaseByUID->second;
		SceneNodePlot1DCurve* curve = dynamic_cast<SceneNodePlot1DCurve*>(sceneNodeBase);
		if (curve != nullptr)
		{
			curveDescriptions.push_back(curve->getName());
		}
	}
	return curveDescriptions;
}

void Model::removedSelectedCurveNodes()
{
	ot::UIDList treeItemIDs;
	getSelectedTreeItemIDs(treeItemIDs);
	ot::UIDList treeItemDeleteList;  // We group the tree item deletions for better performance
	for (ot::UID treeID : treeItemIDs)
	{
		auto sceneNodeBaseByUID = treeItemToSceneNodesMap.find(treeID);
		assert(sceneNodeBaseByUID != treeItemToSceneNodesMap.end());
		auto sceneNodeBase = sceneNodeBaseByUID->second;
		SceneNodePlot1DCurve* curve = dynamic_cast<SceneNodePlot1DCurve*>(sceneNodeBase);
		
		if (curve!= nullptr)
		{
			removeSceneNodeAndChildren(curve, treeItemDeleteList);
		}
	}
	// Remove the shapes from the tree
	getNotifier()->removeTreeItems(treeItemDeleteList);
}



void Model::updateCapGeometryForSceneNodes(SceneNodeBase *root, const osg::Vec3d &normal, const osg::Vec3d &point)
{
	if (root->isVisible() && dynamic_cast<SceneNodeGeometry*>(root) != nullptr)
	{
		// We have a SceneNodeGeometry item -> process it
		updateCapGeometryForGeometryItem(dynamic_cast<SceneNodeGeometry*>(root), normal, point);
	}

	for (auto child : root->getChildren())
	{
		updateCapGeometryForSceneNodes(child, normal, point);
	}
}

void Model::updateCapGeometryForGeometryItem(SceneNodeGeometry *item, const osg::Vec3d &normal, const osg::Vec3d &point)
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
		OutputDebugString(L"No intersection\n\n");
	}
	else {
		OutputDebugString(L"Intersection\n\n");
	}

}
