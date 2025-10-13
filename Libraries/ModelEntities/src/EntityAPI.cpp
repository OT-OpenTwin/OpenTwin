//! @file EntityAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "DataBase.h"
#include "EntityAPI.h"

EntityBase* ot::EntityAPI::readEntityFromEntityIDandVersion(UID _entityID, UID _version, ClassFactoryHandler& classFactory) {
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(_entityID, _version, doc)) {
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase* entity = classFactory.createEntity(entityType);

	if (entity != nullptr) {
		std::map<UID, EntityBase*> entityMap;
		entity->restoreFromDataBase(nullptr, nullptr, nullptr, doc_view, entityMap);
	}

	return entity;
}

