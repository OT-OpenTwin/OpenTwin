// @otlicense
// File: CircuitElement.h
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

// Service Header
#include "ElementNamingRegistry.h"

// Open Twin Header
#include "OTCore/Logging/Logger.h"

// C++ Header
#include <algorithm>

std::string ElementNamingRegistry::registerElement (const std::string& _customName, const std::string& _prefix)
{
    // Schritt 1: Neue ID generieren
    std::string netlistName = generateNextId(_prefix);
    // Schritt 2: Bidirektionales Mapping anlegen
    if (!registerMapping(_customName, netlistName))
    {
        OT_LOG_E("Failed to register element: " + _customName + "with" + netlistName);
    }
    return netlistName;
}

std::string ElementNamingRegistry::getNetlistName(const std::string& _customName) const
{
    auto it = m_customToNetlist.find(_customName);
    if (it != m_customToNetlist.end())
    {
        return it->second;
    }
    OT_LOG_E("Netlist name not found for: " + _customName);
    return "";
}

std::string ElementNamingRegistry::getCustomName(const std::string& _netlistName) const
{
	auto it = m_netlistToCustom.find(_netlistName);
    if(it != m_netlistToCustom.end())
    {
        return it->second;
	}
	OT_LOG_E("Curstom name not found for: " + _netlistName);
	return "";
}

std::string ElementNamingRegistry::generateNextId(const std::string& _prefix)
{
	m_elementCounters[_prefix]++;
	return _prefix + std::to_string(m_elementCounters[_prefix]);
}

bool ElementNamingRegistry::registerMapping(const std::string& _customName, const std::string& _netlistName)
{
    if (m_customToNetlist.find(_customName) != m_customToNetlist.end())
    {
		OT_LOG_E("Custom name already exists: " + _customName);
        return false;
    }

    if (m_netlistToCustom.find(_netlistName) != m_netlistToCustom.end())
    {
        OT_LOG_E("Netlist name already exists: " + _netlistName);
		return false;
    }

	m_customToNetlist[_customName] = _netlistName;
	m_netlistToCustom[_netlistName] = _customName;
	return true;
}

void ElementNamingRegistry::reset()
{
    m_customToNetlist.clear();
    m_netlistToCustom.clear();
    m_elementCounters.clear();
}

std::string ElementNamingRegistry::toLowercase(const std::string& str)
{
    std::string lower = str;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}
