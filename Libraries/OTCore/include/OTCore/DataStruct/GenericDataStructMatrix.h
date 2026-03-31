// @otlicense
// File: GenericDataStructMatrix.h
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
#include "OTCore/DataStruct/GenericDataStruct.h"
#include "OTCore/DataStruct/MatrixEntryPointer.h"
#include "OTCore/DataStruct/MatrixEntryIterator.h"
#include "OTCore/Variable/Variable.h"

// std header
#include <vector>
#include <stdint.h>
#include <functional>

namespace ot {

	//! @brief The GenericDataStructMatrix class represents a matrix data structure that can hold a variable number of entries.
	//! It provides methods to set and retrieve values based on their column and row indices.
	//! The class assumes that the list of values is a linearized matrix of the topology it was initialized with.
	class OT_CORE_API_EXPORT GenericDataStructMatrix : public GenericDataStruct
	{
		OT_DECL_DEFCOPY(GenericDataStructMatrix)
		OT_DECL_DEFMOVE(GenericDataStructMatrix)
	public:
		GenericDataStructMatrix() = default;
		virtual ~GenericDataStructMatrix();

		GenericDataStructMatrix(uint32_t _rows, uint32_t _columns);
		GenericDataStructMatrix(const MatrixEntryPointer& _dimensions);
		GenericDataStructMatrix(uint32_t _rows, uint32_t _columns, const ot::Variable& _defaultValue);
		GenericDataStructMatrix(const MatrixEntryPointer& _dimensions, const ot::Variable& _defaultValue);

		virtual size_t getNumberOfEntries() const override { return m_values.size(); };
		static std::string className() { return "GenericDataStructMatrix"; };
		virtual std::string getClassName() const override { return GenericDataStructMatrix::className(); };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief 
		//! @param _horizontalHeader 
		//! @return 
		GenericDataStructMatrix tableMerge(const GenericDataStructMatrix& _other, bool _horizontalHeader = true) const;
		GenericDataStructMatrix tableMerge(const std::list<GenericDataStructMatrix>& _others, bool _horizontalHeader = true) const;

		void setValue(const MatrixEntryPointer& _matrixEntryPointer, ot::Variable&& _value);
		void setValue(const MatrixEntryPointer& _matrixEntryPointer, const ot::Variable& _value);
		void setValues(const ot::Variable* _values, uint32_t _size);

		void setValues(const std::list<ot::Variable>& _values);
		void setValues(const std::vector<ot::Variable>& _values) { m_values = _values; };
		const std::vector<Variable>& getValues() const { return m_values; };
		ot::Variable& getValue(const MatrixEntryPointer& _matrixEntryPointer);
		const ot::Variable& getValue(const MatrixEntryPointer& _matrixEntryPointer) const;

		const uint32_t getNumberOfColumns() const { return m_numberOfColumns; }
		const uint32_t getNumberOfRows() const { return m_numberOfRows; }

		//! @brief Creates an iterator for the matrix entries.
		//! @param _horizontalAdvance Determines the iteration order.
		//! If true, the iterator will advance horizontally (column-wise) first, then vertically (row-wise).
		MatrixEntryIterator<false> createIterator(bool _horizontalAdvance);

		//! @brief Creates a const iterator for the matrix entries.
		//! @param _horizontalAdvance Determines the iteration order.
		//! If true, the iterator will advance horizontally (column-wise) first, then vertically (row-wise).
		MatrixEntryIterator<true> createIterator(bool _horizontalAdvance) const;

		//! @brief Creates an iterator for the matrix data entries.
		//! @param _horizontalHeader Determines the iteration order and the starting point of the iterator.
		//! If true, the iterator will start at the first data entry after the horizontal header and advance horizontally (column-wise) first, then vertically (row-wise).
		MatrixEntryIterator<false> createDataIterator(bool _horizontalHeader);

		//! @brief Creates a const iterator for the matrix data entries.
		//! @param _horizontalHeader Determines the iteration order and the starting point of the iterator.
		//! If true, the iterator will start at the first data entry after the horizontal header and advance horizontally (column-wise) first, then vertically (row-wise).
		MatrixEntryIterator<true> createDataIterator(bool _horizontalHeader) const;

		//! @brief Creates an iterator for the matrix data entries of a specific column.
		//! @param _columnIndex The index of the column to iterate over.
		//! @param _horizontalHeader Determines the iteration order and the starting point of the iterator.
		//! If true, the iterator will advance horizontally (column-wise), otherwise it will advance vertically (row-wise).
		MatrixEntryIterator<false> createDataColumnIterator(uint32_t _columnIndex, bool _horizontalHeader);

		//! @brief Creates a const iterator for the matrix data entries of a specific column.
		//! @param _columnIndex The index of the column to iterate over.
		//! @param _horizontalHeader Determines the iteration order and the starting point of the iterator.
		//! If true, the iterator will advance horizontally (column-wise), otherwise it will advance vertically (row-wise).
		MatrixEntryIterator<true> createDataColumnIterator(uint32_t _columnIndex, bool _horizontalHeader) const;

		//! @brief Creates an iterator for the matrix header entries.
		//! @param _horizontalHeader Determines the iteration order and the starting point of the iterator.
		//! If true, the iterator will advance horizontally (column-wise), otherwise it will advance vertically (row-wise).
		MatrixEntryIterator<false> createHeaderIterator(bool _horizontalHeader);

		//! @brief Creates a const iterator for the matrix header entries.
		//! @param _horizontalHeader Determines the iteration order and the starting point of the iterator.
		//! If true, the iterator will advance horizontally (column-wise), otherwise it will advance vertically (row-wise).
		MatrixEntryIterator<true> createHeaderIterator(bool _horizontalHeader) const;

		std::vector<std::pair<ot::Variable, std::vector<ot::Variable>>> toTableColumns(bool _horizontalHeader = true) const { return GenericDataStructMatrix::toTableColumns(*this, _horizontalHeader); };
		static std::vector<std::pair<ot::Variable, std::vector<ot::Variable>>> toTableColumns(const GenericDataStructMatrix& _matrix, bool _horizontalHeader = true);

	private:
		uint32_t m_numberOfColumns = 0;
		uint32_t m_numberOfRows = 0;

		std::vector<ot::Variable> m_values;
		
		constexpr bool isValid(const MatrixEntryPointer& _matrixEntryPointer) const
		{
			return _matrixEntryPointer.getColumn() < m_numberOfColumns && _matrixEntryPointer.getRow() < m_numberOfRows;
		};
		constexpr uint32_t getIndex(const uint32_t& _rowIndex, const uint32_t& _columnIndex) const { return (m_numberOfColumns * _rowIndex) + _columnIndex; };
		constexpr uint32_t getIndex(const MatrixEntryPointer& _matrixEntryPointer) const { return getIndex(_matrixEntryPointer.getRow(), _matrixEntryPointer.getColumn()); };
	};
	
}

#include "OTCore/DataStruct/GenericDataStructMatrix.hpp"