// @otLicense
// @otLicense-end

#pragma once

// TINYXML2
#include "tinyxml2.h"

// STD
#include <string>
#include <cstdint>

//! @brief Class to hold the excitation properties for the openEMS solver
class ExcitationProperties {
public:
	ExcitationProperties();
	virtual ~ExcitationProperties();

	// Setters for the excitation data
	void setName(const std::string& _name) { m_name = _name; }
	void setExcite(const std::string& _excite) { m_excite = _excite; }
	void setType(uint32_t _type) { m_type = _type; }

	void setWeightX(const std::string& _weightX) { m_weightX = _weightX; }
	void setWeightY(const std::string& _weightY) { m_weightY = _weightY; }
	void setWeightZ(const std::string& _weightZ) { m_weightZ = _weightZ; }

	void setBoxPriority(int32_t _priority) { m_boxPriority = _priority; }
	void setPrimitives1(double _x, double _y, double _z);
	void setPrimitives2(double _x, double _y, double _z);

	//! @brief Sets the excite direction based on boolean flags for each axis
	//! @param exciteX True to excite in the X direction, false otherwise
	//! @param exciteY True to excite in the Y direction, false otherwise
	//! @param exciteZ True to excite in the Z direction, false otherwise
	void setExciteDirection(bool _exciteX, bool _exciteY, bool _exciteZ);

	//! @brief Writes the excitation properties to an XML element
	//! @param _parentElement The parent XML element to which the excitation properties will be added
	//! @return The created Excitation XML element
	tinyxml2::XMLElement* writeExciteProperties(tinyxml2::XMLElement& _parentElement) const;

private:
	std::string m_name = "excite"; //default name
	std::string m_excite = "1,0,0"; // default excite in X
	uint32_t m_type = 0; // 0: Gaussian

	std::string m_weightX = "1"; // defaults to 1 in X
	std::string m_weightY = "0"; // defaults to 0 in Y
	std::string m_weightZ = "0"; // defaults to 0 in Z

	int32_t m_boxPriority = 0; // default priority 0

	// Box primitives
	double m_primitives1_X = 0.0;
	double m_primitives1_Y = 0.0;
	double m_primitives1_Z = 0.0;

	double m_primitives2_X = 0.0;
	double m_primitives2_Y = 0.0;
	double m_primitives2_Z = 0.0;
};