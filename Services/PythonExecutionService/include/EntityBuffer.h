#pragma once
#include "CPythonObjectNew.h"
#include "EntityBase.h"
#include <string>
#include <memory>
#include "OpenTwinFoundation/ModelComponent.h"

class EntityBuffer
{
public:
	friend class FixtureEntityBuffer;
	static EntityBuffer& INSTANCE()
	{
		static EntityBuffer instance;
		return instance;
	} 
	void setModelComponent(ot::components::ModelComponent* modelComponent) { _modelComponent = modelComponent; };

	PyObject* GetEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName);
	void UpdateEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName, const CPythonObject& values);
	void SaveChangedEntities();
	bool SaveChangedEntities(std::string absoluteEntityName);
	std::shared_ptr<EntityBase> GetEntity (const std::string& absoluteEntityName);

private:
	EntityBuffer() {};

	std::map<std::string, std::shared_ptr<EntityBase>> _bufferedEntities;
	std::map<std::string, EntityPropertiesBase*> _bufferedEntityProperties;
	ot::components::ModelComponent* _modelComponent = nullptr;
	void EnsurePropertyToBeLoaded(const std::string& absoluteEntityName, const std::string& propertyName);
	std::shared_ptr<EntityBase> LoadEntity(const std::string& absoluteEntityName);

	void ClearBuffer();

};
