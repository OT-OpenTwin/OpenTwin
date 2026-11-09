// @otlicense
// File: MatrixEntryPointerStep.h
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
#include "OTCore/CoreTypes.h"

namespace ot {

	class MatrixEntryPointerStep
	{
		OT_DECL_DEFCOPY(MatrixEntryPointerStep)
		OT_DECL_DEFMOVE(MatrixEntryPointerStep)
	public:
		static constexpr MatrixEntryPointerStep horizontal(uint32_t _step = 1) { return MatrixEntryPointerStep(0, _step); };
		static constexpr MatrixEntryPointerStep vertical(uint32_t _step = 1) { return MatrixEntryPointerStep(_step, 0); };

		explicit constexpr MatrixEntryPointerStep() = default;
		explicit constexpr MatrixEntryPointerStep(uint32_t _row, uint32_t _column) : m_columnStep(_column), m_rowStep(_row) {};
		~MatrixEntryPointerStep() = default;

		inline void setColumnStep(uint32_t _columnStep) { m_columnStep = _columnStep; };
		constexpr uint32_t getColumnStep() const { return m_columnStep; };

		inline void setRowStep(uint32_t _rowStep) { m_rowStep = _rowStep; };
		constexpr uint32_t getRowStep() const { return m_rowStep; };

	private:
		uint32_t m_columnStep = 0;
		uint32_t m_rowStep = 0;
	};

}
