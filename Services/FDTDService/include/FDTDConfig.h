/*
 * FDTDConfig.h
 *
 *  Created on: 08.10.2025
 *	Author: Alexandros McCray (alexm-dev)
 *  Copyright (c) 2025
 */

#pragma once

// TINYXML2
#include "tinyxml2.h"

// OpenTwin
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/LogDispatcher.h"
#include "EntityInformation.h"

// STD
#include <type_traits>	// is_integral_v()
#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <vector>
#include <map>

class EntityBase;

//@brief Class to hold the FDTD configuration for the openEMS solver
class FDTDConfig {
public:
	// Enum for excitation types
	enum class ExcitationType : uint8_t {
		GAUSSIAN = 0,
		SINUSOIDAL = 1,
	};
	FDTDConfig();
	virtual ~FDTDConfig();

	uint32_t getTimeSteps() const;
	double getEndCriteria() const;
	double getFrequencyStart() const;
	double getFrequencyStop() const;
	uint8_t getOversampling() const;
	std::array<std::string, 6> getBoundaryConditions() const;
	std::string getBoundaryConditions(size_t index) const;
	uint8_t getExcitationType() const;

	void setTimeSteps(uint32_t timeSteps);
	void setExcitationType(ExcitationType excitationType);
	void setExcitationType(uint8_t value);
	void setEndCriteria(double endCriteria);
	void setFrequencyStart(double freqStart);
	void setFrequencyStop(double freqStop);
	void setOverSampling(uint8_t overSampling);
	void setBoundaryCondition(std::array<std::string, 6> values);
	void setBoundaryCondition(size_t index, std::string value);

	//@brief This function creates an XML element for the FDTD configuration and populates it with the relevant attributes and child elements based on the current configuration settings.
	//@param doc Reference to the XML document
	//@param solverEntity Pointer to the solver entity
	tinyxml2::XMLElement* writeFDTD(tinyxml2::XMLDocument& doc, EntityBase* solverEntity);

	//@brief Reads the configuration from the entity properties and writes the XML file
	//@param solverEntity Pointer to the solver entity
	//@param tempPath Path to the temporary folder where the XML file will be written
	void addToXML(EntityBase* solverEntity, std::string& tempPath);

private:
	uint32_t m_timeSteps = 0;
	ExcitationType m_excitation = ExcitationType::GAUSSIAN; //default = Gaussian (0)
	double m_endCriteria = 1e-5; //default = 1e-5
	double m_freqStart = 0.0;
	double m_freqStop = 0.0;
	uint8_t m_oversampling = 0;
	std::array<std::string, 6> m_boundaryConditions = { "PEC", "PEC", "PEC", "PEC", "PEC", "PEC" }; // default = PEC on all sides
	const std::array<std::string, 4> m_boundaryConditionTypes = { "PEC", "PMC", "MUR", "PML_8" };
	const std::array<std::string, 6> m_boundaryNames = { "xmax", "xmin", "ymax", "ymin", "zmax", "zmin" };

	// Maps to hold the entity properties for different groups
	std::map<ot::UID, EntityProperties> simulationSettingsProperties;
	std::map<ot::UID, EntityProperties> boundaryConditionProperties;
	std::map<ot::UID, EntityProperties> frequencyProperties;

	// Helper functions to read the configuration from the entity properties
	// Usage of template function readEntityPropertiesInfo to read different types of properties
	// Calls readEntityPropertiesInfo with the appropriate type
	uint32_t readTimestepInfo(EntityBase* solverEntity);
	uint8_t readExcitationTypeInfo(EntityBase* solverEntity);
	double readEndCriteriaInfo(EntityBase* solverEntity);
	double readFrequencyStartInfo(EntityBase* solverEntity);
	double readFrequencyStopInfo(EntityBase* solverEntity);
	uint16_t readOversamplingInfo(EntityBase* solverEntity);
	std::array<std::string, 6> readBoundaryConditions(EntityBase* solverEntity);
};
