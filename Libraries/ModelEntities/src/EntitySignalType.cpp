#include "EntitySignalType.h"

#include "OTCommunication/ActionTypes.h"

EntitySignalType::EntitySignalType(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	:EntityContainer(ID,parent,obs,ms,factory,owner)
{
}

EntitySignalType::~EntitySignalType()
{
}

void EntitySignalType::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		OldTreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "Signal";
		treeIcons.hiddenIcon = "Signal";

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

bool EntitySignalType::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	getProperties().forceResetUpdateForAllProperties();

	return false; // No property grid update necessary
}

void EntitySignalType::createProperties()
{
	EntityPropertiesSelection::createProperty("Signal definition", GetPropertyNameExcitationType(), { GetValueSinGuasSignal() }, GetValueSinGuasSignal(), "Signal type", getProperties());
	EntityPropertiesDouble::createProperty("Signal definition", GetPropertyNameMinFrequency(), 1, "Signal type", getProperties());
	EntityPropertiesDouble::createProperty("Signal definition", GetPropertyNameMaxFrequency(), 100, "Signal type", getProperties());
}

const std::string EntitySignalType::GetPropertyNameExcitationType()
{
	static const std::string propertyNameExcitationType = "Excitation type";
	return propertyNameExcitationType;
}

const std::string EntitySignalType::GetPropertyNameMinFrequency()
{
	static const std::string propertyNameMinFrequency = "Minimum frequency";
	return propertyNameMinFrequency;
}

const std::string EntitySignalType::GetPropertyNameMaxFrequency()
{
	static const std::string propertyNameMaxFrequency = "Maximum frequency";
	return propertyNameMaxFrequency;
}

const std::string EntitySignalType::GetValueSinGuasSignal()
{
	static const std::string valueSignalTypeSinGausSignal = "Sinusoidal-Gauss";
	return valueSignalTypeSinGausSignal;
}

bool EntitySignalType::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}






