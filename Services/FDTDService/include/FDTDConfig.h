// @otlicense
// File: FDTDConfig.h
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
#include "Excitation/ExcitationTypes.h"
#include "CSXMeshGrid.h"
#include "CartesianMeshToSTL.h"
#include "OTCore/OTClassHelper.h"

// tinyxml2
#include <tinyxml2.h>

// STD
#include <string>
#include <cstdint>
#include <array>
#include <memory>

// Forward declaration
class EntityBase;
class ExcitationBase;
class CartesianMeshToSTL;

//! @brief Class to hold the FDTD configuration for the openEMS solver
//! @brief This class containts the central addToXML function to write the complete configuration to an XML file
class FDTDConfig {
	OT_DECL_DEFCOPY(FDTDConfig)
public:
	FDTDConfig();
	virtual ~FDTDConfig();

	uint32_t getTimeSteps() const { return m_timeSteps; }
	double getEndCriteria() const { return m_endCriteria; }
	double getFrequencyStart() const { return m_freqStart; }
	double getFrequencyStop() const { return m_freqStop; }
	uint32_t getOversampling() const { return m_oversampling; }
	std::array<std::string, 6> getBoundaryConditions() const { return m_boundaryConditions; }
	std::string getBoundaryConditions(size_t index) const;
	uint32_t getExcitationType() const;

	void setTimeSteps(uint32_t _timeSteps) { m_timeSteps = _timeSteps; }
	void setExcitationType(ExcitationTypes _excitationType) { m_excitationType = _excitationType; }
	//! @brief Sets the excitation type from a uint32_t value
	//! @param _value The excitation type as uint32_t (currently supporting 0: Gaussian, 1: Sinusoidal)
	void setExcitationType(uint32_t _value);
	void setEndCriteria(double _endCriteria) { m_endCriteria = _endCriteria; }
	void setFrequencyStart(double _freqStart) { m_freqStart = _freqStart; }
	void setFrequencyStop(double _freqStop) { m_freqStop = _freqStop; }
	void setOverSampling(uint32_t _overSampling) { m_oversampling = _overSampling; }
	void setBoundaryCondition(const std::array<std::string, 6>& _values);
	void setBoundaryCondition(size_t _index, const std::string& _value);

	//! @brief This function sets the solver entity from which the configuration will be read
	//! @param _solverEntity The solver entity containing the configuration properties
	void setFromEntity(EntityBase* _solverEntity);

	//! @brief This function loads the STL mesh from the given mesh name and puts the STL files in the temporary folder
	//! @brief Wrapper for the CartesianMeshToSTL constructor to load the mesh
	//! @param _meshName The name of the mesh to load
	//! @param _tmpFolderName The temporary folder where the mesh files are located
	void loadSTLMesh(const std::string& _meshName, const std::string& _tmpFolderName);

	//! @brief This function creates an XML element for the FDTD configuration 
	//! @param _parentElement The parent XML element to which the FDTD configuration will be added
	//! @return The created FDTD XML element
	tinyxml2::XMLElement* writeToXML(tinyxml2::XMLElement& _parentElement);

	//! @brief Reads the configuration from the entity properties and writes the XML file
	//! @param _doc The XML document to which the configuration will be added
	void addToXML(tinyxml2::XMLDocument& _doc);

private:
	uint32_t m_timeSteps = 1000; //default = 1000
	ExcitationTypes m_excitationType = ExcitationTypes::GAUSSIAN; //default = Gaussian (0)
	double m_endCriteria = 1e-5; //default = 1e-5
	double m_freqStart = 1000.0; //default = 1000.0
	double m_freqStop = 2000.0; //default = 2000.0
	uint32_t m_oversampling = 6; //default = 6
	std::array<std::string, 6> m_boundaryConditions = { "PEC", "PEC", "PEC", "PEC", "PEC", "PEC" }; // default = PEC on all sides

	// Valid boundary condition types and Names
	const std::array<std::string, 4> m_boundaryConditionTypes = { "PEC", "PMC", "MUR", "PML_8" };
	const std::array<std::string, 6> m_boundaryNames = { "Xmax", "Xmin", "Ymax", "Ymin", "Zmax", "Zmin" };

	// Excitation type string for property reading
	std::string m_excitationString;

	// CSX Mesh Grid
	CSXMeshGrid m_meshGrid;
	// Excitation object for different excitation types
	std::unique_ptr<ExcitationBase> m_excitation;
	// STL Exporter for the mesh
	std::unique_ptr<CartesianMeshToSTL> m_stlExporter;

	//! @brief Sets the excitation properties based on the selected excitation type
	void setExcitationProperties();
	//! @brief Performs property checking and adjustments after reading the FDTD properties from the entity
	void FDTDpropertyChecking();
};
