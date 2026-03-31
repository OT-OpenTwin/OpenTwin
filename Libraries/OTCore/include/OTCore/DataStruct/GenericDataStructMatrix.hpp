// @otlicense
// File: GenericDataStructMatrix.hpp
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

// OpenTwin header
#include "OTCore/DataStruct/GenericDataStructMatrix.h"

inline ot::MatrixEntryIterator<false> ot::GenericDataStructMatrix::createIterator(bool _horizontalAdvance)
{
	return MatrixEntryIterator<false>(this,
		MatrixEntryPointer(0, 0),
		MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
		_horizontalAdvance
	);
}

inline ot::MatrixEntryIterator<true> ot::GenericDataStructMatrix::createIterator(bool _horizontalAdvance) const
{
	return MatrixEntryIterator<true>(this,
		MatrixEntryPointer(0, 0),
		MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
		_horizontalAdvance
	);
}

inline ot::MatrixEntryIterator<false> ot::GenericDataStructMatrix::createDataIterator(bool _horizontalHeader)
{
	return MatrixEntryIterator<false>(this,
		_horizontalHeader ? MatrixEntryPointer(1, 0) : MatrixEntryPointer(0, 1),
		MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
		_horizontalHeader
	);
}

inline ot::MatrixEntryIterator<true> ot::GenericDataStructMatrix::createDataIterator(bool _horizontalHeader) const
{
	return MatrixEntryIterator<true>(this,
		_horizontalHeader ? MatrixEntryPointer(1, 0) : MatrixEntryPointer(0, 1),
		MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
		_horizontalHeader
	);
}

inline ot::MatrixEntryIterator<false> ot::GenericDataStructMatrix::createDataColumnIterator(uint32_t _columnIndex, bool _horizontalHeader)
{
	return MatrixEntryIterator<false>(this,
		_horizontalHeader ? MatrixEntryPointer(1, _columnIndex) : MatrixEntryPointer(_columnIndex, 1),
		MatrixEntryPointer(m_numberOfRows, _columnIndex + 1),
		_horizontalHeader
	);
}

inline ot::MatrixEntryIterator<true> ot::GenericDataStructMatrix::createDataColumnIterator(uint32_t _columnIndex, bool _horizontalHeader) const
{
	return MatrixEntryIterator<true>(this,
		_horizontalHeader ? MatrixEntryPointer(1, _columnIndex) : MatrixEntryPointer(_columnIndex, 1),
		MatrixEntryPointer(m_numberOfRows, _columnIndex + 1),
		_horizontalHeader
	);
}

inline ot::MatrixEntryIterator<false> ot::GenericDataStructMatrix::createHeaderIterator(bool _horizontalHeader)
{
	return MatrixEntryIterator<false>(this,
		MatrixEntryPointer(0, 0),
		_horizontalHeader ? MatrixEntryPointer(1, m_numberOfColumns) : MatrixEntryPointer(m_numberOfRows, 1),
		_horizontalHeader
	);
}

inline ot::MatrixEntryIterator<true> ot::GenericDataStructMatrix::createHeaderIterator(bool _horizontalHeader) const
{
	return MatrixEntryIterator<true>(this,
		MatrixEntryPointer(0, 0),
		_horizontalHeader ? MatrixEntryPointer(1, m_numberOfColumns) : MatrixEntryPointer(m_numberOfRows, 1),
		_horizontalHeader
	);
}
