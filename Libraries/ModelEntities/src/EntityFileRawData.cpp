// @otlicense

// OpenTwin header
#include "EntityFileRawData.h"

#include "OTCommunication/ActionTypes.h"
#include "DataBase.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/RuntimeTests.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/EncodingConverter_UTF16ToUTF8.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"
#include "OTGui/VisualisationTypes.h"

#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntityFileRawData> registrar(EntityFileRawData::className());

EntityFileRawData::EntityFileRawData(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	: EntityFile(_ID, _parent, _obs, _ms, _owner), m_format(ot::ImageFileFormat::PNG) {
}

bool EntityFileRawData::updateFromProperties() {
	return EntityFile::updateFromProperties();
}

void EntityFileRawData::setSpecializedProperties() {
	EntityFile::setSpecializedProperties();
}

void EntityFileRawData::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityFile::addStorageData(_storage);
}

void EntityFileRawData::readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityFile::readSpecificDataFromDataBase(_doc_view, _entityMap);
}
