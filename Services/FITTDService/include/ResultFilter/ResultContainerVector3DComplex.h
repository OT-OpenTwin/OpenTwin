// @otlicense
// File: ResultContainerVector3DComplex.h
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
#include "ResultFilter/ResultContainer.h"
#include "SystemDependencies/SystemDependentDefines.h"

class ResultContainerVector3DComplex : public ResultContainer
{
public:
	ResultContainerVector3DComplex(int currentTimeStep , index_t size, double * componentX, double * componentY, double * componentZ);
	~ResultContainerVector3DComplex();

	double * GetRealComponentX() const { return _componentX; };
	double * GetRealComponentY() const { return _componentY; };
	double * GetRealComponentZ() const { return _componentZ; };
	double * GetImagComponentX() const { return &_componentX[_size]; };
	double * GetImagComponentY() const { return &_componentY[_size]; };
	double * GetImagComponentZ() const { return &_componentZ[_size]; };

	void ApplyFilter(ResultFilter * filter) override;

private:
	double * _componentX;
	double * _componentY;
	double * _componentZ;

	index_t _size;
};