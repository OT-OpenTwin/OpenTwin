// @otlicense
// File: CSVToTableTransformer.h
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
#include "CSVProperties.h"
#include <vector>
#include "OTCore/GenericDataStructMatrix.h"
class CSVToTableTransformer
{
public:
	ot::GenericDataStructMatrix operator() (const std::string& _csvText, const CSVProperties& _properties);
	std::string operator() (const ot::GenericDataStructMatrix& _matrix, const CSVProperties& _properties);

private:	
	ot::GenericDataStructMatrix transformRawMatrixToGenericDatastruct(const std::list<std::vector<std::string>>& _rawMatrix);
};
