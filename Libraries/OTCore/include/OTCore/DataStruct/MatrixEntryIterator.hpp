// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/DataStruct/MatrixEntryIterator.h"

template<bool isConst>
inline ot::MatrixEntryIterator<isConst>::MatrixEntryIterator(MatrixType* _matrix, const MatrixEntryPointer& _start, const MatrixEntryPointer& _end, bool _columnMajor)
	: m_matrix(_matrix), m_pointer(_start), m_start(_start), m_end(_end), m_columnMajor(_columnMajor)
{}

template<bool isConst>
ot::MatrixEntryIterator<isConst>::MatrixEntryIterator(MatrixEntryIterator<isConst>&& _other) noexcept
	: m_matrix(_other.m_matrix), m_pointer(_other.m_pointer), m_end(_other.m_end),
	m_columnMajor(_other.m_columnMajor)
{
	_other.m_matrix = nullptr;
}

template<bool isConst>
ot::MatrixEntryIterator<isConst>& ot::MatrixEntryIterator<isConst>::operator=(MatrixEntryIterator<isConst>&& _other) noexcept
{
	if (this != &_other)
	{
		m_pointer = _other.m_pointer;
		m_end = _other.m_end;

		m_columnMajor = _other.m_columnMajor;

		m_matrix = _other.m_matrix;
		_other.m_matrix = nullptr;
	}

	return *this;
}

template<bool isConst>
bool ot::MatrixEntryIterator<isConst>::isValid() const
{
	return m_matrix != nullptr && m_pointer.getColumn() >= m_start.getColumn() && m_pointer.getColumn() < m_end.getColumn() && m_pointer.getRow() >= m_start.getRow() && m_pointer.getRow() < m_end.getRow();
}

template<bool isConst>
std::conditional_t<isConst, const ot::Variable, ot::Variable> & ot::MatrixEntryIterator<isConst>::operator*() const
{
	OTAssert(isValid(), "Dereferencing an invalid iterator.");
	return m_matrix->getValue(m_pointer);
}

template<bool isConst>
ot::MatrixEntryIterator<isConst>& ot::MatrixEntryIterator<isConst>::operator++()
{
	OTAssert(isValid(), "Incrementing an invalid iterator.");
	if (!moveColumn())
	{
		wrapToNextRow();
	}
	return *this;
}

template<bool isConst>
bool ot::MatrixEntryIterator<isConst>::moveColumn()
{
	m_pointer.move(getColumnStep());
	return isValid();
}

template<bool isConst>
bool ot::MatrixEntryIterator<isConst>::wrapToNextRow()
{
	const MatrixEntryPointerStep rowStep = getRowStep();
	m_pointer.move(rowStep);
	m_pointer.setColumn(std::max(m_start.getColumn(), m_pointer.getColumn() * rowStep.getColumnStep()));
	m_pointer.setRow(std::max(m_start.getRow(), m_pointer.getRow() * rowStep.getRowStep()));

	return isValid();
}
