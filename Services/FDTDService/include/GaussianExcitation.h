//! @file GaussianExcitation.h
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "ExcitationBase.h"

// STD
#include <cstdint>

// Forward declaration
class CSXExcitationProperties;

class GaussianExcitation : public ExcitationBase {
public:
	GaussianExcitation();
	virtual ~GaussianExcitation();

	virtual void applyToCSX(CSXExcitationProperties& _csxProps) override;

	virtual uint32_t getType() const override { return 0; } // 0 for Gaussian
};