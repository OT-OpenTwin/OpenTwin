#pragma once
#include "CPythonObjectNew.h"
#include "EntityBase.h"
#include <string>

class EntityBuffer
{
public:
	EntityBuffer* INSTANCE()
	{
		static EntityBuffer instance;
		return &instance;
	}
	
	
	CPythonObjectNew GetEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName);
	void UpdateEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName, const CPythonObject* values);
	void SaveChangedEntities();
	void BufferEntity(EntityBase* entity); //Eventuell auch property buffern? PropertyBase kriegt ein Feld für einen typenbezeichner?
	void BufferProperty(std::string uniqueName, EntityPropertiesBase* property);
	void ClearBuffer();

private:
	EntityBuffer();
	std::map<std::string, EntityBase*> _bufferedEntities;
	std::map<std::string, EntityPropertiesBase*> _bufferedEntityProperties;
};
