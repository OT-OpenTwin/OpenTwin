// @otlicense
#pragma once
#include "OTCore/CoreAPIExport.h"
#include <string>
#include <stdexcept>

namespace ot
{
	enum class OT_CORE_API_EXPORT ComplexNumberFormat
	{
		Cartesian,
		Polar
	};

	namespace ComplexNumbers
	{
		static std::string getTypeName()
		{
			return "Complex";
		}	


		static std::string getFormatString(const ComplexNumberFormat& _format)
		{
			switch (_format)
			{
			case ComplexNumberFormat::Cartesian:
				return "Cartesian";
			case ComplexNumberFormat::Polar:
				return "Polar";
			default:
				throw std::invalid_argument("No string format known for the provided complex number format.");
			}
		}

		static ComplexNumberFormat getFormatFromString(const std::string& _format)
		{
			if (_format == "Cartesian")
			{
				return ComplexNumberFormat::Cartesian;
			}
			else if (_format == "Polar")
			{
				return ComplexNumberFormat::Polar;
			}
			else
			{
				throw std::invalid_argument("Unknown complex number format: " + _format);
			}
		}
	}
}