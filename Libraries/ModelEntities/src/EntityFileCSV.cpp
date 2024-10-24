#include "EntityFileCSV.h"

#include "OTCommunication/ActionTypes.h"

EntityFileCSV::EntityFileCSV(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
: EntityFileText(ID,parent,obs,ms,factory,owner){}

std::string EntityFileCSV::getRowDelimiter()
{
	auto rowDelim = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Row Delimiter"));
	_rowDelimiter = rowDelim->getValue();
	return _rowDelimiter;
}

std::string EntityFileCSV::getColumnDelimiter()
{
	auto delim = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Column Delimiter"));
	_columnDelimiter = delim->getValue();
	return _columnDelimiter;
}

void EntityFileCSV::setSpecializedProperties()
{
	EntityFileText::setSpecializedProperties();
	EntityPropertiesString::createProperty("CSV Properties", "Row Delimiter", _rowDelimiter, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
	EntityPropertiesString::createProperty("CSV Properties", "Column Delimiter", _columnDelimiter, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
}

void EntityFileCSV::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityFile::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("RowDelimiter",_rowDelimiter),
		bsoncxx::builder::basic::kvp("ColumnDelimiter",_columnDelimiter)
	);
}

void EntityFileCSV::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityFile::readSpecificDataFromDataBase(doc_view, entityMap);
	_rowDelimiter =std::string(doc_view["RowDelimiter"].get_utf8().value.data());
	_columnDelimiter = std::string(doc_view["ColumnDelimiter"].get_utf8().value.data());
}
