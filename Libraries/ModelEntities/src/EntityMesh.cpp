#include "EntityMesh.h"

static EntityFactoryRegistrar<EntityMesh> registrar(EntityMesh::className());

EntityMesh::EntityMesh(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	: EntityContainer(ID, parent, obs, ms, owner) {}