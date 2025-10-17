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
#include "EntityProperties.h"

// STD
#include <type_traits>	// is_integral_v()
#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <vector>
#include <map>

// Forwarard declaration
class EntityBase;

//! @brief Class to hold the FDTD configuration for the openEMS solver
class FDTDConfig {
	OT_DECL_DEFCOPY(FDTDConfig)
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

	void setTimeSteps(uint32_t _timeSteps);
	void setExcitationType(ExcitationType _excitationType);
	void setExcitationType(uint8_t _value);
	void setEndCriteria(double _endCriteria);
	void setFrequencyStart(double _freqStart);
	void setFrequencyStop(double _freqStop);
	void setOverSampling(uint8_t _overSampling);
	void setBoundaryCondition(std::array<std::string, 6> _values);
	void setBoundaryCondition(size_t _index, std::string _value);

	//! @brief This function sets the solver entity from which the configuration will be read
	void setFromEntity(EntityBase* _solverEntity);

	//! @brief This function ensures that the solver entity has been set before reading configuration
	void ensureEntityIsSet() const;

	//! @brief This function creates an XML element for the FDTD configuration 
	//! and populates it with the relevant attributes and child elements based on the current configuration settings.
	//! @param doc Reference to the XML document.
	//! @param solverEntity Pointer to the solver entity.
	tinyxml2::XMLElement* writeFDTD(tinyxml2::XMLElement& _parentElement);

	//! @brief Reads the configuration from the entity properties and writes the XML file
	//! @param solverEntity Pointer to the solver entity
	//! @param tempPath Path to the temporary folder where the XML file will be written
	void addToXML(tinyxml2::XMLDocument& doc);

private:
	uint32_t m_timeSteps = 0;
	ExcitationType m_excitation = ExcitationType::GAUSSIAN; //default = Gaussian (0)
	double m_endCriteria = 1e-5; //default = 1e-5
	double m_freqStart = 0.0;
	double m_freqStop = 0.0;
	uint8_t m_oversampling = 0;
	std::array<std::string, 6> m_boundaryConditions = { "PEC", "PEC", "PEC", "PEC", "PEC", "PEC" }; // default = PEC on all sides
	const std::array<std::string, 4> m_boundaryConditionTypes = { "PEC", "PMC", "MUR", "PML_8" };
	const std::array<std::string, 6> m_boundaryNames = { "Xmax", "Xmin", "Ymax", "Ymin", "Zmax", "Zmin" };

	// Maps to hold the entity properties for different groups
	std::map<ot::UID, EntityProperties> m_simulationSettingsProperties;
	std::map<ot::UID, EntityProperties> m_boundaryConditionProperties;
	std::map<ot::UID, EntityProperties> m_frequencyProperties;

	EntityBase* m_solverEntity = nullptr;

	// Helper functions to read the configuration from the entity properties
	// Usage of template function readEntityPropertiesInfo to read different types of properties
	// Calls readEntityPropertiesInfo with the appropriate type
	uint32_t readTimestepInfo();
	uint8_t readExcitationTypeInfo();
	double readEndCriteriaInfo();
	double readFrequencyStartInfo();
	double readFrequencyStopInfo();
	uint16_t readOversamplingInfo();
	std::array<std::string, 6> readBoundaryConditions();
};
