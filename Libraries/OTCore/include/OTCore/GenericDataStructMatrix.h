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

#include <stdint.h>
#include <vector>
#include <functional>

#include "OTCore/Variable.h"
#include "GenericDataStruct.h"

namespace ot
{
	struct __declspec(dllexport) MatrixEntryPointer
	{
		uint32_t m_column = 0;
		uint32_t m_row = 0;
	};

	class __declspec(dllexport) GenericDataStructMatrix : public GenericDataStruct
	{
	public:
		//! @brief Only for deserialisation. Generally use the constructor with explicit topology data.
		GenericDataStructMatrix();
		~GenericDataStructMatrix();
		GenericDataStructMatrix(const GenericDataStructMatrix& _other);
		GenericDataStructMatrix(GenericDataStructMatrix&& _other)noexcept;
		GenericDataStructMatrix& operator=(const GenericDataStructMatrix& _other) = default;
		GenericDataStructMatrix& operator=(GenericDataStructMatrix&& _other) noexcept = default;


		GenericDataStructMatrix(uint32_t _rows, uint32_t _columns);
		GenericDataStructMatrix(const MatrixEntryPointer& _matrixEntryPointer);
		GenericDataStructMatrix(uint32_t _rows, uint32_t _columns, ot::Variable _defaultValue);
		GenericDataStructMatrix(const MatrixEntryPointer& _matrixEntryPointer, ot::Variable _defaultValue);
		
		void setValue(const MatrixEntryPointer& _matrixEntryPointer, ot::Variable&& _value);
		void setValue(const MatrixEntryPointer& _matrixEntryPointer, const ot::Variable& _value);
		void setValues(const ot::Variable* _values, uint32_t _size);
		
		//! @brief Assumes the list to be a linearised matrix of the topology this class was initialised with.
		void setValues(std::list<ot::Variable> _values);
		/// <summary>
		/// column-major access
		/// </summary>
		const ot::Variable& getValue(const MatrixEntryPointer& _matrixEntryPointer)const;
		const ot::Variable* getValues()const;
		const uint32_t getNumberOfColumns() const { return m_numberOfColumns; }
		const uint32_t getNumberOfRows() const { return m_numberOfRows; }

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		static std::string getClassName() { return "GenericDataStructMatrix"; }

	private:
		uint32_t m_numberOfColumns = 0;
		uint32_t m_numberOfRows = 0;
		
		std::vector<ot::Variable> m_values;
		void allocateValueMemory();
		void allocateValueMemory( const ot::Variable& _defaultValue);
		void allocateValueMemory( ot::Variable&& _defaultValue);
		
		inline uint32_t getIndex(const uint32_t& _columnIndex, const uint32_t& _rowIndex) const { return m_numberOfColumns * _rowIndex + _columnIndex; }
	};


}
