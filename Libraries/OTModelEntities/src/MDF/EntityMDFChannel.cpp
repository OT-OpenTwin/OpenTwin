// @otlicense

// OpenTwin header
#include "OTDataStorage/Helper/BsonValuesHelper.h"
#include "OTModelEntities/MDF/EntityMDFChannel.h"
#include "OTModelEntities/MDF/EntityMDFChannelData.h"

static EntityFactoryRegistrar<ot::EntityMDFChannel> registrar(ot::EntityMDFChannel::className());

ot::EntityMDFChannel::EntityMDFChannel(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBase(_ID, _parent, _obs, _ms)
{
	EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/MDFChannel.png");
	treeItem.setHiddenIcon("Tree/MDFChannel.png");
	setDefaultTreeItem(treeItem);
}	

// ###########################################################################################################################################################################################################################################################################################################################

void ot::EntityMDFChannel::setSamples(std::vector<double>&& _samples)
{
	if (!ensureDataEntityLoaded())
	{
		OT_LOG_E("Failed to set samples for \"" + getName() + "\". Reason: Failed to load data entity.");
		throw Exception::ObjectNotFound("Failed to set samples for \"" + getName() + "\". Reason: Failed to load data entity.");
	}
	m_data->setSamples(std::move(_samples));
}

const std::vector<double>& ot::EntityMDFChannel::getSamples()
{
	if (!ensureDataEntityLoaded())
	{
		OT_LOG_E("Failed to get samples for \"" + getName() + "\". Reason: Failed to load data entity.");
		throw Exception::ObjectNotFound("Failed to get samples for \"" + getName() + "\". Reason: Failed to load data entity.");
	}
	return m_data->getSamples();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Property access

bool ot::EntityMDFChannel::updateFromProperties()
{
	bool updateGrid = EntityBase::updateFromProperties();

	return updateGrid;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Serialization

std::string ot::EntityMDFChannel::serialiseAsJSON()
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

bool ot::EntityMDFChannel::deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
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

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Serialization

void ot::EntityMDFChannel::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBase::addStorageData(_storage);
	_storage.append(bsoncxx::builder::basic::kvp("DataID", static_cast<int64_t>(m_dataID)));
}

void ot::EntityMDFChannel::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBase::readSpecificDataFromDataBase(_docView, _entityMap);

	resetDataEntity();

	m_dataID = DataStorageAPI::BsonValuesHelper::getUInt64FromBsonValue(_docView["DataID"].get_value());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::EntityMDFChannel::resetDataEntity()
{
	if (m_data)
	{
		delete m_data;
		m_data = nullptr;
	}
}

bool ot::EntityMDFChannel::ensureDataEntityLoaded()
{
	if (m_data)
	{
		return true;
	}

	if (m_dataID == ot::invalidUID)
	{
		OT_LOG_E("Failed to load data entity for \"" + getName() + "\". Reason: Invalid data entity ID.");
		return false;
	}

	EntityBase* dataEntityBase = readEntityFromEntityID(this, m_dataID);
	if (!dataEntityBase)
	{
		OT_LOG_E("Failed to load data entity for \"" + getName() + "\". Reason: Failed to read entity from entity ID.");
		return false;
	}
	EntityMDFChannelData* dataEntity = dynamic_cast<EntityMDFChannelData*>(dataEntityBase);

	if (dataEntity == nullptr)
	{
		OT_LOG_ES("Failed to load data entity for " << getClassName() << ". Reason: Unexpected data entity type: \"" << dataEntityBase->getClassName() << "\"");
		delete dataEntityBase;
		return false;
	}

	m_data = dataEntity;

	return true;
}