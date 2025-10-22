//! @file FDTDConfig.h
//! @author Alexandros McCray (alexm-dev)
//! @date 08.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// TINYXML2
#include "tinyxml2.h"

// OpenTwin
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/OTClassHelper.h"
#include "PropertyHelper.h"
#include "EntityProperties.h"

// STD
#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <map>

// Forwarard declaration
class EntityBase;

//! @brief Class to hold the FDTD configuration for the openEMS solver
class FDTDConfig {
	OT_DECL_DEFCOPY(FDTDConfig)
public:
	// Enum for excitation types
	enum class ExcitationType : uint32_t {
		GAUSSIAN = 0,
		SINUSOIDAL = 1,
	};
	FDTDConfig();
	virtual ~FDTDConfig();

	uint32_t getTimeSteps() const;
	double getEndCriteria() const;
	double getFrequencyStart() const;
	double getFrequencyStop() const;
	uint32_t getOversampling() const;
	std::array<std::string, 6> getBoundaryConditions() const;
	std::string getBoundaryConditions(size_t index) const;
	uint32_t getExcitationType() const;

	void setTimeSteps(uint32_t _timeSteps);
	void setExcitationType(ExcitationType _excitationType);
	void setExcitationType(uint32_t _value);
	void setEndCriteria(double _endCriteria);
	void setFrequencyStart(double _freqStart);
	void setFrequencyStop(double _freqStop);
	void setOverSampling(uint32_t _overSampling);
	void setBoundaryCondition(const std::array<std::string, 6>& _values);
	void setBoundaryCondition(size_t _index, const std::string& _value);

	//! @brief This function sets the solver entity from which the configuration will be read
	void setFromEntity(EntityBase* _solverEntity);

	//! @brief This function ensures that the solver entity has been set before reading configuration
	//! @brief Will throw an exception if the entity is not set
	void ensureEntityIsSet() const;

	//! @brief This function creates an XML element for the FDTD configuration 
	//! @param _parentElement The parent XML element to which the FDTD configuration will be added
	//! @return The created FDTD XML element
	tinyxml2::XMLElement* writeFDTD(tinyxml2::XMLElement& _parentElement);

	//! @brief Reads the configuration from the entity properties and writes the XML file
	//! @param _doc The XML document to which the configuration will be added
	void addToXML(tinyxml2::XMLDocument& _doc);

private:
	uint32_t m_timeSteps = 1000; //default = 1000
	ExcitationType m_excitation = ExcitationType::GAUSSIAN; //default = Gaussian (0)
	double m_endCriteria = 1e-5; //default = 1e-5
	double m_freqStart = 1000.0; //default = 1000.0
	double m_freqStop = 2000.0; //default = 2000.0
	uint8_t m_oversampling = 6; //default = 6
	std::array<std::string, 6> m_boundaryConditions = { "PEC", "PEC", "PEC", "PEC", "PEC", "PEC" }; // default = PEC on all sides

	// Valid boundary condition types and Names
	const std::array<std::string, 4> m_boundaryConditionTypes = { "PEC", "PMC", "MUR", "PML_8" };
	const std::array<std::string, 6> m_boundaryNames = { "Xmax", "Xmin", "Ymax", "Ymin", "Zmax", "Zmin" };

	// Solver entiy from which to read the configuration
	EntityBase* m_solverEntity = nullptr;

	//! @brief Refreshes the property information from the entity properties
	//! @brief Calls the helper functions to read each configuration item
	//! @brief Uses a safe read mechanism to log errors and set default values if reading fails
	void refreshPropertyInfo();

	// Helper functions to read the configuration from the entity properties
	// Usage of template function readEntityPropertiesInfo to read different types of properties
	// Calls readEntityPropertiesInfo with the appropriate type
	void readTimestepInfo();
	void readExcitationTypeInfo();
	void readEndCriteriaInfo();
	void readFrequencyStartInfo();
	void readFrequencyStopInfo();
	void readOversamplingInfo();
	void readBoundaryConditions();
};
