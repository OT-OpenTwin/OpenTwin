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
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityParameter.h"
#include "OTModelEntities/EntityAPI.h"

// Service Header
#include "NetlistGenerator.h"
#include "BlockEntityHandler.h"
#include "CircuitElements/VoltageSource.h"
#include "CircuitElements/TransmissionLine.h"
#include "OTModelEntities/EntityUnits.h"

// std Header
#include <sstream>
#include <cstdlib>
#include <unordered_set>
#include <set>

NetlistGenerator::NetlistGenerator(ElementNamingRegistry& _elementNamingRegistry)
	: m_elementNamingRegistry(_elementNamingRegistry)
{
}

std::list<std::string> NetlistGenerator::generate(EntityBase* _solverEntity, Circuit& _circuit, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>>& allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID, const std::string& editorname)
{
    std::list<std::string> netlist;

    // 1. Title
    netlist.push_back("circbyline *Test");

    // 1b. Collect all global parameters and inject as .param lines
    m_parameterNames.clear();
    ot::UIDList parameterIDs = ot::ModelServiceAPI::getIDsOfFolderItemsOfType("Parameters", "EntityParameter", true);
    std::list<ot::EntityInformation> currentParameterInfo;
    ot::ModelServiceAPI::getEntityInformation(parameterIDs, currentParameterInfo);

    for (const auto& paramInfo : currentParameterInfo)
    {
        EntityParameter* param = dynamic_cast<EntityParameter*>(ot::EntityAPI::readEntityFromEntityIDandVersion(paramInfo.getEntityID(), paramInfo.getEntityVersion()));
        if (param)
        {
            std::string paramName = param->getName();
            size_t slashPos = paramName.find_last_of('/');
            if (slashPos != std::string::npos) paramName = paramName.substr(slashPos + 1);

            m_parameterNames.insert(paramName);
            netlist.push_back("circbyline .param " + paramName + "=" + std::to_string(param->getNumericValue()));
            delete param;
        }
    }

    // 1c. Load global project units
    ot::EntityInformation unitsInfo;
    std::unique_ptr<EntityUnits> entityUnits;
    if (ot::ModelServiceAPI::getEntityInformation("Units", unitsInfo))
    {
		EntityUnits* units = dynamic_cast<EntityUnits*>(ot::EntityAPI::readEntityFromEntityIDandVersion(unitsInfo.getEntityID(), unitsInfo.getEntityVersion()));
        if(units)
        {
            entityUnits.reset(units);
        }
        else
        {
            OT_LOG_E("Failed to load Units entity");
		}
    }

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
            if (!vs)
            {
                OT_LOG_E("Failed to cast CircuitElement to VoltageSource");
            }
           
            std::string unit = "V";
            if (entityUnits) {
                unit = entityUnits->getVoltageUnit();
            }

            vs->setValue(formatScaledNetlistValue(vs->getValue(), unit));
            vs->setAmplitude(formatScaledNetlistValue(vs->getAmplitude(), unit));
            line += strategy->getVoltageSourceNetlistType(vs);
        }
        else
        {
		    // add value or model depending on whether a model is specified
            if (modelType.empty()) {
                // BehavioralSource expressions are natively evaluated by NGSpice - no {} wrapping needed
                if (circuitElement->type() == "BehavioralSource") {
                    line += circuitElement->getNetlistValue();
                }
                else if (circuitElement->type() == "Capacitor") {
                    std::string unit = "F";
                    if (entityUnits) 
                    {
                        unit = entityUnits->getCapacitanceUnit();
                    }
                    line += formatScaledNetlistValue(circuitElement->getNetlistValue(), unit);
                }
                else if (circuitElement->type() == "Inductor") {
                    std::string unit = "H";
                    if (entityUnits) 
                    {
                        unit = entityUnits->getInductanceUnit();
                    }
                    line += formatScaledNetlistValue(circuitElement->getNetlistValue(), unit);
                }
                else if (circuitElement->type() == "Resistor") {
                    std::string unit = "Ohm";
                    if (entityUnits) {
                        unit = entityUnits->getResistanceUnit();
                    }
                    line += formatScaledNetlistValue(circuitElement->getNetlistValue(), unit);
                }
                else if (circuitElement->type() == "TransmissionLine") {
                    auto* tl = dynamic_cast<TransmissionLine*>(circuitElement);
                    if (tl) {
                        std::string unitR = "Ohm";
                        if (entityUnits) 
                        {
                            unitR = entityUnits->getResistanceUnit();
                        }

                        std::string unitT = "s";
                        if (entityUnits) 
                        {
                            unitT = entityUnits->getTimeUnit();
                        }

                        line += "Z0=" + formatScaledNetlistValue(tl->getRawImpedance(), unitR) + " TD=" + formatScaledNetlistValue(tl->getRawTransmissionDelay(), unitT);
                    }
                    else {
                        line += wrapParameterExpression(circuitElement->getNetlistValue());
                    }
                }
                else {
                    line += wrapParameterExpression(circuitElement->getNetlistValue());
                }
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
    std::unordered_set<std::string> seen;
    bool dedup = !_element->allowDuplicateNodes();
    auto connections = _element->getList();

    // Use the ordered pole names for correct NGSpice node sequence
    for (const auto& poleName : _element->getOrderedPoleNames())
    {
        if (connections.find(poleName) != connections.end())
        {
            auto& conn = connections.at(poleName);
            if (conn.getNodeNumber() != m_voltMeterConnection)
            {
                if (!dedup || seen.find(conn.getNodeNumber()) == seen.end())
                {
                    netlistNodeNumbers += conn.getNodeNumber() + " ";
                    seen.insert(conn.getNodeNumber());
                }
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

bool NetlistGenerator::isParameterExpression(const std::string& _value) const
{
    if (_value.empty()) return false;

    // Already wrapped in {} -> it is a parameter expression
    if (_value.front() == '{' && _value.back() == '}') return true;

    if (m_parameterNames.empty()) return false;

    // Check if the value contains any known parameter name as a whole word
    for (const auto& paramName : m_parameterNames)
    {
        size_t pos = 0;
        while ((pos = _value.find(paramName, pos)) != std::string::npos)
        {
            // Check word boundary before
            bool boundaryBefore = (pos == 0) || (!std::isalnum(static_cast<unsigned char>(_value[pos - 1])) && _value[pos - 1] != '_');
            // Check word boundary after
            size_t endPos = pos + paramName.size();
            bool boundaryAfter = (endPos >= _value.size()) || (!std::isalnum(static_cast<unsigned char>(_value[endPos])) && _value[endPos] != '_');

            if (boundaryBefore && boundaryAfter)
            {
                return true;
            }
            pos += paramName.size();
        }
    }

    return false;
}

std::string NetlistGenerator::wrapParameterExpression(const std::string& _value) const
{
    if (isParameterExpression(_value))
    {
        if (_value.front() == '{' && _value.back() == '}') return _value;
        return "{" + _value + "}";
    }

    return _value;
}

std::string NetlistGenerator::getSpiceUnitMultiplier(const std::string& _unit) const
{
    if (_unit.empty()) return "";

    // Mega: MOhm, MV, MA, MHz, MS
    if (_unit.rfind("Meg", 0) == 0 || (_unit.size() > 1 && _unit[0] == 'M'))
    {
        return "1Meg";
    }
    // Giga: GOhm, GHz
    if (_unit.size() > 1 && _unit[0] == 'G')
    {
        return "1G";
    }
    // Kilo: kOhm, kV, kA, kHz, kS
    if (_unit.size() > 1 && _unit[0] == 'k')
    {
        return "1k";
    }
    // Milli: mOhm, mV, mA, ms, mH, mF, mS
    if (_unit.size() > 1 && _unit[0] == 'm')
    {
        return "1m";
    }
    // Micro: uOhm, uV, uA, us, uH, uF, uS
    if (_unit.size() > 1 && _unit[0] == 'u')
    {
        return "1u";
    }
    // Nano: nOhm, nV, nA, ns, nH, nF, nS
    if (_unit.size() > 1 && _unit[0] == 'n')
    {
        return "1n";
    }
    // Pico: pH, pF, ps
    if (_unit.size() > 1 && _unit[0] == 'p')
    {
        return "1p";
    }
    // Femto: fF, fs
    if (_unit.size() > 1 && _unit[0] == 'f')
    {
        return "1f";
    }

    // Base units: V, Ohm, F, H, s, Hz, A, S -> no multiplier needed (e.g. for Volt write nothing)
    return "";
}

std::string NetlistGenerator::formatScaledNetlistValue(const std::string& _value, const std::string& _unit) const
{
    if (_value.empty()) return _value;

    std::string trimmed = _value;
    size_t first = trimmed.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = trimmed.find_last_not_of(" \t\r\n");
    trimmed = trimmed.substr(first, last - first + 1);

    std::string multiplier = getSpiceUnitMultiplier(_unit);

    // 1. If it's a parameter or an expression (e.g. "myParam", "{myParam}", "2 * myParam")
    if (isParameterExpression(trimmed))
    {
        std::string expr = trimmed;
        if (expr.front() == '{' && expr.back() == '}')
        {
            expr = expr.substr(1, expr.size() - 2);
            size_t eFirst = expr.find_first_not_of(" \t\r\n");
            if (eFirst != std::string::npos)
            {
                size_t eLast = expr.find_last_not_of(" \t\r\n");
                expr = expr.substr(eFirst, eLast - eFirst + 1);
            }
        }

        if (multiplier.empty())
        {
            // Base unit (e.g. Volt, Ohm, Farad) -> write nothing extra into parameter reference: "{expr}"
            return "{" + expr + "}";
        }

        // Non-base unit (e.g. mV, uF, kOhm) -> "{expr * 1m}", "{expr * 1u}", etc.
        return "{" + expr + " * " + multiplier + "}";
    }

    // 2. Check if trimmed is a pure number (e.g. "10", "200", "0.5", "0")
    char* end = nullptr;
    double val = std::strtod(trimmed.c_str(), &end);
    if (end != trimmed.c_str() && *end == '\0')
    {
        if (val == 0.0)
        {
            return "0";
        }
        if (multiplier.empty())
        {
            return trimmed;
        }
        std::string suffix = multiplier.substr(1);
        return trimmed + suffix;
    }

    // 3. It already has an existing unit or scale suffix (e.g. "10uF", "100mH", "0.5n", "10u")
    if (end != trimmed.c_str())
    {
        std::string suffix(end);
        // Normalize physical unit endings like "uF" -> "u", "mH" -> "m", "ns" -> "n"
        if (suffix == "uF" || suffix == "mF" || suffix == "nF" || suffix == "pF" || suffix == "fF" || suffix == "F")
        {
            return trimmed.substr(0, trimmed.size() - 1);
        }
        if (suffix == "mH" || suffix == "uH" || suffix == "nH" || suffix == "pH" || suffix == "H")
        {
            return trimmed.substr(0, trimmed.size() - 1);
        }
        if (suffix == "ms" || suffix == "us" || suffix == "ns" || suffix == "ps" || suffix == "fs")
        {
            return trimmed.substr(0, trimmed.size() - 1);
        }
        if (suffix == "kOhm" || suffix == "mOhm" || suffix == "uOhm" || suffix == "nOhm" || suffix == "MOhm" || suffix == "GOhm")
        {
            std::string numPart(trimmed.c_str(), end - trimmed.c_str());
            std::string m = getSpiceUnitMultiplier(suffix);
            if (!m.empty())
            {
                return numPart + m.substr(1);
            }
            return numPart;
        }
        if (suffix == "Ohm")
        {
            return std::string(trimmed.c_str(), end - trimmed.c_str());
        }
    }

    return trimmed;
}
