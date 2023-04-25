#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"
#include "EntityResultTableData.h"

#include <memory>

template <class T>
class __declspec(dllexport) EntityResultTable : public EntityBase
{
public:
	EntityResultTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactory *factory, const std::string &owner);
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

protected:
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

private:
	virtual int getSchemaVersion(void) { return 1; };

	void EnsureTableDataLoaded(void);
	std::string className; 
	std::shared_ptr<EntityResultTableData<T>> tableData = nullptr;
	long long tableDataStorageId;
	long long tableDataStorageVersion;
};

#include "EntityResultTable.hpp"