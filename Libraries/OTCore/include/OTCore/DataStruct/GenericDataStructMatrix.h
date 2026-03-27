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


		MatrixEntryIterator<false> createIterator(bool _horizontalAdvance)
		{
			return MatrixEntryIterator<false>(this,
				MatrixEntryPointer(0, 0),
				MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
				_horizontalAdvance
			);
		}
		MatrixEntryIterator<true> createIterator(bool _horizontalAdvance) const
		{
			return MatrixEntryIterator<true>(this,
				MatrixEntryPointer(0, 0),
				MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
				_horizontalAdvance
			);
		}

		MatrixEntryIterator<false> getDataIterator(bool _horizontalHeader)
		{
			return MatrixEntryIterator<false>(this,
				_horizontalHeader ? MatrixEntryPointer(1, 0) : MatrixEntryPointer(0, 1),
				MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
				_horizontalHeader
			);
		}
		MatrixEntryIterator<true> getDataIterator(bool _horizontalHeader) const
		{
			return MatrixEntryIterator<true>(this,
				_horizontalHeader ? MatrixEntryPointer(1, 0) : MatrixEntryPointer(0, 1),
				MatrixEntryPointer(m_numberOfRows, m_numberOfColumns),
				_horizontalHeader
			);
		}

		MatrixEntryIterator<false> getDataColumnIterator(uint32_t _columnIndex, bool _horizontalHeader)
		{
			return MatrixEntryIterator<false>(this,
				_horizontalHeader ? MatrixEntryPointer(1, _columnIndex) : MatrixEntryPointer(_columnIndex, 1),
				MatrixEntryPointer(m_numberOfRows, _columnIndex + 1),
				_horizontalHeader
			);
		}
		MatrixEntryIterator<true> getDataColumnIterator(uint32_t _columnIndex, bool _horizontalHeader) const
		{
			return MatrixEntryIterator<true>(this,
				_horizontalHeader ? MatrixEntryPointer(1, _columnIndex) : MatrixEntryPointer(_columnIndex, 1),
				MatrixEntryPointer(m_numberOfRows, _columnIndex + 1),
				_horizontalHeader
			);
		}

		MatrixEntryIterator<false> getHeaderIterator(bool _horizontalHeader)
		{
			return MatrixEntryIterator<false>(this,
				MatrixEntryPointer(0, 0),
				_horizontalHeader ? MatrixEntryPointer(1, m_numberOfColumns) : MatrixEntryPointer(m_numberOfRows, 1),
				_horizontalHeader
			);
		}
		MatrixEntryIterator<true> getHeaderIterator(bool _horizontalHeader) const
		{
			return MatrixEntryIterator<true>(this,
				MatrixEntryPointer(0, 0),
				_horizontalHeader ? MatrixEntryPointer(1, m_numberOfColumns) : MatrixEntryPointer(m_numberOfRows, 1),
				_horizontalHeader
			);
		}

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
