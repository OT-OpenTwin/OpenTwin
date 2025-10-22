//! @file CSXMeshGrid.h
//! @author Alexandros McCray (alexm-dev)
//! @date 22.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OPENTWIN
#include "OTCore/CoreTypes.h"
#include "EntityMeshCartesian.h"
#include "EntityMeshCartesianData.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/LogDispatcher.h"
#include "EntityProperties.h"
#include "EntityInformation.h"
#include "OTCore/OTClassHelper.h"
#include "EntityAPI.h"

// STD
#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <map>


// Forward declaration
class EntityBase;

class CSXMeshGrid {
	OT_DECL_DEFCOPY(CSXMeshGrid)
public:
	CSXMeshGrid();
	virtual ~CSXMeshGrid();

	void loadMeshGridDataFromEntity(EntityBase* _solverEntity);

	const std::vector<double>& getXLines() const { return m_gridX; }
	const std::vector<double>& getYLines() const { return m_gridY; }
	const std::vector<double>& getZLines() const { return m_gridZ; }

	double getDeltaUnit() const { return m_deltaUnit; }
	double getStepRatio() const { return m_stepRatio; }
	uint32_t getCoordSystem() const { return m_coordSystem; }

	std::string vectorToString(const std::vector<double>& _vector) const;

private:
	void readMeshGridData();

	std::vector<double> m_gridX;
	std::vector<double> m_gridY;
	std::vector<double> m_gridZ;

	uint32_t m_coordSystem = 0; // 0 = Cartesian
	double m_deltaUnit = 0.001; // in mm
	double m_stepRatio = 1.0;
};