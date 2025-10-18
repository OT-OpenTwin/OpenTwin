// OpenTwin header
#include "EntityFileImage.h"

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

static EntityFactoryRegistrar<EntityFileImage> registrar(EntityFileImage::className());

EntityFileImage::EntityFileImage(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	: EntityFile(_ID, _parent, _obs, _ms, _owner), m_format(ot::ImageFileFormat::PNG)
{}

bool EntityFileImage::updateFromProperties() {
	return EntityFile::updateFromProperties();
}

void EntityFileImage::setImage(std::vector<char>&& _image, ot::ImageFileFormat _format) {
	auto dataEntity = getDataEntity();

	if (dataEntity != nullptr) {
		dataEntity->setData(std::move(_image));
		m_format = _format;

		if (getModelState() != nullptr) {
			dataEntity->storeToDataBase();
			setDataEntity(*dataEntity);
			getModelState()->modifyEntityVersion(*dataEntity);
			
			this->storeToDataBase();
			getModelState()->modifyEntityVersion(*this);
		}
		else {
			setModified();
		}
	}
}

const std::vector<char>& EntityFileImage::getImage() {
	auto dataEntity = getDataEntity();
	if (!dataEntity) {
		throw ot::Exception::ObjectNotFound("EntityFileImage::getImage: Data entity not found");
	}
	return dataEntity->getData();
}

void EntityFileImage::setSpecializedProperties() {
	EntityFile::setSpecializedProperties();


}

void EntityFileImage::addStorageData(bsoncxx::builder::basic::document& _storage) {
	EntityFile::addStorageData(_storage);

	_storage.append(bsoncxx::builder::basic::kvp("ImageFormat", ot::toString(m_format)));
}

void EntityFileImage::readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) {
	EntityFile::readSpecificDataFromDataBase(_doc_view, _entityMap);

	m_format = ot::stringToImageFileFormat(_doc_view["ImageFormat"].get_utf8().value.data());
}
