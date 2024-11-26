#include "..\include\EntitySolverMonitor.h"

#include "OTCommunication/ActionTypes.h"

EntitySolverMonitor::EntitySolverMonitor(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	:EntityContainer(ID,parent,obs,ms,factory,owner){}

EntitySolverMonitor::~EntitySolverMonitor(){}

void EntitySolverMonitor::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		OldTreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "Monitor";
		treeIcons.hiddenIcon = "Monitor";

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

bool EntitySolverMonitor::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}

bool EntitySolverMonitor::updatePropertyVisibilities(void)
{
	bool changed = false;
	auto domain = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty(GetPropertyNameDomain()));
	auto timeStep = dynamic_cast<EntityPropertiesInteger*>(this->getProperties().getProperty(GetPropertyNameObservedTimeStep()));
	auto sampleFrequ = dynamic_cast<EntityPropertiesInteger*>(this->getProperties().getProperty(GetPropertyNameMonitorFrequency()));
	auto frequency = dynamic_cast<EntityPropertiesDouble*>(this->getProperties().getProperty(GetPropertyNameObservFreq()));

	if (domain->getValue() == GetPropertyValueDomainTime())
	{
		changed |= frequency->getVisible();
		frequency->setVisible(false);

		auto selection = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty(GetPropertyNameVolume()));
		if (selection->getValue() == GetPropertyValueVolumeFull())
		{
			changed |= !timeStep->getVisible();
			timeStep->setVisible(true);
			changed |= sampleFrequ->getVisible();
			sampleFrequ->setVisible(false);
		}
		else
		{
			changed |= timeStep->getVisible();
			timeStep->setVisible(false);
			changed |= !sampleFrequ->getVisible();
			sampleFrequ->setVisible(true);
		}
	}
	else
	{
		changed |= !frequency->getVisible();
		frequency->setVisible(true);
		changed |= !sampleFrequ->getVisible();
		sampleFrequ->setVisible(true);
		changed |= timeStep->getVisible();
		timeStep->setVisible(false);

	}
	return changed;
}

void EntitySolverMonitor::createProperties()
{
	EntityPropertiesSelection::createProperty("Monitor settings", GetPropertyNameDomain(), { GetPropertyValueDomainFrequency(), GetPropertyValueDomainTime() }, GetPropertyValueDomainFrequency(), "Monitor", getProperties());
	EntityPropertiesSelection::createProperty("Monitor settings", GetPropertyNameDoF(), { GetPropertyValueDoFNode(), GetPropertyValueDoFEdge()}, GetPropertyValueDoFNode(), "Monitor", getProperties());
	EntityPropertiesSelection::createProperty("Monitor settings", GetPropertyNameFieldComp(), { GetPropertyValueFieldCompElectr(),GetPropertyValueFieldCompMagnetic()}, GetPropertyValueFieldCompElectr(), "Monitor", getProperties());
	EntityPropertiesSelection::createProperty("Monitor settings", GetPropertyNameQuantity(), { GetPropertyValueQuantityVector(),GetPropertyValueQuantityComponentX(),GetPropertyValueQuantityComponentY(),GetPropertyValueQuantityComponentZ() }, GetPropertyValueQuantityVector(), "Monitor", getProperties());
	EntityPropertiesDouble::createProperty("Monitor settings", GetPropertyNameObservFreq(), 0., "Monitor", getProperties());
	EntityPropertiesInteger::createProperty("Monitor settings", GetPropertyNameMonitorFrequency(), 1, "Monitor", getProperties());
	EntityPropertiesSelection::createProperty("Volume selection", GetPropertyNameVolume(), { GetPropertyValueVolumeFull(),GetPropertyValueVolumePoint() }, GetPropertyValueVolumeFull(), "Monitor", getProperties());
	EntityPropertiesInteger::createProperty("Monitor settings", GetPropertyNameObservedTimeStep(), 0., "Monitor", getProperties());

	updatePropertyVisibilities();
}

const std::string EntitySolverMonitor::GetPropertyNameVolume()
{
	static const std::string propertyNameVolume = "Observed Volume";
	return propertyNameVolume;
}

const std::string EntitySolverMonitor::GetPropertyNameDomain()
{
	static const std::string propertyNameDomain = "Domain";
	return propertyNameDomain;
}

const std::string EntitySolverMonitor::GetPropertyNameDoF()
{
	static const std::string propertyNameDoF = "Geometry";
	return propertyNameDoF;
}

const std::string EntitySolverMonitor::GetPropertyNameFieldComp()
{
	static const std::string propertyNameOfFieldComp = "Field component";
	return propertyNameOfFieldComp;
}

const std::string EntitySolverMonitor::GetPropertyNameQuantity()
{
	static const std::string propertyNameQuantity = "Quantity";
	return propertyNameQuantity;
}

const std::string EntitySolverMonitor::GetPropertyNameMonitorFrequency()
{
	static const std::string propertyNameMonitorFrequency = "Sampling rate";
	return propertyNameMonitorFrequency;
}


const std::string EntitySolverMonitor::GetPropertyValueFieldCompElectr()
{
	static const std::string propertyValueFieldCompVectorElectr = "Electric";
	return propertyValueFieldCompVectorElectr;
}

const std::string EntitySolverMonitor::GetPropertyValueFieldCompMagnetic()
{
	static const std::string propertyValueFieldCompVectorElectr = "Magnetic";
	return propertyValueFieldCompVectorElectr;
}


const std::string EntitySolverMonitor::GetPropertyValueDomainFrequency()
{
	static const std::string propertyValueDomainFrequency = "Frequency";
	return propertyValueDomainFrequency;
}

const std::string EntitySolverMonitor::GetPropertyValueDomainTime()
{
	static const std::string propertyValueDomainFrequency = "Time";
	return propertyValueDomainFrequency;
}


const std::string EntitySolverMonitor::GetPropertyValueDoFNode()
{
	static const std::string propertyValueDofNode = "Node";
	return propertyValueDofNode;
}

const std::string EntitySolverMonitor::GetPropertyValueDoFEdge()
{
	static const std::string propertyValueDofNode = "Edge";
	return propertyValueDofNode;
}

const std::string EntitySolverMonitor::GetPropertyValueQuantityVector()
{
	static const std::string propertyValueQuantityVector = "Vector";
	return propertyValueQuantityVector;
}

const std::string EntitySolverMonitor::GetPropertyValueQuantityComponentX()
{
	static const std::string propertyValueQuantityVector = "X";
	return propertyValueQuantityVector;
}

const std::string EntitySolverMonitor::GetPropertyValueQuantityComponentY()
{
	static const std::string propertyValueQuantityVector = "Y";
	return propertyValueQuantityVector;
}
const std::string EntitySolverMonitor::GetPropertyValueQuantityComponentZ()
{
	static const std::string propertyValueQuantityVector = "Z";
	return propertyValueQuantityVector;
}

const std::string EntitySolverMonitor::GetPropertyValueVolumeFull()
{
	static const std::string propertyValueVolumeFull = "Full";
	return propertyValueVolumeFull;
}

const std::string EntitySolverMonitor::GetPropertyValueVolumePoint()
{
	static const std::string propertyValueVolumeFull = "Center-Point";
	return propertyValueVolumeFull;
}

const std::string EntitySolverMonitor::GetPropertyNameXCoord()
{
	static const std::string propertyNameXCoord = "X-Coord";
	return propertyNameXCoord;
}
const std::string EntitySolverMonitor::GetPropertyNameYCoord()
{
	static const std::string propertyNameXCoord = "Y-Coord";
	return propertyNameXCoord;
}
const std::string EntitySolverMonitor::GetPropertyNameZCoord()
{
	static const std::string propertyNameXCoord = "Z-Coord";
	return propertyNameXCoord;
}

const std::string EntitySolverMonitor::GetPropertyNameObservedTimeStep(void)
{
	static const std::string propertyNameObservedTimeStep = "Timestep";
	return propertyNameObservedTimeStep;
}

const std::string EntitySolverMonitor::GetPropertyNameObservFreq()
{
	static const std::string propertyNameObservationFrequency = "Filter frequency";
	return propertyNameObservationFrequency;
}