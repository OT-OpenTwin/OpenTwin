#pragma once

#include "EntityResultTableData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <exception>
#include <bsoncxx/builder/basic/array.hpp>

#include <OTCore/TemplateTypeName.h>

#include "BsonArrayTypeGetterWrapper.h"

template <class T>
EntityResultTableData<T>::EntityResultTableData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms,  factory, owner)
{
	className = className + "_" + ot::TemplateTypeName<T>::getTypeName();
}

template <class T>
EntityResultTableData<T>::~EntityResultTableData()
{
}

template <class T>
bool EntityResultTableData<T>::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

template <class T>
void EntityResultTableData<T>::StoreToDataBase(void)
{
	EntityBase::StoreToDataBase();
}

template <class T>
void EntityResultTableData<T>::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	auto headerArray = bsoncxx::builder::basic::array();

	for (auto &text : header)
	{
		headerArray.append(text);
	}

	auto dataArray = bsoncxx::builder::basic::array();

	for (unsigned int iRow = 0; iRow < getNumberOfRows(); iRow++)
	{
		for (unsigned int iCol = 0; iCol < getNumberOfColumns(); iCol++)
		{
			dataArray.append(getValue(iRow, iCol));
		}
	}

	// Now add the actual text data

	storage.append(
		bsoncxx::builder::basic::kvp("NumberColums", (int) getNumberOfColumns()),
		bsoncxx::builder::basic::kvp("NumberRows", (int) getNumberOfRows()),
		bsoncxx::builder::basic::kvp("Header", headerArray),
		bsoncxx::builder::basic::kvp("Data", dataArray)
	);
}

template <class T>
void EntityResultTableData<T>::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the text data
	header.clear();
	data.clear();

	// Now load the data from the storage
	unsigned int numberCols = doc_view["NumberColums"].get_int32();
	unsigned int numberRows = doc_view["NumberRows"].get_int32();

	setNumberOfColumns(numberCols);
	setNumberOfRows(numberRows);

	auto headerArray = doc_view["Header"].get_array().value;
	auto dataArray = doc_view["Data"].get_array().value;

	unsigned int iCol = 0;
	for (const auto& text : headerArray)
	{
		setHeaderText(iCol, text.get_utf8().value.data());
		iCol++;
	}

	unsigned int iRow = 0;
	iCol = 0;

	for (const auto& data : dataArray)
	{

		setValue(iRow, iCol, BsonArrayTypeGetterWrapper<T>::getValue(data));

		iCol++;
		if (iCol == numberCols)
		{
			iRow++;
			iCol = 0;
		}
	}

	resetModified();
}

template <class T>
void EntityResultTableData<T>::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

template <class T>
void EntityResultTableData<T>::setNumberOfColumns(uint64_t cols)
{
	header.resize(cols, "");

	for (unsigned int iRow = 0; iRow < getNumberOfRows(); iRow++)
	{
		data[iRow].resize(cols);
	}
}

template <class T>
void EntityResultTableData<T>::setNumberOfRows(uint64_t rows)
{
	uint64_t oldRows = getNumberOfRows();

	data.resize(rows);

	if (rows > oldRows)
	{
		for (uint64_t iRow = oldRows; iRow < rows; iRow++)
		{
			data[iRow].resize(getNumberOfColumns());
		}
	}
}

template <class T>
void EntityResultTableData<T>::setHeaderText(unsigned int col, const std::string &text)
{
	assert(col < getNumberOfColumns());
	header[col] = text;
}

template <class T>
void EntityResultTableData<T>::setValue(unsigned int row, unsigned int col, T value)
{
	assert(row < getNumberOfRows());
	assert(col < getNumberOfColumns());

	data[row][col] = value;
}
template<class T>
void EntityResultTableData<T>::setRow(unsigned int row, std::vector<T>& value)
{
	if (row >= getNumberOfRows())
	{
		throw std::runtime_error("Row index out of range.");
	}
	if (value.size() > getNumberOfColumns())
	{
		throw std::runtime_error("Vector has too many entries for the set number of columns.");
	}

	uint64_t i = 0;
	for (i; i < value.size(); i++)
	{
		data[row][i] = value[i];
	}
	while (i < getNumberOfColumns())
	{
		data[row][i] = "";
		i++;
	}
}

template<class T>
void EntityResultTableData<T>::setColumn(unsigned int column, std::vector<T>& value)
{
	if (column >= getNumberOfColumns())
	{
		throw std::runtime_error("Column index out of range.");
	}
	if (value.size() >= getNumberOfRows())
	{
		throw std::runtime_error("Vector has too many entries for the set number of rows.");
	}

	for (int i = 0; i < value.size(); i++)
	{
		data[i][column] = value[i];
	}

}

template <class T>
uint64_t EntityResultTableData<T>::getNumberOfRows(void)
{
	return (data.size());
}

template <class T>
uint64_t EntityResultTableData<T>::getNumberOfColumns(void)
{
	return (header.size());
}

template <class T>
std::string EntityResultTableData<T>::getHeaderText(unsigned int col)
{
	assert(col < getNumberOfColumns());
	return header[col];
}

template <class T>
T EntityResultTableData<T>::getValue(unsigned int row, unsigned int col)
{
	assert(row < getNumberOfRows());
	assert(col < getNumberOfColumns());

	return data[row][col];
}

