//! @file ExcitationBase.h
//! @author Alexandros McCray (alexm-dev)
//! @brief Abstract base class for different types of excitations in FDTD simulations
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "CSXExcitationProperties.h"

// STD
#include <cstdint>

class ExcitationBase {
public:
	ExcitationBase() {}
	virtual ~ExcitationBase() {}
	//! @brief Applies the excitation properties to the given CSXExcitationProperties object
	//! @param csxProps The CSXExcitationProperties object to which the excitation properties will be applied
	virtual void applyToCSX(CSXExcitationProperties& _csxProps) = 0;

	virtual uint32_t getType() const = 0;
};