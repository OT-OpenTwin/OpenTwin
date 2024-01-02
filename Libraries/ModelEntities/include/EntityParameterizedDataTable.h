#pragma once

#include "EntityResultTable.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <map>

class __declspec(dllexport) EntityParameterizedDataTable : public EntityResultTable<std::string>
{
public:
	enum HeaderOrientation {horizontal,vertical};
	EntityParameterizedDataTable(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	void SetTableDimensions(uint32_t numberOfRows, uint32_t numberOfColumns);
	void SetSourceFile(std::string sourceFileName, std::string sourceFilePath);
	virtual std::string getClassName(void) { return "EntityParameterizedDataTable"; };
	std::string getFileName() const { return _sourceFileName; }
	std::string getFilePath() const { return _sourceFilePath; }

	void createProperties(HeaderOrientation defaultOrientation);
	std::string getSelectedHeaderOrientationString();
	HeaderOrientation getSelectedHeaderOrientation();
	char getSelectedDecimalSeparator();
	static std::string GetHeaderOrientation(HeaderOrientation orientation);

private:
	void createProperties() override {};
	uint32_t _numberOfColumns;
	uint32_t _numberOfRows;
	std::vector<uint32_t> _numberOfUniquesInColumns;
	std::string _sourceFileName;
	std::string _sourceFilePath;

	static const std::string _headerSettingHorizontal;
	static const std::string _headerSettingVertical;
	const std::string _defaulCategory = "tableInformation";

	static std::map<HeaderOrientation, const std::string> _orientationToString;

	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;


};
