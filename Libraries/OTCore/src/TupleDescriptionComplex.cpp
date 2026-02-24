#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Units/SI.h"


TupleDescriptionComplex::TupleDescriptionComplex()
{
	//Define format names for the complex number tuple.
	m_tupleFormatNames = { ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian), ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar) };
	
	// Define element names for each format
	m_tupleElementNamesByFormatName[ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian)] = { "Real", "Imaginary" };
	m_tupleElementNamesByFormatName[ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar)] = { "Magnitude", "Phase" };

	// Define possible unit combinations for each format.
	m_unitCombinationsByFormatName[ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Cartesian)] = { createCombinedUnitsString({"",""}) };
	std::vector<std::string> angleUnits{ ot::SIUnits::Derived::getDegreeUnit(), ot::SIUnits::Derived::getRadianUnit() };
	std::vector<std::string> magnitudeUnits{ "", "dB"};
	for (const std::string& magnitudeUnit : magnitudeUnits)
	{
		for (const std::string& angleUnit : angleUnits)
		{
			m_unitCombinationsByFormatName[ot::ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar)].push_back(createCombinedUnitsString({ magnitudeUnit, angleUnit }));
		}
	}
}

TupleInstance TupleDescriptionComplex::createInstance(ot::ComplexNumberFormat _format, 
	const std::vector<std::string>& _units, 
	const std::vector<std::string>& _dataTypes)
{
	TupleInstance tupleInstance (getName());
	tupleInstance.setTupleFormatName(ot::ComplexNumbers::getFormatString(_format));

	if (validateDataTypesForFormat(_dataTypes, _format))
	{
		tupleInstance.setTupleElementDataTypes(_dataTypes);
	}
	else
	{
		assert(false);
		throw std::invalid_argument("Invalid data types provided for the complex number tuple instance.");
	}

	if(validateUnitsForFormat(_units, _format))
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

bool TupleDescriptionComplex::validateUnitsForFormat(const std::vector<std::string>& _units, const ot::ComplexNumberFormat& _format) const
{
	const std::string combinedUnitsString = createCombinedUnitsString(_units);
	return m_unitCombinationsByFormatName.find(combinedUnitsString) != m_unitCombinationsByFormatName.end();
}

bool TupleDescriptionComplex::validateDataTypesForFormat(const std::vector<std::string>& _dataTypes, const ot::ComplexNumberFormat& _format) const
{
	for( size_t i = 0; i < _dataTypes.size(); ++i)
	{
		if(_dataTypes[i] != ot::TypeNames::getDoubleTypeName())
		{
			return false;
		}
	}
	return true;
}
