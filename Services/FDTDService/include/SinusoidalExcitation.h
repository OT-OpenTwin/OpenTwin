//! @file SinusoidalExcitation.h
//! @brief Implementation of the Sinus Excitation class
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "ExcitationBase.h"

// STD
#include <cstdint>

class SinusoidalExcitation : public ExcitationBase {
public:
	SinusoidalExcitation();
	virtual ~SinusoidalExcitation();

	virtual void applyProperties() override;
};