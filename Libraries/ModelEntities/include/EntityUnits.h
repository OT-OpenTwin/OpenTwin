// @otlicense
// File: EntityUnits.h
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

#pragma once
#include "EntityBase.h"

#include <map>
#include <string>
#include <list>

class __declspec(dllexport) EntityUnits : public EntityBase
{
public:
	EntityUnits() : EntityUnits(0, nullptr, nullptr, nullptr) {};
	EntityUnits(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);

	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	
	virtual void createProperties(void);
	virtual bool updateFromProperties(void) override;
	virtual void addVisualizationNodes(void) override;
	virtual std::string getClassName(void) const override { return "EntityUnits"; };

	void TurnToSIDimension(double & value, std::string& formerUnit);
	void TurnToSITemperature(double & value, std::string& formerUnit);
	void TurnToSIFrequency(double & value, std::string& formerUnit);
	void TurnToSITime(double & value, std::string& formerUnit);
	void TurnToSIVoltage(double & value, std::string& formerUnit);
	void TurnToSICurrent(double & value, std::string& formerUnit);
	void TurnToSIConductance(double & value, std::string& formerUnit);
	void TurnToSIResistance(double & value, std::string& formerUnit);
	void TurnToSIInductance(double & value, std::string& formerUnit);
	void TurnToSICapacitance(double& value, std::string& formerUnit);
	void TurnToSIConductivity(double& value, std::string& formerUnit);

private:
	
	const std::string unitGroupname = "Units";

	const std::string unitNameDimension = "Dimension";
	const std::string unitNameTemperature = "Temperature";
	const std::string unitNameFrequency = "Frequency";
	const std::string unitNameTime = "Time";
	const std::string unitNameVoltage = "Voltage";
	const std::string unitNameCurrent = "Current";
	const std::string unitNameConductance = "Conductance";
	const std::string unitNameResistance = "Resistance";
	const std::string unitNameInductance = "Inductance";
	const std::string unitNameCapacitance = "Capacitance";
	const std::string unitNameConductivity = "Conductivity";

	std::list<std::pair<std::string, double>> dimensionsList;
	std::list<std::pair<std::string, double>> temperatureList;
	std::list<std::pair<std::string, double>> frequencyList;
	std::list<std::pair<std::string, double>> timeList;
	std::list<std::pair<std::string, double>> voltageList;
	std::list<std::pair<std::string, double>> currentList;
	std::list<std::pair<std::string, double>> conductanceList;
	std::list<std::pair<std::string, double>> resistanceList;
	std::list<std::pair<std::string, double>> inductanceList;
	std::list<std::pair<std::string, double>> capacitanceList;
	std::list<std::pair<std::string, double>> conductivityList;

	std::map<std::string, double> dimensions;
	std::map<std::string, double> temperature;
	std::map<std::string, double> frequency;
	std::map<std::string, double> time;
	std::map<std::string, double> voltage;
	std::map<std::string, double> current;
	std::map<std::string, double> conductance;
	std::map<std::string, double> resistance;
	std::map<std::string, double> inductance;
	std::map<std::string, double> capacitance;
	std::map<std::string, double> conductivity;

	void setUnitLists();
	std::list<std::string> getUnitList(std::list<std::pair<std::string, double>>& quantity);
	void buildMap(const std::list<std::pair<std::string, double>>& list, std::map<std::string, double>& map);
};
