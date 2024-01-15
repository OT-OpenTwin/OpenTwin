#pragma once
#include <stdint.h>
#include <vector>

#include "OTCore/Variable.h"

class GenericDataBlock
{
public:
	GenericDataBlock(uint32_t numberofColumns, uint32_t numberofRows);
	~GenericDataBlock();
	void setValue(uint32_t columnIndex, uint32_t rowIndex, ot::Variable&& value);
	void setValue(uint32_t columnIndex, uint32_t rowIndex, const ot::Variable& value);
	const ot::Variable& getValue(uint32_t columnIndex, uint32_t rowIndex)const;
	const uint32_t getNumberOfColumns() const { return _numberOfColumns; }
	const uint32_t getNumberOfRows() const { return _numberOfRows; }
	const uint32_t getNumberOfEntries() const { return _numberOfEntries; }
private:
	uint32_t _numberOfColumns = 0;
	uint32_t _numberOfRows = 0;
	uint32_t _numberOfEntries;
	std::vector<std::vector<ot::Variable>> _values;
	/*ot::Variable* _values = nullptr;
	inline uint32_t getIndex(const uint32_t& columnIndex, const uint32_t& rowIndex) const { return _numberOfColumns * rowIndex + columnIndex; }*/
};

