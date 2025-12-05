// @otlicense
// File: FDTDConfig.cpp
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

// OpenTwin
#include "FDTDConfig.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/LogDispatcher.h"
#include "PropertyHelper.h"
#include "EntityProperties.h"

#include "Excitation/ExcitationProperties.h"
#include "Excitation/ExcitationBase.h"
#include "Excitation/GaussianExcitation.h"
#include "Excitation/SinusoidalExcitation.h"

// STD
#include <stdexcept>


FDTDConfig::FDTDConfig()
{
}

FDTDConfig::~FDTDConfig()
{
}

std::string FDTDConfig::getBoundaryConditions(size_t _index) const {
	if (_index >= m_boundaryConditions.size()) {
		throw std::out_of_range("[Boundary Conditions] Index out of range!");
	}
	return m_boundaryConditions[_index];
}

uint32_t FDTDConfig::getExcitationType() const {
	return static_cast<uint32_t>(m_excitationType);
}

void FDTDConfig::setExcitationType(uint32_t _value) {
	if (_value > 1 || _value < 0) {
		throw std::invalid_argument("[Excitation Type] Invalid excitation type! Must be 0 (Gaussian), 1 (Sinusoidal)");
	}
	m_excitationType = static_cast<ExcitationTypes>(_value);
}

void FDTDConfig::setBoundaryCondition(const std::array<std::string, 6>& _values) {
	for (const auto& val : _values) {
		if (std::find(m_boundaryConditionTypes.begin(), m_boundaryConditionTypes.end(), val) == m_boundaryConditionTypes.end()) {
			throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + val));
		}
	}
	m_boundaryConditions = _values;
}

void FDTDConfig::setBoundaryCondition(size_t _index, const std::string& _value) {
	if (std::find(m_boundaryConditionTypes.begin(), m_boundaryConditionTypes.end(), _value) == m_boundaryConditionTypes.end()) {
		throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + _value));
	}
	if (_index >= m_boundaryConditions.size()) {
		throw std::out_of_range(std::string("[Boundary Condition] Index out of range"));
	}
	m_boundaryConditions[_index] = _value;
}

void FDTDConfig::setFromEntity(EntityBase* _solverEntity) {
	if (!_solverEntity) {
		throw std::invalid_argument("[FDTDConfig] Solver entity pointer is null!");
	}

	// Load basic FDTD properties
	m_timeSteps = PropertyHelper::getIntegerPropertyValue(_solverEntity, "Timesteps", "Simulation Settings");
	m_endCriteria = PropertyHelper::getDoublePropertyValue(_solverEntity, "End Criteria", "Simulation Settings");
	m_freqStart = PropertyHelper::getDoublePropertyValue(_solverEntity, "Start Frequency", "Frequency");
	m_freqStop = PropertyHelper::getDoublePropertyValue(_solverEntity, "End Frequency", "Frequency");
	m_oversampling = PropertyHelper::getIntegerPropertyValue(_solverEntity, "Oversampling", "Simulation Settings");

	// Load excitation type
	m_excitationString = PropertyHelper::getSelectionPropertyValue(_solverEntity, "Excitation Type", "Simulation Settings");

	// Perform property checking and adjustments after reading all basic FDTD properties
	FDTDpropertyChecking();
	// Set excitation properties based on the selected type
	setExcitationProperties();

	// Load boundary conditions
	for (size_t i = 0; i < m_boundaryNames.size(); ++i) {
		auto value = PropertyHelper::getSelectionPropertyValue(_solverEntity, m_boundaryNames[i], "Boundary Conditions");
		if (value.empty()) {
			value = "PEC"; // default to PEC
		}
		m_boundaryConditions[i] = value;
	}

	// Load the mesh grid data from the solver entity
	m_meshGrid.loadMeshGridDataFromEntity(_solverEntity);
}

void FDTDConfig::loadSTLMesh(const std::string& _meshName, const std::string& _tmpFolderName) {
	m_stlExporter = std::make_unique<CartesianMeshToSTL>(_meshName, _tmpFolderName);
}

tinyxml2::XMLElement* FDTDConfig::writeFDTD(tinyxml2::XMLElement& _parentElement) {
	// Defining the boundary names used for the solver XML parser
	// These must match the expected names in the XML and will be different from the GUI
	const std::array<std::string, 6> solverBoundaryNames = { "xmax", "xmin", "ymax", "ymin", "zmax", "zmin" };
	const double f0 = (m_freqStart + m_freqStop) / 2.0;
	const double fc = (m_freqStop - m_freqStart) / 2.0;

	// Now we create the FDTD root node and set its attributes
	auto FDTD = _parentElement.GetDocument()->NewElement("FDTD");
	FDTD->SetAttribute("NumberOfTimesteps", m_timeSteps);
	FDTD->SetAttribute("OverSampling", m_oversampling);
	FDTD->SetAttribute("endCriteria", m_endCriteria);
	FDTD->SetAttribute("f_max", m_freqStop);

	// Now we add the excitation nodes to the FDTD root node
	auto excitation = _parentElement.GetDocument()->NewElement("Excitation");
	excitation->SetAttribute("Type", static_cast<int>(m_excitationType));
	excitation->SetAttribute("f0", f0);
	excitation->SetAttribute("fc", fc);
	FDTD->InsertEndChild(excitation);

	// Finally we add the boundary conditions to the FDTD root node
	auto boundary = _parentElement.GetDocument()->NewElement("BoundaryCond");
	for (size_t i = 0; i < solverBoundaryNames.size(); ++i) {
		boundary->SetAttribute(solverBoundaryNames[i].c_str(), m_boundaryConditions[i].c_str());
	}
	FDTD->InsertEndChild(boundary);
	return FDTD;
}

void FDTDConfig::setExcitationProperties() {
	switch (m_excitationType) {
		case ExcitationTypes::GAUSSIAN:
			m_excitation = std::make_unique<GaussianExcitation>();
			break;
		case ExcitationTypes::SINUSOIDAL:
			m_excitation = std::make_unique<SinusoidalExcitation>();
			break;
		default:
			OT_LOG_W("[Excitation Type] Unknown excitation type! Defaulting to Gaussian.");
			m_excitation = std::make_unique<GaussianExcitation>();
			break;
	}
	m_excitation->applyProperties();
}

void FDTDConfig::FDTDpropertyChecking() {
	if (m_freqStart >= m_freqStop) {
		OT_LOG_WA("[FDTDConfig] [Frequency] Start Frequency is greater than or equal to End Frequency. Adjusting End Frequency.");
		m_freqStart = m_freqStop * 0.1; // default 10% higher than start frequency
	}

	if (m_timeSteps == 0) {
		OT_LOG_WA("[FDTDConfig] [Timesteps] Number of timesteps is set to zero. Adjusting to default value of 1000.");
		m_timeSteps = 1000; // default to 1000 timesteps
	}

	if (m_excitationString == "Gauss Excitation") {
		m_excitationType = ExcitationTypes::GAUSSIAN;
	}
	else if (m_excitationString == "Sinus Excitation") {
		m_excitationType = ExcitationTypes::SINUSOIDAL;
	}
	else {
		OT_LOG_WA("[Excitation Type] Invalid excitation type string. Defaulting to Gaussian.");
		m_excitationType = ExcitationTypes::GAUSSIAN;
	}
}

void FDTDConfig::addToXML(tinyxml2::XMLDocument& _doc) {
	auto declaration = _doc.NewDeclaration("xml version=\"1.0\" encoding=\"utf-8\"");
	_doc.InsertFirstChild(declaration);
	auto root = _doc.NewElement("openEMS");
	_doc.InsertEndChild(root);
	auto FDTD = writeFDTD(*root);
	root->InsertEndChild(FDTD);
	auto CSX = _doc.NewElement("ContinuousStructure");
	auto CSXProperties = _doc.NewElement("Properties");

	// load and write the excitation properties
	auto& excitations = m_excitation->getExciteProperties();
	CSXProperties->InsertEndChild(excitations.writeExciteProperties(*CSXProperties));
	auto stlFile = m_stlExporter->writeToXML(*CSXProperties);
	CSXProperties->InsertEndChild(stlFile);

	CSX->InsertEndChild(CSXProperties);
	auto CSXRectGrid = m_meshGrid.writeCSXMeshGrid(*root);
	CSX->InsertEndChild(CSXRectGrid);
	root->InsertEndChild(CSX);
}
