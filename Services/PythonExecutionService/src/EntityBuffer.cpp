#include "EntityBuffer.h"

CPythonObjectNew EntityBuffer::GetEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName)
{
	CPythonObjectNew returnValue(nullptr);
	return returnValue;
}

void EntityBuffer::UpdateEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName, const CPythonObject* values)
{
	throw std::exception("ToDo");
}

void EntityBuffer::SaveChangedEntities()
{
	throw std::exception("ToDo");
}

void EntityBuffer::BufferEntity(EntityBase* entity)
{
	if (_bufferedEntities.find(entity->getName()) == _bufferedEntities.end())
	{
		_bufferedEntities[entity->getName()] = entity;
	}
}

void EntityBuffer::BufferProperty(std::string uniqueName, EntityPropertiesBase* property)
{
	if (_bufferedEntityProperties.find(uniqueName) == _bufferedEntityProperties.end())
	{
		_bufferedEntityProperties[uniqueName] = property;
	}
}

void EntityBuffer::ClearBuffer()
{
	_bufferedEntities.clear();
}
