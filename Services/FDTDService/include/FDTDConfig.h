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

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/LogDispatcher.h"

#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <vector>
#include <map>

//@brief Class to hold the FDTD configuration for the FDTD solver
class FDTDConfig {
public:
	// Enum for excitation types
	enum class ExcitationType : uint8_t {
		GAUSSIAN = 0,
		SINUSOIDAL = 1,
		RAMP = 2
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

	tinyxml2::XMLElement* writeFDTD(tinyxml2::XMLDocument& doc, EntityBase* solverEntity);

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
	const std::array<std::string, 6> m_boundaryNames = { "x-max", "x-min", "y-max", "y-min", "z-max", "z-min" };

	std::map<ot::UID, EntityProperties> simulationSettingsProperties;
	std::map<ot::UID, EntityProperties> boundaryConditionProperties;
	std::map<ot::UID, EntityProperties> frequencyProperties;

	// Helper functions to read the configuration from the entity properties
	uint32_t readTimestepInfo(EntityBase* solverEntity);
	uint8_t readExcitationTypeInfo(EntityBase* solverEntity);
	double readFrequencyStartInfo(EntityBase* solverEntity);
	double readFrequencyStopInfo(EntityBase* solverEntity);
	std::array<std::string, 6> readBoundaryConditions(EntityBase* solverEntity);
};