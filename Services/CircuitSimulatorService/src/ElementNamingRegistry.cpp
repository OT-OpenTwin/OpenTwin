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
#include <cctype>

std::string ElementNamingRegistry::registerElement (const std::string& _customName, const std::string& _prefix, const std::string& _scope)
{
    // 1. Extract local block name (strip folder path if present, e.g. "Circuits/Sub/V1" -> "V1")
	std::string localName = _customName;
	size_t slashPos = localName.find_last_of('/');
    if (slashPos != std::string::npos)
    {
		localName = localName.substr(slashPos + 1);
    }

	// Strip optional UI display suffix in parentheses (e.g. "V1 (UI Display)" -> "V1")
    size_t parenPos = localName.rfind(" (");
    if (parenPos != std::string::npos && !localName.empty() && localName.back() == ')')
    {
        localName = localName.substr(0, parenPos);
    }
    // 2. Always build "<Prefix>_<sanitizedName>"
	std::string baseName = _prefix + "_" + sanitizesForSpice(localName);

	// 3. Resolve collisions case-insensitively within the current scope (e.g. "Hans Dieter" vs "Hans_Dieter")
	auto& usedInScope = m_usedNetlistNamesByScope[_scope];
    std::string candidate = baseName;
    int collisionIndex = 2;
    while(usedInScope.count(toLowercase(candidate)) > 0)
    {
        candidate = baseName + "_" + std::to_string(collisionIndex++);
	}
	usedInScope.insert(toLowercase(candidate));

	// 4. Register bidirectional mapping between custom name and netlist name
    if (!registerMapping(_customName, candidate, _scope))
    {
        OT_LOG_E("Failed to register element: " + _customName + " with " + candidate);
    }
    return candidate;
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
	auto it = m_netlistToCustom.find(toLowercase(_netlistName));
    if(it != m_netlistToCustom.end())
    {
        return it->second;
	}
	OT_LOG_E("Custom name not found for: " + _netlistName);
	return "";
}

std::string ElementNamingRegistry::generateNextId(const std::string& _prefix)
{
	m_elementCounters[_prefix]++;
	return _prefix + std::to_string(m_elementCounters[_prefix]);
}

bool ElementNamingRegistry::registerMapping(const std::string& _customName, const std::string& _netlistName, const std::string& _scope)
{

    if (m_customToNetlist.find(_customName) != m_customToNetlist.end())
    {
		OT_LOG_E("Custom name already exists: " + _customName);
        return false;
    }

    std::string reverseKey;
    if(_scope.empty())
    {
		reverseKey = toLowercase(_netlistName);
    }
    else
	{
		reverseKey = toLowercase(_scope + "." + _netlistName);
	}

    if (m_netlistToCustom.find(reverseKey) != m_netlistToCustom.end())
    {
        OT_LOG_E("Netlist name already exists: " + reverseKey);
		return false;
    }

	m_customToNetlist[_customName] = _netlistName;
	m_netlistToCustom[reverseKey] = _customName;
	return true;
}

void ElementNamingRegistry::reset()
{
    m_customToNetlist.clear();
    m_netlistToCustom.clear();
    m_elementCounters.clear();
	m_usedNetlistNamesByScope.clear();
}

std::string ElementNamingRegistry::toLowercase(const std::string& str)
{
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
        });
    return lower;
}

std::string ElementNamingRegistry::sanitizesForSpice(const std::string& _rawName)
{
    std::string result;
	result.reserve(_rawName.size());

    for(size_t i = 0; i < _rawName.size(); ++i)
    {
		unsigned char c = static_cast<unsigned char>(_rawName[i]); 

        // Handle common UTF-8 German umlauts (2-byte sequences starting with 0xC3)
        if(c == 0xC3 && i+1 < _rawName.size())
        {
            unsigned char nextChar = static_cast<unsigned char>(_rawName[i + 1]);
            switch(nextChar)
            {
                case 0xA4: result += "ae"; i++; continue; 
                case 0xB6: result += "oe"; i++; continue; 
                case 0xBC: result += "ue"; i++; continue; 
                case 0x84: result += "Ae"; i++; continue; 
                case 0x96: result += "Oe"; i++; continue; 
                case 0x9C: result += "Ue"; i++; continue;
                case 0x9F: result += "ss"; i++; continue; 
                default: break;
            }
		}

        if (std::isalnum(c) || c == '_')
        {
            result += static_cast<char>(c);
        }
        else if (c < 0x80)
        {
            // ASCII space or special char (+, -, /, etc.) -> '_'
            result += '_';
        }
        else
        {
            // Other multi-byte UTF-8 non-ASCII: collapse into single '_'
            if (result.empty() || result.back() != '_')
            {
                result += '_';
            }
        }
    }
    if (result.empty())
    {
        return "elem";
    }
    return result;
}
