// @otlicense
// File: TupleDescriptionComplex.h
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

#pragma once

// OpenTwin header
#include "OTCore/TypeNames.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#include "OTCore/Tuple/TupleDescription.h"

namespace ot {

	class OT_CORE_API_EXPORT TupleDescriptionComplex : public TupleDescription
	{
		friend class TupleFactory;

	public:
		TupleDescriptionComplex();

		TupleInstance createInstance(ot::ComplexNumberFormat _format, const std::vector<std::string>& _units, const std::vector<std::string>& _dataTypes = { ot::TypeNames::getDoubleTypeName(), ot::TypeNames::getDoubleTypeName() });

		static std::string name() { return m_name; }
		std::string getName() const override { return m_name; }
	private:
		inline static const std::string m_name = ot::ComplexNumbers::getTypeName();
		bool validateUnitsForFormat(const std::vector<std::string>& _units, const ot::ComplexNumberFormat& _format) const;
		bool validateDataTypesForFormat(const std::vector<std::string>& _dataTypes, const ot::ComplexNumberFormat& _format) const;
	};

}