#include "..\include\EntityUnits.h"
#include <math.h>

#include "OTCommunication/ActionTypes.h"

EntityUnits::EntityUnits(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	: EntityBase(ID,parent,obs,ms,factory,owner)
{
	SetUnitLists();
}

bool EntityUnits::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityUnits::createProperties(void)
{
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameDimension, GetUnitList(dimensions), dimensions.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameTemperature, GetUnitList(temperature), temperature.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameFrequency, GetUnitList(frequency), frequency.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameTime, GetUnitList(time), time.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameVoltage, GetUnitList(voltage), voltage.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameCurrent, GetUnitList(current), current.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameConductance, GetUnitList(conductance), conductance.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameResistance, GetUnitList(resistance), resistance.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameInductance, GetUnitList(inductance), inductance.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameCapacitance, GetUnitList(capacitance), capacitance.begin()->first, unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameConductivity, GetUnitList(conductivity), conductivity.begin()->first, unitGroupname, getProperties());
}

void EntityUnits::TurnToSIDimension(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameDimension));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= dimensions[formerUnit];
}

void EntityUnits::TurnToSITemperature(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameTemperature));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	if (formerUnit == "Celsius" || formerUnit == "°C")
	{
		value += 273.15;
	}
	else if (formerUnit == "Kelvin" || formerUnit == "K")
	{
		return;
	}
	else if (formerUnit == "Fahrenheit" || formerUnit == "°F")
	{
		value = (value - 32) * (5 / 9) + 273.15;
	}
}

void EntityUnits::TurnToSIFrequency(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameFrequency));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *=  frequency[formerUnit];
}

void EntityUnits::TurnToSITime(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameTime));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= time[formerUnit];
}

void EntityUnits::TurnToSIVoltage(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameVoltage));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *=voltage[formerUnit];
}

void EntityUnits::TurnToSICurrent(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameCurrent));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= current[formerUnit];
}

void EntityUnits::TurnToSIConductance(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameConductance));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= conductance[formerUnit];
}

void EntityUnits::TurnToSIResistance(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameResistance));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= resistance[formerUnit];
}

void EntityUnits::TurnToSIInductance(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameInductance));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= inductance[formerUnit];
}

void EntityUnits::TurnToSICapacitance(double & value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameCapacitance));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= capacitance[formerUnit];
}

void EntityUnits::TurnToSIConductivity(double& value, std::string& formerUnit)
{
	auto entity = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty(unitNameConductivity));
	assert(entity != nullptr);
	formerUnit = entity->getValue();
	value *= conductivity[formerUnit];
}

bool EntityUnits::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return false;
}

void EntityUnits::addVisualizationNodes(void)
{
		if (!getName().empty())
		{
			TreeIcon treeIcons;
			treeIcons.size = 32;
			treeIcons.visibleIcon = "Units";
			treeIcons.hiddenIcon = "Units";

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

			treeIcons.addToJsonDoc(doc);

			getObserver()->sendMessageToViewer(doc);
		}

		EntityBase::addVisualizationNodes();
}

void EntityUnits::SetUnitLists()
{
	dimensions = { {"m",1.}, {"cm",0.1}, {"mm",0.001}, {"um", pow(10,-6)}, {"nm", pow(10,-6)}, {"ft",0.3048}, {"mil", pow(2.54, -5)}, {"in",0.0254}};
	temperature = { {"°C",1.}, {"K",1.}, {"°F",1.} };
	frequency = { {"Hz", 1.}, {"kHz", pow(10,3)}, {"MHz", pow(10,6)}, {"GHz", pow(10,6)}, {"THz", pow(10,9)}, {"PHz", pow(10,12)} };
	time = { {"fs", pow(10,-15)}, {"ps", pow(10,-12)}, {"ns", pow(10,-9)}, {"us", pow(10,-6)}, {"ms", pow(10,-3)}, {"s", 1.} };
	voltage = { {"V",1.}, {"MV", pow(10,6)},{"kV", pow(10,3)}, {"mV", pow(10,-3)}, {"uV", pow(10,-6)}, {"nV", pow(10,-9)} };
	current = { {"A",1.}, {"MA", pow(10,6)},{"kA", pow(10,3)}, {"mA", pow(10,-3)}, {"uA", pow(10,-6)}, {"nA", pow(10,-9)} };
	conductance = { {"S",1.}, {"MS", pow(10,6)},{"kS", pow(10,3)}, {"mS", pow(10,-3)}, {"uS", pow(10,-6)}, {"nS", pow(10,-9)} };
	resistance = { {"Ohm",1.}, {"GOhm", pow(10,9)} , {"MOhm", pow(10,6)},{"kOhm", pow(10,3)}, {"mOhm", pow(10,-3)}, {"uOhm", pow(10,-6)}, {"nOhm", pow(10,-9)} };
	inductance = { {"H",1.}, {"mH", pow(10,-3)}, {"uH", pow(10,-6)}, {"nH", pow(10,-9)}, {"pH", pow(10,-12)} };
	capacitance = { {"F",1.}, {"mF", pow(10,-3)}, {"uF", pow(10,-6)}, {"nF", pow(10,-9)}, {"pF", pow(10,-12)}, {"fF", pow(10,-15)} };
	conductivity = { {"S/m",1.} };
}

std::list<std::string> EntityUnits::GetUnitList(std::map<std::string, double> &quantity)
{
	std::list<std::string> units;
	for (auto it = quantity.begin(); it != quantity.end(); it++)
	{
		units.push_back(it->first);
	}
	return units;
}
