//! @file SinusoidalExcitation.cpp
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################


#include "SinusoidalExcitation.h"
#include "CSXExcitationProperties.h"

SinusoidalExcitation::SinusoidalExcitation()
{
}

SinusoidalExcitation::~SinusoidalExcitation()
{
}

void SinusoidalExcitation::applyToCSX(CSXExcitationProperties& _csxProps) {
	_csxProps.setName("excite");
	_csxProps.setType(1); // 1 for Sinusoidal
	_csxProps.setExciteDirection(true, false, false); // excite in X direction
	_csxProps.setBoxPriority(0);
	_csxProps.setPrimitives1(0, 0, 0);
	_csxProps.setPrimitives2(500, 500, 0);
	_csxProps.setWeightX("1");
	_csxProps.setWeightY("0");
	_csxProps.setWeightZ("0");
}