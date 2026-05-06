// @otlicense
// File: IVisualisationTable.h
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
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTGui/Widgets/TableCfg.h"

// std header
#include <string>

namespace ot {

	class __declspec(dllexport) IVisualisationTable
	{
	public:
		virtual ~IVisualisationTable() {}
		virtual ot::GenericDataStructMatrix getTable() = 0;
		virtual void setTable(const ot::GenericDataStructMatrix& _table) = 0;
		virtual ot::TableCfg getTableConfig(bool _includeData) = 0;

		//! @brief Returns the decimal delimiter used for numeric values in the table (e.g., '.' or ',').
		//! This is important for correctly parsing and displaying numeric values, especially in international contexts where the decimal delimiter may vary.
		virtual char getDecimalDelimiter() = 0;

		virtual bool visualiseTable() = 0;
		virtual ot::TableCfg::TableHeaderMode getHeaderMode() = 0;

		virtual void setActiveFilters(const std::list<ValueComparisonDescription>& _filters) = 0;
		virtual std::list<ValueComparisonDescription> getActiveFilters() const = 0;
	};

}