// @otlicense
// File: GenericDataStructMatrix.cpp
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

// OpenTwin header
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTCore/DataStruct/GenericDataStructMatrixMerger.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTCore/Variable/VariableToJSONConverter.h"
#include "OTCore/Variable/VariableToStringConverter.h"

static ot::GenericDataStruct::Registrar<ot::GenericDataStructMatrix> registrar(ot::GenericDataStructMatrix::className());

ot::GenericDataStructMatrix::GenericDataStructMatrix(uint32_t _rows, uint32_t _columns)
	: m_numberOfColumns(_columns), m_numberOfRows(_rows)
{
	m_values.resize(m_numberOfRows * m_numberOfColumns, ot::Variable());
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(const MatrixEntryPointer& _dimensions)
	: m_numberOfColumns(_dimensions.getColumn()), m_numberOfRows(_dimensions.getRow())
{
	m_values.resize(m_numberOfRows * m_numberOfColumns, ot::Variable());
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(uint32_t _rows, uint32_t _columns, const ot::Variable& _defaultValue)
	: m_numberOfColumns(_columns), m_numberOfRows(_rows)
{
	m_values.resize(m_numberOfRows * m_numberOfColumns, _defaultValue);
}

ot::GenericDataStructMatrix::GenericDataStructMatrix(const MatrixEntryPointer& _dimensions, const ot::Variable& _defaultValue)
	: m_numberOfColumns(_dimensions.getColumn()), m_numberOfRows(_dimensions.getRow())
{
	m_values.resize(m_numberOfRows * m_numberOfColumns, _defaultValue);
}

ot::GenericDataStructMatrix::~GenericDataStructMatrix()
{
}

ot::GenericDataStructMatrix ot::GenericDataStructMatrix::tableMerge(const GenericDataStructMatrix& _other, bool _horizontalHeader) const
{
	std::list<GenericDataStructMatrix> others = { _other };
	return tableMerge(others, _horizontalHeader);
}

ot::GenericDataStructMatrix ot::GenericDataStructMatrix::tableMerge(const std::list<GenericDataStructMatrix>& _others, bool _horizontalHeader) const
{
	std::list<GenericDataStructMatrix> lst(_others);
	lst.push_front(*this);
	return GenericDataStructMatrixMerger::tableMerge(lst, _horizontalHeader);
}
void ot::GenericDataStructMatrix::setValue(const MatrixEntryPointer& _matrixEntryPointer, ot::Variable&& _value)
{
	const uint32_t index = getIndex(_matrixEntryPointer);
	m_values[index] = std::move(_value);
}

void ot::GenericDataStructMatrix::setValue(const MatrixEntryPointer& _matrixEntryPointer, const ot::Variable& _value)
{
	const uint32_t index = getIndex(_matrixEntryPointer);
	m_values[index] = _value;
}

void ot::GenericDataStructMatrix::setValues(const ot::Variable* _values, uint32_t _size)
{
	m_values.clear();
	m_values.reserve(_size);
	m_values.insert(m_values.begin(), &_values[0], &_values[_size]);
}

void ot::GenericDataStructMatrix::setValues(const std::list<ot::Variable>& _values)
{
	assert(_values.size() == getNumberOfEntries());
	m_values = { _values.begin(), _values.end() };
}

ot::Variable& ot::GenericDataStructMatrix::getValue(const MatrixEntryPointer& _matrixEntryPointer)
{
	OTAssert(isValid(_matrixEntryPointer), "Matrix entry pointer is out of bounds.");
	const uint32_t index = getIndex(_matrixEntryPointer);
	return m_values[index];
}

const ot::Variable& ot::GenericDataStructMatrix::getValue(const MatrixEntryPointer& _matrixEntryPointer) const
{
	OTAssert(isValid(_matrixEntryPointer), "Matrix entry pointer is out of bounds.");
	const uint32_t index = getIndex(_matrixEntryPointer);
	return m_values[index];
}

std::vector<std::pair<ot::Variable, std::vector<ot::Variable>>> ot::GenericDataStructMatrix::toTableColumns(const GenericDataStructMatrix& _matrix, bool _horizontalHeader)
{
	std::vector<std::pair<ot::Variable, std::vector<ot::Variable>>> columns;
	
	auto headerIt = _matrix.createHeaderIterator(_horizontalHeader);
	while (headerIt.isValid())
	{
		Variable header = headerIt.get();
		std::vector<Variable> columnValues;

		auto colIt = _matrix.createDataColumnIterator(headerIt.getDataColumnIndex(), _horizontalHeader);
		while (colIt.isValid())
		{
			columnValues.push_back(colIt.get());
			++colIt;
		}

		columns.emplace_back(std::move(header), std::move(columnValues));
		++headerIt;
	}

	return columns;
}

void ot::GenericDataStructMatrix::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	GenericDataStruct::addToJsonObject(_object, _allocator);
	_object.AddMember("numberOfColumns", m_numberOfColumns, _allocator);
	_object.AddMember("numberOfRows", m_numberOfRows, _allocator);

	VariableToJSONConverter converter;
	ot::JsonArray jArr;

	for (const Variable& var : m_values)
	{
		jArr.PushBack(converter(var, _allocator), _allocator);
	}
	_object.AddMember("values", jArr, _allocator);
}

void ot::GenericDataStructMatrix::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	m_numberOfColumns = ot::json::getUInt(_object, "numberOfColumns");
	m_numberOfRows = ot::json::getUInt(_object, "numberOfRows");
	
	const size_t nrEntries = m_numberOfRows * m_numberOfColumns;

	JSONToVariableConverter converter;
	const ConstJsonArray jArray = ot::json::getArray(_object, "values");
	OTAssert(jArray.Size() == nrEntries, "Number of entries in JSON does not match the expected number of entries based on the number of rows and columns.");
	m_values.resize(nrEntries);
	for (uint32_t index = 0; index < nrEntries; index++)
	{
		const ot::Variable& var = converter(jArray[index]);
		m_values[index] = var;
	}
}
