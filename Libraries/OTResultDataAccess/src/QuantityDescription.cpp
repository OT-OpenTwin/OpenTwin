// @otlicense
// File: QuantityDescription.cpp
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
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescription.h"
#include "OTResultDataAccess/SerialisationInterfaces/TupleDescriptionComplex.h"
void QuantityDescription::defineQuantityAsSingle(const std::string& _dataType, const std::string& _unit)
{
	TupleDescription tupleDescription;
	tupleDescription.setDataType(_dataType);
	tupleDescription.setFormatName("");
	tupleDescription.setUnits({ _unit });
	m_metadataQuantity.m_tupleDescription = std::make_unique<TupleDescription>(tupleDescription);
}

void QuantityDescription::defineQuantityAsComplex(ot::ComplexNumberFormat _format, const std::string& _dataType, const std::string& _firstUnit, const std::string& _secondUnit)
{
	TupleDescriptionComplex tupleDescription(_format);

	if (_format == ot::ComplexNumberFormat::Cartesian)
	{
		assert(_secondUnit == _firstUnit);
		tupleDescription.setUnits({ _firstUnit,_secondUnit});
	}
	else
	{
		assert(_secondUnit == u8"°" || _secondUnit == "rad");
		tupleDescription.setUnits({ _firstUnit,_secondUnit });
	}
	m_metadataQuantity.m_tupleDescription = std::make_unique<TupleDescriptionComplex>(tupleDescription);
}

