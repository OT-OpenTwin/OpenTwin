// @otlicense
// File: NetlistGenerator.cpp
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


// Open Twin Header
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTCore/Logging/Logger.h"

// Service Header
#include "NetlistGenerator.h"
#include "BlockEntityHandler.h"
#include "CircuitElements/VoltageSource.h"

// std Header
#include <sstream>
#include <unordered_set>

NetlistGenerator::NetlistGenerator(ElementNamingRegistry& _elementNamingRegistry)
	: m_elementNamingRegistry(_elementNamingRegistry)
{
}

std::list<std::string> NetlistGenerator::generate(EntityBase* _solverEntity, Circuit& _circuit, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>>& allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID, const std::string& editorname)
{
    std::list<std::string> netlist;

    // 1. Title
    netlist.push_back("circbyline *Test");

    // 2. Create Simulation-Strategy
	EntityPropertiesBase* simTypePropBase = _solverEntity->getProperties().getProperty("Simulation Type");
    if(!simTypePropBase)
    {
        OT_LOG_E("Simulation Type property not found in solver entity");
        return netlist;
	}

    auto* simTypeProp = dynamic_cast<EntityPropertiesSelection*>(simTypePropBase);
    if(!simTypeProp)
    {
        OT_LOG_E("Simulation Type property is not of type Selection");
        return netlist;
	}

    std::string simulationType = simTypeProp->getValue();
    auto strategy = createSimulationStrategy(simulationType);

    // 3. Create Element lines 
    std::vector<MeterData> voltageMeterData;
    std::vector<MeterData> currentMeterData;
    std::unordered_set<std::string> usedModels;
    int rshuntCounter = 1;
    for (const auto& [uid, elementPtr] : _circuit.getMapOfElements())
    {
		CircuitElement* circuitElement = elementPtr.get();
		// Meter Handling: VoltageMeter and CurrentMeter are handled separately
        if (circuitElement->type() == "VoltageMeter")
        {
            voltageMeterData.push_back(extractMeterNodes(circuitElement));
            continue;
        }
        if (circuitElement->type() == "CurrentMeter")
        {
            MeterData data = extractMeterNodes(circuitElement);
			// For CurrentMeter , we create a Rshunt element with a unique name and register it in the naming registry
            std::string shuntName = "Rshunt" + std::to_string(rshuntCounter++);
            m_elementNamingRegistry.registerMapping(data.customName,
                ElementNamingRegistry::toLowercase(shuntName));
            data.customName = shuntName;  
            currentMeterData.push_back(data);
            continue;
        }
        // Model-Handling for all element types
        std::string modelType = "";
        std::vector<std::string> modelLines;
        if (circuitElement->getModel() != "failed")
        {
            auto modelEntity = getModelEntity(
                circuitElement->getFolderName(), circuitElement->getModel());
            modelType = getCircuitModelType(modelEntity);
            if (usedModels.find(circuitElement->getModel()) == usedModels.end())
            {
                modelLines = convertToCircByLine(getCircuitModelText(modelEntity));
                usedModels.insert(circuitElement->getModel());
            }
        }
        else
        {
            circuitElement->setModel("");
        }
        // Build element line 
        std::string line = buildElementLine(circuitElement, modelType, *strategy);
        // Add node numbers
        line += buildNodeNumbers(circuitElement);
		// Add Voltage Source type if applicable
        if (circuitElement->type() == "VoltageSource")
        {
            auto* vs = dynamic_cast<VoltageSource*>(circuitElement);
            line += strategy->getVoltageSourceNetlistType(vs);
        }
        else
        {
		    // add value or model depending on whether a model is specified
            if (modelType.empty()) {
                line += circuitElement->getNetlistValue();
            }
            else {
                line += circuitElement->getModel();
            }
        }
        netlist.push_back(line);
		// Add model lines 
        for (const auto& modelLine : modelLines) {
            netlist.push_back(modelLine);
        }
    }
    // 4. CurrentMeter with Rshunt
    std::vector<std::string> shuntNames;
    for (const auto& meter : currentMeterData)
    {
        std::string nodeString;
        for (const auto& node : meter.nodeNumbers) {
            nodeString += node + " ";
        }
        netlist.push_back("circbyline " + meter.customName + " " + nodeString + "0");
        shuntNames.push_back(meter.customName);
    }
    // 5. Simulation lines
    std::string simLine = strategy->generateSimulationLine(_solverEntity, m_elementNamingRegistry);
    if (simLine == "failed")
    {
        OT_LOG_E("Failed creating simulation line");
        netlist.clear();
        return netlist;
    }
    netlist.push_back("circbyline " + simLine);
    // 6. Voltage Meter Probes 
    for (const auto& meter : voltageMeterData)
    {
        std::string nodeString = "(";
        for (size_t i = 0; i < meter.nodeNumbers.size(); ++i) {
            nodeString += meter.nodeNumbers[i];
            if (i < meter.nodeNumbers.size() - 1) nodeString += ",";
        }
        nodeString += ")";
        netlist.push_back("circbyline .probe vd" + nodeString);
    }
    // 7. Current Meter Probes 
    for (const auto& name : shuntNames)
    {
        netlist.push_back("circbyline .probe I(" + name + ")");
    }
    // 8. Control Block 
    netlist.push_back("circbyline .Control");
    netlist.push_back("circbyline run");
    netlist.push_back("circbyline echo \"Simulation Completed!\"");
    netlist.push_back("circbyline .endc");
    netlist.push_back("circbyline .end");
    return netlist;
}

std::string NetlistGenerator::buildElementLine(CircuitElement* _element, const std::string& _modelType, const SimulationStrategy& _strategy)
{
    std::string netlistElementName;
    if (_modelType.empty())
    {
        netlistElementName = _element->getNetlistName();
    }
    else
    {
        if (_modelType == m_subcktType)
        {
            netlistElementName = m_elementNamingRegistry.generateNextId("X");
        }
        else
        {
            netlistElementName = _element->getNetlistName();
        }
    }
    return "circbyline " + netlistElementName + " ";
}

std::string NetlistGenerator::buildNodeNumbers(CircuitElement* _element)
{
    std::string netlistNodeNumbers;
    std::unordered_set<std::string> temp;
    auto connections = _element->getList();

    // Use the polymorphic pole names from the element
    for (const auto& poleName : _element->getPositivePoleNames())
    {
        if (connections.find(poleName) != connections.end())
        {
            auto& conn = connections.at(poleName);
            if (conn.getNodeNumber() != m_voltMeterConnection && temp.find(conn.getNodeNumber()) == temp.end())
            {
                netlistNodeNumbers += conn.getNodeNumber() + " ";
                temp.insert(conn.getNodeNumber());
            }
        }
    }

    for (const auto& poleName : _element->getNegativePoleNames())
    {
        if (connections.find(poleName) != connections.end())
        {
            auto& conn = connections.at(poleName);
            if (conn.getNodeNumber() != m_voltMeterConnection && temp.find(conn.getNodeNumber()) == temp.end())
            {
                netlistNodeNumbers += conn.getNodeNumber() + " ";
                temp.insert(conn.getNodeNumber());
            }
        }
    }

    return netlistNodeNumbers;
}

NetlistGenerator::MeterData NetlistGenerator::extractMeterNodes(CircuitElement* element)
{
    MeterData data;
    data.customName = element->getCustomName();

    std::unordered_set<std::string> temp;
    auto connections = element->getList();

    // First process the positivePole connection
    if (connections.find("positivePole") != connections.end())
    {
        auto& positiveConn = connections.at("positivePole");
        if (temp.find(positiveConn.getNodeNumber()) == temp.end())
        {
            temp.insert(positiveConn.getNodeNumber());
            data.nodeNumbers.push_back(positiveConn.getNodeNumber());
        }
    }

    // Process the negativePole connection
    if (connections.find("negativePole") != connections.end())
    {
        auto& negativeConn = connections.at("negativePole");
        if (temp.find(negativeConn.getNodeNumber()) == temp.end())
        {
            temp.insert(negativeConn.getNodeNumber());
            data.nodeNumbers.push_back(negativeConn.getNodeNumber());
        }
    }

    return data;
}

std::shared_ptr<EntityFileText> NetlistGenerator::getModelEntity(const std::string& _folderName, std::string _modelName)
{
    BlockEntityHandler blockHandler;
    std::shared_ptr<EntityFileText> circuitModelEntity = blockHandler.getCircuitModel(_folderName, _modelName);

    if (circuitModelEntity != nullptr)
    {
        return circuitModelEntity;
    }

    OT_LOG_E("No CircuitModelEntity found with name: " + _modelName);
    return nullptr;
}

std::string NetlistGenerator::getCircuitModelType(std::shared_ptr<EntityFileText> _circuitModelEntity)
{
    if (_circuitModelEntity != nullptr)
    {
        auto propertyBase = _circuitModelEntity->getProperties().getProperty("ModelType");
        if (propertyBase)
        {
            auto circuitModelType = dynamic_cast<EntityPropertiesString*>(propertyBase);
            if (circuitModelType)
            {
                return circuitModelType->getValue();
            }
        }
        return "";
    }

    OT_LOG_E("No Circuit model type found: EntityFileText is null");
    return "";
}

std::string NetlistGenerator::getCircuitModelText(std::shared_ptr<EntityFileText> _circuitModelEntity)
{
    if (_circuitModelEntity != nullptr)
    {
        const std::vector<char>& data = _circuitModelEntity->getDataEntity()->getData();
        std::string modelText(data.begin(), data.end());
        return modelText;
    }

    OT_LOG_E("No Circuit model text found: EntityFileText is null");
    return "";
}

std::vector<std::string> NetlistGenerator::convertToCircByLine(const std::string& lines)
{
    std::istringstream stream(lines);
    std::string line;
    std::vector<std::string> circLines;

    while (std::getline(stream, line))
    {
        if (line.empty()) continue; // skip empty line
        circLines.push_back("circbyline " + line);
    }

    return circLines;
}
