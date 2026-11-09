// @otlicense
// File: TupleDescription.h
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
#include "OTCore/Serializable.h"
#include "OTCore/CoreAPIExport.h"
#include "OTCore/Tuple/TupleInstance.h"

// std header
#include <map>
#include <list>
#include <string>
#include <vector>

namespace ot {

	class OT_CORE_API_EXPORT TupleDescription
	{
	public:
		TupleDescription() = default;
		TupleDescription& operator=(const TupleDescription& _other) = default;
		TupleDescription& operator=(TupleDescription&& _other) noexcept = default;
		TupleDescription(const TupleDescription& _other) = default;
		TupleDescription(TupleDescription&& _other) noexcept = default;
		virtual ~TupleDescription() {};

		virtual std::string getName() const { return ""; };

		std::vector<std::string> getTupleElementNames(const std::string& _formatName) const;
		std::vector<std::string> getAllTupleElementNames() const;
		std::vector<std::string> getUnitCombinations(const std::string& _formatName) const;
		const std::vector<std::string>& getAllTupleFormatNames() const { return m_tupleFormatNames; }

		static std::string createCombinedUnitsString(const std::vector<std::string>& _units) ;
		static std::vector<std::string> separateCombinedUnitString(const std::string& _combinedUnitString);
	private:

	protected:
		std::vector<std::string> m_tupleFormatNames; // All possible format names.
		std::map<std::string, std::vector<std::string>> m_tupleElementNamesByFormatName; // element names of the current format.
		std::map<std::string, std::vector<std::string>>  m_unitCombinationsByFormatName;

		virtual void initialiseTupleSpecifics() {};
	};

}
