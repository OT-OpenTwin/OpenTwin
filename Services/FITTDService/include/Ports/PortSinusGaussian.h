#pragma once
#include "Port.h"
template <class T>
class PortSinusGaussian : public Port<T>
{
	using Port<T>::_index;
	using Port<T>::EFunc;

	double _frequencyMin =0;
	double _frequencyMax =0;

	double _frequencyMiddle = 0.;
	double _exponentCoefficient = 0.;
	double _timeShift = 0.;
	const double _fadedAmplitude = 0.00001;

	void CalculateCoefficients();
	T CalculateCurrentValue(float currentTime) override;

public:
	PortSinusGaussian(std::string portName, std::pair<Point3D,index_t> location , sourceType fieldType, axis sourceAxis, std::string signalName, double frequencyMin, double frequencyMax)
		: Port<T>(portName,location, fieldType, sourceAxis, signalName), _frequencyMin(frequencyMin), _frequencyMax(frequencyMax)
	{
		assert(_frequencyMin <= _frequencyMax);
		CalculateCoefficients();
	};
};

#include "Ports/PortSinusGaussian.hpp"