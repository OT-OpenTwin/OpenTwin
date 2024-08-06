#include "MetadataEntityInterface.h"
#include "MetadataEntry.h"
#include "MetadataEntryArray.h"
#include "MetadataEntryObject.h"
#include "MetadataEntrySingle.h"

#include "OTCore/VariableListToStringListConverter.h"
#include <vector>

MetadataEntityInterface::MetadataEntityInterface(const std::string& _ownerServiceName):m_ownerServiceName(_ownerServiceName)
{
}

MetadataEntityInterface::MetadataEntityInterface()
	:m_ownerServiceName("")
{
}

MetadataCampaign MetadataEntityInterface::createCampaign(std::shared_ptr<EntityMetadataCampaign> _rmd, std::list<std::shared_ptr<EntityMetadataSeries>> _msmds)
{
	MetadataCampaign measurementCampaign;
	extractCampaignMetadata(measurementCampaign, _rmd);
	extractSeriesMetadata(measurementCampaign, _msmds);
	measurementCampaign.updateMetadataOverview();
	return measurementCampaign;
}

MetadataSeries MetadataEntityInterface::createSeries(std::shared_ptr<EntityMetadataSeries> _seriesMetadataEntity)
{
	std::string entityName = _seriesMetadataEntity->getName();
	const std::string name = entityName.substr(entityName.find_last_of("/") + 1);
	MetadataSeries seriesMetadata(name);
	std::vector<std::string> allTopLevelDocumentNames = _seriesMetadataEntity->getDocumentsNames();
	
	//First we extract the series meta data
	for (const std::string& documentName : allTopLevelDocumentNames)
	{
		if (documentName != _seriesMetadataEntity->getParameterDocumentName() && documentName != _seriesMetadataEntity->getQuantityDocumentName())
		{
			const GenericDocument* document = _seriesMetadataEntity->getDocument(documentName);
			const auto listOfMetadata = extractMetadataFields(*document);
			for (const auto& metadata : listOfMetadata)
			{
				seriesMetadata.addMetadata(metadata);
			}
			auto objectList = extractMetadataObjects(*document);
			for (auto& object : objectList)
			{
				seriesMetadata.addMetadata(object);
			}
		}
	}

	//Now we extract the parameter meta data
	const GenericDocument* parameterTopLevel = _seriesMetadataEntity->getDocument(_seriesMetadataEntity->getParameterDocumentName());
	const std::vector<const GenericDocument*> allParameterDocuments = parameterTopLevel->getSubDocuments();
	for (const GenericDocument* parameterDocument : allParameterDocuments)
	{
		MetadataParameter parameter;
		parameter.parameterUID = std::stoll(parameterDocument->getDocumentName());
		auto parameterFields = extractMetadataFields(*parameterDocument);
		for (std::shared_ptr<MetadataEntry> entry : parameterFields)
		{
			const std::string currentEntryName = entry->getEntryName();
			if (currentEntryName == m_nameField)
			{
				parameter.parameterName = convertToString(entry.get());
			}
			else if(currentEntryName == m_labelField)
			{
				parameter.parameterLabel = convertToString(entry.get());
			}
			else if (currentEntryName == m_valuesField)
			{
				parameter.values = convertToVariableList(entry.get());
			}
			else if (currentEntryName == m_unitField)
			{
				parameter.unit = convertToString(entry.get());
			}
			else if (currentEntryName == m_dataTypeNameField)
			{
				parameter.typeName = convertToString(entry.get());
			}
			else
			{
				parameter.metaData[entry->getEntryName()] = entry;
			}
		}

		auto objectList = extractMetadataObjects(*parameterDocument);
		for (auto& object : objectList)
		{
			parameter.metaData[object->getEntryName()] = object;
		}
		seriesMetadata.addParameter(parameter);
	}

	//Now the quantity meta data
	const GenericDocument* quantityTopLevel = _seriesMetadataEntity->getDocument(_seriesMetadataEntity->getQuantityDocumentName());
	const std::vector<const GenericDocument*> allQuantityDocuments = quantityTopLevel->getSubDocuments();
	for (const GenericDocument* quantityDocument : allQuantityDocuments)
	{
		MetadataQuantity quantity;
		
		auto quantityFields = extractMetadataFields(*quantityDocument);
		for (std::shared_ptr<MetadataEntry> entry : quantityFields)
		{
			const std::string entryName = entry->getEntryName();
			if (entryName == m_nameField)
			{
				quantity.quantityName =	convertToString(entry.get());
			}
			else if (entryName == m_labelField)
			{
				quantity.quantityLabel = convertToString(entry.get());
			}
			else if (entryName == m_valueDescriptionsField)
			{
				auto allValueDescriptionDocuments =_seriesMetadataEntity->getDocument(entryName)->getSubDocuments();
				for (auto valueDescriptionDocument : allValueDescriptionDocuments)
				{
					MetadataQuantityValueDescription valueDescription;
					const std::string indexAsString = valueDescriptionDocument->getDocumentName();
					valueDescription.quantityIndex = static_cast<ot::UID>(std::stoll(indexAsString));
					auto fieldsByFieldNames = valueDescriptionDocument->getFields();
					
					auto valueDescriptionName =	fieldsByFieldNames->find(m_nameField);
					assert(valueDescriptionName != fieldsByFieldNames->end());
					valueDescription.quantityValueName = valueDescriptionName->second.begin()->getConstCharPtr();
					
					auto valueDescriptionLabel = fieldsByFieldNames->find(m_labelField);
					assert(valueDescriptionLabel != fieldsByFieldNames->end());
					valueDescription.quantityValueLabel = valueDescriptionLabel->second.begin()->getConstCharPtr();

					auto valueDescriptionUnit = fieldsByFieldNames->find(m_unitField);
					assert(valueDescriptionUnit != fieldsByFieldNames->end());
					valueDescription.unit= valueDescriptionUnit->second.begin()->getConstCharPtr();

					auto valueDescriptionType = fieldsByFieldNames->find(m_dataTypeNameField);
					assert(valueDescriptionType != fieldsByFieldNames->end());
					valueDescription.dataTypeName = valueDescriptionType->second.begin()->getConstCharPtr();

					quantity.valueDescriptions.push_back(valueDescription);
				}
			}
			else if (entry->getEntryName() == m_dataDimensionsField)
			{
				quantity.dataDimensions = convertToUInt32Vector(entry.get());
			}
			else if (entry->getEntryName() == m_dependingParameterField)
			{
				quantity.dependingParameterIds = convertToUInt64Vector(entry.get());
			}
			else
			{
				quantity.metaData[entry->getEntryName()] = entry;
			}
		}

		auto objectList = extractMetadataObjects(*quantityDocument);
		for (auto& object : objectList)
		{
			quantity.metaData[object->getEntryName()] = object;
		}
		seriesMetadata.addQuantity(quantity);
	}

	return seriesMetadata;
}

void MetadataEntityInterface::storeCampaign(ot::components::ModelComponent& _modelComponent, MetadataCampaign& _metaDataCampaign)
{
	EntityMetadataCampaign entityCampaign(_modelComponent.createEntityUID(), nullptr, nullptr, nullptr, nullptr, m_ownerServiceName);
	for (auto& metadata : _metaDataCampaign.getMetaData())
	{
		insertMetadata(&entityCampaign, metadata.second.get());
	}
	entityCampaign.StoreToDataBase();
	m_newEntityIDs.push_back(entityCampaign.getEntityID());
	m_newEntityVersions.push_back(entityCampaign.getEntityStorageVersion());
}

void MetadataEntityInterface::storeCampaign(ot::components::ModelComponent& _modelComponent, MetadataCampaign& _metaDataCampaign, std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel)
{
	storeCampaign(_modelComponent, _metaDataCampaign);
	storeCampaign(_modelComponent, _seriesMetadata, _saveModel);
}

void MetadataEntityInterface::storeCampaign(ot::components::ModelComponent& _modelComponent, std::list<const MetadataSeries*>& _seriesMetadata, bool _saveModel)
{
	std::list< EntityMetadataSeries> entitiesMetadataSeries;
	for (auto& newSeriesMetadata : _seriesMetadata)
	{
		const std::string name = newSeriesMetadata->getName();
		EntityMetadataSeries entitySeries(newSeriesMetadata->getSeriesIndex(), nullptr, nullptr, nullptr, nullptr, m_ownerServiceName);
		entitySeries.setName(name);
		for (const MetadataParameter& parameter : newSeriesMetadata->getParameter())
		{
			MetadataParameter& parameterForChange = const_cast<MetadataParameter&>(parameter);
			const std::string parameterUID =	std::to_string(parameterForChange.parameterUID);
			entitySeries.InsertToParameterField(m_nameField, { ot::Variable(parameter.parameterName) }, parameterUID);
			entitySeries.InsertToParameterField(m_labelField, { ot::Variable(parameter.parameterLabel) }, parameterUID);
			entitySeries.InsertToParameterField(m_valuesField, std::move(parameterForChange.values), parameterUID);
			entitySeries.InsertToParameterField(m_unitField, { ot::Variable(parameterForChange.unit) }, parameterUID);
			entitySeries.InsertToParameterField(m_dataTypeNameField, { ot::Variable(parameterForChange.typeName) }, parameterUID);

			for (auto& metadata : parameter.metaData)
			{
				insertMetadata(&entitySeries,metadata.second.get());
			}
		}
		for (const MetadataQuantity& quantity : newSeriesMetadata->getQuantities())
		{
			assert(quantity.valueDescriptions.size() >= 1);
			const std::string quantityName = quantity.quantityLabel;

			//Fields of the quantity itself
			uint64_t firstValueIndex = quantity.quantityIndex;
			const std::string quantityFieldKey = std::to_string(firstValueIndex);
			entitySeries.InsertToQuantityField(m_nameField, { ot::Variable(quantity.quantityName) }, quantityFieldKey);
			entitySeries.InsertToQuantityField(m_labelField, { ot::Variable(quantity.quantityLabel) }, quantityFieldKey);
			
			const std::vector<uint32_t>& dataDimensions = quantity.dataDimensions;
			std::list<ot::Variable> dataDimensionsAsVariable = convertFromUInt32Vector(dataDimensions);
			entitySeries.InsertToQuantityField(m_dataDimensionsField, std::move(dataDimensionsAsVariable), quantityFieldKey);
	
			std::list<ot::Variable> dependingParameter =convertFromUInt64Vector(quantity.dependingParameterIds);
			entitySeries.InsertToQuantityField(m_dependingParameterField, std::move(dependingParameter), quantityFieldKey);

			const std::string valueDocumentsFieldName = quantityFieldKey + "/" + m_valueDescriptionsField;

			for (auto& valueDesciption : quantity.valueDescriptions)
			{
				const std::string valueFieldKey = std::to_string(valueDesciption.quantityIndex);
				const std::string valueDocumentFieldName = valueDocumentsFieldName + "/" + valueFieldKey;

				entitySeries.InsertToQuantityField(m_nameField, {ot::Variable(valueDesciption.quantityValueName)}, valueDocumentFieldName);
				entitySeries.InsertToQuantityField(m_labelField, {ot::Variable(valueDesciption.quantityValueLabel)}, valueDocumentFieldName);
				entitySeries.InsertToQuantityField(m_dataTypeNameField, {ot::Variable(valueDesciption.dataTypeName)}, valueDocumentFieldName);
				entitySeries.InsertToQuantityField(m_unitField, {ot::Variable(valueDesciption.unit)}, valueDocumentFieldName);
			}

			
			for (auto& metadata : quantity.metaData)
			{
				insertMetadata(&entitySeries, metadata.second.get());
			}

		}
		for (auto& metadata : newSeriesMetadata->getMetadata())
		{
			insertMetadata(&entitySeries, metadata.second.get());
		}
		entitySeries.StoreToDataBase();
		m_newEntityIDs.push_back(entitySeries.getEntityID());
		m_newEntityVersions.push_back(entitySeries.getEntityStorageVersion());
	}

	std::list<bool> visibillity(false);
	_modelComponent.addEntitiesToModel(std::move(m_newEntityIDs), std::move(m_newEntityVersions), std::move(visibillity), {}, {}, {}, "Updated result data collection", true, _saveModel);
}

void MetadataEntityInterface::extractCampaignMetadata(MetadataCampaign& _measurementCampaign, std::shared_ptr<EntityMetadataCampaign> _rmd)
{
	const GenericDocument* topLevel= _rmd->getDocumentTopLevel();
	auto fieldList = extractMetadataFields(*topLevel);
	_measurementCampaign.setCampaignName(_rmd->getName());
	for (auto& field : fieldList)
	{
		_measurementCampaign.addMetaInformation(field->getEntryName(), field);
	}
	auto objectList = extractMetadataObjects(*topLevel);
	for (auto& object : objectList)
	{
		_measurementCampaign.addMetaInformation(object->getEntryName(), object);
	}
}

std::vector<std::string> MetadataEntityInterface::convertToStringVector(const MetadataEntry* _metaData) const
{
	std::vector<std::string> output;
	ot::VariableListToStringListConverter converter;

	auto valueEntryArr = dynamic_cast<const MetadataEntryArray*>(_metaData);
	if (valueEntryArr != nullptr)
	{
		std::list<std::string> values = converter(valueEntryArr->getValues());
		assert(values.size() == valueEntryArr->getValues().size());
		output = { values.begin(), values.end() };
	}
	else
	{
		auto valueEntry = dynamic_cast<const MetadataEntrySingle*>(_metaData);
		assert(valueEntry != nullptr);

		const ot::Variable& value = valueEntry->getValue();
		assert(value.isConstCharPtr());
		output = { value.getConstCharPtr() };
	}
	return output;
}

std::vector<uint32_t> MetadataEntityInterface::convertToUInt32Vector(const MetadataEntry* _metaData) const
{
	std::vector<uint32_t> output;

	auto valueEntryArr = dynamic_cast<const MetadataEntryArray*>(_metaData);
	if (valueEntryArr != nullptr)
	{
		output.reserve(valueEntryArr->getValues().size());
		for (auto& value : valueEntryArr->getValues())
		{
			assert(value.isInt32());
			output.push_back(static_cast<uint32_t>(value.getInt32()));
		}
	}
	else
	{
		auto valueEntry = dynamic_cast<const MetadataEntrySingle*>(_metaData);
		assert(valueEntry != nullptr);

		const ot::Variable& value = valueEntry->getValue();
		assert(value.isInt32());
		output = { static_cast<uint32_t>(value.getInt32())};
	}
	return output;
}

std::vector<uint64_t> MetadataEntityInterface::convertToUInt64Vector(const MetadataEntry* _metaData) const
{
	std::vector<uint64_t> output;

	auto valueEntryArr = dynamic_cast<const MetadataEntryArray*>(_metaData);
	if (valueEntryArr != nullptr)
	{
		output.reserve(valueEntryArr->getValues().size());
		for (auto& value : valueEntryArr->getValues())
		{
			assert(value.isInt64());
			output.push_back(static_cast<uint64_t>(value.getInt64()));
		}
	}
	else
	{
		auto valueEntry = dynamic_cast<const MetadataEntrySingle*>(_metaData);
		assert(valueEntry != nullptr);

		const ot::Variable& value = valueEntry->getValue();
		assert(value.isInt64());
		output = { static_cast<uint64_t>(value.getInt64()) };
	}
	return output;
}

std::string MetadataEntityInterface::convertToString(const MetadataEntry* _metaData) const
{
	auto entry = dynamic_cast<const MetadataEntrySingle*>(_metaData);
	assert(entry != nullptr);
	assert(entry->getValue().isConstCharPtr());
	return entry->getValue().getConstCharPtr();
}

std::list<ot::Variable> MetadataEntityInterface::convertToVariableList(const MetadataEntry* _metaData) const
{
	std::list<ot::Variable> output;
	auto valueEntryArr = dynamic_cast<const MetadataEntryArray*>(_metaData);
	if (valueEntryArr != nullptr)
	{
		output = valueEntryArr->getValues();
	}
	else
	{
		auto valueEntry = dynamic_cast<const MetadataEntrySingle*>(_metaData);
		assert(valueEntry != nullptr);
		output = { valueEntry->getValue() };
	}
	return std::list<ot::Variable>();
}

std::list<ot::Variable> MetadataEntityInterface::convertFromStringVector(const std::vector<std::string> _values) const
{
	std::list<ot::Variable> output;
	for (const std::string& value : _values)
	{
		output.push_back(ot::Variable(value));
	}

	return output;
}

std::list<ot::Variable> MetadataEntityInterface::convertFromUInt32Vector(const std::vector<uint32_t> _values) const
{
	std::list<ot::Variable> output;
	for (const uint32_t& value : _values)
	{
		output.push_back(ot::Variable(static_cast<int32_t>(value)));
	}

	return output;
}

std::list<ot::Variable> MetadataEntityInterface::convertFromUInt64Vector(const std::vector<uint64_t> _values) const
{
	std::list<ot::Variable> output;
	for (const uint64_t& value : _values)
	{
		output.push_back(ot::Variable(static_cast<int64_t>(value)));
	}

	return output;
}

std::list<std::shared_ptr<MetadataEntry>> MetadataEntityInterface::extractMetadataObjects(const GenericDocument& _document)
{
	auto& subDocuments = _document.getSubDocuments();
	std::list<std::shared_ptr<MetadataEntry>> allMetadata;
	for (const GenericDocument* subDoc : subDocuments)
	{
		std::shared_ptr<MetadataEntryObject> object(new MetadataEntryObject(subDoc->getDocumentName()));
		auto fieldList = extractMetadataFields(*subDoc);
		object->AddMetadataEntry(fieldList);
		auto subObjectList = extractMetadataObjects(*subDoc);
		object->AddMetadataEntry(subObjectList);
		allMetadata.push_back(object);
	}
	return allMetadata;
}

std::list<std::shared_ptr<MetadataEntry>> MetadataEntityInterface::extractMetadataFields(const GenericDocument& _document)
{
	auto fields = _document.getFields();
	std::list<std::shared_ptr<MetadataEntry>> metadata;
	for (auto field = fields->begin(); field != fields->end(); field++)
	{
		const std::string name = field->first;
		const std::list<ot::Variable>& values = field->second;
		if (values.size() == 1)
		{
			metadata.push_back(std::shared_ptr<MetadataEntry>(new MetadataEntrySingle(name, *values.begin())));
		} 
		else
		{
			metadata.push_back(std::shared_ptr<MetadataEntry>(new MetadataEntryArray(name, values)));
		}
	}
	return metadata;
}

void MetadataEntityInterface::extractSeriesMetadata(MetadataCampaign& measurementCampaign, std::list<std::shared_ptr<EntityMetadataSeries>> _msmds)
{
	for (auto msmd : _msmds)
	{
		MetadataSeries seriesMetadata = createSeries(msmd);
		measurementCampaign.addSeriesMetadata(std::move(seriesMetadata));
	}
}

void MetadataEntityInterface::insertMetadata(EntityWithDynamicFields* _entity, MetadataEntry* _metadata, const std::string _documentName)
{
	auto singleEntry = dynamic_cast<MetadataEntrySingle*>(_metadata);
	if(singleEntry != nullptr)
	{
		_entity->InsertInField(singleEntry->getEntryName(), { singleEntry->getValue() }, _documentName);
	}
	else
	{
		auto arrayEntry = dynamic_cast<MetadataEntryArray*>(_metadata);
		if (arrayEntry != nullptr)
		{
			_entity->InsertInField(arrayEntry->getEntryName(), arrayEntry->getValues(), _documentName);
		}
		else
		{
			auto objectEntry = dynamic_cast<MetadataEntryObject*>(_metadata);
			assert(objectEntry != nullptr);
			const std::string& name = _documentName + "/" + objectEntry->getEntryName();
			for (auto entry : objectEntry->getEntries())
			{
				insertMetadata(_entity, entry.get(), name);
			}
		}

	}


	
}
