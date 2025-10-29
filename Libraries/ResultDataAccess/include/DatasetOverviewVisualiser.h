// @otlicense

#pragma once
#include "OTCore/GenericDataStructMatrix.h"
#include "MetadataSeries.h"
#include "MetadataCampaign.h"

class TableLocationPointer
{
public:
	uint32_t getColumn() const { return m_column; }
	uint32_t getRow() const { return m_row; }
	void moveRight() { m_column++; }
	void moveLeft() { m_column--; }
	void moveUp() { m_row--; }
	void moveDown() { m_row++; }
	void moveDownAndLeft() { m_row++; m_column--; }

private:
	uint32_t m_column = 0;
	uint32_t m_row = 0;
};

struct TableDimension
{
	uint32_t columns;
	uint32_t rows;
};

class __declspec(dllexport) DatasetOverviewVisualiser
{
public:
	ot::GenericDataStruct* buildTableOverview(const MetadataSeries& _metadataSeries);
	ot::GenericDataStruct* buildTableOverview(const MetadataCampaign& _metadataCampaign);
	~DatasetOverviewVisualiser();

private:
	uint32_t m_maxNbOfValues = 10;
	TableLocationPointer m_tablePointer;
	ot::GenericDataStructMatrix* m_genericDataStruct = nullptr;

	TableDimension calculateNeededTableSize(const MetadataSeries& _metadataSeries);

	void addParameterToOverview(const MetadataParameter& _parameter);
	void addQuantityToOverview(const MetadataQuantity& _quantity);
	void addMetadataToOverview(std::shared_ptr<MetadataEntry> _metadataEntries);

	std::string concatinateMaxNbOfVariablesInString(const std::list<ot::Variable>& _values);

	void addKeyValuePair(const std::string& _key,const ot::Variable& _value);
	void addSingleValue(const ot::Variable& _value);
	void addHeading(const std::string& _heading);

	void addSeriesToOverview(const MetadataSeries& _metadataSeries);

	void initiate(TableDimension& _tableDimension);
	ot::GenericDataStructMatrix* release();
};

