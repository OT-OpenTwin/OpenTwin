// @otlicense
// File: MaterialProperties.cpp
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

// OPENTWIN
#include "Materials/MaterialProperties.h"

#include "OTCore/CoreTypes.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityProperties.h"
#include "EntityInformation.h"


MaterialProperties::MaterialProperties()
{
}

MaterialProperties::~MaterialProperties()
{
}

void MaterialProperties::setPrimitives(double _x, double _y, double _z) {
	m_primitives_X = _x;
	m_primitives_Y = _y;
	m_primitives_Z = _z;
}

void MaterialProperties::loadMaterialData(std::map<std::string, EntityProperties>& _materialProperty) {
	if (_materialProperty.empty()) {
		setType("Default");
		setPermittivityRelative(0.0);
		setPermeabilityRelative(0.0);
		setConductivity(0.0);
		setPriority(0.0);
		return;
	}
	auto it = _materialProperty.begin();
	
	const std::string selectedName = it->first;
	const EntityProperties& matProps = it->second;

	// Set type to the entity name as a sensible default
	m_type = selectedName;

	const auto* materialTypeProp = matProps.getProperty("Material type", "General");
	if (materialTypeProp != nullptr) {
		const EntityPropertiesSelection* selection = dynamic_cast<const EntityPropertiesSelection*>(materialTypeProp);
		if (selection != nullptr) {
			m_type = selection->getValue();
		}
	}

	const auto* epsProp = matProps.getProperty("Permittivity (relative)", "Electromagnetic");
	if (epsProp != nullptr) {
		const EntityPropertiesDouble* doubleProp = dynamic_cast<const EntityPropertiesDouble*>(epsProp);
		if (doubleProp != nullptr) {
			m_permittivityRel = doubleProp->getValue();
		}
		else {
			m_permittivityRel = 0.0;
		}
	}
	else {
		m_permittivityRel = 0.0;
	}

	const auto* muProp = matProps.getProperty("Permeability (relative)", "Electromagnetic");
	if (muProp != nullptr) {
		const EntityPropertiesDouble* doubleProp = dynamic_cast<const EntityPropertiesDouble*>(muProp);
		if (doubleProp != nullptr) {
			m_permeabilityRel = doubleProp->getValue();
		}
		else {
			m_permeabilityRel = 0.0;
		}
	}
	else {
		m_permeabilityRel = 0.0;
	}

	const auto* sigmaProp = matProps.getProperty("Conductivity", "Electromagnetic");
	if (sigmaProp != nullptr) {
		const EntityPropertiesDouble* doubleProp = dynamic_cast<const EntityPropertiesDouble*>(sigmaProp);
		if (doubleProp != nullptr) {
			m_conductivity = doubleProp->getValue();
		}
		else {
			m_conductivity = 0.0;
		}
	}
	else {
		m_conductivity = 0.0;
	}

	const auto* priProp = matProps.getProperty("Mesh priority", "General");
	if (priProp != nullptr) {
		const EntityPropertiesDouble* doubleProp = dynamic_cast<const EntityPropertiesDouble*>(priProp);
		if (doubleProp != nullptr) {
			m_priority = doubleProp->getValue();
		}
		else {
			m_priority = 0.0;
		}
	}
	else {
		m_priority = 0.0;
	}
}

tinyxml2::XMLElement* MaterialProperties::writeMaterialProperties(tinyxml2::XMLElement& _parentElement) const {
	auto materialElement = _parentElement.GetDocument()->NewElement("Material");
	auto propertyElement = _parentElement.GetDocument()->NewElement("Property");
	auto primitivesElement = _parentElement.GetDocument()->NewElement("Primitives");
	auto boxElement = _parentElement.GetDocument()->NewElement("Box");
	materialElement->SetAttribute("Name", m_type.c_str());
	boxElement->SetAttribute("Priority", m_priority);
	if (m_permittivityRel != 0.0) {
		propertyElement->SetAttribute("Epsilon", m_permittivityRel);
	}
	if (m_permeabilityRel != 0.0) {
		propertyElement->SetAttribute("Mu", m_permeabilityRel);
	}
	if (m_conductivity != 0.0) {
		propertyElement->SetAttribute("Sigma", m_conductivity);
	}
	materialElement->InsertEndChild(propertyElement);

	primitivesElement->InsertEndChild(boxElement);
	materialElement->InsertEndChild(primitivesElement);
	return materialElement;
}
