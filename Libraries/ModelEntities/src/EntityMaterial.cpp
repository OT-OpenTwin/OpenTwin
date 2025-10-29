// @otlicense

// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityMaterial.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMaterial> registrar(EntityMaterial::className());

EntityMaterial::EntityMaterial(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, owner)
{
	
}

EntityMaterial::~EntityMaterial()
{
}

bool EntityMaterial::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityMaterial::storeToDataBase(void)
{
	EntityBase::storeToDataBase();
}

void EntityMaterial::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object
}


void EntityMaterial::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the material

	resetModified();
}

void EntityMaterial::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityMaterial::addVisualizationItem(bool isHidden)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "MaterialVisible";
	treeIcons.hiddenIcon = "MaterialHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);
}

void EntityMaterial::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityMaterial::createProperties(void)
{
	// General properties
	std::list<std::string> matTypes = { "Volumetric", "PEC" };
	EntityPropertiesSelection::createProperty("General", "Material type", matTypes, "Volumetric", "Material Types", getProperties());
	EntityPropertiesDouble::createProperty("General", "Mesh priority", 0.0, "Material Types", getProperties());

	// Electromagnetic properties
	EntityPropertiesDouble::createProperty("Electromagnetic", "Permittivity (relative)", 1.0, "Material Types", getProperties());
	EntityPropertiesDouble::createProperty("Electromagnetic", "Permeability (relative)", 1.0, "Material Types", getProperties());
	EntityPropertiesDouble::createProperty("Electromagnetic", "Conductivity", 0.0, "Material Types", getProperties());


	getProperties().forceResetUpdateForAllProperties();
}

bool EntityMaterial::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	bool updatePropertiesGrid = false;

	EntityPropertiesSelection *materialType = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Material type"));
	EntityPropertiesDouble *meshPriority = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Mesh priority"));
	EntityPropertiesDouble *permittivity = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Permittivity (relative)"));
	EntityPropertiesDouble *permeability = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Permeability (relative)"));
	EntityPropertiesDouble *conductivity = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Conductivity"));

	if (materialType != nullptr)
	{
		if (materialType->needsUpdate())
		{
			// The material type has changed
			if (materialType->getValue() == "Volumetric")
			{
				if (meshPriority != nullptr)
				{
					meshPriority->setValue(0.0);
					updatePropertiesGrid = true;
				}

				if (permittivity != nullptr) permittivity->setVisible(true);
				if (permeability != nullptr) permeability->setVisible(true);
				if (conductivity != nullptr) conductivity->setVisible(true);
			}
			else if(materialType->getValue() == "PEC")
			{
				if (meshPriority != nullptr)
				{
					meshPriority->setValue(100.0);
					updatePropertiesGrid = true;
				}

				if (permittivity != nullptr) permittivity->setVisible(false);
				if (permeability != nullptr) permeability->setVisible(false);
				if (conductivity != nullptr) conductivity->setVisible(false);
			}
			else
			{
				assert(0); // Unknown material type
			}
		}
	}

	if (materialType != nullptr) materialType->resetNeedsUpdate(); // This will be used for the simulation only, no action necessary here.
	if (meshPriority != nullptr) meshPriority->resetNeedsUpdate(); // This will be used for the simulation only, no action necessary here.
	if (permittivity != nullptr) permittivity->resetNeedsUpdate(); // This will be used for the simulation only, no action necessary here.
	if (permeability != nullptr) permeability->resetNeedsUpdate(); // This will be used for the simulation only, no action necessary here.
	if (conductivity != nullptr) conductivity->resetNeedsUpdate(); // This will be used for the simulation only, no action necessary here.

	return updatePropertiesGrid; // Notify, whether property grid update is necessary
}
