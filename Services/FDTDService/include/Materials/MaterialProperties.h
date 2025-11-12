// @otlicense
// File: MaterialProperties.h
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

// TINYXML2
#include <tinyxml2.h>

// OpenTwin
#include "EntityProperties.h"

// STD Includes
#include <string>
#include <map>
#include <list>

// Forward declaration
class EntityBase;

// !brief Container class to manage material properties
class MaterialProperties {
public:
	MaterialProperties();
	virtual ~MaterialProperties();

	//! @brief Loads the material properties from an map
	//! @param _materialProperty The already loaded map containing material properties
	void loadMaterialData(std::map<std::string, EntityProperties>& _materialProperty);

	void setType(const std::string& _type) { m_type = _type; }
	void setPermittivityRelative(double _epsilonR) { m_permittivityRel = _epsilonR; }
	void setPermeabilityRelative(double _muR) { m_permeabilityRel = _muR; }
	void setConductivity(double _sigma) { m_conductivity = _sigma; }
	void setPriority(double _priority) { m_priority = _priority; }
	void setPrimitives(double _x, double _y, double _z);
	void setPrimitivesX(double _x) { m_primitives_X = _x; }
	void setPrimitivesY(double _y) { m_primitives_Y = _y; }
	void setPrimitivesZ(double _z) { m_primitives_Z = _z; }

	std::string getType() const { return m_type; }
	double getPermittivityRelative() const { return m_permittivityRel; }
	double getPermeabilityRelative() const { return m_permeabilityRel; }
	double getConductivity() const { return m_conductivity; }
	double getPriority() const { return m_priority; }
	double getPrimitivesX() const { return m_primitives_X; }
	double getPrimitivesY() const { return m_primitives_Y; }
	double getPrimitivesZ() const { return m_primitives_Z; }

	//! @brief Writes the material properties to an XML element
	//! @param _parentElement The parent XML element to write to
	//! @return The created XML element containing the material properties
	tinyxml2::XMLElement* writeMaterialProperties(tinyxml2::XMLElement& _parentElement) const;

private:
	std::string m_type = "Volumetric";
	double m_permittivityRel = 0.0;
	double m_permeabilityRel = 0.0;
	double m_conductivity = 0.0;
	double m_priority = 0.0;

	double m_primitives_X = 0.0;
	double m_primitives_Y = 0.0;
	double m_primitives_Z = 0.0;
};
