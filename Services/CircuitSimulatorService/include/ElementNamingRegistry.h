// @otlicense
// File: ElementNamingRegistry.h
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

// OpenTwin Header
#include "OTCore/OTClassHelper.h"

// std Header
#include <string>
#include <map>

class ElementNamingRegistry
{
	OT_DECL_NOCOPY(ElementNamingRegistry)
public:
	ElementNamingRegistry() = default;

	//! @brief Registers a new element with a custom name and prefix, returning the generated netlist name.
	std::string registerElement(const std::string& _customName, const std::string& _prefix);

	//! @brief Retrieves the netlist name associated with a given custom name.
	std::string getNetlistName(const std::string& _customName) const;

	//! @brief Retrieves the custom name associated with a given netlist name.
	std::string getCustomName(const std::string& _netlistName) const;

	//! @brief Generates the next unique netlist name based on the provided prefix.
	std::string generateNextId(const std::string& _prefix);

	//! @brief Registers a mapping between a custom name and a netlist name. Returns true if successful, false if the mapping already exists.
	bool registerMapping(const std::string& _customName, const std::string& _netlistName);

	//! @brief Resets the registry, clearing all mappings and counters.
	void reset();

	//! @brief Getter for external access to the custom-to-netlist and netlist-to-custom maps
	const std::map<std::string, std::string>& getCustomToNetlistMap() const { return m_customToNetlist; }
	const std::map<std::string, std::string>& getNetlistToCustomMap() const { return m_netlistToCustom; }


	//! @brief Converts a string to lowercase.
	static std::string toLowercase(const std::string& str);
private:
	std::map<std::string, std::string> m_customToNetlist;
	std::map<std::string, std::string> m_netlistToCustom;
	std::map<std::string, int> m_elementCounters;
};