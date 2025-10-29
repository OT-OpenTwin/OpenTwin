// @otlicense

#pragma once
#include "EntityBase.h"
#include "EntityParameterizedDataCategorization.h"
#include "EntityParameterizedDataTable.h"
#include "OTGui/TableRange.h"
#include <vector>

class __declspec(dllexport) EntityTableSelectedRanges : public EntityBase
{
public:
	EntityTableSelectedRanges() : EntityTableSelectedRanges(0, nullptr, nullptr, nullptr, "") {};
	EntityTableSelectedRanges(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	~EntityTableSelectedRanges() = default;
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	virtual std::string getClassName(void) const override { return "EntityTableSelectedRanges"; };
	virtual entityType getEntityType() const override { return TOPOLOGY; }
	virtual void addVisualizationNodes() override;
	virtual bool updateFromProperties(void) override;
	void createProperties(const std::string& pythonScriptFolder, ot::UID pythonScriptFolderID, const std::string& pythonScriptName, ot::UID pythonScriptID, const std::string& _defaultType);
	void setTableProperties(std::string tableName, ot::UID tableID, std::string tableOrientation);

	bool getConsiderForImport();
	void setConsiderForImport(bool _considerForImport);
	std::string getSelectedType();
	std::string getTableName();
	ot::TableCfg::TableHeaderMode getTableHeaderMode();
	ot::TableRange getSelectedRange();
	bool getConsiderForBatchprocessing();
	void setConsiderForBatchprocessing(bool considerForBatchprocessing);

	bool getSelectEntireColumn();
	bool getSelectEntireRow();

	void setSelectEntireColumn(bool _selectAll);
	void setSelectEntireRow(bool _selectAll);
	
	uint32_t getBatchingPriority();
	void setBatchingPriority(uint32_t _priority);

	bool getPassOnScript();
	void setPassOnScript(bool _passOn);

	std::string getScriptName();
	void setRange(const ot::TableRange& _range);

private:

	ot::UID _tableID = 0;
	const std::string _propNameConsiderForBatchProcessing = "Consider for batching";
	const std::string _propNamePassOnScript= "Pass on script";
	const std::string _pythonScriptProperty = "Update script";

	bool UpdateVisibility();

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
