//! @file SinusoidalExcitation.h
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "ExcitationBase.h"

// STD
#include <cstdint>

// Forward declaration
class CSXExcitationProperties;

class SinusoidalExcitation : public ExcitationBase {
public:
	SinusoidalExcitation();
	virtual ~SinusoidalExcitation();

	virtual void applyToCSX(CSXExcitationProperties& _csxProps) override;

	virtual uint32_t getType() const override { return 1; } // 1 for Sinusoidal
};