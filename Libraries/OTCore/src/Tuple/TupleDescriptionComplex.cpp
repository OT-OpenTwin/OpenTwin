// @otlicense
// File: TupleDescriptionComplex.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Units/SIUnits.h"

ot::TupleDescriptionComplex::TupleDescriptionComplex()
{
	//Define format names for the complex number tuple.
	m_tupleFormatNames = { ComplexNumbers::getFormatString(ComplexNumberFormat::Cartesian), ComplexNumbers::getFormatString(ot::ComplexNumberFormat::Polar) };

	// Define element names for each format
	m_tupleElementNamesByFormatName[ComplexNumbers::getFormatString(ComplexNumberFormat::Cartesian)] = { ot::ComplexNumbers::getComponentReal(), ot::ComplexNumbers::getComponentImaginary() };
	m_tupleElementNamesByFormatName[ComplexNumbers::getFormatString(ComplexNumberFormat::Polar)] = { ot::ComplexNumbers::getComponentMagnitude(), ot::ComplexNumbers::getComponentPhase()};

	// Define possible unit combinations for each format.
	m_unitCombinationsByFormatName[ComplexNumbers::getFormatString(ComplexNumberFormat::Cartesian)] = { createCombinedUnitsString({"",""}) };
	std::vector<std::string> angleUnits{ ot::SIUnits::getAngleUnitDegrees(), ot::SIUnits::getAngleUnitRad() };
	std::vector<std::string> magnitudeUnits{ "", ot::SIUnits::getUnitDecibel() };
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
