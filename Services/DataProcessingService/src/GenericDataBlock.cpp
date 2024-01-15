#include "GenericDataBlock.h"

GenericDataBlock::GenericDataBlock(uint32_t numberOfColumns, uint32_t numberOfRows)
	:_numberOfColumns(numberOfColumns), _numberOfRows(numberOfRows)
{
	_numberOfEntries = _numberOfColumns * _numberOfRows;
	_values.resize(_numberOfRows);
	for (uint32_t row = 0; row < _numberOfRows; row++)
	{
		_values[row].resize(_numberOfColumns);
	}
	/*_values = static_cast<ot::Variable*>(malloc(sizeof(ot::Variable)*_numberOfEntries));*/
}

GenericDataBlock::~GenericDataBlock()
{
	//free(_values);
	//_values = NULL;
}

void GenericDataBlock::setValue(uint32_t columnIndex, uint32_t rowIndex, ot::Variable&& value)
{
	/*const uint32_t index = getIndex(columnIndex, rowIndex);
	_values[index] = value;*/
	_values[rowIndex][columnIndex] = std::move(value);
}

void GenericDataBlock::setValue(uint32_t columnIndex, uint32_t rowIndex, const ot::Variable& value)
{
	_values[rowIndex][columnIndex] = value;
}

const ot::Variable& GenericDataBlock::getValue(uint32_t columnIndex, uint32_t rowIndex) const
{
	/*const uint32_t index =  getIndex(columnIndex, rowIndex);
	return _values[index];*/
	return _values[rowIndex][columnIndex];
}
