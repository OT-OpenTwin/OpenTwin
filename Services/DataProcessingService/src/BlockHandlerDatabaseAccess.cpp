#include "BlockHandlerDatabaseAccess.h"

#include "Application.h"

#include "AdvancedQueryBuilder.h"
#include "OTCore/String.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructSingle.h"

#include "OTCore/ValueComparisionDefinition.h"

#include "ResultDataStorageAPI.h"
#include "QuantityContainer.h"
#include "PropertyHandlerDatabaseAccessBlock.h"

#include "OTCore/ExplicitStringValueConverter.h"

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	: BlockHandler(blockEntity, handlerMap)
{
	//First get handler of the selected project result data.
	std::string collectionName;
	m_resultCollectionMetadataAccess = (PropertyHandlerDatabaseAccessBlock::getResultCollectionMetadataAccess(blockEntity,collectionName));

	if (!m_resultCollectionMetadataAccess->collectionHasMetadata())
	{
		const std::string errorMessage = "Database Access not possible. The selected collection has no meta data.";
		throw std::exception(errorMessage.c_str());
	}
	
	m_resultCollectionAccess = new DataStorageAPI::ResultDataStorageAPI(collectionName);
	
	buildQuery(blockEntity);
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
	OT_LOG_I("Executing Database Acccess Block: " + _blockName);
	const std::string debugQuery = bsoncxx::to_json(m_query.view());
	OT_LOG_I("Executing query: " + debugQuery);
	
	const std::string debugProjection = bsoncxx::to_json(m_projection.view());
	_uiComponent->displayMessage("Executing projection: " + debugProjection);

	auto dbResponse = m_resultCollectionAccess->SearchInResultCollection(m_query, m_projection, m_documentLimit);

	if (dbResponse.getSuccess())
	{
		const std::string queryResponse = dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		auto allMongoDocuments = ot::json::getArray(doc, "Documents");
		
		//We look through the returned documents
		const uint32_t numberOfDocuments = allMongoDocuments.Size();
		if (numberOfDocuments == 0)
		{
			throw std::exception("Query returned nothing.");
		}
		
		if (Application::instance()->uiComponent()) {
			Application::instance()->uiComponent()->displayMessage("Query returned " + std::to_string(numberOfDocuments) + " results.");
		}

		for (uint32_t i = 0; i < numberOfDocuments; i++)
		{
			auto singleMongoDocument = ot::json::getObject(allMongoDocuments, i);

			//Now we extract each of the projected value
			for (QueryDescription& queryDescription : m_queryDescriptions)
			{
				const std::string& projectionName = queryDescription.m_projectionName;

				//If the entry was not fixed (query for an equal value), then the value is not known and needs to be extracted.
				if (m_fixedParameter.find(projectionName) == m_fixedParameter.end())
				{
					//The query does not contain one of the projected values.
					if (!singleMongoDocument.HasMember(projectionName.c_str()))
					{
						const std::string message = ("Expected value was not returned with the queried documents: " + projectionName);
						throw std::exception(message.c_str());
					}

					const bool projectedValueIsQuantityValue = projectionName == QuantityContainer::getFieldName();
					if (projectedValueIsQuantityValue)
					{
						extractQuantity(queryDescription, singleMongoDocument);
					}
					else
					{
						extractParameter(queryDescription, singleMongoDocument);
					}
				}
			}
		}
	
	}
	else
	{
		const std::string message = "Data base query failed with this response: " + dbResponse.getMessage();
		throw std::exception(message.c_str());
	}
	return true;

}

void BlockHandlerDatabaseAccess::buildQuery(EntityBlockDatabaseAccess* _blockEntity)
{
	AdvancedQueryBuilder builder;
	const MetadataCampaign* campaign = &m_resultCollectionMetadataAccess->getMetadataCampaign();
	
	//If a series is selected, we need to add a corresponding query.
	const MetadataSeries* series = addSeriesQuery(_blockEntity);

	//Next we add a query corresponding to the selected quantity.
	addQuantityQuery(_blockEntity);

	//Next are the parameter. A 2D plot requires two variables, thus at least one parameter has to be defined.
	addParameterQueries(_blockEntity);

	m_query = builder.connectWithAND(std::move(m_comparisons));

	std::vector<std::string> projectionNamesForExclusion{ "SchemaVersion", "SchemaType"};
	for (QueryDescription& queryDescription : m_queryDescriptions)
	{
		//If a field is queried for an equal value, we can exclude it from the returned documents.
		if (m_fixedParameter.find(queryDescription.m_projectionName) != m_fixedParameter.end())
		{
			projectionNamesForExclusion.push_back(queryDescription.m_projectionName);
		}
		queryDescription.m_pipelineData.m_campaign = campaign;
		queryDescription.m_pipelineData.m_series = series;
		queryDescription.m_pipelineData.m_fixedParameter = m_fixedParameter;
		
		_dataPerPort[queryDescription.m_connectorName].m_campaign = campaign;
		_dataPerPort[queryDescription.m_connectorName].m_series = series;
		_dataPerPort[queryDescription.m_connectorName].m_quantity = queryDescription.m_pipelineData.m_quantity;
		_dataPerPort[queryDescription.m_connectorName].m_quantityDescription = queryDescription.m_pipelineData.m_quantityDescription;
		_dataPerPort[queryDescription.m_connectorName].m_parameter = queryDescription.m_pipelineData.m_parameter;
	}
	m_projection = builder.GenerateSelectQuery(projectionNamesForExclusion, false,false);

}

void BlockHandlerDatabaseAccess::addParameterQueries(EntityBlockDatabaseAccess* _blockEntity)
{
	const std::string parameterConnectorName = _blockEntity->getConnectorParameter1().getConnectorName();
	ValueComparisionDefinition param1Def = _blockEntity->getSelectedParameter1Definition();
	const auto parameter1 = m_resultCollectionMetadataAccess->findMetadataParameter(param1Def.getName());
	if (parameter1 == nullptr)
	{
		throw std::exception("DatabaseAccessBlock has the parameter 1 not set.");
	}
	addParameterQueryDescription(param1Def, *parameter1, parameterConnectorName);
	addComparision(param1Def);

	//Depending on the entity setting, we are now adding additional parameter queries.
	const bool queryDimensionIs3D = _blockEntity->isQueryDimension3D();
	const bool queryDimensionIs2D = _blockEntity->isQueryDimension2D();

	if (queryDimensionIs2D || queryDimensionIs3D)
	{
		auto param2Def = _blockEntity->getSelectedParameter2Definition();
		const std::string param2ConnectorName = _blockEntity->getConnectorParameter2().getConnectorName();
		const auto parameter = m_resultCollectionMetadataAccess->findMetadataParameter(param2Def.getName());
		if (parameter == nullptr)
		{
			throw std::exception("DatabaseAccessBlock has the parameter 2 not set.");
		}
		addParameterQueryDescription(param2Def, *parameter, param2ConnectorName);
		addComparision(param2Def);
	}
	if (queryDimensionIs3D)
	{
		auto param3Def = _blockEntity->getSelectedParameter3Definition();
		const std::string param3ConnectorName = _blockEntity->getConnectorParameter3().getConnectorName();
		const auto parameter = m_resultCollectionMetadataAccess->findMetadataParameter(param3Def.getName());
		if (parameter == nullptr)
		{
			throw std::exception("DatabaseAccessBlock has the parameter 3 not set.");
		}
		addParameterQueryDescription(param3Def, *parameter, param3ConnectorName);
		addComparision(param3Def);
	}
}

const MetadataSeries* BlockHandlerDatabaseAccess::addSeriesQuery(EntityBlockDatabaseAccess* _blockEntity)
{
	const MetadataSeries* series = nullptr;
	const std::string seriesLabel = _blockEntity->getSeriesSelection()->getValue();
	if (seriesLabel != "")
	{
		series = m_resultCollectionMetadataAccess->findMetadataSeries(seriesLabel);
		assert(series != nullptr);
		ot::UID valueUID = series->getSeriesIndex();
		ValueComparisionDefinition seriesComparision(MetadataSeries::getFieldName(), "=", std::to_string(valueUID), ot::TypeNames::getInt64TypeName(), "");
		addComparision(seriesComparision);
	}
	return  series;
}

void BlockHandlerDatabaseAccess::addQuantityQuery(EntityBlockDatabaseAccess* _blockEntity)
{
	ValueComparisionDefinition quantityDef = _blockEntity->getSelectedQuantityDefinition();
	if (quantityDef.getName() == "")
	{
		throw std::exception("DatabaseAccessBlock has no quantity set.");
	}

	const std::string& valueDescriptionLabel = _blockEntity->getQuantityValueDescriptionSelection()->getValue();
	//The entity selection contains the names of the quantity/parameter. In the mongodb documents only the abbreviations are used.
	const auto selectedQuantity = m_resultCollectionMetadataAccess->findMetadataQuantity(quantityDef.getName());
	if (selectedQuantity == nullptr)
	{
		throw std::exception("DatabaseAccessBlock has quantity set which is not part of the selected result collection.");
	}

	auto& valueDescriptions = selectedQuantity->valueDescriptions;
	ot::UID valueUID = 0;
	QueryDescription quantityQueryDescr;
	for (auto& valueDescription : valueDescriptions)
	{
		if (valueDescription.quantityValueLabel == valueDescriptionLabel)
		{
			valueUID = valueDescription.quantityIndex;
			quantityQueryDescr.m_pipelineData.m_quantityDescription = &valueDescription;
		}
	}
	assert(valueUID != 0);

	//Now we add the query for the quantity ID
	ValueComparisionDefinition selectedQuantityDef(MetadataQuantity::getFieldName(), "=", std::to_string(valueUID), ot::TypeNames::getInt64TypeName(), "");
	addComparision(selectedQuantityDef);

	//Now we add a comparision for the searched quantity value.
	quantityDef.setName(QuantityContainer::getFieldName());
	quantityQueryDescr.m_projectionName = quantityDef.getName();
	quantityQueryDescr.m_pipelineData.m_quantity = selectedQuantity;
	
	const std::string quantityConnectorName = _blockEntity->getConnectorQuantity().getConnectorName();
	quantityQueryDescr.m_connectorName = quantityConnectorName;
	m_queryDescriptions.push_back(quantityQueryDescr);
	addComparision(quantityDef);
}




void BlockHandlerDatabaseAccess::extractQuantity(QueryDescription& _queryDescription, ot::ConstJsonObject& _databaseDocument)
{
	const MetadataQuantity* quantity = _queryDescription.m_pipelineData.m_quantity;
	const auto& dimensions = quantity->dataDimensions;
	ot::JSONToVariableConverter converter; //Maybe better the type that is explicitly given by the metadata definition

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
		mantrixEntry.m_column = dimensions[0];
		
		//Could be that the value array is smaller then the data array because of the query.
		auto jsValues = ot::json::getArray(_databaseDocument, _queryDescription.m_projectionName.c_str());
		const std::string typeName = _queryDescription.m_pipelineData.m_quantityDescription->dataTypeName;
		std::list<ot::Variable> values;
		for (auto& jsValue : jsValues)
		{			
			values.push_back(ot::ExplicitStringValueConverter::setValueFromString(jsValue, typeName));
		}
		
		ot::GenericDataStructMatrix* dataBlock(new ot::GenericDataStructMatrix(mantrixEntry));
		dataBlock->setValues(values);
		
		PipelineDataDocument pipelineDocument;
		pipelineDocument.m_quantity = std::shared_ptr<ot::GenericDataStruct>(dataBlock);
		addNotFixedParameter(pipelineDocument, _databaseDocument, _queryDescription.m_pipelineData.m_series, _queryDescription.m_pipelineData.m_campaign);
		_dataPerPort[_queryDescription.m_connectorName].m_data.push_back(pipelineDocument);
	}
	else
	{
		uint32_t vectorDimension = dimensions[0];

		//Value is stored as a single point
		if (vectorDimension == 1)
		{
			ot::Variable value = converter(_databaseDocument[_queryDescription.m_projectionName.c_str()]);
			ot::GenericDataStructSingle* dataBlock(new ot::GenericDataStructSingle());
			dataBlock->setValue(value);
			
			PipelineDataDocument returnedDocument;
			returnedDocument.m_quantity = std::shared_ptr<ot::GenericDataStruct>(dataBlock);
			addNotFixedParameter(returnedDocument, _databaseDocument, _queryDescription.m_pipelineData.m_series, _queryDescription.m_pipelineData.m_campaign);
			_dataPerPort[_queryDescription.m_connectorName].m_data.push_back(returnedDocument);
		}
		else
		{
			//Value is stored as a vector
			const std::string errorMessage = "Extracted values of quantity " + quantity->quantityLabel + " are of a vector structure, which is currently not supported.";
			throw std::exception(errorMessage.c_str());
		}
	}

}

void BlockHandlerDatabaseAccess::addNotFixedParameter(PipelineDataDocument& _document, ot::ConstJsonObject& _databaseDocument, const MetadataSeries* _series, const MetadataCampaign* _campaign)
{
	//First we find all parameter that are not fixed and that should be part of the mongodb document.
	std::map<std::string, const MetadataParameter*> freeParameters;
	if (_series != nullptr)
	{
		const std::list<MetadataParameter>&metadataParameters = _series->getParameter();
		for (auto& parameter : metadataParameters)
		{
			const std::string projectionName = std::to_string(parameter.parameterUID);
			if (m_fixedParameter.find(projectionName) != m_fixedParameter.end())
			{
				freeParameters[projectionName]= &parameter;
			}
		}
	}
	else
	{
		assert(_campaign != nullptr);
		const auto& allParameter =	_campaign->getMetadataParameterByUID();
		for (auto& parameter : allParameter)
		{
			const std::string projectionName = std::to_string(parameter.first);
			if (m_fixedParameter.find(projectionName) != m_fixedParameter.end())
			{
				freeParameters[projectionName] = &parameter.second;
			}
		}
	}

	//now extract the values of the parameters
	for (const auto& freeParameter : freeParameters)
	{
		if (_databaseDocument.HasMember(freeParameter.first.c_str()))
		{
			auto& entry =	_databaseDocument[freeParameter.first.c_str()];
			_document.m_parameter[freeParameter.second->parameterLabel] = ot::ExplicitStringValueConverter::setValueFromString(entry, freeParameter.second->typeName);
		}
		else
		{
			std::string message = ("Expected value was not returned with the queried documents: " + freeParameter.second->parameterLabel);
			throw std::exception(message.c_str());
		}
	}
}

void BlockHandlerDatabaseAccess::extractParameter(QueryDescription& _queryDescription, ot::ConstJsonObject& _databaseDocument)
{
	assert(_queryDescription.m_pipelineData.m_parameter != nullptr);
	//Projected value is a parameter value
	auto& entry = _databaseDocument[_queryDescription.m_projectionName.c_str()];
	const std::string& typeName =	_queryDescription.m_pipelineData.m_parameter->typeName;
	
	PipelineDataDocument document;
	document.m_parameter[_queryDescription.m_projectionName] = ot::ExplicitStringValueConverter::setValueFromString(entry, typeName);
	_dataPerPort[_queryDescription.m_connectorName].m_data.push_back(document);
}

void BlockHandlerDatabaseAccess::addComparision(const ValueComparisionDefinition& _definition)
{
	const std::string& comparator = _definition.getComparator();
	AdvancedQueryBuilder builder;
	BsonViewOrValue query =	builder.createComparison(_definition);
	m_comparisons.push_back(query);
}

void BlockHandlerDatabaseAccess::addParameterQueryDescription(ValueComparisionDefinition& definition, const MetadataParameter& parameter, const std::string& connectorName)
{
	definition.setName(std::to_string(parameter.parameterUID));
	
	QueryDescription parameterQueryDesc;
	parameterQueryDesc.m_connectorName = connectorName;
	parameterQueryDesc.m_projectionName = definition.getName();
	parameterQueryDesc.m_pipelineData.m_parameter = &parameter;
	
	m_queryDescriptions.push_back(parameterQueryDesc);
}