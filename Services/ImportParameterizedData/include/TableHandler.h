/*****************************************************************//**
 * \file   TableHandler.h
 * \brief  Central class for dealing with table views.
 * 
 * \author Wagner
 * \date   April 2023
 *********************************************************************/
#pragma once
#include <memory>
#include <string>

#include "BusinessLogicHandler.h"
#include "EntityResultTableData.h"
#include "TableExtractor.h"

class TableHandler : public BusinessLogicHandler
{
public:
	TableHandler(const std::string tableFolder);
	/**
	 * Creates a table from a given source file.
	 * 
	 * \param sourceID
	 * \param sourceVersionID
	 */
	void AddTableView(ot::UID sourceID, ot::UID sourceVersionID);

private:
	const std::string _tableFolder;
	/**
	 * Creates the tabledata, depending on a sourcefile specific table extractor.
	 * 
	 * \param extractor
	 * \param numberOfRows
	 * \param numberOfColumns
	 * \return Tabledata
	 */
	std::shared_ptr<EntityResultTableData<std::string>>	ExtractTableData(std::shared_ptr<TableExtractor> extractor);
	void TrimTrailingControlCharacter(std::vector<std::string>& line);
	bool IsEmptyLine(std::vector<std::string>& line);
};
