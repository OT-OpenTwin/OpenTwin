
#include "OTCore/ComplexNumbers/ComplexNumberConversion.h"
#include "OTCore/Units/SI.h"
#include "OTCore/String.h"


std::complex<double> ot::ComplexNumberConversion::polarToCartesian(const ComplexNumberDefinition& _complexNumberDefinition)
{
	constexpr double pi = 3.14159265358979323846;
	double angle = 0;
	if(_complexNumberDefinition.m_angleUnit == ot::SIUnits::Derived::getDegreeUnit())
	{
		angle = _complexNumberDefinition.m_secondValue * pi / 180.0; // Convert degrees to radians
	}
	else if(_complexNumberDefinition.m_angleUnit != SIUnits::Derived::getRadianUnit() && !_complexNumberDefinition.m_angleUnit.empty())
	{
		throw std::invalid_argument("Invalid angle unit. Expected 'rad' or '°'.");
	}

	double realPart = _complexNumberDefinition.m_firstValue * cos(_complexNumberDefinition.m_secondValue);
	double imagPart = _complexNumberDefinition.m_firstValue * sin(_complexNumberDefinition.m_secondValue);
	return std::complex<double>(realPart, imagPart);
}

ot::ComplexNumberDefinition ot::ComplexNumberConversion::cartesianToPolar(std::complex<double> _complexNb)
{
	ComplexNumberDefinition complexNumber;
	complexNumber.m_firstValue = std::abs(_complexNb);
	complexNumber.m_secondValue = std::arg(_complexNb);
	complexNumber.m_format = ComplexNumberFormat::Polar;
	return complexNumber;
}

std::complex<double> ot::ComplexNumberConversion::fromString(const std::string& _complexString, const ot::ComplexNumberFormat& _format, const std::string& _angleUnit)
{
	std::list<std::string> components = ot::String::split(_complexString, ',');
	std::complex<double> value;
	if (components.size() != 2)
	{
		throw std::invalid_argument("Complex number value expected in the format '<first value>,<second value>'.");
	}
	try
	{
		ComplexNumberDefinition complexNumberDefinition;
		complexNumberDefinition.m_format = _format;
		complexNumberDefinition.m_firstValue = std::stod(components.front());
		complexNumberDefinition.m_secondValue = std::stod(components.back());
		complexNumberDefinition.m_angleUnit = _angleUnit;
		
		if(_format == ot::ComplexNumberFormat::Polar)
		{
			value = polarToCartesian(complexNumberDefinition);
			
		}
		else
		{
			value = std::complex<double>(complexNumberDefinition.m_firstValue, complexNumberDefinition.m_secondValue);
		}
		return value;
	}
	catch (const std::exception& e)
	{
		throw std::invalid_argument("Complex number value expected in the format '<first value>,<second value>'. " + std::string(e.what()));
	}
}
