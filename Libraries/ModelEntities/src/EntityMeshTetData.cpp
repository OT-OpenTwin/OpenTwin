
#include "EntityMeshTetData.h"
#include "EntityMeshTetNodes.h"
#include "EntityMeshTetFace.h"
#include "EntityMeshTetFaceData.h"
#include "DataBase.h"
#include "Types.h"

#include <OpenTwinCommunication/ActionTypes.h>

#include <bsoncxx/builder/basic/array.hpp>

EntityMeshTetData::EntityMeshTetData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner) :
	EntityContainer(ID, parent, obs, ms, factory, owner),
	meshNodes(nullptr),
	meshNodesStorageId(-1),
	meshFaces(nullptr),
	meshFacesStorageId(-1),
	gmshDataStorageId(-1)
{
	
}

EntityMeshTetData::~EntityMeshTetData()
{
	if (meshNodes != nullptr) delete meshNodes;
	meshNodes = nullptr;

	if (meshFaces != nullptr) delete meshFaces;
	meshFaces = nullptr;
}

bool EntityMeshTetData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

EntityMeshTetFace *EntityMeshTetData::getFace(int faceId)
{
	EnsureFacesLoaded();
	assert(meshFaces != nullptr);

	return meshFaces->getFace(faceId);
}

void EntityMeshTetData::setFace(int faceId, EntityMeshTetFace *face)
{
	EnsureFacesLoaded();
	assert(meshFaces != nullptr);

	return meshFaces->setFace(faceId, face);
}

void EntityMeshTetData::EnsureFacesLoaded(void)
{
	if (meshFaces == nullptr)
	{
		if (meshFacesStorageId == -1)
		{
			meshFaces = new EntityMeshTetFaceData(0, this, getObserver(), getModelState(), getClassFactory(), getOwningService());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshFaces = dynamic_cast<EntityMeshTetFaceData *>(readEntityFromEntityID(this, meshFacesStorageId, entityMap));
		}
	}
}

void EntityMeshTetData::storeMeshFaces(void)
{
	assert(meshFaces != nullptr);

	// Assign an entity id if needed
	if (meshFaces->getEntityID() == 0)
	{
		meshFaces->setEntityID(createEntityUID());
	}

	meshFaces->StoreToDataBase();
	meshFacesStorageId = meshFaces->getEntityID();
}

void EntityMeshTetData::releaseMeshFaces(void)
{
	if (meshFaces == nullptr) return;

	storeMeshFaces();

	delete meshFaces;
	meshFaces = nullptr;
}

void EntityMeshTetData::getNodeCoords(size_t node, double coord[3])
{
	EnsureNodesLoaded();

	assert(meshNodes != nullptr);

	coord[0] = meshNodes->getNodeCoordX(node);
	coord[1] = meshNodes->getNodeCoordY(node);
	coord[2] = meshNodes->getNodeCoordZ(node);
}

double EntityMeshTetData::getNodeCoordX(size_t node)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	return meshNodes->getNodeCoordX(node);
}

double EntityMeshTetData::getNodeCoordY(size_t node)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	return meshNodes->getNodeCoordY(node);
}

double EntityMeshTetData::getNodeCoordZ(size_t node)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	return meshNodes->getNodeCoordZ(node);
}

void EntityMeshTetData::setNodeCoordX(size_t node, double x)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNodeCoordX(node, x);
}

void EntityMeshTetData::setNodeCoordY(size_t node, double y)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNodeCoordY(node, y);
}

void EntityMeshTetData::setNodeCoordZ(size_t node, double z)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNodeCoordZ(node, z);
}

void EntityMeshTetData::setNumberOfNodes(size_t n)
{
	EnsureNodesLoaded();
	assert(meshNodes != nullptr);

	meshNodes->setNumberOfNodes(n);
}

void EntityMeshTetData::EnsureNodesLoaded(void)
{
	if (meshNodes == nullptr)
	{
		if (meshNodesStorageId == -1)
		{
			meshNodes = new EntityMeshTetNodes(0, this, getObserver(), getModelState(), getClassFactory(), getOwningService());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			meshNodes = dynamic_cast<EntityMeshTetNodes *>(readEntityFromEntityID(this, meshNodesStorageId, entityMap));
		}
	}
}

void EntityMeshTetData::storeMeshNodes(void)
{
	assert(meshNodes != nullptr);

	// Assign an entity id if needed
	if (meshNodes->getEntityID() == 0)
	{
		meshNodes->setEntityID(createEntityUID());
	}

	meshNodes->StoreToDataBase();
	meshNodesStorageId = meshNodes->getEntityID();
}

void EntityMeshTetData::releaseMeshNodes(void)
{
	if (meshNodes == nullptr) return;

	storeMeshNodes();

	delete meshNodes;
	meshNodes = nullptr;
}

void EntityMeshTetData::StoreToDataBase(void)
{
	// If the pointers to faces or nodes are 0, then the objects are stored in the data storage and the storage IDs are up to date
	if (meshFaces != nullptr)
	{
		storeMeshFaces();
	}

	if (meshNodes != nullptr)
	{
		storeMeshNodes();
	}

	// Afterward, we store the container itself
	EntityContainer::StoreToDataBase();
}

void EntityMeshTetData::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::AddStorageData(storage);

	// Now check whether the geometry is modified and we need to create a new entry

	long long meshNodesVersion = -1;
	long long meshFacesVersion = -1;

	if (meshNodesStorageId != -1)
	{
		meshNodesVersion = getMeshNodes() != nullptr ? getMeshNodes()->getEntityStorageVersion() : getCurrentEntityVersion(meshNodesStorageId);
	}

	if (meshFacesStorageId != -1)
	{
		meshFacesVersion = getMeshFaces() != nullptr ? getMeshFaces()->getEntityStorageVersion() : getCurrentEntityVersion(meshFacesStorageId);
	}

	storage.append(
		bsoncxx::builder::basic::kvp("MeshNodesID", meshNodesStorageId),
		bsoncxx::builder::basic::kvp("MeshNodesVersion", meshNodesVersion),
		bsoncxx::builder::basic::kvp("MeshFacesID", meshFacesStorageId),
		bsoncxx::builder::basic::kvp("MeshFacesVersion", meshFacesVersion),
		bsoncxx::builder::basic::kvp("GmshDataStorageID", gmshDataStorageId)
	);
}

void EntityMeshTetData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the coordinates

	// First reset the current mesh data
	if (meshNodes != nullptr)
	{
		delete meshNodes;
		meshNodes = nullptr;
	}

	if (meshFaces != nullptr)
	{
		delete meshFaces;
		meshFaces = nullptr;
	}

	// Now load the data from the storage

	meshNodesStorageId = doc_view["MeshNodesID"].get_int64();
	meshFacesStorageId = doc_view["MeshFacesID"].get_int64();

	gmshDataStorageId = -1;
	try
	{
		gmshDataStorageId = doc_view["GmshDataStorageID"].get_int64();
	}
	catch (std::exception)
	{
	}

	resetModified();
}

void EntityMeshTetData::releaseMeshData(void)
{
	releaseMeshFaces();
	releaseMeshNodes();
}

void EntityMeshTetData::addVisualizationItem(void)
{
	if (getObserver() == nullptr) return;

	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "ContainerVisible";
	treeIcons.hiddenIcon = "ContainerHidden";
	double edgeColorRGB[3] = { 1.0, 1.0, 1.0 };

	bool displayTetEdges = false;
	EntityPropertiesBoolean *showVolumeMesh = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Show volume mesh"));
	if (showVolumeMesh != nullptr)
	{
		displayTetEdges = showVolumeMesh->getValue();
	}

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshNodeFromFacetDatabase);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_UID, getEntityID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EDGE_COLOR_R, edgeColorRGB[0]);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EDGE_COLOR_G, edgeColorRGB[1]);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EDGE_COLOR_B, edgeColorRGB[2]);
	ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, DataBase::GetDataBase()->getProjectName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_ID, getEntityID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_Version, getCurrentEntityVersion(getEntityID()));
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES, displayTetEdges);
	
	treeIcons.addToJsonDoc(&doc);

	getObserver()->sendMessageToViewer(doc);
}

void EntityMeshTetData::addVisualizationNodes(void)
{
	addVisualizationItem();

	EntityContainer::addVisualizationNodes();
}

void EntityMeshTetData::removeChild(EntityBase *child)
{
	if (child == meshNodes)
	{
		meshNodes = nullptr;
	}
	else if (child == meshFaces)
	{
		meshFaces = nullptr;
	}
	else
	{
		EntityContainer::removeChild(child);
	}
}

bool EntityMeshTetData::updateFromProperties(void)
{
	EntityPropertiesBoolean *showVolumeMesh = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Show volume mesh"));
	if (showVolumeMesh != nullptr)
	{
		if (showVolumeMesh->needsUpdate())
		{
			// Since there is a change now, we need to set the modified flag
			setModified();

			bool displayTetEdges = showVolumeMesh->getValue();

			OT_rJSON_createDOC(doc);
			ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_TetMeshNodeTetEdges);
			ot::rJSON::add(doc, OT_ACTION_PARAM_UI_UID, getEntityID());
			ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES, displayTetEdges);

			getObserver()->sendMessageToViewer(doc);
		}
	}
	
	getProperties().forceResetUpdateForAllProperties();

	return false; // No property grid update necessary
}