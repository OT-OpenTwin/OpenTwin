// @otLicense

//! @file SinusoidalExcitation.h
//! @author Alexandros McCray (alexm-dev)
//! @date October 2025
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