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
#include <string>
#include "OTGui/TableCfg.h"
#include "ContentChangedHandling.h"
#include "OTCore/GenericDataStructMatrix.h"

class __declspec(dllexport) IVisualisationTable
{
public:
	virtual ~IVisualisationTable() {}
	virtual const ot::GenericDataStructMatrix getTable() = 0;
	virtual void setTable(const ot::GenericDataStructMatrix& _table) = 0;
	virtual ot::TableCfg getTableConfig(bool _includeData) = 0;
	virtual char getDecimalDelimiter() = 0; //Needed for text to numeric conversions
	virtual bool visualiseTable() = 0;
	virtual ot::ContentChangedHandling getTableContentChangedHandling() = 0;
	virtual ot::TableCfg::TableHeaderMode getHeaderMode(void) = 0;
};