// @otlicense

#include "..\include\EntityUnits.h"
#include <math.h>

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityUnits> registrar("EntityUnits");

EntityUnits::EntityUnits(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, const std::string & owner)
	: EntityBase(ID,parent,obs,ms,owner)
{
	setUnitLists();
}

bool EntityUnits::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityUnits::createProperties(void)
{
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameDimension, getUnitList(dimensionsList), "m", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameTemperature, getUnitList(temperatureList), "degC", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameFrequency, getUnitList(frequencyList), "Hz", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameTime, getUnitList(timeList), "s", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameVoltage, getUnitList(voltageList), "V", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameCurrent, getUnitList(currentList), "A", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameConductance, getUnitList(conductanceList), "S", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameResistance, getUnitList(resistanceList), "Ohm", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameInductance, getUnitList(inductanceList), "H", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameCapacitance, getUnitList(capacitanceList), "F", unitGroupname, getProperties());
	EntityPropertiesSelection::createProperty(unitGroupname, unitNameConductivity, getUnitList(conductivityList), "S/m", unitGroupname, getProperties());
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
	if (formerUnit == "Celsius" || formerUnit == "C")
	{
		value += 273.15;
	}
	else if (formerUnit == "Kelvin" || formerUnit == "K")
	{
		return;
	}
	else if (formerUnit == "Fahrenheit" || formerUnit == "F")
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
			OldTreeIcon treeIcons;
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

void EntityUnits::setUnitLists()
{
	dimensionsList = { {"nm", pow(10,-6)}, {"um", pow(10,-6)}, {"mm",0.001}, {"cm",0.1}, {"m",1.}, {"mil", pow(2.54, -5)}, {"in",0.0254}, {"ft",0.3048} };
	temperatureList = { {"degC",1.}, {"K",1.}, {"degF",1.} };
	frequencyList = { {"Hz", 1.}, {"kHz", pow(10,3)}, {"MHz", pow(10,6)}, {"GHz", pow(10,6)}, {"THz", pow(10,9)}, {"PHz", pow(10,12)} };
	timeList = { {"fs", pow(10,-15)}, {"ps", pow(10,-12)}, {"ns", pow(10,-9)}, {"us", pow(10,-6)}, {"ms", pow(10,-3)}, {"s", 1.} };
	voltageList = { {"nV", pow(10,-9)}, {"uV", pow(10,-6)}, {"mV", pow(10,-3)}, {"V",1.}, {"kV", pow(10,3)}, {"MV", pow(10,6)} };
	currentList = { {"nA", pow(10,-9)}, {"uA", pow(10,-6)}, {"mA", pow(10,-3)}, {"A",1.}, {"kA", pow(10,3)}, {"MA", pow(10,6)} };
	conductanceList = { {"nS", pow(10,-9)}, {"uS", pow(10,-6)}, {"mS", pow(10,-3)}, {"S",1.}, {"kS", pow(10,3)}, {"MS", pow(10,6)} };
	resistanceList = { {"nOhm", pow(10,-9)}, {"uOhm", pow(10,-6)}, {"mOhm", pow(10,-3)}, {"Ohm",1.}, {"kOhm", pow(10,3)}, {"MOhm", pow(10,6)}, {"GOhm", pow(10,9)} };
	inductanceList = { {"pH", pow(10,-12)}, {"nH", pow(10,-9)}, {"uH", pow(10,-6)}, {"mH", pow(10,-3)}, {"H",1.} };
	capacitanceList = { {"fF", pow(10,-15)}, {"pF", pow(10,-12)}, {"nF", pow(10,-9)}, {"uF", pow(10,-6)}, {"mF", pow(10,-3)}, {"F",1.} };
	conductivityList = { {"S/m",1.} };

	buildMap(dimensionsList, dimensions);
	buildMap(temperatureList, temperature);
	buildMap(frequencyList, frequency);
	buildMap(timeList, time);
	buildMap(voltageList, voltage);
	buildMap(currentList, current);
	buildMap(conductanceList, conductance);
	buildMap(resistanceList, resistance);
	buildMap(inductanceList, inductance);
	buildMap(capacitanceList, capacitance);
	buildMap(conductivityList, conductivity);
}

void EntityUnits::buildMap(const std::list<std::pair<std::string, double>>& list, std::map<std::string, double> &map)
{
	for (auto it = list.begin(); it != list.end(); it++)
	{
		map[it->first] = it->second;
	}
}

std::list<std::string> EntityUnits::getUnitList(std::list<std::pair<std::string, double>> &quantity)
{
	std::list<std::string> units;
	for (auto it = quantity.begin(); it != quantity.end(); it++)
	{
		units.push_back(it->first);
	}
	return units;
}
