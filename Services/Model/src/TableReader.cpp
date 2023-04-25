
#include "stdafx.h"

#include "TableReader.h"
#include "Model.h"

#include "EntityResultTable.h"
#include "EntityResultTableData.h"

#include <memory>

TableReader::TableReader()
	: model(nullptr)
{

}

TableReader::~TableReader()
{

}

std::string TableReader::readFromFile(const std::string fileName, const std::string &itemName, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner)
{
	assert(model != nullptr);

	std::ifstream file;

	try
	{
		file.open(fileName);
	}
	catch (std::exception &e)
	{
		return "Unable to open file (" + std::string(e.what()) + ")";
	}

	// Read the header from the file
	std::string header;
	if (!readNextLine(file, header))
	{
		return "No header found in file";
	}

	// Check which separator we have and split the header
	char separator = ';';

	if (header.find(',') != std::string::npos) separator = ',';
	if (header.find(';') != std::string::npos) separator = ';';

	std::list<std::string> headerData = separateLine(header, separator);

	// Now we read all data lines from the file
	std::list<std::string> dataContent;
	std::string line;
	while (readNextLine(file, line)) dataContent.push_back(line);

	// Create the new table item
	EntityResultTable<double> *tableItem = new EntityResultTable<double>(model->createEntityUID(), nullptr, model, ms, factory, owner);

	tableItem->setName(itemName);
	tableItem->setEditable(false);

	// Now we store the data in the entity

	// Get the data entity and set the table dimensions
	std::shared_ptr<EntityResultTableData<double>> dataItem = tableItem->getTableData();

	dataItem->setNumberOfColumns((unsigned int) headerData.size());
	dataItem->setNumberOfRows((unsigned int) dataContent.size());

	// Store the header data
	unsigned int iCol = 0;
	for (auto item : headerData)
	{
		dataItem->setHeaderText(iCol, item);
		iCol++;
	}

	// Store the table data

	unsigned int iRow = 0;
	for (auto lineItem : dataContent)
	{
		std::list<std::string> lineData = separateLine(lineItem, separator);

		iCol = 0;
		for (auto item : lineData)
		{
			double value = std::stod(item);
			dataItem->setValue(iRow, iCol, value);

			iCol++;
		}

		iRow++;
	}

	// Finally write everything to the data base and add the item to the model


	tableItem->StoreToDataBase();

	//UIDList newEntitiesID{ tableItem->getEntityID(), dataItem->getEntityID() };
	//UIDList newEntitiesVersion{ tableItem->getEntityStorageVersion(), dataItem->getEntityStorageVersion() };

	tableItem->releaseTableData();

	std::list<EntityBase *> allNewEntities;
	model->addEntityToModel(tableItem->getName(), tableItem, model->getRootNode(), true, allNewEntities);
	tableItem->addVisualizationItem(false);

	model->modelChangeOperationCompleted("table item imported");

	return "";
}

bool TableReader::readNextLine(std::ifstream &file, std::string &line)
{
	do
	{
		if (file.eof()) return false;
		std::getline(file, line);
		trimString(line);

	} while (line.empty());

	return true;
}

std::list<std::string> TableReader::separateLine(std::string &line, char separator)
{
	std::list<std::string> list;

	trimString(line);

	while (!line.empty())
	{
		size_t index = line.find(separator);
		if (index == line.npos)
		{
			// No separator found
			list.push_back(line);
			line.clear();
		}
		else
		{
			// We found a separator
			std::string item = line.substr(0, index);
			line = line.substr(index+1);

			trimString(item);
			trimString(line);

			list.push_back(item);
		}
	}

	return list;
}

void TableReader::trimString(std::string &str) 
{
	while (str.length() > 0 && std::isspace(str[0])) str.erase(0, 1);
	while (str.length() > 0 && std::isspace(str[str.length()-1])) str.erase(str.length()-1, 1);
}