// @otlicense

// OpenTwin header
#include "OTModelEntities/MDF/EntityMDFFile.h"

ot::EntityMDFFile::EntityMDFFile(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBase(_ID, _parent, _obs, _ms)
{
	EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/MDFFile.png");
	treeItem.setHiddenIcon("Tree/MDFFile.png");
	setDefaultTreeItem(treeItem);
}

bool ot::EntityMDFFile::updateFromProperties()
{
	return EntityBase::updateFromProperties();
}

void ot::EntityMDFFile::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBase::addStorageData(_storage);
}

void ot::EntityMDFFile::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBase::readSpecificDataFromDataBase(_docView, _entityMap);
}

std::string ot::EntityMDFFile::serialiseAsJSON()
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