#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"
#include "EntityResultTableData.h"

#include <memory>

template <class T>
class __declspec(dllexport) EntityResultTable : public EntityBase
{
public:
	EntityResultTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResultTable();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;
	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };
	virtual void StoreToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	void addVisualizationItem(bool isHidden);
	virtual std::string getClassName(void) { return className; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual void createProperties(void);
	virtual bool updateFromProperties(void) override;

	void deleteTableData(void);
	void releaseTableData(void);
	long long getTableDataStorageId(void) { return tableDataStorageId; }
	long long getTableDataStorageVersion(void) { return tableDataStorageVersion; }
	std::shared_ptr<EntityResultTableData<T>> getTableData(void);
	void setTableData(std::shared_ptr<EntityResultTableData<T>> tableData);

	int getMinColumn();
	int getMinRow();
	int getMaxColumn();
	int getMaxRow();

	void setMinColumn(int minCol);
	void setMinRow(int minRow);
	void setMaxColumn(int maxCol);
	void setMaxRow(int maxRow);

protected:
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	int _minCol = 0;
	int _minRow = 1;

private:
	int _maxCol = 50;
	int _maxRow = 50;
	virtual int getSchemaVersion(void) { return 1; };
	void EnsureTableDataLoaded(void);
	std::string className; 
	std::shared_ptr<EntityResultTableData<T>> tableData = nullptr;
	long long tableDataStorageId;
	long long tableDataStorageVersion;
};

#include "EntityResultTable.hpp"