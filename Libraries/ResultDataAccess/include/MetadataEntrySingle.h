// @otlicense
// File: MetadataEntrySingle.h
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

class MetadataEntrySingle : public MetadataEntry
{
public:
	MetadataEntrySingle(const std::string& name, ot::Variable&& value) : MetadataEntry(name), _value(value){}
	MetadataEntrySingle(const std::string& name, const ot::Variable& value) : MetadataEntry(name), _value(value){}
	const ot::Variable& getValue() const { return _value; }
	bool operator==(const MetadataEntrySingle& other);

private:
	const ot::Variable _value;
};
