// @otlicense
// File: EntityFileCSV.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "EntityFileCSV.h"

#include "CSVToTableTransformer.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/RuntimeTests.h"
#include "OTCommunication/ActionTypes.h"
#include "CSVProperties.h"
#include <string>
#include "PropertyHelper.h"
#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_ENTITYFILECSV_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_ENTITYFILECSV_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_ENTITYFILECSV_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_ENTITYFILECSV_PERFORMANCETEST_ENABLED==true
#define OT_TEST_ENTITYFILECSV_Interval(___testText) OT_TEST_Interval(ot_intern_entityfilecsv_lcl_performancetest, "EntityFileCSV", ___testText)
#else
#define OT_TEST_ENTITYFILECSV_Interval(___testText)
#endif

static EntityFactoryRegistrar<EntityFileCSV> registrar(EntityFileCSV::className());

EntityFileCSV::EntityFileCSV(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms)
: EntityFileText(ID, parent, obs, ms) {}

bool EntityFileCSV::updateFromProperties(void)
{
	assert(getProperties().anyPropertyNeedsUpdate());

	setModified();	
	bool requiresDataToBeFetched = false;
	requiresDataToBeFetched |= PropertyHelper::getStringProperty(this, "Row Delimiter", "CSV Properties")->needsUpdate();
	requiresDataToBeFetched |= PropertyHelper::getStringProperty(this, "Column Delimiter", "CSV Properties")->needsUpdate();
	requiresDataToBeFetched |= PropertyHelper::getBoolProperty(this, "Evaluate Escape Characters", "CSV Properties")->needsUpdate();
	requiresDataToBeFetched |= PropertyHelper::getSelectionProperty(this, "Header position", "Table header")->needsUpdate();

	//Needs to be executed after the csvFile properties are investigated. Otherwise the properties are already reset.
	bool refreshGrid =	EntityFileText::updateFromProperties();
	//If the text file requires a data update, the table also needs one, since it uses the same data
	requiresDataToBeFetched |= requiresDataUpdate();
	if (requiresDataToBeFetched)
	{

		ot::VisualisationCfg visualisationCfg;
		visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TABLE_Setup);
		visualisationCfg.setOverrideViewerContent(requiresDataToBeFetched);
		visualisationCfg.setAsActiveView(true);
		getObserver()->requestVisualisation(getEntityID(), visualisationCfg);
	}

	getProperties().forceResetUpdateForAllProperties();
	return refreshGrid;
}

void EntityFileCSV::setRowDelimiter(std::string _delimiter)
{
	auto baseProp = getProperties().getProperty("Row Delimiter");
	assert(baseProp != nullptr);
	auto delim = dynamic_cast<EntityPropertiesString*>(baseProp);
	delim->setValue(_delimiter);
	setModified();
}

void EntityFileCSV::setColumnDelimiter(std::string _delimiter)
{
	auto baseProp = getProperties().getProperty("Column Delimiter");
	assert(baseProp != nullptr);
	auto delim = dynamic_cast<EntityPropertiesString*>(baseProp);
	delim->setValue(_delimiter);
	setModified();
}

std::string EntityFileCSV::getRowDelimiter()
{
	auto baseProp =	getProperties().getProperty("Row Delimiter");
	if (baseProp == nullptr)
	{
		return m_rowDelimiterDefault;
	}
	auto rowDelim = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string rowDelimiter = rowDelim->getValue();
	if (rowDelimiter == "")
	{
		return m_rowDelimiterDefault;
	}
	else
	{
		return rowDelimiter;
	}
}

std::string EntityFileCSV::getColumnDelimiter()
{
	auto baseProp = getProperties().getProperty("Column Delimiter");
	if (baseProp == nullptr)
	{
		return m_columnDelimiterDefault;
	}
	auto delim = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string columnDelimiter = delim->getValue();
	if (columnDelimiter== "")
	{
		return m_columnDelimiterDefault;
	}
	else
	{
		return columnDelimiter;
	}
}

ot::TableCfg::TableHeaderMode EntityFileCSV::getHeaderMode() {
	auto selectedOrientation = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Header position"));
	if (selectedOrientation == nullptr) {
		throw std::exception("Legacy entity. Please reimport the file");
	}
	ot::TableCfg::TableHeaderMode mode = ot::TableCfg::stringToHeaderMode(selectedOrientation->getValue());
	return mode;
}

bool EntityFileCSV::visualiseText() {
	return true;
}

char EntityFileCSV::getDecimalDelimiter() {
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Decimal point character");
	if (baseProperty == nullptr)
	{
		return '.';
	}

	auto selection = dynamic_cast<EntityPropertiesSelection*>(baseProperty);

	const char separator = selection->getValue()[0];
	return separator;
}

bool EntityFileCSV::getEvaluateEscapeCharacter() {
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Evaluate Escape Characters");
	if (baseProperty == nullptr) {
		return false;
	}
	auto prop = dynamic_cast<EntityPropertiesBoolean*>(baseProperty);
	return prop->getValue();
}

void EntityFileCSV::setSpecializedProperties() {
	EntityFileText::setSpecializedProperties();
	EntityPropertiesString::createProperty("CSV Properties", "Row Delimiter", m_rowDelimiterDefault, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
	EntityPropertiesString::createProperty("CSV Properties", "Column Delimiter", m_columnDelimiterDefault, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());
	EntityPropertiesBoolean::createProperty("CSV Properties", "Evaluate Escape Characters", m_evaluateEscapeCharacterDefault, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties())->setToolTip("If enabled escape characters in the text will be evaluated (e.g. \"\\n\" in the text will result in a new line character)");
	EntityPropertiesSelection::createProperty("Table header", "Header position", {
		ot::TableCfg::toString(ot::TableCfg::TableHeaderMode::Horizontal),
		ot::TableCfg::toString(ot::TableCfg::TableHeaderMode::Vertical),
		ot::TableCfg::toString(ot::TableCfg::TableHeaderMode::NoHeader)
		},
		ot::TableCfg::toString(ot::TableCfg::TableHeaderMode::Horizontal), "tableInformation", getProperties())->setToolTip("Indicates how the table header is determined.");
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

void EntityFileCSV::addStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityFile::addStorageData(storage);
}

void EntityFileCSV::readSpecificDataFromDataBase(const bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityFile::readSpecificDataFromDataBase(doc_view, entityMap);
}

const ot::GenericDataStructMatrix EntityFileCSV::getTable()
{
	OT_TEST_ENTITYFILECSV_Interval("Get table");

	CSVProperties properties;

	properties.m_rowDelimiter = ot::String::evaluateEscapeCharacters(getRowDelimiter());
	properties.m_columnDelimiter = ot::String::evaluateEscapeCharacters(getColumnDelimiter());
	properties.m_decimalDelimiter = getDecimalDelimiter();
	properties.m_evaluateEscapeCharacters = getEvaluateEscapeCharacter();
	const std::string text = getText();

	CSVToTableTransformer transformer;
	ot::GenericDataStructMatrix matrix = transformer(text, properties);
	
	return matrix;
}

void EntityFileCSV::setTable(const ot::GenericDataStructMatrix& _table)
{
	OT_TEST_ENTITYFILECSV_Interval("Set table");

	CSVProperties properties;

	properties.m_rowDelimiter = ot::String::evaluateEscapeCharacters(getRowDelimiter());
	properties.m_columnDelimiter = ot::String::evaluateEscapeCharacters(getColumnDelimiter());
	properties.m_decimalDelimiter = getDecimalDelimiter();
	properties.m_evaluateEscapeCharacters = getEvaluateEscapeCharacter();

	CSVToTableTransformer transformer;

	const std::string csvTableText = transformer(_table, properties);
	setText(csvTableText);
}

ot::TableCfg EntityFileCSV::getTableConfig(bool _includeData)
{
	ot::TableCfg tableCfg;
	if (_includeData)
	{
		OT_TEST_ENTITYFILECSV_Interval("Get table config");
		ot::GenericDataStructMatrix matrix = getTable();
		ot::TableCfg::TableHeaderMode headerMode = getHeaderMode();
		tableCfg = ot::TableCfg(matrix, headerMode);
	}
	tableCfg.setEntityName(getName());
	tableCfg.setTitle(getName());
	return tableCfg;
}

bool EntityFileCSV::visualiseTable()
{
	return true;
}
