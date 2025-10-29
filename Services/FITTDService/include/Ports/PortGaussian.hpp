// @otlicense

#pragma once
#include "Ports/PortGaussian.h"

template<class T>
inline T PortGaussian<T>::CalculateCurrentValue(float currentTime)
{
	return EFunc(-static_cast<T>(currentTime - _middlePoint)*static_cast<T>(currentTime - _middlePoint) / static_cast<T>(_deviation));;
}