// @otlicense
// File: EntityResultBase.h
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
#pragma warning(disable : 4251)

#include "OldTreeIcon.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityResultBase
{
public:
	enum tResultType
	{
		UNKNOWN = 0,
		CARTESIAN_EDGES_3D = 1,
		CARTESIAN_FACES_3D = 2,
		CARTESIAN_DUAL_EDGES_3D = 3,
		CARTESIAN_DUAL_FACES_3D = 4,
		CARTESIAN_NODE_VECTORS = 5,
		CARTESIAN_DUAL_NODE_VECTORS = 6,
		UNSTRUCTURED_SCALAR = 7,
		UNSTRUCTURED_VECTOR = 8
	};

	EntityResultBase() : resultType(UNKNOWN) {};
	virtual ~EntityResultBase() {};

	void setResultType(tResultType type) { resultType = type; }
	tResultType getResultType(void) { return resultType; }

private:
	tResultType resultType;
};


