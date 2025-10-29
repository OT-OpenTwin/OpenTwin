//! @file GaussianExcitation.h
//! @brief Implementation of the Gaussian Excitation class
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "ExcitationBase.h"

// STD
#include <cstdint>

class GaussianExcitation : public ExcitationBase {
public:
	GaussianExcitation();
	virtual ~GaussianExcitation();

	virtual void applyProperties() override;
};