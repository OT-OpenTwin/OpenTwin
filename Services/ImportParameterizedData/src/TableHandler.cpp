#include "TableHandler.h"
#include "FileToTableExtractorFactory.h"

#include "TableStatisticAnalyser.h"

#include "ClassFactory.h"
#include "Application.h"
#include "EntityParameterizedDataTable.h"
#include <chrono>

TableHandler::TableHandler(const std::string tableFolder) : _tableFolder(tableFolder)
{}

void TableHandler::AddTableView(ot::UID sourceID, ot::UID sourceVersionID)
{	
	auto sourceFile = dynamic_cast<EntityFile*>(_modelComponent->readEntityFromEntityIDandVersion(sourceID, sourceVersionID, Application::instance()->getClassFactory()));
	if (sourceFile == nullptr)
	{
		assert(0); // Only EntityFile should reach here.
	}
	auto topoEnt = std::unique_ptr< EntityParameterizedDataTable>(new EntityParameterizedDataTable(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, &Application::instance()->getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	std::list<std::string> takenNames;
	std::string fullName = CreateNewUniqueTopologyNamePlainPossible(_tableFolder, sourceFile->getFileName(), takenNames);
	topoEnt->setName(fullName);
	std::string type = sourceFile->getFileType();
	auto tableExtractor = FileToTableExtractorFactory::GetInstance()->Create(sourceFile->getFileType());

	auto tp1 = std::chrono::system_clock::now();
	tableExtractor->ExtractFromEntitySource(sourceFile);

	auto tp2 = std::chrono::system_clock::now();
	
	//TableStatisticAnalyser analyser(tableExtractor.get());
	//int numberOfColumns = static_cast<int>(analyser.CountColumns());
	//int numberOfRows = static_cast<int>(analyser.CountRows());
	
	auto tableData = ExtractTableData(tableExtractor);

	auto tp3 = std::chrono::system_clock::now();


	tableData->StoreToDataBase();

	auto tp4 = std::chrono::system_clock::now();
	//_uiComponent->displayMessage("Total duration: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(tp4 - tp1).count()) + " mys\n");
	//_uiComponent->displayMessage("Extraction from source entity: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(tp2- tp1).count()) + " mys\n");
	//_uiComponent->displayMessage("Table data created: " +std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(tp3-tp2).count()) + " mys\n");
	//_uiComponent->displayMessage("Table data stored: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(tp4- tp3).count()) + " mys\n");


	topoEnt->setTableData(tableData);
	topoEnt->SetTableDimensions(static_cast<unsigned int> (tableData->getNumberOfRows()), static_cast<unsigned int> (tableData->getNumberOfColumns()));
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
	_uiComponent->displayMessage("Created table " + fullName + "\n");
}

std::shared_ptr<EntityResultTableData<std::string>> TableHandler::ExtractTableData(std::shared_ptr<TableExtractor> extractor)
{
	std::list<std::vector<std::string>> processedTableData;
	
	while (extractor->HasNextLine())
	{
		std::vector<std::string> line;
		extractor->GetNextLine(line);
		TrimTrailingControlCharacter(line);
		if (!IsEmptyLine(line))
		{
			processedTableData.emplace_back(line);
		}
	}
	
	auto tableData = std::make_shared<EntityResultTableData<std::string>>(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, &Application::instance()->getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	uint64_t numberOfColumns = 0;
	for (auto it = processedTableData.begin(); it != processedTableData.end();it++)
	{
		const uint64_t temp = it->size();
		if (temp > numberOfColumns)
		{
			numberOfColumns = temp;
		}
	}
	
	tableData->setNumberOfColumns(numberOfColumns);
	tableData->setNumberOfRows(processedTableData.size());
	unsigned int lineCounter(0);
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
