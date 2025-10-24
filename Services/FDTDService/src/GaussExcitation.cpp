//! @file GaussianExcitation.cpp
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################


#include "GaussianExcitation.h"
#include "CSXExcitationProperties.h"

GaussianExcitation::GaussianExcitation() 
{
}

GaussianExcitation::~GaussianExcitation() 
{
}

void GaussianExcitation::applyToCSX(CSXExcitationProperties& _csxProps) {
	_csxProps.setName("excitation");
	_csxProps.setType(0); // 0 for Gaussian
	_csxProps.setExciteDirection(true, true, false); // excite in X direction
	_csxProps.setBoxPriority(0);
	_csxProps.setPrimitives1(0, 0, 0);
	_csxProps.setPrimitives2(1000, 600, 0);
	_csxProps.setWeightX("0*cos(0.0031416*x)*sin(0*y))");
	_csxProps.setWeightY("-1*sin(0.0031416*x)*cos(0*y))");
	_csxProps.setWeightZ("0");
}