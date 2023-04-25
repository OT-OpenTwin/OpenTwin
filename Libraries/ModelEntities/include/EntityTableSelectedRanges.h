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
	void createProperties();
	void SetTableProperties(std::string tableName, ot::UID tableID, ot::UID tableVersion, std::string tableOrientation);

	std::string getSelectedType();
	const std::string getTableName() const { return _tableName; }
	const std::string getTableOrientation() const { return _tableOrientation; }
	void getSelectedRange(uint32_t& topRow, uint32_t& bottomRow, uint32_t & leftColumn, uint32_t & rightColumn);

	void AddRange(uint32_t topCell, uint32_t buttomCell, uint32_t leftCell, uint32_t rightCell);

private:
	uint32_t _topCells;
	uint32_t _buttomCells;
	uint32_t _leftCells;
	uint32_t _rightCells;

	std::string _tableName;
	std::string _tableOrientation;
	ot::UID _tableID;
	ot::UID _tableVersion;

	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
