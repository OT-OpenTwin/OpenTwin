/*
 * FDTDConfig.cpp
 *
 *  Created on: 08.10.2025
 *	Author: Alexandros McCray (alexm-dev)
 *  Copyright (c) 2025
 */

#include "FDTDConfig.h"

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
	FDTD->SetAttribute("OverSampling", m_oversampling);
	FDTD->SetAttribute("endCriteria", m_endCriteria);
	FDTD->SetAttribute("f_max", m_freqStop);
	doc.InsertFirstChild(FDTD);

	// Now we add the excitation and boundary conditions nodes to the FDTD root node;
	const auto& excitation = doc.NewElement("excitation");
	excitation->SetAttribute("Type", static_cast<int>(m_excitation));
	excitation->SetAttribute("f0", f0);
	excitation->SetAttribute("fc", fc);
	FDTD->InsertEndChild(excitation);

	const auto& boundary = doc.NewElement("boundaryCond");
	for (size_t i = 0; i < m_boundaryNames.size(); ++i) {
		boundary->SetAttribute(m_boundaryNames[i].c_str(), readBoundaryConditions(solverEntity)[i].c_str());
	}
	FDTD->InsertEndChild(boundary);

	return FDTD;
}

uint32_t FDTDConfig::readTimestepInfo(EntityBase* solverEntity) {
	ot::ModelServiceAPI::getEntityProperties(solverEntity->getName(), true, "Simulation Settings", simulationSettingsProperties);
	for (auto& item : simulationSettingsProperties) {
		EntityProperties& props = item.second;
		EntityPropertiesInteger* timeSteps = dynamic_cast<EntityPropertiesInteger*>(props.getProperty("Timesteps"));
		if (timeSteps != nullptr) {
			return timeSteps->getValue();
		}
	}
	OT_LOG_EA("Unable to read the number of timesteps, defaulting to 0.");
	return 0;
}

uint8_t FDTDConfig::readExcitationTypeInfo(EntityBase* solverEntity) {
	ot::ModelServiceAPI::getEntityProperties(solverEntity->getName(), true, "Simulation Settings", simulationSettingsProperties);
	std::list<uint8_t> excitationTypes = { 0, 1, 2 }; // 0: Gaussian, 1: Sine, 2: Ricker
	for (auto& item : simulationSettingsProperties) {
		EntityProperties& props = item.second;
		EntityPropertiesSelection* excitationType = dynamic_cast<EntityPropertiesSelection*>(props.getProperty("Excitation type"));
		if (excitationType != nullptr) {
			std::string value = excitationType->getValue();
			if (value == "Gauss Excitation") return 0;
		}
	}
	OT_LOG_EA("Unable to read the excitation type, defaulting to 0 (Gauss Excitation).");
	return 0;
}

//uint16_t FDTDConfig::readOversamplingInfo(EntityBase* solverEntity)
//{
//	ot::ModelServiceAPI::getEntityProperties(solverEntity->getName(), true, "Simulation Settings", simulationSettingsProperties);
//	for (auto& item : simulationSettingsProperties) {
//		EntityProperties& props = item.second;
//		EntityPropertiesInteger* oversampling = dynamic_cast<EntityPropertiesInteger*>(props.getProperty("Oversampling"));
//		if (oversampling != nullptr) return oversampling->getValue();
//	}
//	OT_LOG_EA("Unable to read the oversampling factor, defaulting to 0.");
//	return 0;
//}

double FDTDConfig::readFrequencyStartInfo(EntityBase* solverEntity) {
	ot::ModelServiceAPI::getEntityProperties(solverEntity->getName(), true, "Frequency", frequencyProperties);
	for (auto& item : frequencyProperties) {
		EntityProperties& props = item.second;
		EntityPropertiesDouble* freqStart = dynamic_cast<EntityPropertiesDouble*>(props.getProperty("Start Frequency"));
		if (freqStart != nullptr) return freqStart->getValue();
	}
	OT_LOG_EA("Unable to read the start frequency, defaulting to 0.");
	return 0;
}

double FDTDConfig::readFrequencyStopInfo(EntityBase* solverEntity) {
	ot::ModelServiceAPI::getEntityProperties(solverEntity->getName(), true, "Frequency", frequencyProperties);
	for (auto& item : frequencyProperties) {
		EntityProperties& props = item.second;
		EntityPropertiesDouble* freqStop = dynamic_cast<EntityPropertiesDouble*>(props.getProperty("End Frequency"));
		if (freqStop != nullptr) return freqStop->getValue();
	}
	OT_LOG_EA("Unable to read the end frequency, defaulting to 0.");
	return 0;
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
	// Implementation for adding configuration to an existing XML document
	tinyxml2::XMLDocument doc;	
	auto root = doc.NewElement("OpenEMS");
	doc.InsertFirstChild(root);
	auto FDTD = writeFDTD(doc, solverEntity);
	root->InsertEndChild(FDTD);
	if (doc.SaveFile(tempPath.c_str()) != tinyxml2::XML_SUCCESS) {
		OT_LOG_EA("Failed to write FDTD configuration to XML file.");
	}
}