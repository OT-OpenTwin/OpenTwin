#pragma once
#include "Ports/PortSinusGaussian.h"
#define _USE_MATH_DEFINES
#include <math.h>


template<class T>
inline void PortSinusGaussian<T>::CalculateCoefficients()
{
	_frequencyMiddle = (_frequencyMax + _frequencyMin) / 2;
	_exponentCoefficient = -pow(M_PI, 2) / (4 * log(_fadedAmplitude)) * pow(_frequencyMax - _frequencyMin, 2);
	_timeShift = sqrt(-(log(_fadedAmplitude)/_exponentCoefficient));
}

template<class T>
inline T PortSinusGaussian<T>::CalculateCurrentValue(float currentTime)
{
	double t1 = exp(-_exponentCoefficient * pow(currentTime - _timeShift, 2));
	double t2 = cos(2 * M_PI * _frequencyMiddle * (currentTime - _timeShift));
	T currentValue =static_cast<T>(t1 * t2);
	return currentValue; 
}