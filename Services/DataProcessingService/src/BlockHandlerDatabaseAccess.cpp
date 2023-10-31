#include "BlockHandlerDatabaseAccess.h"
#include "ResultCollectionHandler.h"
#include "MeasurementCampaign.h"
#include "Application.h"
//#include "OpenTwinCore/JSONToVariableConverter.h"
//#include "PropertyHandlerDatabaseAccessBlock.h"

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(handlerMap)
{
	const std::string projectName =	blockEntity->getSelectedProjectName();

	ResultCollectionHandler resultCollectionHandler;
	std::string resultCollectionName = resultCollectionHandler.getProjectCollection(projectName) + ".results";
	_isValid  = resultCollectionHandler.CollectionExists(resultCollectionName);
	if (_isValid)
	{
		_quantityConnectorName = blockEntity->getConnectorQuantity().getConnectorName();
		_parameterConnectorName =  blockEntity->getConnectorParameter1().getConnectorName();
		
		const std::string dbURL = "Projects";
		_dataStorageAccess = new DataStorageAPI::DocumentAccess(dbURL, resultCollectionName);
		
		OT_rJSON_createDOC(query);
		OT_rJSON_createDOC(projection);
		
		ot::rJSON::add(projection, "_id", 0);
	
		const std::string sessionServiceURL = Application::instance()->sessionServiceURL();
		
		auto modelServiceInfo = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_MODEL);
		const std::string& modelServiceURL = modelServiceInfo->serviceURL();
		const MeasurementCampaign measurementCampaign =	getMeasurementCampaign(blockEntity, sessionServiceURL, modelServiceURL);
		const std::map<std::string,MetadataParameter> allParameter = measurementCampaign.getMetadataParameterByName();
		const std::map <std::string, MetadataQuantity> allQuantities = measurementCampaign.getMetadataQuantitiesByName();
		
		const std::string selectedParameter1Name = blockEntity->getSelectedParameter1Name();
		const std::string selectedQuantityName = blockEntity->getSelectedQuantityName();
		
		for (auto& parameter : allParameter)
		{
			if (parameter.second.parameterName == selectedParameter1Name)
			{
				const std::string selectedParameter1Abbreviation = parameter.second.parameterAbbreviation;
				ot::rJSON::add(projection, selectedParameter1Abbreviation, 1);

				const std::string selectedParam1Value =	blockEntity->getParameter1QueryValue();
				const std::string parameterTypeName = parameter.second.typeName;
				std::unique_ptr<ot::Variable>selectedParam1GenericValue (nullptr);
				if (parameterTypeName == ot::TypeNames::getInt64TypeName())
				{
					selectedParam1GenericValue.reset(new ot::Variable(std::stoll(selectedParam1Value)));				
				}
				else if (parameterTypeName == ot::TypeNames::getInt32TypeName())
				{
					selectedParam1GenericValue.reset(new ot::Variable(std::stoi(selectedParam1Value)));
				}
				else if (parameterTypeName == ot::TypeNames::getDoubleTypeName())
				{
					selectedParam1GenericValue.reset(new ot::Variable(std::stod(selectedParam1Value)));
				}
				else if (parameterTypeName == ot::TypeNames::getFloatTypeName())
				{
					selectedParam1GenericValue.reset(new ot::Variable(std::stof(selectedParam1Value)));
				}
				else if (parameterTypeName == ot::TypeNames::getBoolTypeName())
				{
					if (selectedParam1Value == "1" || selectedParam1Value == "true" || selectedParam1Value == "True" || selectedParam1Value == "TRUE")
					{
						selectedParam1GenericValue.reset(new ot::Variable(true));
					}
					else if (selectedParam1Value == "0" || selectedParam1Value == "False" || selectedParam1Value == "false" || selectedParam1Value == "FALSE")
					{
						selectedParam1GenericValue.reset(new ot::Variable(false));
					}
					else
					{
						std::exception("Parameter value cannot be recognised as boolean");
					}
				}
				else if (parameterTypeName == ot::TypeNames::getCharTypeName() || parameterTypeName == ot::TypeNames::getStringTypeName())
				{
					selectedParam1GenericValue.reset(new ot::Variable(selectedParam1Value));
				}
				else
				{
					assert(0);
				}
				
				//Needs some redesign of data model!!
				const genericDataBlock& allParameterValues = parameter.second.values;
				for (const ot::Variable& parameterValue : allParameterValues)
				{
					assert(parameterTypeName == parameterValue.getTypeName());
					if (parameterValue == *selectedParam1GenericValue)
					{
						if (parameterValue.isBool())
						{
							ot::rJSON::add(query, selectedParameter1Abbreviation, parameterValue.getBool());
						}
						else if (parameterValue.isConstCharPtr())
						{
							ot::rJSON::add(query, selectedParameter1Abbreviation, parameterValue.getConstCharPtr());
						}
						else if (parameterValue.isDouble())
						{
							ot::rJSON::add(query, selectedParameter1Abbreviation, parameterValue.getDouble());
						}
						else if (parameterValue.isFloat())
						{
							ot::rJSON::add(query, selectedParameter1Abbreviation, parameterValue.getFloat());
						}
						else if (parameterValue.isInt32())
						{
							ot::rJSON::add(query, selectedParameter1Abbreviation, parameterValue.getInt32());
						}
						else if (parameterValue.isInt64())
						{
							ot::rJSON::add(query, selectedParameter1Abbreviation, parameterValue.getInt64());
						}
						else
						{
							assert(0);
						}
						break;
					}
					
				}

				break;
			}
		}
		 
		for (auto& quantity: allQuantities)
		{
			if (quantity.second.quantityName== selectedQuantityName)
			{
				const std::string documentFieldName = MetadataQuantity::getFieldName();
				ot::rJSON::add(projection, documentFieldName, 1);
				
				const int32_t seklectedQuantityIndex = quantity.second.quantityIndex;
				ot::rJSON::add(query, documentFieldName, seklectedQuantityIndex);
				break;
			}
		}

		
		
		//_collectionInfos =	&PropertyHandlerDatabaseAccessBlock::instance().getBuffer(blockEntity->getEntityID());
		//auto selectedQuantity = _collectionInfos->quantities[_collectionInfos->SelectedQuantity];
		//
		//ot::rJSON::add(query, "Quantity", selectedQuantity.quantityIndex);
		//_queryString = ot::rJSON::toJSON(query);
		//_projectionString = ot::rJSON::toJSON(projection);
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
	genericDataBlock quantity{0,2,4,8,16,32,64,128,256,512,1024};
	genericDataBlock parameter{ 0,1,2,3,4,5,6,7,8,9,10 };

	_dataPerPort[_quantityConnectorName] = quantity;
	_dataPerPort[_parameterConnectorName] = parameter;

	return true;
}

const MeasurementCampaign BlockHandlerDatabaseAccess::getMeasurementCampaign(EntityBlockDatabaseAccess* dbAccessEntity, const std::string& sessionServiceURL, const std::string& modelServiceURL)
{
	return MeasurementCampaign();
}

//BlockHandler::genericDataBlock BlockHandlerDatabaseAccess::Execute(BlockHandler::genericDataBlock& inputData)
//{
//	if (_output.size() == 0)
//	{
//		auto selectedParameter = _collectionInfos->parameters[_collectionInfos->SelectedParameter1];
//		auto& temp = selectedParameter.values;
//		std::vector<ot::Variable> parameterValues(temp.begin(), temp.end());
//
//		auto dbResponse = _dataStorageAccess->GetAllDocuments(_queryString, _projectionString, 0);
//		bool success = dbResponse.getSuccess();
//
//		auto resultDoc = ot::rJSON::fromJSON(dbResponse.getResult());
//		auto allEntries = resultDoc["Documents"].GetArray();
//
//		BlockHandler::genericDataBlock result;
//		ot::JSONToVariableConverter converter;
//
//		for (uint32_t i = 0; i < allEntries.Size(); i++)
//		{
//			if (!allEntries[i].IsNull())
//			{
//				auto arrayEntry = allEntries[i].GetObject();
//				result[_dataConnectorName].push_back(converter(arrayEntry["Value"]));
//				int32_t parameterIndexZeroBased = arrayEntry["P_1"].GetInt() - 1;
//				result[_parameterConnectorName].push_back(parameterValues[parameterIndexZeroBased]);
//			}
//		}
//		_output = std::move(result);
//
//		return _output;
//	}
//	else
//	{
//		return _output;
//	}
//}
