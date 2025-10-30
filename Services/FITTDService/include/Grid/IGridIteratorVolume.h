// @otlicense
// File: IGridIteratorVolume.h
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
#include "Grid/GridIteratorComponent.h"

#include <cassert>
class IGridIteratorVolume
{
protected:
	index_t _volumeSize;
	index_t _currentIndex = 0;
	index_t _startIndex = 0;

	GridIteratorComponent * _iteratorXAxis = nullptr;
	GridIteratorComponent * _iteratorYAxis = nullptr;
	GridIteratorComponent * _iteratorZAxis = nullptr;

public:
	virtual ~IGridIteratorVolume()
	{
		delete _iteratorXAxis;
		delete _iteratorYAxis;
		delete _iteratorZAxis;
	}

	GridIteratorComponent * GetXAxisIterator() const;
	GridIteratorComponent * GetYAxisIterator() const;
	GridIteratorComponent * GetZAxisIterator() const;

	void Reset()
	{
		index_t temp;
		_iteratorXAxis->SetFromLastToFirst(temp);
		_iteratorYAxis->SetFromLastToFirst(temp);
		_iteratorZAxis->SetFromLastToFirst(temp);
		_currentIndex = _startIndex;
	}
	const index_t GetVolumenSize() const { return _volumeSize; };
	index_t GetCurrentIndex() const { return _currentIndex; };

	bool HasNext() const 
	{ 
		return	_iteratorZAxis->HasNext() ||
				_iteratorYAxis->HasNext() ||
				_iteratorXAxis->HasNext();
	};

	const index_t & GetNextIndex();
};
