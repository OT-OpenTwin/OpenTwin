#include "EntityParameterizedDataSourceCSV.h"

#include <OpenTwinCommunication/ActionTypes.h>

EntityParameterizedDataSourceCSV::EntityParameterizedDataSourceCSV(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactory * factory, const std::string & owner)
: EntityFile(ID,parent,obs,ms,factory,owner){}

std::string EntityParameterizedDataSourceCSV::getRowDelimiter()
{
	auto rowDelim = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Row Delimiter"));
	_rowDelimiter = rowDelim->getValue();
	return _rowDelimiter;
}

std::string EntityParameterizedDataSourceCSV::getColumnDelimiter()
{
	auto delim = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Column Delimiter"));
	_columnDelimiter = delim->getValue();
	return _columnDelimiter;
}

void EntityParameterizedDataSourceCSV::setSpecializedProperties()
{
	EntityPropertiesString::createProperty("CSV Properties", "Row Delimiter", _rowDelimiter, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
	EntityPropertiesString::createProperty("CSV Properties", "Column Delimiter", _columnDelimiter, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
}

void EntityParameterizedDataSourceCSV::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityFile::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("RowDelimiter",_rowDelimiter),
		bsoncxx::builder::basic::kvp("ColumnDelimiter",_columnDelimiter)
	);
}

void EntityParameterizedDataSourceCSV::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityFile::readSpecificDataFromDataBase(doc_view, entityMap);
	_rowDelimiter = doc_view["RowDelimiter"].get_utf8().value.to_string();
	_columnDelimiter = doc_view["ColumnDelimiter"].get_utf8().value.to_string();
}
