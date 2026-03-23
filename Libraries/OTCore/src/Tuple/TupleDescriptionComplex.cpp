// @otlicense

// OpenTwin header
#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Units/SIUnits.h"

ot::TupleDescriptionComplex::TupleDescriptionComplex()
{
	//Define format names for the complex number tuple.
	m_tupleFormatNames = { ComplexNumbers::getFormatString(ComplexNumberFormat::Cartesian), ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar) };

	// Define element names for each format
	m_tupleElementNamesByFormatName[ComplexNumbers::getFormatString(ComplexNumberFormat::Cartesian)] = { "Real", "Imaginary" };
	m_tupleElementNamesByFormatName[ComplexNumbers::getFormatString(ComplexNumberFormat::Polar)] = { "Magnitude", "Phase" };

	// Define possible unit combinations for each format.
	m_unitCombinationsByFormatName[ComplexNumbers::getFormatString(ComplexNumberFormat::Cartesian)] = { createCombinedUnitsString({"",""}) };
	std::vector<std::string> angleUnits{ "Deg", "rad" };
	std::vector<std::string> magnitudeUnits{ "", "dB" };
	for (const std::string& magnitudeUnit : magnitudeUnits)
	{
		for (const std::string& angleUnit : angleUnits)
		{
			m_unitCombinationsByFormatName[ComplexNumbers::getFormatString(ComplexNumberFormat::Polar)].push_back(createCombinedUnitsString({ magnitudeUnit, angleUnit }));
		}
	}
}

ot::TupleInstance ot::TupleDescriptionComplex::createInstance(ComplexNumberFormat _format,
	const std::vector<std::string>& _units,
	const std::vector<std::string>& _dataTypes)
{
	TupleInstance tupleInstance(getName());
	tupleInstance.setTupleFormatName(ComplexNumbers::getFormatString(_format));

	if (validateDataTypesForFormat(_dataTypes, _format))
	{
		tupleInstance.setTupleElementDataTypes(_dataTypes);
	}
	else
	{
		assert(false);
		throw std::invalid_argument("Invalid data types provided for the complex number tuple instance.");
	}

	if (validateUnitsForFormat(_units, _format))
	{
		tupleInstance.setTupleUnits(_units);
	}
	else
	{
		assert(false);
		throw std::invalid_argument("Invalid units provided for the complex number tuple instance.");
	}
	return tupleInstance;
}

bool ot::TupleDescriptionComplex::validateUnitsForFormat(const std::vector<std::string>& _units, const ComplexNumberFormat& _format) const
{
	const std::string combinedUnitsString = createCombinedUnitsString(_units);
	const auto& options = m_unitCombinationsByFormatName.find(ComplexNumbers::getFormatString(_format))->second;
	bool valid = std::find(options.begin(), options.end(), combinedUnitsString) != options.end();
	return valid;
}

bool ot::TupleDescriptionComplex::validateDataTypesForFormat(const std::vector<std::string>& _dataTypes, const ComplexNumberFormat& _format) const
{
	for (size_t i = 0; i < _dataTypes.size(); ++i)
	{
		if (_dataTypes[i] != TypeNames::getDoubleTypeName())
		{
			return false;
		}
	}
	return true;
}
