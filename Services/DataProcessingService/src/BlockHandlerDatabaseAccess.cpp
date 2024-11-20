#include "BlockHandlerDatabaseAccess.h"

#include "Application.h"

#include "AdvancedQueryBuilder.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/StringToVariableConverter.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructSingle.h"

#include "ValueComparisionDefinition.h"

#include "ResultCollectionMetadataAccess.h"
#include "ProjectToCollectionConverter.h"
#include "DataBase.h"

#include "ResultDataStorageAPI.h"
#include "QuantityContainer.h"
#include "PropertyHandlerDatabaseAccessBlock.h"

#include <cctype>
#include <algorithm>

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity,handlerMap)
{
	//First get handler of the selected project result data.
	std::string collectionName;
	m_resultCollectionMetadataAccess = (PropertyHandlerDatabaseAccessBlock::getResultCollectionMetadataAccess(blockEntity,collectionName));
	const MetadataCampaign* campaign = &m_resultCollectionMetadataAccess->getMetadataCampaign();
	
	m_resultCollectionAccess = new DataStorageAPI::ResultDataStorageAPI(collectionName);
	
	if (!m_resultCollectionMetadataAccess->collectionHasMetadata())
	{
		const std::string errorMessage = "Database Access not possible. The selected collection has no meta data.";
		throw std::exception(errorMessage.c_str());
	}
	
	AdvancedQueryBuilder builder;

	//If a series is selected, we need to add a corresponding query.
	const MetadataSeries* series = nullptr;
	const std::string seriesLabel =	blockEntity->getSeriesSelection()->getValue();
	if (seriesLabel != "")
	{
		series = m_resultCollectionMetadataAccess->findMetadataSeries(seriesLabel);
		assert(series != nullptr);
		ot::UID valueUID = series->getSeriesIndex();
		ValueComparisionDefinition seriesComparision(MetadataSeries::getFieldName(), "=", std::to_string(valueUID), ot::TypeNames::getInt64TypeName(), "");
		AddComparision(seriesComparision);
	}

	//Next we add a query corresponding to the selected quantity. Its value is not projected.
	ValueComparisionDefinition quantityDef = blockEntity->getSelectedQuantityDefinition();
	const std::string& valueDescriptionLabel = blockEntity->getQuantityValueDescriptionSelection()->getValue();
	//The entity selection contains the names of the quantity/parameter. In the mongodb documents only the abbreviations are used.
	const auto selectedQuantity = m_resultCollectionMetadataAccess->findMetadataQuantity(quantityDef.getName());
	if (selectedQuantity == nullptr) 
	{
		throw std::exception("DatabaseAccessBlock has no quantity set.");
	}
		
	auto& valueDescriptions = selectedQuantity->valueDescriptions;
	ot::UID valueUID = 0;
	QueryDescription quantityQueryDescr;
	for (auto& valueDescription : valueDescriptions)
	{
		if (valueDescription.quantityValueLabel == valueDescriptionLabel)
		{
			valueUID = valueDescription.quantityIndex;
			quantityQueryDescr.m_outputData.m_quantityDescription = &valueDescription;
		}
	}
	assert(valueUID != 0);

	ValueComparisionDefinition selectedQuantityDef(MetadataQuantity::getFieldName(),"=",std::to_string(valueUID),ot::TypeNames::getInt64TypeName(),"");
	AddComparision(selectedQuantityDef);
	
	//Now we add a comparision for the searched quantity value.
	quantityDef.setName(QuantityContainer::getFieldName());
	quantityQueryDescr.m_projectionName = quantityDef.getName();
	quantityQueryDescr.m_outputData.m_quantity = selectedQuantity;
	quantityQueryDescr.m_outputData.m_series = series;
	quantityQueryDescr.m_outputData.m_campaign = campaign;
	const std::string quantityConnectorName = blockEntity->getConnectorQuantity().getConnectorName();
	quantityQueryDescr.m_connectorName = quantityConnectorName;
	m_queryDescriptions.push_back(quantityQueryDescr);

	AddComparision(quantityDef);

	//Next are the parameter. A 2D plot requires two variables, thus at least one parameter has to be defined.
	const std::string parameterConnectorName = blockEntity->getConnectorParameter1().getConnectorName();
	ValueComparisionDefinition param1Def = blockEntity->getSelectedParameter1Definition();
	const auto parameter1 = m_resultCollectionMetadataAccess->findMetadataParameter(param1Def.getName());
	if (parameter1 == nullptr)
	{
		throw std::exception("DatabaseAccessBlock has the parameter 1 not set.");
	}
	AddParameter(param1Def, *parameter1, parameterConnectorName);
	m_queryDescriptions.back().m_outputData.m_campaign = campaign;
	m_queryDescriptions.back().m_outputData.m_series = series;
	AddComparision(param1Def);


	//Depending on the entity setting, we are now adding additional parameter queries.
	const bool queryDimensionIs3D = blockEntity->isQueryDimension3D();
	const bool queryDimensionIs2D = blockEntity->isQueryDimension2D();

	if (queryDimensionIs2D || queryDimensionIs3D)
	{
		auto param2Def = blockEntity->getSelectedParameter2Definition();
		const std::string param2ConnectorName =	blockEntity->getConnectorParameter2().getConnectorName();
		const auto parameter = m_resultCollectionMetadataAccess->findMetadataParameter(param2Def.getName());
		if (parameter == nullptr)
		{
			throw std::exception("DatabaseAccessBlock has the parameter 2 not set.");
		}
		AddParameter(param2Def, *parameter, param2ConnectorName);
		m_queryDescriptions.back().m_outputData.m_campaign = campaign;
		m_queryDescriptions.back().m_outputData.m_series = series;
		AddComparision(param2Def);
	}
	if (queryDimensionIs3D)
	{	
		auto param3Def = blockEntity->getSelectedParameter3Definition();
		const std::string param3ConnectorName = blockEntity->getConnectorParameter3().getConnectorName();
		const auto parameter = m_resultCollectionMetadataAccess->findMetadataParameter(param3Def.getName());
		if (parameter == nullptr)
		{
			throw std::exception("DatabaseAccessBlock has the parameter 3 not set.");
		}
		AddParameter(param3Def, *parameter, param3ConnectorName);
		m_queryDescriptions.back().m_outputData.m_campaign = campaign;
		m_queryDescriptions.back().m_outputData.m_series = series;
		AddComparision(param3Def);
	}

	_query = builder.ConnectWithAND(std::move(_comparisons));

	std::vector<std::string> projectionNames;
	for (QueryDescription& queryDescription : m_queryDescriptions)
	{
		projectionNames.push_back(queryDescription.m_projectionName);
		queryDescription.m_outputData.m_campaign;
		queryDescription.m_outputData.m_series;
	}
	_projection = builder.GenerateSelectQuery(projectionNames, false);
}

BlockHandlerDatabaseAccess::~BlockHandlerDatabaseAccess()
{
	if (m_resultCollectionAccess != nullptr)
	{
		delete m_resultCollectionAccess;
		m_resultCollectionAccess = nullptr;
	}
	if (m_resultCollectionMetadataAccess != nullptr)
	{
		delete m_resultCollectionMetadataAccess;
		m_resultCollectionMetadataAccess = nullptr;
	}
}

bool BlockHandlerDatabaseAccess::executeSpecialized()
{
	const std::string debugQuery = bsoncxx::to_json(_query.view());
	const std::string debugProjection = bsoncxx::to_json(_projection.view());
	auto dbResponse = m_resultCollectionAccess->SearchInResultCollection(_query, _projection, 0);
	ot::JSONToVariableConverter converter;

	if (dbResponse.getSuccess())
	{
		const std::string queryResponse = dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		auto allEntries = ot::json::getArray(doc, "Documents");
		
		//We look through the returned documents
		const uint32_t numberOfDocuments = allEntries.Size();
		if (numberOfDocuments == 0)
		{
			throw std::exception("Query returned nothing.");
		}

		for (uint32_t i = 0; i < numberOfDocuments; i++)
		{
			auto projectedValues = ot::json::getObject(allEntries, i);

			//Now we extract each of the projected value
			for (QueryDescription& queryDescription : m_queryDescriptions)
			{
				const std::string& projectionName = queryDescription.m_projectionName;
				PipelineData& pipelineData = queryDescription.m_outputData;
				
				//The query does not contain one of the projected values.
				if (!projectedValues.HasMember(projectionName.c_str()))
				{
					std::string searchedField;
					if (pipelineData.m_parameter != nullptr)
					{
						searchedField = pipelineData.m_parameter->parameterLabel;
					}
					else
					{
						assert(pipelineData.m_quantity != nullptr);
						searchedField = pipelineData.m_quantity->quantityLabel;
					}

					const std::string message = "Database access failed. The searched field: "+ searchedField + " was not found in the result of the defined query";
					throw std::exception(message.c_str());
				}
				const MetadataParameter* parameter = pipelineData.m_parameter;
				const bool projectedValueIsQuantityValue = projectionName == QuantityContainer::getFieldName();
				

				if (projectedValueIsQuantityValue)
				{
					const MetadataQuantity* quantity = pipelineData.m_quantity;
					const auto& dimensions = quantity->dataDimensions;
					
					//Value is stored as a tensor
					if (dimensions.size() != 1)
					{
						if (dimensions.size() != 2)
						{
							const std::string errorMessage = "Extracted values of quantity " + quantity->quantityLabel + " have " + std::to_string(dimensions.size()) + " dimensions. Currently a maximum of 2 dimensions (matrices) are supported.";
							throw std::exception(errorMessage.c_str());
						}
						
						if (dimensions[0] != dimensions[1])
						{
							const std::string errorMessage = "Extracted values of quantity " + quantity->quantityLabel + " are of a matrix structure but of unequal dimensions, which is currently not supported.";
							throw std::exception(errorMessage.c_str());
						}

						ot::MatrixEntryPointer mantrixEntry;
						mantrixEntry.m_row = dimensions[0];
						mantrixEntry.m_column =dimensions[0];
						//Could be that the value array is smaller then the data array because of the query.
						auto jsValues = ot::json::getArray(projectedValues, projectionName.c_str());
						std::list<ot::Variable> values = converter(jsValues);
						
						ot::GenericDataStructMatrix* dataBlock(new ot::GenericDataStructMatrix(mantrixEntry));
						dataBlock->setValues(values);
						std::shared_ptr<ot::GenericDataStruct> dataBlockBase(dataBlock);
						_dataPerPort[queryDescription.m_connectorName].m_data.push_back(dataBlockBase);
					}
					else
					{
						uint32_t vectorDimension =	dimensions[0];
						
						//Value is stored as a single point
						if (vectorDimension == 1)
						{
							ot::Variable value = converter(projectedValues[projectionName.c_str()]);
							ot::GenericDataStructSingle* dataBlock(new ot::GenericDataStructSingle());
							dataBlock->setValue(value);
							std::shared_ptr<ot::GenericDataStruct>datablockBase(dataBlock);
							_dataPerPort[queryDescription.m_connectorName].m_data.push_back(datablockBase);
						}
						else
						{
							//Value is stored as a vector
							const std::string errorMessage = "Extracted values of quantity " + quantity->quantityLabel + " are of a vector structure, which is currently not supported.";
							throw std::exception(errorMessage.c_str());
						}
					}

				}
				else
				{
					//Projected value is a parameter value
					ot::Variable value = converter(projectedValues[projectionName.c_str()]);
					ot::GenericDataStructSingle* dataBlock(new ot::GenericDataStructSingle());
					dataBlock->setValue(value);
					std::shared_ptr<ot::GenericDataStruct>datablockBase(dataBlock);
					_dataPerPort[queryDescription.m_connectorName].m_data.push_back(datablockBase);
				}
			}
		}
	
		//Now we set the meta data pointer of the port pipeline data
		for (QueryDescription& queryDescription : m_queryDescriptions)
		{
			auto dataPortData =	_dataPerPort.find(queryDescription.m_connectorName);
			assert(dataPortData != _dataPerPort.end());
			dataPortData->second.m_campaign = queryDescription.m_outputData.m_campaign;
			dataPortData->second.m_series = queryDescription.m_outputData.m_series;
			dataPortData->second.m_parameter = queryDescription.m_outputData.m_parameter;
			dataPortData->second.m_quantity = queryDescription.m_outputData.m_quantity;
			dataPortData->second.m_quantityDescription = queryDescription.m_outputData.m_quantityDescription;
		}
}
	else
	{
		const std::string message = "Data base query with this response: " + dbResponse.getMessage();
		throw std::exception(message.c_str());
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

				ot::Variable vFirstValue(converter(firstValue,'.'));
				ot::Variable vSecondValue(converter(secondValue,'.'));

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
	definition.setName(std::to_string(parameter.parameterUID));
	
	QueryDescription parameterQueryDesc;
	parameterQueryDesc.m_connectorName = connectorName;
	parameterQueryDesc.m_projectionName = definition.getName();
	parameterQueryDesc.m_outputData.m_parameter = &parameter;
	
	m_queryDescriptions.push_back(parameterQueryDesc);
}