
#include "EntityVis2D3D.h"
#include "Types.h"
#include "Database.h"

#include <OpenTwinCommunication/ActionTypes.h>

#include <bsoncxx/builder/basic/array.hpp>

EntityVis2D3D::EntityVis2D3D(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner) :
	EntityContainer(ID, parent, obs, ms, factory, owner),
	resultType(EntityResultBase::UNKNOWN),
	sourceID(0),
	sourceVersion(0),
	meshID(0),
	meshVersion(0),
	visualizationDataID(0),
	visualizationDataVersion(0)
	//source(nullptr)
{
}

EntityVis2D3D::~EntityVis2D3D()
{
	//if (source != nullptr)
	//{
	//	delete source;
	//	source = nullptr;
	//}
}

void EntityVis2D3D::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::AddStorageData(storage);

	storage.append(
		bsoncxx::builder::basic::kvp("visualizationDataID", (long long) visualizationDataID),
		bsoncxx::builder::basic::kvp("visualizationDataVersion", (long long) visualizationDataVersion),
		bsoncxx::builder::basic::kvp("sourceID", (long long) sourceID),
		bsoncxx::builder::basic::kvp("sourceVersion", (long long) sourceVersion),
		bsoncxx::builder::basic::kvp("meshID", (long long) meshID),
		bsoncxx::builder::basic::kvp("meshVersion", (long long) meshVersion)
	);
}

void EntityVis2D3D::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	try {
		visualizationDataID      = doc_view["visualizationDataID"].get_int64();
		visualizationDataVersion = doc_view["visualizationDataVersion"].get_int64();
	}
	catch (std::exception)
	{
		visualizationDataID      = 0;
		visualizationDataVersion = 0;
	}

	try {
		sourceID      = doc_view["sourceID"].get_int64();
		sourceVersion = doc_view["sourceVersion"].get_int64();
	}
	catch (std::exception)
	{
		sourceID      = 0;
		sourceVersion = 0;
	}

	try {
		meshID      = doc_view["meshID"].get_int64();
		meshVersion = doc_view["meshVersion"].get_int64();
	}
	catch (std::exception)
	{
		meshID      = 0;
		meshVersion = 0;
	}

	resetModified();
}

void EntityVis2D3D::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "Vis2DVisible";
		treeIcons.hiddenIcon = "Vis2DHidden";

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddVis2D3DNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());
		ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, DataBase::GetDataBase()->getProjectName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_DataID, visualizationDataID);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_DataVersion, visualizationDataVersion);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_SourceID, sourceID);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_SourceVersion, sourceVersion);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_MeshID, meshID);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_MeshVersion, meshVersion);
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsHidden, getInitiallyHidden());
		
		treeIcons.addToJsonDoc(&doc);

		getObserver()->sendMessageToViewer(doc);
	}

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityVis2D3D::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);
	
	propertyBundlePlane.SetProperties(this);
	propertyBundleScaling.SetProperties(this);
	propertyBundleVis2D3D.SetProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVis2D3D::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	updatePropertiesGrid = propertyBundlePlane.UpdatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleScaling.UpdatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleVis2D3D.UpdatePropertyVisibility(this);

	return updatePropertiesGrid;
}


bool EntityVis2D3D::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}

//void EntityVis2D3D::ensureSourceDataLoaded(void)
//{
//	if (source == nullptr)
//	{
//		if (sourceID != 0 && sourceVersion != 0)
//		{
//			assert(sourceID != 0);
//			assert(sourceVersion != 0);
//
//			std::map<ot::UID, EntityBase *> entityMap;
//			source = dynamic_cast<EntityResultBase *>(readEntityFromEntityIDAndVersion(this, sourceID, sourceVersion, entityMap));
//
//			if (source != nullptr)
//			{
//				assert(source->getResultType() == getResultType());
//			}
//		}
//	}
//}
