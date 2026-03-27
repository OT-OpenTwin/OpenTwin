// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/DataStruct/MatrixEntryPointerStep.h"

namespace ot {

	class MatrixEntryPointer
	{
		OT_DECL_DEFCOPY(MatrixEntryPointer)
		OT_DECL_DEFMOVE(MatrixEntryPointer)
	public:
		constexpr MatrixEntryPointer() = default;
		constexpr MatrixEntryPointer(uint32_t _row, uint32_t _column) : m_column(_column), m_row(_row) {};
		~MatrixEntryPointer() = default;

		inline void setColumn(uint32_t _column) { m_column = _column; };
		inline void moveColumn(uint32_t _columnOffset = 1) { m_column += _columnOffset; };
		inline void moveColumnBack(uint32_t _columnOffset = 1) { m_column -= _columnOffset; };
		constexpr uint32_t getColumn() const { return m_column; };

		inline void setRow(uint32_t _row) { m_row = _row; };
		inline void moveRow(uint32_t _rowOffset = 1) { m_row += _rowOffset; };
		inline void moveRowBack(uint32_t _rowOffset = 1) { m_row -= _rowOffset; };
		constexpr uint32_t getRow() const { return m_row; };

		inline void move(uint32_t _row, uint32_t _column) { m_column += _column; m_row += _row; };
		inline void move(const MatrixEntryPointerStep& _step) { m_column += _step.getColumnStep(); m_row += _step.getRowStep(); };
		inline void moveBack(uint32_t _row, uint32_t _column) { m_column -= _column; m_row -= _row; };
		inline void moveBack(const MatrixEntryPointerStep& _step) { m_column -= _step.getColumnStep(); m_row -= _step.getRowStep(); };

	private:
		uint32_t m_column = 0;
		uint32_t m_row = 0;
	};
}