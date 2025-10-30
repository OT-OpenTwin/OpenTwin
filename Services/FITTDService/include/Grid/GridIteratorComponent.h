// @otlicense
// File: GridIteratorComponent.h
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
#include "SystemDependencies/SystemDependentDefines.h"

class GridIteratorComponent
{
private:
	index_t _resetDistance;
	index_t _stepWidth;

protected:
	index_t _currentPoint;
	index_t _startPoint;
	index_t _endPoint;

	void SetResetDistance() { _resetDistance = (-_endPoint + _startPoint) * _stepWidth; };
public:
	// Maybe make grid a friend class to check if endpoint does not exceed the dimensions.
	GridIteratorComponent(index_t startPoint, index_t endPoint, index_t stepWidth)
		: _startPoint(startPoint), _endPoint(endPoint), _stepWidth(stepWidth), _currentPoint(startPoint)
	{
		SetResetDistance();
	};
	virtual ~GridIteratorComponent() {};

	bool HasNext() const 
	{
		bool hasNext = _currentPoint < _endPoint;
		return hasNext; 
	};

	/* 
		!ATTENTION! BREAK OF CONCEPT !ATTENTION!
		HasNext refers to _endPoint which is information from the subvolume. HasPrior now looks at the entire computational domain.
		Hack for ResultFilterSourceVektorVolumeEdgeToNode.
	*/
	bool HasPrior()const
	{
		bool hasPrior = _currentPoint > 0;
		return hasPrior;
	}

	void SetToNext(index_t & index) 
	{
		index += _stepWidth; 
		_currentPoint++;
	};
	void SetFromLastToFirst(index_t & index) 
	{ 
		index += _resetDistance; 
		_currentPoint = _startPoint;
	};
	
	index_t GetNextTo(index_t index) { return index + _stepWidth; };
	index_t GetPreviousTo(index_t index) { return index - _stepWidth; };

	index_t GetStartPoint() const { return _startPoint; };
	index_t GetEndPoint()const { return _endPoint; };

	index_t GetStepwidth() const { return _stepWidth; };
};