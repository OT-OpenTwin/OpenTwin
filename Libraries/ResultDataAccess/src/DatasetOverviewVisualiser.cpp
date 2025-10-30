// @otlicense
// File: DatasetOverviewVisualiser.cpp
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

#include "DatasetOverviewVisualiser.h"
#include "OTCore/VariableToStringConverter.h"
#include "MetadataEntrySingle.h"
#include "MetadataEntryArray.h"
#include "MetadataEntryObject.h"

ot::GenericDataStruct* DatasetOverviewVisualiser::buildTableOverview(const MetadataSeries& _metadataSeries)
{
	TableDimension tableDimension =	calculateNeededTableSize(_metadataSeries);
	initiate(tableDimension);
	addSeriesToOverview(_metadataSeries);
	return release();	
}

void DatasetOverviewVisualiser::addSeriesToOverview(const MetadataSeries& _metadataSeries)
{
	addSingleValue("Series Metadata Overview");

	std::string label = _metadataSeries.getLabel();
	addKeyValuePair("Label", ot::Variable(label));

	/*ot::UID seriesID = _metadataSeries.getSeriesIndex();
	addKeyValuePair("UID", ot::Variable(static_cast<int64_t>(seriesID)));*/

	addHeading("Metadata");
	auto allMetadataByName = _metadataSeries.getMetadata();
	for (auto metadataByName : allMetadataByName)
	{
		addMetadataToOverview(metadataByName.second);
	}
	m_tablePointer.moveLeft();

	addHeading("Parameter");
	const std::list<MetadataParameter>& allParameter = _metadataSeries.getParameter();
	for (const MetadataParameter& parameter : allParameter)
	{
		addParameterToOverview(parameter);
	}
	m_tablePointer.moveLeft();


	addHeading("Quantity");
	const std::list<MetadataQuantity>& allQuantities = _metadataSeries.getQuantities();
	for (const MetadataQuantity& quantity : allQuantities)
	{
		addQuantityToOverview(quantity);
	}
	m_tablePointer.moveLeft();
}

ot::GenericDataStruct* DatasetOverviewVisualiser::buildTableOverview(const MetadataCampaign& _metadataCampaign)
{
	const std::list<MetadataSeries>& allSeries = _metadataCampaign.getSeriesMetadata();

	uint32_t totalNbOfSeriesRows(0), columns(1);
	for (const MetadataSeries& series : allSeries)
	{
		TableDimension tableDimension =	calculateNeededTableSize(series);
		totalNbOfSeriesRows += tableDimension.rows;
		columns = tableDimension.columns> columns ? tableDimension.columns: columns;
	}
	
	const uint32_t numberOfMetadata	= static_cast<uint32_t>(_metadataCampaign.getMetaData().size());
	const uint32_t numberOfHeader = 2;

	TableDimension totalTableDimension;
	totalTableDimension.rows = numberOfMetadata + numberOfHeader + totalNbOfSeriesRows;
	totalTableDimension.columns = columns;
	initiate(totalTableDimension);

	addSingleValue("Campaign Overview");
	addHeading("Metadata");
	for (auto metadata : _metadataCampaign.getMetaData())
	{
		addMetadataToOverview(metadata.second);
	}
	m_tablePointer.moveLeft();

	for (const auto& series : _metadataCampaign.getSeriesMetadata())
	{
		addSeriesToOverview(series);
	}

	return release();
}

DatasetOverviewVisualiser::~DatasetOverviewVisualiser()
{
	if (m_genericDataStruct != nullptr)
	{
		delete m_genericDataStruct;
		m_genericDataStruct = nullptr;
	}
}

TableDimension DatasetOverviewVisualiser::calculateNeededTableSize(const MetadataSeries& _metadataSeries)
{	
	const uint32_t numberOfSeriesHeading = 4;
	const uint32_t numberOfSeriesKeyValues = 1;
	const uint32_t numberOfSeriesMetadata = static_cast<uint32_t>(_metadataSeries.getMetadata().size());
	
	const uint32_t numberOfParameter = static_cast<uint32_t>(_metadataSeries.getParameter().size());
	const uint32_t numberOfParmaterKeyValues = 4;
	const uint32_t numberOfParmaterHeading = 3;
	uint32_t numberOfParmaterMetadata(0);
	for (const MetadataParameter& parameter : _metadataSeries.getParameter())
	{
		numberOfParmaterMetadata += static_cast<uint32_t>(parameter.metaData.size());
	}
	
	const uint32_t numberOfQuantity = static_cast<uint32_t>(_metadataSeries.getQuantities().size());
	const uint32_t numberOfQuantityKeyValues = 2;
	const uint32_t numberOfQuantityHeading = 3;
	const uint32_t numberOfQuantityValueDescriptionKeyValues = 3;
	const uint32_t numberOfQuantityValueDescriptionHeading = 1;
	uint32_t numberOfQuantityDescriptions(0);
	uint32_t numberOfQuantityMetadata(0);
	for (const MetadataQuantity& quantity : _metadataSeries.getQuantities())
	{
		numberOfQuantityDescriptions += static_cast<uint32_t>(quantity.valueDescriptions.size());
		numberOfQuantityMetadata += static_cast<uint32_t>(quantity.metaData.size());
	}


	uint32_t numberOfRows = numberOfSeriesHeading + numberOfSeriesKeyValues + numberOfSeriesMetadata +
		numberOfParameter * (numberOfParmaterKeyValues + numberOfParmaterHeading) + numberOfParmaterMetadata +
		numberOfQuantity * (numberOfQuantityKeyValues + numberOfQuantityHeading) + numberOfQuantityMetadata +
		numberOfQuantityDescriptions * (numberOfQuantityValueDescriptionKeyValues + numberOfQuantityValueDescriptionHeading);
	
	uint32_t numberOfColumns = 6;

	TableDimension tableDimension;
	tableDimension.rows = numberOfRows;
	tableDimension.columns = numberOfColumns;
	return tableDimension;
}

void DatasetOverviewVisualiser::addParameterToOverview(const MetadataParameter& _parameter)
{
	const std::string& label =	_parameter.parameterLabel;
	addHeading(label);
	
	const ot::Variable name = ot::Variable(_parameter.parameterLabel);
	addKeyValuePair("Name", name);
	
	/*const ot::Variable uid = ot::Variable(static_cast<int64_t>( _parameter.parameterUID));
	addKeyValuePair("UID", uid);*/

	const ot::Variable typeName = ot::Variable(_parameter.typeName);
	addKeyValuePair("Data type", typeName);

	const ot::Variable unit = _parameter.unit;
	addKeyValuePair("Unit", unit);

	const std::string concatinated = concatinateMaxNbOfVariablesInString(_parameter.values);
	addKeyValuePair("Values", ot::Variable(concatinated));

	addHeading("Metadata");
	auto allMetadataByName = _parameter.metaData;
	for (auto metadataByName : allMetadataByName)
	{
		addMetadataToOverview(metadataByName.second);
	}
	m_tablePointer.moveLeft();
	m_tablePointer.moveLeft();
}

void DatasetOverviewVisualiser::addQuantityToOverview(const MetadataQuantity& _quantity)
{
	const std::string& label = _quantity.quantityLabel;
	addHeading(label);

	const ot::Variable name = ot::Variable(ot::Variable(_quantity.quantityName));
	addKeyValuePair("Name", name);
		
	std::string dataDimension ("<");
	for (uint32_t dimension : _quantity.dataDimensions)
	{
		dataDimension += std::to_string(dimension) + ", ";
	}
	dataDimension =	dataDimension.substr(0, dataDimension.size() - 2) + ">";
	addKeyValuePair("Data dimensions", ot::Variable(dataDimension));

	addHeading("Value descriptions");
	for (const MetadataQuantityValueDescription& valueDescription : _quantity.valueDescriptions)
	{
		addHeading("Label: " +  valueDescription.quantityValueLabel);
		
		const ot::Variable name (valueDescription.quantityValueName);
		addKeyValuePair("Name", name);

		const ot::Variable unit (valueDescription.unit);
		addKeyValuePair("Unit", unit);

		const ot::Variable datatype (valueDescription.dataTypeName);
		addKeyValuePair("Datatype", datatype);

		/*const ot::Variable index(static_cast<int64_t>(valueDescription.quantityIndex));
		addKeyValuePair("UID", index);*/
		
		m_tablePointer.moveLeft();
	}
	m_tablePointer.moveLeft();
	
	addHeading("Metadata");
	auto allMetadataByName =_quantity.metaData;
	for (auto metadataByName : allMetadataByName)
	{
		addMetadataToOverview(metadataByName.second);
	}
	m_tablePointer.moveLeft();
	m_tablePointer.moveLeft();
}

void DatasetOverviewVisualiser::addMetadataToOverview(std::shared_ptr<MetadataEntry> _metadataEntries)
{
	const std::string& name = _metadataEntries->getEntryName();
	ot::MatrixEntryPointer matrixPointer;
	matrixPointer.m_column = m_tablePointer.getColumn();
	matrixPointer.m_row = m_tablePointer.getRow();
	m_genericDataStruct->setValue(matrixPointer, ot::Variable(name));
	m_tablePointer.moveRight();
	MetadataEntrySingle* singleEntry = dynamic_cast<MetadataEntrySingle*>(_metadataEntries.get());
	if (singleEntry != nullptr)
	{
		const ot::Variable& value = singleEntry->getValue();
		matrixPointer.m_column = m_tablePointer.getColumn();
		matrixPointer.m_row = m_tablePointer.getRow();
		m_genericDataStruct->setValue(matrixPointer, value);
	}
	else
	{
		MetadataEntryArray* arrayEntry = dynamic_cast<MetadataEntryArray*>(_metadataEntries.get());
		matrixPointer.m_column = m_tablePointer.getColumn();
		matrixPointer.m_row = m_tablePointer.getRow();
		if (arrayEntry != nullptr)
		{
			const std::list<ot::Variable>& values =	arrayEntry->getValues();
			const std::string concatinated = concatinateMaxNbOfVariablesInString(values);
			m_genericDataStruct->setValue(matrixPointer, concatinated);
		}
		else
		{

			m_genericDataStruct->setValue(matrixPointer, "Object");
		}
	}
	m_tablePointer.moveDownAndLeft();
}

std::string DatasetOverviewVisualiser::concatinateMaxNbOfVariablesInString(const std::list<ot::Variable>& _values)
{
	ot::VariableToStringConverter converter;
	std::string concatinated("");
	int count(0);
	for (const ot::Variable& value : _values)
	{
		if (count == m_maxNbOfValues)
		{
			concatinated += "; ...  ";
			break;
		}
		else
		{
			concatinated += converter(value) + "; ";
		}
		count++;
	}
	if (_values.size() != 0)
	{

		concatinated = concatinated.substr(0, concatinated.size()-2);
	}
	return concatinated;
}

void DatasetOverviewVisualiser::addKeyValuePair(const std::string& _key,const ot::Variable& _value)
{
	ot::MatrixEntryPointer matrixPointer;
	matrixPointer.m_column = m_tablePointer.getColumn();
	matrixPointer.m_row = m_tablePointer.getRow();
	m_genericDataStruct->setValue(matrixPointer,ot::Variable(_key));
	m_tablePointer.moveRight();
	matrixPointer.m_column = m_tablePointer.getColumn();
	matrixPointer.m_row = m_tablePointer.getRow();
	m_genericDataStruct->setValue(matrixPointer, _value);
	m_tablePointer.moveDownAndLeft();
}

void DatasetOverviewVisualiser::addSingleValue(const ot::Variable& _value)
{
	ot::MatrixEntryPointer matrixPointer;
	matrixPointer.m_column = m_tablePointer.getColumn();
	matrixPointer.m_row = m_tablePointer.getRow();
	m_genericDataStruct->setValue(matrixPointer, _value);
	m_tablePointer.moveDown();
}



void DatasetOverviewVisualiser::addHeading(const std::string& _heading)
{
	ot::MatrixEntryPointer matrixPointer;
	matrixPointer.m_column = m_tablePointer.getColumn();
	matrixPointer.m_row = m_tablePointer.getRow();
	m_genericDataStruct->setValue(matrixPointer, ot::Variable(_heading));
	m_tablePointer.moveRight();
	m_tablePointer.moveDown();
}

void DatasetOverviewVisualiser::initiate(TableDimension& _tableDimension)
{
	ot::MatrixEntryPointer matrixPointer;
	matrixPointer.m_column = _tableDimension.columns;
	matrixPointer.m_row = _tableDimension.rows;

	m_genericDataStruct = new ot::GenericDataStructMatrix(matrixPointer,ot::Variable(""));
}

ot::GenericDataStructMatrix* DatasetOverviewVisualiser::release()
{
	ot::GenericDataStructMatrix* temp = m_genericDataStruct;
	m_genericDataStruct = nullptr;
	m_tablePointer = TableLocationPointer();
	return temp;
}
