// @otlicense

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
