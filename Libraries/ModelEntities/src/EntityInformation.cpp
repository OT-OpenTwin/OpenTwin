// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "EntityBase.h"
#include "EntityInformation.h"

ot::EntityInformation::EntityInformation() {}

ot::EntityInformation::EntityInformation(EntityBase* _entity) {
	OTAssertNullptr(_entity);
	this->setEntityID(_entity->getEntityID());
	this->setEntityVersion(_entity->getEntityStorageVersion());
	this->setEntityName(_entity->getName());
	m_type = _entity->getClassName();
}

void ot::EntityInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	BasicEntityInformation::addToJsonObject(_object, _allocator);
	_object.AddMember("EntityType", JsonString(m_type, _allocator), _allocator);
}

void ot::EntityInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	BasicEntityInformation::setFromJsonObject(_object);
	m_type = json::getString(_object, "EntityType");
}

ot::EntityInformation::~EntityInformation() {}