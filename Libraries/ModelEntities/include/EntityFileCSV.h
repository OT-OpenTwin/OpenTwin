#pragma once
#include "EntityFileText.h"
#include "IVisualisationTable.h"
#include "OTGui/TableHeaderOrientation.h"
class __declspec(dllexport) EntityFileCSV : public EntityFileText, public IVisualisationTable
{
public:
	
	EntityFileCSV(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);

	void setRowDelimiter(std::string delim) { m_rowDelimiter = delim; };
	void setColumnDelimiter(std::string delim) { m_columnDelimiter = delim; };
	std::string getRowDelimiter() ;
	std::string getColumnDelimiter();
	virtual std::string getClassName(void) override { return "EntityFileCSV"; };

	// Inherited via IVisualisationTable
	const ot::GenericDataStructMatrix getTable() override;
	void setTable(const ot::GenericDataStructMatrix& _table) override;
	ot::TableCfg getTableConfig() override;
	bool visualiseTable() override;
	ot::ContentChangedHandling getTableContentChangedHandling() override;

	ot::TableHeaderOrientation getHeaderOrientation() override;
	bool visualiseText() override;
private:
	void setSpecializedProperties() override;
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::string m_rowDelimiter = "\n";
	std::string m_columnDelimiter = ";";

	const std::string m_headerSettingHorizontal = "first row";
	const std::string m_headerSettingVertical = "first column";

	ot::ContentChangedHandling m_tableContentChangedHandling = ot::ContentChangedHandling::ModelServiceSavesNotifyOwner;
};
