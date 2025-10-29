// @otlicense

#pragma once
#include "Options.h"

namespace ts
{
	class OptionSettings
	{
	public:
		OptionSettings(option::Frequency frequency, option::Format format, option::Parameter parameter, ot::Variable referenceResistance);
		OptionSettings();

		bool operator==(const OptionSettings& other)const;
		
		const option::Frequency getFrequency() const { return _frequency; }
		void setFrequency(const option::Frequency& frequency) { _frequency = frequency; }
		
		const option::Format getFormat() const { return _format; }
		void setFormat(const option::Format& format) { _format = format; }

		const option::Parameter getParameter() const { return _parameter; }
		void setParameter(const option::Parameter& parameter) { _parameter = parameter; }

		/// <summary>
		/// Unit: Ohm
		/// </summary>
		/// <returns></returns>
		const ot::Variable getReferenceResistance() const { return _referenceResistance; }
		/// <summary>
		/// Unit: Ohm
		/// </summary>
		void setReferenceResistance(const ot::Variable& value) { _referenceResistance = value; }
	private:
		option::Frequency _frequency;
		option::Format _format;
		option::Parameter _parameter;
		ot::Variable _referenceResistance;
	};
}
