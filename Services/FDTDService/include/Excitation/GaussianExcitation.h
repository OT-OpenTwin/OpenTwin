// @otLicense

//! @file GaussianExcitation.h
//! @author Alexandros McCray (alexm-dev)
//! @date October 2025
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