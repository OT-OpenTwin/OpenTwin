#pragma once
#include "EntityBase.h"
#include "EntityParameterizedDataCategorization.h"
#include <vector>

class __declspec(dllexport) EntityTableSelectedRanges : public EntityBase
{
public:
	EntityTableSelectedRanges(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	virtual std::string getClassName(void) { return "EntityTableSelectedRanges"; };
	virtual entityType getEntityType() override { return TOPOLOGY; }
	virtual void addVisualizationNodes() override;
	virtual bool updateFromProperties(void) override;
	void createProperties(const std::string& pythonScriptFolder, ot::UID pythonScriptFolderID, const std::string& pythonScriptName, ot::UID pythonScriptID);
	void SetTableProperties(std::string tableName, ot::UID tableID, ot::UID tableVersion, std::string tableOrientation);

	std::string getSelectedType();
	const std::string getTableName() const { return _tableName; }
	const std::string getTableOrientation() const { return _tableOrientation; }
	void getSelectedRange(uint32_t& topRow, uint32_t& bottomRow, uint32_t & leftColumn, uint32_t & rightColumn);
	bool getConsiderForBatchprocessing();
	void setConsiderForBatchprocessing(bool considerForBatchprocessing);
	bool getPassOnScript();
	std::string getScriptName();
	void AddRange(uint32_t topCell, uint32_t buttomCell, uint32_t leftCell, uint32_t rightCell);

private:

	const std::string _propNameConsiderForBatchProcessing = "Consider for batching";
	const std::string _propNamePassOnScript= "Pass on script";
	const std::string _pythonScriptProperty = "Update script";
	uint32_t _topCells = 0;
	uint32_t _buttomCells = 0;
	uint32_t _leftCells = 0;
	uint32_t _rightCells = 0;

	std::string _tableName = "";
	std::string _tableOrientation = "";
	ot::UID _tableID = -1;
	ot::UID _tableVersion = -1;

	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
