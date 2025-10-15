/*
 * FDTDConfig.cpp
 *
 *  Created on: 08.10.2025
 *	Author: Alexandros McCray (alexm-dev)
 *  Copyright (c) 2025
 */

#include "FDTDConfig.h"
#include "FDTDPropertyReader.hpp"

FDTDConfig::FDTDConfig()
{
}

FDTDConfig::~FDTDConfig()
{
}

uint32_t FDTDConfig::getTimeSteps() const {
	return m_timeSteps;
}

double FDTDConfig::getEndCriteria() const {
	return m_endCriteria;
}

double FDTDConfig::getFrequencyStart() const {
	return m_freqStart;
}

double FDTDConfig::getFrequencyStop() const {
	return m_freqStop;
}

uint8_t FDTDConfig::getOversampling() const {
	return m_oversampling;
}

std::array<std::string, 6> FDTDConfig::getBoundaryConditions() const {
	return m_boundaryConditions;
}

std::string FDTDConfig::getBoundaryConditions(size_t index) const {
	if (index >= m_boundaryConditions.size()) {
		throw std::out_of_range("[Boundary Conditions] Index out of range!");
	}
	return m_boundaryConditions[index];
}

uint8_t FDTDConfig::getExcitationType() const {
	return static_cast<uint8_t>(m_excitation);
}

void FDTDConfig::setTimeSteps(uint32_t timeSteps) {
	m_timeSteps = timeSteps;
}

void FDTDConfig::setExcitationType(ExcitationType excitationType) {
	m_excitation = excitationType;
}

void FDTDConfig::setExcitationType(uint8_t value) {
	if (value > 2 || value < 0) {
		throw std::invalid_argument("[Excitation Type] Invalid excitation type! Must be 0 (Gaussian), 1 (Sinusoidal), or 2 (Ramp)");
	}
	m_excitation = static_cast<ExcitationType>(value);
}

void FDTDConfig::setEndCriteria(double endCriteria) {
	m_endCriteria = endCriteria;
}

void FDTDConfig::setFrequencyStart(double freqStart) {
	m_freqStart = freqStart;
}

void FDTDConfig::setFrequencyStop(double freqStop) {
	m_freqStop = freqStop;
}

void FDTDConfig::setOverSampling(uint8_t overSampling) {
	m_oversampling = overSampling;
}

void FDTDConfig::setBoundaryCondition(std::array<std::string, 6> values) {
	for (const auto& val : values) {
		if (std::find(m_boundaryConditionTypes.begin(), m_boundaryConditionTypes.end(), val) == m_boundaryConditionTypes.end()) {
			throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + val));
		}
	}
	m_boundaryConditions = values;
}

void FDTDConfig::setBoundaryCondition(size_t index, std::string value) {
	if (std::find(m_boundaryConditionTypes.begin(), m_boundaryConditionTypes.end(), value) == m_boundaryConditionTypes.end()) {
		throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + value));
	}
	if (index >= m_boundaryConditions.size() || index < 0) {
		throw std::out_of_range(std::string("[Boundary Condition] Index out of range"));
	}
	m_boundaryConditions[index] = value;
}

tinyxml2::XMLElement* FDTDConfig::writeFDTD(tinyxml2::XMLDocument& doc, EntityBase* solverEntity) {
	m_freqStart = readFrequencyStartInfo(solverEntity);
	m_freqStop = readFrequencyStopInfo(solverEntity);
	const double f0 = (m_freqStart + m_freqStop) / 2.0;
	const double fc = (m_freqStop - m_freqStart) / 2.0;

	const auto& FDTD = doc.NewElement("FDTD");
	FDTD->SetAttribute("NumberOfTimesteps", readTimestepInfo(solverEntity));
	FDTD->SetAttribute("OverSampling", readOversamplingInfo(solverEntity));
	FDTD->SetAttribute("endCriteria", readEndCriteriaInfo(solverEntity));
	FDTD->SetAttribute("f_max", m_freqStop);
	doc.InsertFirstChild(FDTD);

	// Now we add the excitation and boundary conditions nodes to the FDTD root node;
	const auto& excitation = doc.NewElement("Excitation");
	excitation->SetAttribute("Type", static_cast<int>(readExcitationTypeInfo(solverEntity)));
	excitation->SetAttribute("f0", f0);
	excitation->SetAttribute("fc", fc);
	FDTD->InsertEndChild(excitation);

	const auto& boundary = doc.NewElement("BoundaryCond");
	for (size_t i = 0; i < m_boundaryNames.size(); ++i) {
		boundary->SetAttribute(m_boundaryNames[i].c_str(), readBoundaryConditions(solverEntity)[i].c_str());
	}
	FDTD->InsertEndChild(boundary);

	return FDTD;
}

uint32_t FDTDConfig::readTimestepInfo(EntityBase* solverEntity) {
	return readEntityPropertiesInfo<uint32_t>(solverEntity, "Simulation Settings", simulationSettingsProperties, "Timesteps");
}

uint8_t FDTDConfig::readExcitationTypeInfo(EntityBase* solverEntity) {
	std::string excitation = readEntityPropertiesInfo<std::string> (solverEntity, "Simulation Settings", simulationSettingsProperties, "Excitation type");
	if (excitation == "Gauss Excitation") {
		m_excitation = ExcitationType::GAUSSIAN;
	}
	else if (excitation == "Sinus Excitation") {
		m_excitation = ExcitationType::SINUSOIDAL;
	}
	else {
		throw std::invalid_argument("[Excitation Type] Invalid excitation type! Must be 'Gaussian', 'Sinusoidal', or 'Ramp'");
	}
	return static_cast<uint8_t>(m_excitation);
}

uint16_t FDTDConfig::readOversamplingInfo(EntityBase* solverEntity) {
	return readEntityPropertiesInfo<uint16_t>(solverEntity, "Simulation Settings", simulationSettingsProperties, "Oversampling");;
}

double FDTDConfig::readEndCriteriaInfo(EntityBase* solverEntity) {
	return readEntityPropertiesInfo<double>(solverEntity, "Simulation Settings", simulationSettingsProperties, "End Criteria");
}

double FDTDConfig::readFrequencyStartInfo(EntityBase* solverEntity) {
	return readEntityPropertiesInfo<double>(solverEntity, "Frequency", frequencyProperties, "Start Frequency");
}

double FDTDConfig::readFrequencyStopInfo(EntityBase* solverEntity) {
	return readEntityPropertiesInfo<double>(solverEntity, "Frequency", frequencyProperties, "End Frequency");
}

std::array<std::string, 6> FDTDConfig::readBoundaryConditions(EntityBase* solverEntity) {
	ot::ModelServiceAPI::getEntityProperties(solverEntity->getName(), true, "Boundary Conditions", boundaryConditionProperties);
	for (auto& item : boundaryConditionProperties) {
		EntityProperties& props = item.second;
		for (size_t i = 0; i < m_boundaryNames.size(); ++i) {
			EntityPropertiesSelection* boundaryCondition = dynamic_cast<EntityPropertiesSelection*>(props.getProperty(m_boundaryNames[i]));
			if (boundaryCondition != nullptr) {
				m_boundaryConditions[i] = boundaryCondition->getValue();
			}
		}
	}
	return m_boundaryConditions;
}

void FDTDConfig::addToXML(EntityBase* solverEntity, std::string& tempPath) {
	tinyxml2::XMLDocument doc;	
	auto declaration = doc.NewDeclaration("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	doc.InsertFirstChild(declaration);
	auto root = doc.NewElement("openEMS");
	doc.InsertFirstChild(root);
	auto FDTD = writeFDTD(doc, solverEntity);
	root->InsertEndChild(FDTD);
	if (doc.SaveFile(tempPath.c_str()) != tinyxml2::XML_SUCCESS) {
		OT_LOG_EA("Failed to write FDTD configuration to XML file.");
	}
}