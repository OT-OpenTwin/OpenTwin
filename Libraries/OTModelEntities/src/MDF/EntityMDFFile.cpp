// @otlicense

// OpenTwin header
#include "OTModelEntities/MDF/EntityMDFFile.h"

static EntityFactoryRegistrar<ot::EntityMDFFile> registrar(ot::EntityMDFFile::className());

ot::EntityMDFFile::EntityMDFFile(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityContainer(_ID, _parent, _obs, _ms)
{
	EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/MDFFile");
	treeItem.setHiddenIcon("Tree/MDFFile");
	setDefaultTreeItem(treeItem);
}

bool ot::EntityMDFFile::updateFromProperties()
{
	return EntityContainer::updateFromProperties();
}

void ot::EntityMDFFile::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityContainer::addStorageData(_storage);
}

void ot::EntityMDFFile::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityContainer::readSpecificDataFromDataBase(_docView, _entityMap);
}

std::string ot::EntityMDFFile::serialiseAsJSON()
{
	// Serialize general entity data
	auto docBlock = EntityContainer::serialiseAsMongoDocument();
	const std::string jsonDocBlock = bsoncxx::to_json(docBlock);
	ot::JsonDocument entireDoc;
	if (!entireDoc.fromJson(jsonDocBlock))
	{
		OT_LOG_E("Failed to serialise " + getClassName() + " to JSON. Reason: Failed to parse BSON document as JSON.");
		return "";
	}

	return entireDoc.toJson();
}

bool ot::EntityMDFFile::deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
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