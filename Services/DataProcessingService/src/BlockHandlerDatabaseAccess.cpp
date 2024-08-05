#include "BlockHandlerDatabaseAccess.h"

#include "Application.h"

#include "AdvancedQueryBuilder.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/StringToVariableConverter.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructSingle.h"

#include "ValueComparisionDefinition.h"

#include "ResultMetadataAccess.h"
#include "ProjectToCollectionConverter.h"
#include "DataBase.h"
#include "BufferResultCollectionAccess.h"
#include "ResultDataStorageAPI.h"
#include "QuantityContainer.h"
#include <cctype>
#include <algorithm>

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity,handlerMap)
{
	//First get a handle of the selected project.
	std::shared_ptr<ResultMetadataAccess> resultCollectionAccess = BufferResultCollectionAccess::INSTANCE().getResultCollectionAccessMetadata(blockEntity);

	
	if (!resultCollectionAccess->collectionHasMetadata())
	{
		const std::string errorMessage = "Database Access not possible. The selected collection has no meta data.";
		throw std::exception(errorMessage.c_str());
	}
	const std::string dbURL = "Projects";
	_dataStorageAccess = new DataStorageAPI::ResultDataStorageAPI(dbURL, resultCollectionAccess->getCollectionName());
		
	AdvancedQueryBuilder builder;
	//ot::StringToVariableConverter converter;
	_projectionNames.reserve(4);
	_connectorNames.reserve(4);
		
	//Now build a query depending on the selections made in the BlockEntity
	//First the quantity is selected. Its value is not projected.
	ValueComparisionDefinition quantityDef = blockEntity->getSelectedQuantityDefinition();
	//The entity selection contains the names of the quantity/parameter. In the mongodb documents only the abbreviations are used.
	const auto selectedQuantity = resultCollectionAccess->findMetadataQuantity(quantityDef.getName());
	if (selectedQuantity == nullptr) 
	{
		throw std::exception("DatabaseAccessBlock has no quantity set.");
	}
	//_dataRows= selectedQuantity->dataRows;
	//_dataColumns = selectedQuantity->dataColumns;

	ValueComparisionDefinition selectedQuantityDef(MetadataQuantity::getFieldName(),"=",std::to_string(selectedQuantity->quantityIndex),ot::TypeNames::getInt64TypeName());
	AddComparision(selectedQuantityDef);
	
	//Now we add a comparision for the searched quantity value.
	quantityDef.setName(QuantityContainer::getFieldName());
	_projectionNames.push_back(quantityDef.getName());
	_projectionNameToClearName[quantityDef.getName()] = selectedQuantity->quantityName;
	
	AddComparision(quantityDef);

	//Next are the parameter. A 2D plot requires two variables, thus at least one parameter has to be defined.
	const std::string quantityConnectorName = blockEntity->getConnectorQuantity().getConnectorName();
	_connectorNames.push_back(quantityConnectorName);
		
	const std::string parameterConnectorName = blockEntity->getConnectorParameter1().getConnectorName();
	ValueComparisionDefinition param1Def = blockEntity->getSelectedParameter1Definition();
	const auto parameter1 =	resultCollectionAccess->findMetadataParameter(param1Def.getName());
	//_projectionNameToClearName[parameter1->parameterAbbreviation] = parameter1->parameterName;
	if (parameter1 == nullptr)
	{
		throw std::exception("DatabaseAccessBlock has the parameter 1 not set.");
	}
	AddParameter(param1Def, *parameter1, parameterConnectorName);
	AddComparision(param1Def);

	const bool queryDimensionIs3D = blockEntity->isQueryDimension3D();
	const bool queryDimensionIs2D = blockEntity->isQueryDimension2D();

	if (queryDimensionIs2D|| queryDimensionIs3D)
	{
		auto param2Def = blockEntity->getSelectedParameter2Definition();
		const std::string param2ConnectorName =	blockEntity->getConnectorParameter2().getConnectorName();
		const auto parameter = resultCollectionAccess->findMetadataParameter(param2Def.getName());
		//_projectionNameToClearName[parameter->parameterAbbreviation] = parameter->parameterName;
		if (parameter == nullptr)
		{
			throw std::exception("DatabaseAccessBlock has the parameter 2 not set.");
		}
		AddParameter(param2Def, *parameter, param2ConnectorName);
		AddComparision(param2Def);
	}
	if (queryDimensionIs3D)
	{	
		auto param3Def = blockEntity->getSelectedParameter3Definition();
		const std::string param3ConnectorName = blockEntity->getConnectorParameter3().getConnectorName();
		const auto parameter = resultCollectionAccess->findMetadataParameter(param3Def.getName());
		//_projectionNameToClearName[parameter->parameterAbbreviation] = parameter->parameterName;
		if (parameter == nullptr)
		{
			throw std::exception("DatabaseAccessBlock has the parameter 3 not set.");
		}
		AddParameter(param3Def, *parameter, param3ConnectorName);
		AddComparision(param3Def);
	}
	_projectionNames.shrink_to_fit();
	_connectorNames.shrink_to_fit();
	_query = builder.ConnectWithAND(std::move(_comparisons));
	_projection = builder.GenerateSelectQuery(_projectionNames, false);
}

BlockHandlerDatabaseAccess::~BlockHandlerDatabaseAccess()
{
	if (_dataStorageAccess != nullptr)
	{
		delete _dataStorageAccess;
		_dataStorageAccess = nullptr;
	}
}

bool BlockHandlerDatabaseAccess::executeSpecialized()
{
	const std::string debugQuery = bsoncxx::to_json(_query.view());
	const std::string debugProjection = bsoncxx::to_json(_projection.view());
	auto dbResponse = _dataStorageAccess->SearchInResultCollection(_query, _projection, 0);
	ot::JSONToVariableConverter converter;

	if (dbResponse.getSuccess())
	{
		const std::string queryResponse = dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		auto allEntries = ot::json::getArray(doc, "Documents");


		const uint32_t numberOfDocuments = allEntries.Size();
		for (uint32_t i = 0; i < numberOfDocuments; i++)
		{
			auto projectedValues = ot::json::getObject(allEntries, i);
			uint32_t count(0);

			for (std::string projectionName : _projectionNames)
			{
				if (!projectedValues.HasMember(projectionName.c_str()))
				{
					auto clearNameByProjectionName = _projectionNameToClearName.find(projectionName);
					assert(clearNameByProjectionName != _projectionNameToClearName.end());
					const std::string message = "Database access failed. The selected quantity is not depending on the parameter: "+ clearNameByProjectionName->second;
					throw std::exception(message.c_str());
				}

				if (projectionName == QuantityContainer::getFieldName() && (_dataRows != 1 || _dataColumns != 1))
				{
					const std::string connectorName = _connectorNames[count];

					count++;
					uint32_t rowCounter(0), columnCounter(0);
					//Could be that the value array is smaller then the data array because of the query.
					auto jsValues = ot::json::getArray(projectedValues, projectionName.c_str());
					std::list<ot::Variable> values = converter(jsValues);
					auto currentValue = values.begin();
					const uint32_t numberOfEntries = _dataColumns * _dataRows;
					ot::GenericDataStructMatrix* dataBlock(new ot::GenericDataStructMatrix(_dataColumns, _dataRows));
					for (uint32_t j = 0; j < numberOfEntries; j++)
					{
						dataBlock->setValue(columnCounter, rowCounter, *currentValue);
						currentValue++;
						if (columnCounter < _dataColumns - 1)
						{
							columnCounter++;
						}
						else
						{
							columnCounter = 0;
							rowCounter++;
						}
					}
					_dataPerPort[connectorName].push_back(std::shared_ptr<ot::GenericDataStruct>(dataBlock));
				}
				else
				{
					ot::Variable value = converter(projectedValues[projectionName.c_str()]);
					ot::GenericDataStructSingle* dataBlock(new ot::GenericDataStructSingle());
					dataBlock->setValue(value);
					const std::string connectorName = _connectorNames[count];
					_dataPerPort[connectorName].push_back(std::shared_ptr<ot::GenericDataStruct>(dataBlock));
					count++;
				}
			}
		}
	}

	return true;

}


void BlockHandlerDatabaseAccess::AddComparision(const ValueComparisionDefinition& definition)
{
	const std::string& comparator = definition.getComparator();
	AdvancedQueryBuilder builder;
	ot::StringToVariableConverter converter;
	
	if (comparator == BlockEntityHandler::getQueryForRangeSelection())
	{
		const std::string& name = definition.getName();
		std::string valueStr = definition.getValue();
		const std::string& type = definition.getType();
		valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ' '), valueStr.end());
		char openingBracket = valueStr[0];
		char closingBracket = valueStr[valueStr.size()-1];
		size_t posDelimiter = valueStr.find(",");
		const bool notationIsCorrect = (openingBracket == '(' || openingBracket == '[') && (closingBracket == ')' || closingBracket == ']') && posDelimiter != std::string::npos;
		if (notationIsCorrect)
		{

			const bool dataTypeCompatible = type == ot::TypeNames::getInt32TypeName() || type == ot::TypeNames::getInt64TypeName() || type == ot::TypeNames::getDoubleTypeName() || type == ot::TypeNames::getFloatTypeName();
			if (dataTypeCompatible)
			{
				const std::string firstValue = valueStr.substr(1, posDelimiter - 1);
				const std::string secondValue = valueStr.substr(posDelimiter + 1, valueStr.size()-1);

				ot::Variable vFirstValue(converter(firstValue));
				ot::Variable vSecondValue(converter(secondValue));

				std::string correspondingComparator;
				if (openingBracket == '(')
				{
					correspondingComparator = ">";
				}
				else
				{
					correspondingComparator = ">=";
				}
				
				auto firstCompare = builder.CreateComparison(correspondingComparator, vFirstValue);

				if (closingBracket == ')')
				{
					correspondingComparator = "<";
				}
				else
				{
					correspondingComparator = "<=";
				}

				auto secondCompare = builder.CreateComparison(correspondingComparator, vSecondValue);
				auto intervalCompare = builder.ConnectWithAND({ firstCompare,secondCompare });
				_comparisons.push_back(builder.GenerateFilterQuery(name, std::move(intervalCompare)));

			}
			else
			{
				throw std::invalid_argument("Query for interval incorrect. The datatype of the selected field helds no numerical value.");
			}

			
		}
		else
		{
			throw std::invalid_argument("Query for interval incorrect. The interval to follow the english notation, e.g.: (2,3.5].");
		}

	}
	else if (comparator != " ")
	{
		const std::string& name = definition.getName();
		const std::string& valueStr = definition.getValue();
		const std::string& type = definition.getType();


		std::unique_ptr<ot::Variable> value;
		if (type == ot::TypeNames::getBoolTypeName())
		{
			std::string boolString = valueStr;
			std::transform(boolString.begin(), boolString.end(), boolString.begin(),
				[](unsigned char c) { return std::tolower(c); });
			if (boolString == "true")
			{
				value.reset(new ot::Variable(true));
			}
			else if (boolString == "false")
			{
				value.reset(new ot::Variable(false));
			}
			else
			{
				throw std::invalid_argument("Boolean value expected (true or false).");
			}
		}
		else if (type == ot::TypeNames::getStringTypeName())
		{
			value.reset(new ot::Variable(valueStr));
		}
		else if (type == ot::TypeNames::getDoubleTypeName())
		{
			value.reset(new ot::Variable(std::stod(valueStr)));
		}
		else if (type == ot::TypeNames::getFloatTypeName())
		{
			value.reset(new ot::Variable(std::stof(valueStr)));
		}
		else if (type == ot::TypeNames::getInt32TypeName())
		{
			value.reset(new ot::Variable(std::stoi(valueStr)));
		}
		else if (type == ot::TypeNames::getInt64TypeName())
		{
			value.reset(new ot::Variable(std::stoll(valueStr)));
		}

		auto compare = builder.CreateComparison(comparator, *value);
		_comparisons.push_back(builder.GenerateFilterQuery(name, std::move(compare)));
	}
	else
	{
		return;
	}
}

void BlockHandlerDatabaseAccess::AddParameter(ValueComparisionDefinition& definition, const MetadataParameter& parameter, const std::string& connectorName)
{
	definition.setName(std::to_string( parameter.parameterUID));
	_projectionNames.push_back(definition.getName());
	_connectorNames.push_back(connectorName);
}