// @otLicense
// @otLicense-end

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