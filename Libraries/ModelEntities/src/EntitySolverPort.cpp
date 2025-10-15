#include "../include/EntitySolverPort.h"

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntitySolverPort> registrar(EntitySolverPort::className());

EntitySolverPort::EntitySolverPort(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, const std::string & owner)
	: EntityContainer(ID,parent,obs,ms,owner){}

EntitySolverPort::~EntitySolverPort(){}

void EntitySolverPort::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		OldTreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "Port";
		treeIcons.hiddenIcon = "Port";

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

bool EntitySolverPort::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	getProperties().forceResetUpdateForAllProperties();
	
	UpdateVisibillity();

	return true;
}

void EntitySolverPort::createProperties(const std::string signalTypeFolderName, ot::UID signalTypeFolderID, std::string signalName, ot::UID signalUID)
{
	EntityPropertiesEntityList::createProperty("Signal type", GetPropertyNameSignal(), signalTypeFolderName, signalTypeFolderID, signalName, signalUID , "FITTD", getProperties());
	
	EntityPropertiesSelection::createProperty("Location", GetPropertyNameSelectionMethod(), { GetValueSelectionMethodCentre(), GetValueSelectionMethodFree()}, GetValueSelectionMethodCentre(), "Location", getProperties());

	EntityPropertiesInteger::createProperty("Location", GetPropertyNameXCoordinate(), 1,"Location",getProperties());
	EntityPropertiesInteger::createProperty("Location", GetPropertyNameYCoordinate(), 1,"Location",getProperties());
	EntityPropertiesInteger::createProperty("Location", GetPropertyNameZCoordinate(), 1,"Location",getProperties());

	EntityPropertiesSelection::createProperty("Excitation", GetPropertyNameExcitationAxis(), { GetValueExcitationAxisX() , GetValueExcitationAxisY(), GetValueExcitationAxisZ() }, GetValueExcitationAxisX(), "Excitation", getProperties());
	EntityPropertiesSelection::createProperty("Excitation", GetPropertyNameExcitationTarget(), { GetValueExcitationTargetE(), GetValueExcitationTargetH()}, GetValueExcitationTargetE(), "Excitation", getProperties());
	
	UpdateVisibillity();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntitySolverPort::UpdateVisibillity()
{
	auto selectionMethod = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(GetPropertyNameSelectionMethod()));
	assert(selectionMethod != nullptr);
	auto yLocation = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty(GetPropertyNameXCoordinate()));
	assert(yLocation != nullptr);
	auto xLocation = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty(GetPropertyNameYCoordinate()));
	assert(xLocation != nullptr);
	auto zLocation = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty(GetPropertyNameZCoordinate()));
	assert(zLocation != nullptr);
	
	bool changeOfVisibility = false;
	if (selectionMethod->getValue() != GetValueSelectionMethodFree())
	{
		if (xLocation->getVisible()) 
		{
			xLocation->setVisible(false); 
			changeOfVisibility = true; 
		}
		if (yLocation->getVisible()) 
		{ 
			yLocation->setVisible(false); 
			changeOfVisibility = true; 
		}
		if (zLocation->getVisible()) 
		{ 
			zLocation->setVisible(false); 
			changeOfVisibility = true; 
		}
	}
	else
	{
		if (!xLocation->getVisible()) 
		{ 
			xLocation->setVisible(true); 
			changeOfVisibility = true;
		}
		if (!yLocation->getVisible()) 
		{ 
			yLocation->setVisible(true); 
			changeOfVisibility = true;
		}
		if (!zLocation->getVisible()) 
		{ 
			zLocation->setVisible(true); 
			changeOfVisibility = true;
		}	
	}
	return  changeOfVisibility;
}

const std::string EntitySolverPort::GetPropertyNameSignal()
{
	static const std::string propertyName = "Signal";
	return propertyName;
}

const std::string EntitySolverPort::GetPropertyNameSelectionMethod()
{
	static const std::string propertyName = "Location selection";
	return propertyName;
}

const std::string EntitySolverPort::GetPropertyNameXCoordinate()
{
	static const std::string propertyName = "X-Coordinate";
	return propertyName;
}

const std::string EntitySolverPort::GetPropertyNameYCoordinate()
{
	static const std::string propertyName = "Y-Coordinate";
	return propertyName;
}

const std::string EntitySolverPort::GetPropertyNameZCoordinate()
{
	static const std::string propertyName = "Z-Coordinate";
	return propertyName;
}

const std::string EntitySolverPort::GetPropertyNameExcitationAxis()
{
	static const std::string propertyName = "Excitation Axis";
	return propertyName;
}

const std::string EntitySolverPort::GetPropertyNameExcitationTarget()
{
	static const std::string propertyName = "Excitation Target";
	return propertyName;
}

const std::string EntitySolverPort::GetValueSelectionMethodCentre()
{
	static const std::string centreSelection = "Centre of calculation domain";
	return centreSelection;
}

const std::string EntitySolverPort::GetValueSelectionMethodFree()
{
	static const std::string freeSelection = "Free";
	return freeSelection;
}

const std::string EntitySolverPort::GetValueExcitationAxisX()
{
	static const std::string excitationAcisX = "X-Axis";
	return excitationAcisX;
}

const std::string EntitySolverPort::GetValueExcitationAxisY()
{
	static const std::string excitationAxisY = "Y-Axis";
	return excitationAxisY;
}

const std::string EntitySolverPort::GetValueExcitationAxisZ()
{
	static const std::string excitationAxisZ = "Z-Axis";
	return excitationAxisZ;
}

const std::string EntitySolverPort::GetValueExcitationTargetE()
{
	static const std::string excitationTargetE = "E-Field";
	return excitationTargetE;
}

const std::string EntitySolverPort::GetValueExcitationTargetH()
{
	static const std::string excitationTargetH = "H-Field";
	return excitationTargetH;
}

