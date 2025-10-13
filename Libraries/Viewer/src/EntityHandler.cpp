#include <stdafx.h>

#include "EntityHandler.h"
#include "DataBase.h"


EntityBase* EntityHandler::readEntityFromEntityIDandVersion(ot::UID _entityID, ot::UID _version)
{
	auto doc = bsoncxx::builder::basic::document{};

	if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(_entityID, _version, doc))
	{
		return nullptr;
	}

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	EntityBase* entity = getClassFactory().createEntity(entityType);

	if (entity != nullptr)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		entity->restoreFromDataBase(nullptr, nullptr, nullptr, doc_view, entityMap);
	}

	return entity;
}
