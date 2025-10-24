//! @file ExcitationTypes.h
//! @brief Enumeration for different types of excitations in FDTD simulations
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include <cstdint>

enum class ExcitationTypes : uint32_t {
	GAUSSIAN = 0,
	SINUSOIDAL = 1,
};