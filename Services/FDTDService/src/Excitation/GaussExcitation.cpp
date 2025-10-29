// @otLicense
// @otLicense-end

#include "Excitation/GaussianExcitation.h"
#include "Excitation/ExcitationProperties.h"

GaussianExcitation::GaussianExcitation() 
{
}

GaussianExcitation::~GaussianExcitation() 
{
}

void GaussianExcitation::applyProperties() {
	m_exciteProperties.setName("excite");
	m_exciteProperties.setType(0); // 0 for Gaussian
	m_exciteProperties.setExciteDirection(true, true, false); // excite in X, Y direction
	m_exciteProperties.setBoxPriority(0);
	m_exciteProperties.setPrimitives1(0, 0, 0);
	m_exciteProperties.setPrimitives2(1000, 600, 0);
	m_exciteProperties.setWeightX("0*cos(0.0031416*x)*sin(0*y))");
	m_exciteProperties.setWeightY("-1*sin(0.0031416*x)*cos(0*y))");
	m_exciteProperties.setWeightZ("0");
}