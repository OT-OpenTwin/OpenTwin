#pragma once
#pragma warning(disable : 4251)


#include "EntityBase.h"
#include "Types.h"

#include <list>
#include <vector>

template <class T>
class __declspec(dllexport) EntityResultTableData : public EntityBase
{
public:
	EntityResultTableData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResultTableData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) { return className; };

	virtual entityType getEntityType(void) override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void setNumberOfColumns(uint64_t cols);
	void setNumberOfRows(uint64_t rows);

	void setHeaderText(unsigned int col, const std::string &text);
	void setValue(unsigned int row, unsigned int col, T value);

	void setRow(unsigned int row, std::vector<T>& value);
	void setColumn(unsigned int column, std::vector<T>& value);

	uint64_t getNumberOfRows(void);
	uint64_t getNumberOfColumns(void);
	std::string getHeaderText(unsigned int col);
	T getValue(unsigned int row, unsigned int col);

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	
	std::string className = "EntityResultTableData";
	std::vector<std::string> header;
	std::vector<std::vector<T>> data;
};

#include "EntityResultTableData.hpp"


