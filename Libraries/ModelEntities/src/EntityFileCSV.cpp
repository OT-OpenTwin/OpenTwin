#include "EntityFileCSV.h"

#include "CSVToTableTransformer.h"
#include "OTCommunication/ActionTypes.h"
#include "CSVProperties.h"
#include <string>


EntityFileCSV::EntityFileCSV(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
: EntityFileText(ID,parent,obs,ms,factory,owner){}

std::string EntityFileCSV::getRowDelimiter()
{
	auto rowDelim = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Row Delimiter"));
	m_rowDelimiter = rowDelim->getValue();
	return m_rowDelimiter;
}

std::string EntityFileCSV::getColumnDelimiter()
{
	auto delim = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Column Delimiter"));
	m_columnDelimiter = delim->getValue();
	return m_columnDelimiter;
}

ot::TableHeaderOrientation EntityFileCSV::getHeaderOrientation()
{
	auto selectedOrientation = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Header position"));
	if (selectedOrientation == nullptr)
	{
		throw std::exception("Legacy entity. Please reimport the file");
	}
	ot::TableHeaderOrientation orientation =ot::stringToHeaderOrientation(selectedOrientation->getValue());
	return orientation;
}

bool EntityFileCSV::visualiseText()
{
	return true;
}

char EntityFileCSV::getDecimalDelimiter()
{
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Decimal point character");
	if (baseProperty == nullptr)
	{
		return '.';
	}
	auto selection = dynamic_cast<EntityPropertiesSelection*>(baseProperty);

	const char separator = selection->getValue()[0];
	return separator;
}

void EntityFileCSV::setSpecializedProperties()
{
	EntityFileText::setSpecializedProperties();
	EntityPropertiesString::createProperty("CSV Properties", "Row Delimiter", m_rowDelimiter, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
	EntityPropertiesString::createProperty("CSV Properties", "Column Delimiter", m_columnDelimiter, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
	EntityPropertiesSelection::createProperty("Table header", "Header position", { ot::toString(ot::TableHeaderOrientation::horizontal), ot::toString(ot::TableHeaderOrientation::vertical) }, ot::toString(ot::TableHeaderOrientation::horizontal), "tableInformation", getProperties());
	std::locale mylocale("");
	auto defaulDecimalSeparator = std::use_facet<std::numpunct<char>>(mylocale).decimal_point();
	EntityPropertiesSelection::createProperty("Text Properties", "Decimal point character",
		{
			".",
			","
		}
		, std::string(1, defaulDecimalSeparator),
		"default", getProperties());

}

void EntityFileCSV::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityFile::AddStorageData(storage);
	storage.append(
		bsoncxx::builder::basic::kvp("RowDelimiter",m_rowDelimiter),
		bsoncxx::builder::basic::kvp("ColumnDelimiter",m_columnDelimiter)
	);
}

void EntityFileCSV::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityFile::readSpecificDataFromDataBase(doc_view, entityMap);
	m_rowDelimiter =std::string(doc_view["RowDelimiter"].get_utf8().value.data());
	m_columnDelimiter = std::string(doc_view["ColumnDelimiter"].get_utf8().value.data());
}

const ot::GenericDataStructMatrix EntityFileCSV::getTable()
{
	CSVProperties properties;

	properties.m_rowDelimiter = getRowDelimiter();
	properties.m_columnDelimiter = getColumnDelimiter();
	properties.m_decimalDelimiter = getDecimalDelimiter();
	const std::string text = getText();

	CSVToTableTransformer transformer;
	ot::GenericDataStructMatrix matrix = transformer(text, properties);
	
	return matrix;
}

void EntityFileCSV::setTable(const ot::GenericDataStructMatrix& _table)
{
	CSVProperties properties;

	properties.m_rowDelimiter = getRowDelimiter();
	properties.m_columnDelimiter = getColumnDelimiter();
	properties.m_decimalDelimiter = getDecimalDelimiter();

	CSVToTableTransformer transformer;

	const std::string csvTableText = transformer(_table, properties);
	setText(csvTableText);
}

ot::TableCfg EntityFileCSV::getTableConfig()
{
	ot::GenericDataStructMatrix matrix = getTable();
	ot::TableHeaderOrientation headerOrientation =	getHeaderOrientation();
	ot::TableCfg tableCfg(matrix, headerOrientation);
	tableCfg.setEntityName(getName());
	tableCfg.setTitle(getName());
	return tableCfg;
}

bool EntityFileCSV::visualiseTable()
{
	return true;
}

ot::ContentChangedHandling EntityFileCSV::getTableContentChangedHandling()
{
	return m_tableContentChangedHandling;
}
