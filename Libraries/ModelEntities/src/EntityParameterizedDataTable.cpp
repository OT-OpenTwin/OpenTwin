#include "EntityParameterizedDataTable.h"
#include <bsoncxx/builder/basic/array.hpp>

EntityParameterizedDataTable::EntityParameterizedDataTable(ot::UID ID, EntityBase * parent, EntityObserver * mdl, ModelState * ms, ClassFactory * factory, const std::string & owner)
: EntityResultTable<std::string>(ID,parent,mdl,ms,factory,owner)
{
}

void EntityParameterizedDataTable::SetTableDimensions(uint32_t numberOfRows, uint32_t numberOfColumns)
{
	_numberOfRows = numberOfRows;
	_numberOfColumns = numberOfColumns;
}

void EntityParameterizedDataTable::SetSourceFile(std::string sourceFileName, std::string sourceFilePath)
{
	_sourceFileName = sourceFileName;
	_sourceFilePath = sourceFilePath;
}

void EntityParameterizedDataTable::createProperties(HeaderOrientation defaultOrientation)
{
	auto numberOfRowsProperty = new EntityPropertiesInteger("Number of rows", _numberOfRows);
	numberOfRowsProperty->setReadOnly(true);
	getProperties().createProperty(numberOfRowsProperty,"Table properties");

	auto numberOfColumnsProperty = new EntityPropertiesInteger("Number of columns", _numberOfColumns);
	numberOfColumnsProperty->setReadOnly(true);
	getProperties().createProperty(numberOfColumnsProperty, "Table properties");
	
	auto fileNameProperty = new EntityPropertiesString("File name", _sourceFileName);
	fileNameProperty->setReadOnly(true);
	getProperties().createProperty(fileNameProperty, "Source file");

	auto filePathProperty = new EntityPropertiesString("File path", _sourceFilePath);
	filePathProperty->setReadOnly(true);
	getProperties().createProperty(filePathProperty, "Source file");

	EntityPropertiesSelection::createProperty("Table header", "Header position", { _headerSettingHorizontal,_headerSettingVertical }, GetHeaderOrientation(defaultOrientation) , _defaulCategory, getProperties());

	EntityResultTable<std::string>::createProperties();
}

std::string EntityParameterizedDataTable::getSelectedHeaderOrientationString()
{
	auto selectedOrientation = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Header position"));
	return selectedOrientation->getValue();
}

EntityParameterizedDataTable::HeaderOrientation EntityParameterizedDataTable::getSelectedHeaderOrientation()
{
	auto selectedOrientation = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Header position"));
	if (selectedOrientation->getValue() == _headerSettingHorizontal)
	{
		return HeaderOrientation::horizontal;
	}
	else
	{
		return HeaderOrientation::vertical;
	}

}

void EntityParameterizedDataTable::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityResultTable::AddStorageData(storage);

	auto dataArray = bsoncxx::builder::basic::array();

	for (int i = 0; i < _numberOfUniquesInColumns.size(); i++)
	{
		dataArray.append(static_cast<int32_t>(_numberOfUniquesInColumns[i]));
	}
	
	storage.append(
		bsoncxx::builder::basic::kvp("NumberOfRows",static_cast<int32_t>(_numberOfRows)),
		bsoncxx::builder::basic::kvp("NumberOfColumns", static_cast<int32_t>(_numberOfColumns)),
		bsoncxx::builder::basic::kvp("UniquesPerColumn", dataArray),
		bsoncxx::builder::basic::kvp("SourceFileName",_sourceFileName),
		bsoncxx::builder::basic::kvp("SourceFilePath",_sourceFilePath)
	);
}

void EntityParameterizedDataTable::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityResultTable::readSpecificDataFromDataBase(doc_view, entityMap);
	_numberOfRows = static_cast<uint32_t>(doc_view["NumberOfRows"].get_int32());
	_numberOfColumns = static_cast<uint32_t>(doc_view["NumberOfColumns"].get_int32());
	_sourceFileName = doc_view["SourceFileName"].get_utf8().value.data();
	_sourceFilePath = doc_view["SourceFilePath"].get_utf8().value.data();

	auto dataArray = doc_view["UniquesPerColumn"].get_array().value;

	_numberOfUniquesInColumns.clear();
	_numberOfUniquesInColumns.reserve(dataArray.length());

	for (auto data : dataArray)
	{
		_numberOfUniquesInColumns.push_back(static_cast<uint32_t>(data.get_int32()));
	}
}

std::string EntityParameterizedDataTable::GetHeaderOrientation(HeaderOrientation orientation)
{
	std::string temp = EntityParameterizedDataTable::_orientationToString[orientation];
	return temp;
}

const std::string EntityParameterizedDataTable::_headerSettingHorizontal = "first row";
const std::string EntityParameterizedDataTable::_headerSettingVertical = "first column";

std::map<EntityParameterizedDataTable::HeaderOrientation, const std::string> EntityParameterizedDataTable::_orientationToString{ 
	{HeaderOrientation::horizontal, EntityParameterizedDataTable::_headerSettingHorizontal}, 
	{HeaderOrientation::vertical, EntityParameterizedDataTable::_headerSettingVertical}
};