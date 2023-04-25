#include "TableHandler.h"
#include "FileToTableExtractorFactory.h"

#include "TableStatisticAnalyser.h"

#include "ClassFactory.h"
#include "EntityParameterizedDataTable.h"

TableHandler::TableHandler(const std::string tableFolder) : _tableFolder(tableFolder)
{}

void TableHandler::AddTableView(ot::UID sourceID, ot::UID sourceVersionID)
{	
	ClassFactory classFactory;
	auto sourceFile = dynamic_cast<EntityParameterizedDataSource*>(_modelComponent->readEntityFromEntityIDandVersion(sourceID, sourceVersionID, classFactory));
	if (sourceFile == nullptr)
	{
		assert(0); // Only EntityParameterizedDataSource should reach here.
	}
	auto topoEnt = new EntityParameterizedDataTable(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, &classFactory, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	std::string fullName = CreateNewUniqueTopologyName(_tableFolder, sourceFile->getFileName());
	topoEnt->setName(fullName);
	std::string type = sourceFile->getFileType();
	auto tableExtractor = FileToTableExtractorFactory::GetInstance()->Create(sourceFile->getFileType());
	tableExtractor->ExtractFromEntitySource(sourceFile);
	TableStatisticAnalyser analyser(tableExtractor.get());
	int numberOfColumns = static_cast<int>(analyser.CountColumns());
	int numberOfRows = static_cast<int>(analyser.CountRows());
	   
	auto tableData = ExtractTableData(tableExtractor,numberOfRows,numberOfColumns);
	tableData->StoreToDataBase();

	topoEnt->setTableData(tableData);
	topoEnt->SetTableDimensions(tableData->getNumberOfRows(), tableData->getNumberOfColumns());
	topoEnt->SetSourceFile(sourceFile->getFileName() + "." + sourceFile->getFileType(), sourceFile->getPath());
	topoEnt->createProperties(EntityParameterizedDataTable::HeaderOrientation::horizontal);
	topoEnt->StoreToDataBase();

	std::list<ot::UID> topologyEntityIDList = { topoEnt->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { topoEnt->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };

	std::list<ot::UID> dataEntityIDList{ tableData->getEntityID() };
	std::list<ot::UID> dataEntityVersionList{ tableData->getEntityStorageVersion()};
	std::list<ot::UID> dataEntityParentList{ topoEnt->getEntityID() };

	_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new table");
}

std::shared_ptr<EntityResultTableData<std::string>> TableHandler::ExtractTableData(std::shared_ptr<TableExtractor> extractor, int numberOfRows, int numberOfColumns)
{
	std::vector<std::vector<std::string>> processedTableData;
	processedTableData.reserve(numberOfRows);
	
	while (extractor->HasNextLine())
	{
		std::vector<std::string> line;
		extractor->GetNextLine(line);
		TrimTrailingControlCharacter(line);
		if (!IsEmptyLine(line))
		{
			processedTableData.push_back(line);
		}
	}
	processedTableData.shrink_to_fit();
	ClassFactory classFactory;
	auto tableData = std::make_shared<EntityResultTableData<std::string>>(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, &classFactory, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	tableData->setNumberOfColumns(numberOfColumns);
	tableData->setNumberOfRows(processedTableData.size());
	uint64_t lineCounter(0);
	for (auto& row : processedTableData)
	{
		tableData->setRow(lineCounter, row);
		lineCounter++;
	}
	return tableData;
}

void TableHandler::TrimTrailingControlCharacter(std::vector<std::string>& line)
{
	for (std::string& cell : line)
	{
		while (cell.size() != 0 && ( cell.back() == '\r' || cell.back() == '\n' || cell.back() == '\t'))
		{
			cell.pop_back();
		}
	}
}

bool TableHandler::IsEmptyLine(std::vector<std::string>& line)
{
	for (std::string& cell : line)
	{
		std::string::difference_type n = std::count(cell.begin(), cell.end(), ' ');
		if (cell != "" && cell.size() != n)
		{
			return false;
		}
	}
	return true;
}
