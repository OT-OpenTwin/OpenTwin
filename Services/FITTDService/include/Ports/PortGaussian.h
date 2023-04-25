#pragma once
#include "Ports/Port.h"
template <class T>
class PortGaussian : public Port<T>
{
	using Port<T>::_index;
	using Port<T>::EFunc;

	int _middlePoint;
	int _deviation;
	T CalculateCurrentValue(float currentTime) override;
	
public:
	PortGaussian(std::string portName, std::pair<Point3D, index_t> &location, sourceType fieldType, axis sourceAxis, std::string signalName, int middlePoint, int deviation)
		: Port<T>(portName, location, fieldType, sourceAxis, signalName), _middlePoint(middlePoint) , _deviation(deviation)
	{
		//std::string specializedSettings = "Gaussian exitation with middlepoint: " + std::to_string(_middlePoint) +
		//" and deviation: " + std::to_string(_deviation);
		//CreateSettings(specializedSettings);
	};
};

#include "Ports/PortGaussian.hpp"