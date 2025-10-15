#pragma once

#include "EntityFileText.h"
#include "IVisualisationTable.h"

class __declspec(dllexport) EntityFileCSV : public EntityFileText, public IVisualisationTable
{
public:
	EntityFileCSV() : EntityFileCSV(0, nullptr, nullptr, nullptr, "") {};
	EntityFileCSV(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	~EntityFileCSV() = default;
	void setRowDelimiter(std::string _delimiter); 
	void setColumnDelimiter(std::string _delimiter);
	std::string getRowDelimiter() ;
	std::string getColumnDelimiter();
	static std::string className() { return "EntityFileCSV"; };
	virtual std::string getClassName(void) override { return EntityFileCSV::className(); };

	// Inherited via IVisualisationTable
	const ot::GenericDataStructMatrix getTable() override;
	void setTable(const ot::GenericDataStructMatrix& _table) override;
	ot::TableCfg getTableConfig(bool _includeData) override;
	bool visualiseTable() override;
	virtual bool updateFromProperties(void) override;
	ot::ContentChangedHandling getTableContentChangedHandling() override;

	ot::TableCfg::TableHeaderMode getHeaderMode(void) override;
	bool visualiseText() override;
	char getDecimalDelimiter() override;
	bool getEvaluateEscapeCharacter(void);

private:
	void setSpecializedProperties() override;
	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::string m_rowDelimiterDefault = "\\n";
	std::string m_columnDelimiterDefault = ";";
	bool m_evaluateEscapeCharacterDefault = false;

	ot::ContentChangedHandling m_tableContentChangedHandling = ot::ContentChangedHandling::ModelServiceSaves;
};
