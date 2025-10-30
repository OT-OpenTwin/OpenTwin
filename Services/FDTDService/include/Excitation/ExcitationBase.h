// @otLicense

//! @file ExcitationBase.h
//! @author Alexandros McCray (alexm-dev)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "ExcitationProperties.h"

// STD
#include <cstdint>

class ExcitationBase {
public:
	ExcitationBase() {}
	virtual ~ExcitationBase() {}
	//! @brief Applies the excitation properties to the given ExcitationProperties object
	virtual void applyProperties() = 0;
	//! @brief Retrieves the excitation properties
	const ExcitationProperties& getExciteProperties() const { return m_exciteProperties; }

protected:
	// Excitation properties
	ExcitationProperties m_exciteProperties;
};