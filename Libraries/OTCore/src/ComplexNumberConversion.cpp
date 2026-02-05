
#include "OTCore/ComplexNumbers/ComplexNumberConversion.h"

std::complex<double> ot::ComplexNumberConversion::polarToCartesian(double _magnitude, double _angleInRad)
{
	double realPart = _magnitude * cos(_angleInRad);
	double imagPart = _magnitude * sin(_angleInRad);
	return std::complex<double>(realPart, imagPart);
}

std::vector<double> ot::ComplexNumberConversion::cartesianToPolar(std::complex<double> _complexNb)
{
		double magnitude = std::abs(_complexNb);
		double angleInRad = std::arg(_complexNb);
		return { magnitude, angleInRad };
}
