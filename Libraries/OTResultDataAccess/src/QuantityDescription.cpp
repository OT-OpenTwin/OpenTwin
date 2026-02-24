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
#include "OTCore/Tuple/TupleDescriptionComplex.h"
#include "OTCore/Tuple/TupleDescriptionSingle.h"
void QuantityDescription::defineQuantityAsSingle(const std::string& _dataType, const std::string& _unit)
{

	const TupleInstance tuple =	TupleDescriptionSingle::createInstance(_unit, _dataType);
	m_metadataQuantity.m_tupleDescription = tuple;
}

void QuantityDescription::defineQuantityAsComplex(ot::ComplexNumberFormat _format, const std::string& _dataType, const std::string& _firstUnit, const std::string& _secondUnit)
{

	TupleDescriptionComplex tupleDescription;
	const TupleInstance instance = tupleDescription.createInstance(_format, { _firstUnit,_secondUnit }, { _dataType,_dataType });
	m_metadataQuantity.m_tupleDescription = instance;
}

