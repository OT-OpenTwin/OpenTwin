// @otLicense

#pragma once

// TINYXML2
#include "tinyxml2.h"

// OpenTwin
#include "OTCore/CoreTypes.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/OTClassHelper.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityMeshCartesianData.h"
#include "EntityProperties.h"
#include "EntityInformation.h"
#include "EntityAPI.h"

// STD
#include <memory>
#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <map>

// Forward declaration
class EntityBase;

//! @brief Class to hold the mesh grid data for the openEMS solver
//! This class is responsible for loading and storing the mesh grid lines and properties
class CSXMeshGrid {
	OT_DECL_DEFCOPY(CSXMeshGrid)
public:
	CSXMeshGrid();
	virtual ~CSXMeshGrid();

	//! @brief Loads the mesh grid data from the given solver entity
	//! @brief Acts as an setter for each member variable
	//! @param _solverEntity The solver entity to load data from
	void loadMeshGridDataFromEntity(EntityBase* _solverEntity);

	//! @brief Getters for the mesh grid X lines
	const std::vector<double>& getXLines() const { return m_gridX; }
	//! @brief Getters for the mesh grid Y lines
	const std::vector<double>& getYLines() const { return m_gridY; }
	//! @brief Getters for the mesh grid Z lines
	const std::vector<double>& getZLines() const { return m_gridZ; }
	//! @brief Getter for the delta unit
	double getDeltaUnit() const { return m_deltaUnit; }
	//! @brief Getter for the step ratio
	double getStepRatio() const { return m_stepRatio; }
	//! @brief Getter for the coordinate system
	uint32_t getCoordSystem() const { return m_coordSystem; }

	//! @brief Creates an XML element for the mesh grid data
	//! @brief Is being used by FDTDConfig to then write the complete solver XML
	//! @param _parentElement The parent XML element to which the mesh grid data will be added
	//! @return The created mesh grid XML element
	tinyxml2::XMLElement* writeCSXMeshGrid(tinyxml2::XMLElement& _parentElement) const;

private:
	// Mesh grid data
	std::vector<double> m_gridX;
	std::vector<double> m_gridY;
	std::vector<double> m_gridZ;

	// Mesh grid properties
	uint32_t m_coordSystem = 0; // 0 = Cartesian
	double m_deltaUnit = 0.001; // in mm
	double m_stepRatio = 1.0; //1.0

	//! @brief Converts a vector of doubles to a comma-separated string
	//! @brief Formating for solver XML
	//! @param _vector The vector to convert
	std::string vectorToString(const std::vector<double>& _vector) const;
};