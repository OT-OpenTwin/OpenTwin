// @otlicense
// File: MetadataEntryArray.h
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
#include "MetadataEntry.h"
#include "OTCore/Variable.h"

#include <list>

class MetadataEntryArray : public MetadataEntry
{
public:
	MetadataEntryArray(const std::string name, std::list<ot::Variable>&& values) : MetadataEntry(name), _values(values){}
	MetadataEntryArray(const std::string name, const std::list<ot::Variable>& values) : MetadataEntry(name), _values(values){}
	const std::list<ot::Variable>& getValues() const { return _values; }

	bool operator==(const MetadataEntryArray& other);
private:
	std::list<ot::Variable> _values;
};
