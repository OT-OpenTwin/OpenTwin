// @otlicense
// File: CSXMeshGrid.h
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

// OpenTwin
#include "OTCore/OTClassHelper.h"

// tinyxml2
#include <tinyxml2.h>

// STD
#include <cstdint>
#include <string>
#include <vector>

// Forward declaration
class EntityBase;

//! @brief Class to hold the mesh grid data for the openEMS solver
//! This class is responsible for loading and storing the mesh grid lines and properties
class CSXMeshGrid {
	OT_DECL_DEFCOPY(CSXMeshGrid)
public:
	//! @brief Coordinate system enumeration
	//! represents the different coordinate systems available for the mesh grid
	enum class CoordSystem : uint32_t {
		Cartesian = 0,
		Cylindrical = 1,
		Spherical = 2
	};
	CSXMeshGrid();
	virtual ~CSXMeshGrid();

	//! @brief Loads the mesh grid data from the given solver entity
	//! @brief Acts as an setter for each member variable
	//! @param _solverEntity The solver entity to load data from
	void loadMeshGridDataFromEntity(EntityBase* _solverEntity);

	const std::vector<double>& getXLines() const { return m_gridX; }
	const std::vector<double>& getYLines() const { return m_gridY; }
	const std::vector<double>& getZLines() const { return m_gridZ; }
	double getDeltaUnit() const { return m_deltaUnit; }
	double getStepRatio() const { return m_stepRatio; }
	CoordSystem getCoordSystem() const { return m_coordSystem; }

	//! @brief Creates an XML element for the mesh grid data
	//! @brief Is being used by FDTDConfig to then write the complete solver XML
	//! @param _parentElement The parent XML element to which the mesh grid data will be added
	//! @return The created mesh grid XML element
	tinyxml2::XMLElement* writeToXML(tinyxml2::XMLElement& _parentElement) const;

private:
	// Mesh grid data
	std::vector<double> m_gridX;
	std::vector<double> m_gridY;
	std::vector<double> m_gridZ;

	// Mesh grid properties
	CoordSystem m_coordSystem = CoordSystem::Cartesian;
	double m_deltaUnit = 0.0; // default 0.0
	double m_stepRatio = 0.0; // default 0.0

	//! @brief Converts a vector of doubles to a comma-separated string
	//! @brief Formating for solver XML
	//! @param _vector The vector to convert
	std::string vectorToString(const std::vector<double>& _vector) const;
};