#pragma once
#include "EntityBase.h"
#include "EntityParameterizedDataCategorization.h"
#include "EntityParameterizedDataTable.h"
#include <vector>

class __declspec(dllexport) EntityTableSelectedRanges : public EntityBase
{
public:
	EntityTableSelectedRanges(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	virtual std::string getClassName(void) { return "EntityTableSelectedRanges"; };
	virtual entityType getEntityType() override { return TOPOLOGY; }
	virtual void addVisualizationNodes() override;
	virtual bool updateFromProperties(void) override;
	void createProperties(const std::string& pythonScriptFolder, ot::UID pythonScriptFolderID, const std::string& pythonScriptName, ot::UID pythonScriptID, const std::string& _defaultType, bool selectEntireRow = false, bool selectEntireColumn = false);
	void SetTableProperties(std::string tableName, ot::UID tableID, std::string tableOrientation);

	std::string getSelectedType();
	const std::string getTableName();
	const std::string getTableOrientation();
	void getSelectedRange(uint32_t& topRow, uint32_t& bottomRow, uint32_t & leftColumn, uint32_t & rightColumn, std::shared_ptr<EntityParameterizedDataTable> referencedTable);
	void getSelectedRange(uint32_t& topRow, uint32_t& bottomRow, uint32_t & leftColumn, uint32_t & rightColumn);
	bool getConsiderForBatchprocessing();
	void setConsiderForBatchprocessing(bool considerForBatchprocessing);
	bool getSelectEntireRow();
	bool getSelectEntireColumn();

	bool getPassOnScript();
	std::string getScriptName();
	void SetRange(uint32_t topRow, uint32_t bottomRow, uint32_t leftColumn, uint32_t rightColumn);

private:

	ot::UID _tableID = 0;
	const std::string _propNameConsiderForBatchProcessing = "Consider for batching";
	const std::string _propNamePassOnScript= "Pass on script";
	const std::string _pythonScriptProperty = "Update script";

	bool UpdateVisibility();

	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
