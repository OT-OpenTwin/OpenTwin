#include "OptionSettings.h"

ts::OptionSettings::OptionSettings(option::Frequency frequency, option::Format format, option::Parameter parameter, ot::Variable referenceResistance)
	:_frequency(frequency), _format(format), _parameter(parameter), _referenceResistance(referenceResistance)
{
}

ts::OptionSettings::OptionSettings()
	:_frequency(option::Frequency::GHz), _format(option::Format::magnitude_angle), _parameter(option::Parameter::Scattering), _referenceResistance(50)
{}

bool ts::OptionSettings::operator==(const OptionSettings& other) const
{
	return _frequency == other._frequency && _format == other._format && _parameter == other._parameter && _referenceResistance == other._referenceResistance;
}
