//otlicense
#pragma once
#include "OTCore/CoreAPIExport.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#pragma warning(disable:4251)

namespace ot
{
	struct OT_CORE_API_EXPORT ComplexNumberDefinition
	{
		double m_firstValue; //Real part if Cartesian, magnitude if Polar.
		double m_secondValue; //Imaginary part if Cartesian, angle if Polar. 
		ot::ComplexNumberFormat m_format;
		std::string m_angleUnit; // Only relevant if the format is Polar. Should be taken out of the SIUnits::Derived namespace.
	};
}


