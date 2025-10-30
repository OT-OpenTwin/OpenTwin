// @otlicense
// File: IGridIteratorVolume.cpp
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

#include "Grid/IGridIteratorVolume.h"

const index_t & IGridIteratorVolume::GetNextIndex()
{
	if (_iteratorXAxis->HasNext())
	{
		_iteratorXAxis->SetToNext(_currentIndex);
	}
	else
	{
		_iteratorXAxis->SetFromLastToFirst(_currentIndex);
		if (_iteratorYAxis->HasNext())
		{
			_iteratorYAxis->SetToNext(_currentIndex);
		}
		else
		{
			assert(_iteratorZAxis->HasNext());
			_iteratorYAxis->SetFromLastToFirst(_currentIndex);
			_iteratorZAxis->SetToNext(_currentIndex);
		}
	}
	return _currentIndex;
}

GridIteratorComponent * IGridIteratorVolume::GetXAxisIterator() const
{
	return _iteratorXAxis;
}

GridIteratorComponent * IGridIteratorVolume::GetYAxisIterator() const
{
	return _iteratorYAxis;
}

GridIteratorComponent * IGridIteratorVolume::GetZAxisIterator() const
{
	return _iteratorZAxis;
}
