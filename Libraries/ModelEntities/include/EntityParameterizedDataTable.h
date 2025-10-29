// @otlicense

#pragma once

#include "EntityResultTable.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <map>
#include "OTGui/TableCfg.h"

class __declspec(dllexport) EntityParameterizedDataTable : public EntityResultTable<std::string>
{
public:
	EntityParameterizedDataTable() : EntityParameterizedDataTable(0, nullptr, nullptr, nullptr, "") {};
	EntityParameterizedDataTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, const std::string &owner);
	void SetTableDimensions(uint32_t numberOfRows, uint32_t numberOfColumns);
	void SetSourceFile(std::string sourceFileName, std::string sourceFilePath);
	virtual std::string getClassName(void) const override { return "EntityParameterizedDataTable"; };
	std::string getFileName() const { return _sourceFileName; }
	std::string getFilePath() const { return _sourceFilePath; }

	void createProperties(ot::TableCfg::TableHeaderMode _defaultHeaderMode);
	std::string getSelectedHeaderModeString();
	ot::TableCfg::TableHeaderMode getSelectedHeaderMode();
	
private:
	void createProperties() override {};
	uint32_t _numberOfColumns = 0;
	uint32_t _numberOfRows = 0;
	std::vector<uint32_t> _numberOfUniquesInColumns;
	std::string _sourceFileName;
	std::string _sourceFilePath;
	const std::string _defaulCategory = "tableInformation";

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;


};
