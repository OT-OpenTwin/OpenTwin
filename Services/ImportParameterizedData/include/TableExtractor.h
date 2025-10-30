// @otlicense
// File: TableExtractor.h
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
#include <vector>
#include "EntityFile.h"

//! @brief API for creating a Table entity from any source. 
//! The task of this class is only the extraction of single rows, depending in the set delimiter. 
//! The contained strings are not altered in any way.
class TableExtractor
{
public:
	TableExtractor() {};
	virtual ~TableExtractor() {};
	
	virtual void ExtractFromEntitySource(EntityFile* source) = 0;

	virtual void GetNextLine(std::vector<std::string> & line)=0;
	virtual void GetNextLine(std::string & entireLine)=0;
	virtual bool HasNextLine()=0;
	virtual void ResetIterator()=0;
};