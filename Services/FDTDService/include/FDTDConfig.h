// @otLicense
// @otLicense-end
//! @file FDTDConfig.h
//! @brief Definition of the FDTDConfig class for managing FDTD simulation configuration in openEMS
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

#include "ExcitationProperties.h"
#include "ExcitationTypes.h"
#include "CSXMeshGrid.h"

// STD
#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <memory>

// Forward declaration
class EntityBase;
class ExcitationBase;

//! @brief Class to hold the FDTD configuration for the openEMS solver
//! @brief This class containts the central addToXML function to write the complete configuration to an XML file
class FDTDConfig {
	OT_DECL_DEFCOPY(FDTDConfig)
public:
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
	void setExcitationType(ExcitationTypes _excitationType);
	void setExcitationType(uint32_t _value);	
	void setEndCriteria(double _endCriteria);
	void setFrequencyStart(double _freqStart);
	void setFrequencyStop(double _freqStop);
	void setOverSampling(uint32_t _overSampling);
	void setBoundaryCondition(const std::array<std::string, 6>& _values);
	void setBoundaryCondition(size_t _index, const std::string& _value);

	//! @brief This function sets the solver entity from which the configuration will be read
	//! @param _solverEntity The solver entity containing the configuration properties
	void setFromEntity(EntityBase* _solverEntity);

	//! @brief This function creates an XML element for the FDTD configuration 
	//! @param _parentElement The parent XML element to which the FDTD configuration will be added
	//! @return The created FDTD XML element
	tinyxml2::XMLElement* writeFDTD(tinyxml2::XMLElement& _parentElement);

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

	std::string m_excitationString;

	// CSX Mesh Grid
	CSXMeshGrid m_meshGrid;

	// Excitation object for different excitation types
	std::unique_ptr<ExcitationBase> m_excitation;

	//! @brief Sets the excitation properties based on the selected excitation type
	void setExcitationProperties();

	void FDTDpropertyChecking();
};
