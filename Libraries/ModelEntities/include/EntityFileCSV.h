#pragma once
#include "EntityFileText.h"

class __declspec(dllexport) EntityFileCSV : public EntityFileText
{
public:
	EntityFileCSV(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	void setRowDelimiter(std::string delim) { _rowDelimiter = delim; };
	void setColumnDelimiter(std::string delim) { _columnDelimiter = delim; };
	std::string getRowDelimiter() ;
	std::string getColumnDelimiter();
	virtual std::string getClassName(void) override { return "EntityFileCSV"; };

private:
	void setSpecializedProperties() override;
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::string _rowDelimiter = "\n";
	std::string _columnDelimiter = ";";
};
