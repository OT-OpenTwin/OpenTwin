// @otlicense
// File: QuantityContainer.h
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
#include <stdint.h>
#include <list>
#include <string>
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/CoreTypes.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>


class __declspec(dllexport) QuantityContainer
{
public:
	QuantityContainer(int64_t _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>&& _parameterValues, int64_t _quantityIndex);
	QuantityContainer(int64_t _seriesIndex, std::list<ot::UID>& _parameterIDs, std::list<ot::Variable>& _parameterValues, int64_t _quantityIndex);
	QuantityContainer(QuantityContainer&& _other) noexcept;
	QuantityContainer(const QuantityContainer& _other) = delete;
	QuantityContainer& operator=(const QuantityContainer& _other) = delete;
	QuantityContainer& operator=(QuantityContainer&& _other);
	~QuantityContainer();

	void addValue(const ot::Variable& _value);
	int64_t getValueArraySize() const { return m_values.size(); };
	bsoncxx::builder::basic::document& getMongoDocument();
	static const std::string getFieldName() { return "Values"; }

private:
	bsoncxx::builder::basic::document m_mongoDocument;
	std::list<ot::Variable> m_values;
};
