// @otlicense

// OpenTwin header
#include "OTDataStorage/Helper/BsonValuesHelper.h"
#include "OTModelEntities/MDF/EntityMDFChannelData.h"

static EntityFactoryRegistrar<ot::EntityMDFChannelData> registrar(ot::EntityMDFChannelData::className());

ot::EntityMDFChannelData::EntityMDFChannelData(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBase(_ID, _parent, _obs, _ms)
{}


std::string ot::EntityMDFChannelData::serialiseAsJSON()
{
	// Serialize general entity data
	auto docBlock = EntityBase::serialiseAsMongoDocument();
	const std::string jsonDocBlock = bsoncxx::to_json(docBlock);
	ot::JsonDocument entireDoc;
	if (!entireDoc.fromJson(jsonDocBlock))
	{
		OT_LOG_E("Failed to serialise " + getClassName() + " to JSON. Reason: Failed to parse BSON document as JSON.");
		return "";
	}

	return entireDoc.toJson();
}

bool ot::EntityMDFChannelData::deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
{
	try
	{
		const std::string serialisationString = ot::json::toJson(_serialisation);
		std::string_view serialisedEntityJSONView(serialisationString);
		auto serialisedEntityBSON = bsoncxx::from_json(serialisedEntityJSONView);
		auto serialisedEntityBSONView = serialisedEntityBSON.view();

		readSpecificDataFromDataBase(serialisedEntityBSONView, _entityMap);
		setEntityID(createEntityUID());

		_entityMap[getEntityID()] = this;

		return true;
	}
	catch (std::exception _e)
	{
		OT_LOG_E("Failed to deserialise " + getClassName() + ". Reason: " + std::string(_e.what()));
		return false;
	}
}

void ot::EntityMDFChannelData::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBase::addStorageData(_storage);

	bsoncxx::builder::basic::array samples;

	for (const auto& sample : m_samples)
	{
		samples.append(sample);
	}

	_storage.append(bsoncxx::builder::basic::kvp("Samples", samples));
}

void ot::EntityMDFChannelData::readSpecificDataFromDataBase(const bsoncxx::document::view & _docView, std::map<ot::UID, EntityBase*>&_entityMap)
{
	EntityBase::readSpecificDataFromDataBase(_docView, _entityMap);
	try
	{
		bsoncxx::array::view samplesArray = _docView["Samples"].get_array().value;
		m_samples.clear();
		
		for (const auto& sample : samplesArray)
		{
			m_samples.push_back(DataStorageAPI::BsonValuesHelper::getDoubleFromBsonValue(sample.get_value()));
		}
	}
	catch (const std::exception& _e)
	{
		OT_LOG_E("Failed to read Samples from database for " + getClassName() + ". Reason: " + std::string(_e.what()));
	}

	resetModified();
}
