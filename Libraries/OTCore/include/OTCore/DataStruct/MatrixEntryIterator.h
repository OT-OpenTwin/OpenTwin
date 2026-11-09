// @otlicense
// File: MatrixEntryIterator.h
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
#include "OTCore/Variable/Variable.h"
#include "OTCore/DataStruct/MatrixEntryPointer.h"

namespace ot {

	class GenericDataStructMatrix;

	template<bool isConst>
	class MatrixEntryIterator
	{
		OT_DECL_NOCOPY(MatrixEntryIterator)
		OT_DECL_NODEFAULT(MatrixEntryIterator)
	public:
		using MatrixType = typename std::conditional_t<isConst, const GenericDataStructMatrix, GenericDataStructMatrix>;
		using VariableType = typename std::conditional_t<isConst, const Variable, Variable>;

		MatrixEntryIterator(MatrixType* _matrix, const MatrixEntryPointer& _start, const MatrixEntryPointer& _end, bool _columnMajor = true);
		MatrixEntryIterator(MatrixEntryIterator&& _other) noexcept;
		MatrixEntryIterator& operator=(MatrixEntryIterator&& _other) noexcept;

		bool isValid() const;

		VariableType& operator*() const;
		VariableType& get() const { return operator*(); };
		MatrixEntryIterator& operator++();

		inline MatrixEntryPointerStep getColumnStep() const { return (m_columnMajor ? MatrixEntryPointerStep::horizontal() : MatrixEntryPointerStep::vertical()); };
		inline MatrixEntryPointerStep getRowStep() const { return (m_columnMajor ? MatrixEntryPointerStep::vertical() : MatrixEntryPointerStep::horizontal()); };

		uint32_t getDataColumnIndex() const { return (m_columnMajor ? m_pointer.getColumn() : m_pointer.getRow()); };
		uint32_t getDataRowIndex() const { return (m_columnMajor ? m_pointer.getRow() : m_pointer.getColumn()); };

		MatrixEntryPointer getStart() const { return m_start; };
		MatrixEntryPointer getEnd() const { return m_end; };

	private:
		bool moveColumn();
		bool wrapToNextRow();

		MatrixType* m_matrix = nullptr;
		bool m_columnMajor = true;
		MatrixEntryPointer m_pointer;
		MatrixEntryPointer m_start;
		MatrixEntryPointer m_end;
	};

}

// OpenTwin header
#include "OTCore/DataStruct/MatrixEntryIterator.hpp"