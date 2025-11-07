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

class MaterialProperties {
public:
	MaterialProperties();
	virtual ~MaterialProperties();

	void loadMaterialData(std::map<std::string, EntityProperties>& materialsByName);

	void setType(const std::string& _type) { m_type = _type; }
	void setPermittivityRelative(double _epsilonR) { m_permittivityRel = _epsilonR; }
	void setPermeabilityRelative(double _muR) { m_permeabilityRel = _muR; }
	void setConductivity(double _sigma) { m_conductivity = _sigma; }
	void setPriority(double _priority) { m_priority = _priority; }
	void setPrimitives(double _x, double _y, double _z);

	std::string getType() const { return m_type; }
	double getPermittivityRelative() const { return m_permittivityRel; }
	double getPermeabilityRelative() const { return m_permeabilityRel; }
	double getConductivity() const { return m_conductivity; }

	tinyxml2::XMLElement* writeMaterialProperties(tinyxml2::XMLElement& _parentElement) const;

private:

	std::string m_type = "Default";
	double m_permittivityRel = 0.0;
	double m_permeabilityRel = 0.0;
	double m_conductivity = 0.0;
	double m_priority = 0.0;

	double m_primitives_X = 0.0;
	double m_primitives_Y = 0.0;
	double m_primitives_Z = 0.0;
};
