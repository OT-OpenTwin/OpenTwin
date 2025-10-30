// @otlicense

// OpenTwin header
#include "DataBase.h"
#include "EntityAPI.h"
#include "EntityFactory.h"

EntityBase* ot::EntityAPI::readEntityFromEntityIDandVersion(UID _entityID, UID _version) {
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::instance().getDocumentFromEntityIDandVersion(_entityID, _version, doc)) {
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase* entity = EntityFactory::instance().create(entityType);

	if (entity != nullptr) {
		std::map<UID, EntityBase*> entityMap;
		entity->restoreFromDataBase(nullptr, nullptr, nullptr, doc_view, entityMap);
	}

	return entity;
}

