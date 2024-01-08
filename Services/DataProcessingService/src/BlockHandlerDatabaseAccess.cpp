#include "BlockHandlerDatabaseAccess.h"
#include "ResultCollectionHandler.h"

#include "Application.h"

#include "AdvancedQueryBuilder.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/StringToVariableConverter.h"

#include "ValueComparisionDefinition.h"
#include "PropertyHandlerDatabaseAccessBlock.h"


BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(handlerMap)
{
	//First check if the selected project has a corresponding result collection.
	const std::string projectName =	blockEntity->getSelectedProjectName();
	ResultCollectionHandler resultCollectionHandler;
	const std::string collectionName = resultCollectionHandler.getProjectCollection(projectName);
	const std::string resultCollectionName = collectionName + ".results";
	_isValid  = resultCollectionHandler.CollectionExists(resultCollectionName);

	
	if (_isValid)
	{
		const auto& buffer = PropertyHandlerDatabaseAccessBlock::instance().getBuffer(blockEntity->getEntityID());
		const auto& parameterByName = buffer.parameterByName;
		const auto& quantityByName = buffer.quantitiesByName;

		const std::string dbURL = "Projects";
		_dataStorageAccess = new DataStorageAPI::DocumentAccess(dbURL, resultCollectionName);
		
		AdvancedQueryBuilder builder;
		ot::StringToVariableConverter converter;
		_projectionNames.reserve(4);
		_connectorNames.reserve(4);
		
		//Now build a query depending on the selections made in the BlockEntity
		ValueComparisionDefinition quantityDef = blockEntity->getSelectedQuantityDefinition();
		//The entity selection contains the names of the quantity/parameter. In the mongodb documents only the abbreviations are used.
		const auto& selectedQuantity = quantityByName.find(quantityDef.getName())->second;
		quantityDef.setName(selectedQuantity.quantityAbbreviation);
		_projectionNames.push_back(quantityDef.getName());
		
		//The data pipeline works on the connector names. Thus we do here mapping from the document field name to the connector name.
		const std::string quantityConnectorName = blockEntity->getConnectorQuantity().getConnectorName();
		_connectorNames.push_back(quantityConnectorName);
		
		const std::string parameterConnectorName = blockEntity->getConnectorParameter1().getConnectorName();
		ValueComparisionDefinition param1Def = blockEntity->getSelectedParameter1Definition();
		AddParameter(param1Def, parameterByName.find(param1Def.getName())->second, parameterConnectorName);
		AddComparision(param1Def);

		const bool queryDimensionIs3D = blockEntity->isQueryDimension3D();
		const bool queryDimensionIs2D = blockEntity->isQueryDimension2D();

		if (queryDimensionIs2D|| queryDimensionIs3D)
		{
			auto param2Def = blockEntity->getSelectedParameter2Definition();
			const std::string param2ConnectorName =	blockEntity->getConnectorParameter2().getConnectorName();
			AddParameter(param2Def, parameterByName.find(param2Def.getName())->second, param2ConnectorName);
			AddComparision(param2Def);
		}
		if (queryDimensionIs3D)
		{	
			auto param3Def = blockEntity->getSelectedParameter3Definition();
			const std::string param3ConnectorName = blockEntity->getConnectorParameter3().getConnectorName();
			AddParameter(param3Def, parameterByName.find(param3Def.getName())->second, param3ConnectorName);
			AddComparision(param3Def);
		}
		_projectionNames.shrink_to_fit();
		_connectorNames.shrink_to_fit();
		_query = builder.ConnectWithAND(std::move(_comparisons));
		_projection = builder.GenerateSelectQuery(_projectionNames, false);
	}
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

	auto dbResponse = _dataStorageAccess->GetAllDocuments(_query, _projection, 0);
	ot::JSONToVariableConverter converter;

	if (dbResponse.getSuccess())
	{
		const std::string queryResponse =	dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		auto allEntries = doc["Documents"].GetArray();
		for (uint64_t i = 0; i< allEntries.Size();i++)
		{
			auto projectedValues = allEntries[i].GetArray();
			for (uint64_t j = 0; j < projectedValues.Size(); j++)
			{
				ot::Variable value = converter(projectedValues[j]);
				const std::string connectorName = _connectorNames[j];
				_dataPerPort[connectorName].push_back(value);
			}
		}
	}

	return true;
}


void BlockHandlerDatabaseAccess::AddComparision(const ValueComparisionDefinition& definition)
{
	AdvancedQueryBuilder builder;
	ot::StringToVariableConverter converter;
	const std::string& name = definition.getName();
	const std::string& valueStr = definition.getValue();
	const std::string& comparator = definition.getComparator();

	ot::Variable value = converter(valueStr);

	auto compare = builder.CreateComparison(comparator, value);
	_comparisons.push_back(builder.GenerateFilterQuery(name, std::move(compare)));
}

void BlockHandlerDatabaseAccess::AddParameter(ValueComparisionDefinition& definition, const MetadataParameter& parameter, const std::string& connectorName)
{
	definition.setName(parameter.parameterAbbreviation);
	_projectionNames.push_back(definition.getName());
	_connectorNames.push_back(connectorName);
}