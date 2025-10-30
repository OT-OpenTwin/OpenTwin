// @otLicense

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