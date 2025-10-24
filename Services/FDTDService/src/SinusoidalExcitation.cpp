//! @file SinusoidalExcitation.cpp
//! @author Alexandros McCray (alexm-dev)
//! @date 24.10.2025
// ###########################################################################################################################################################################################################################################################################################################################


#include "SinusoidalExcitation.h"
#include "ExcitationProperties.h"

SinusoidalExcitation::SinusoidalExcitation()
{
}

SinusoidalExcitation::~SinusoidalExcitation()
{
}

void SinusoidalExcitation::applyProperties() {
	m_exciteProperties.setName("excite");
	m_exciteProperties.setType(1); // 1 for Sinusoidal
	m_exciteProperties.setExciteDirection(true, false, false); // excite in X direction
	m_exciteProperties.setBoxPriority(0);
	m_exciteProperties.setPrimitives1(0, 0, 0);
	m_exciteProperties.setPrimitives2(500, 500, 0);
	m_exciteProperties.setWeightX("1");
	m_exciteProperties.setWeightY("0");
	m_exciteProperties.setWeightZ("0");
}