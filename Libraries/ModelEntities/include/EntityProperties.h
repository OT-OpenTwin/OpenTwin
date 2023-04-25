#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <map>

#include "EntityPropertiesItems.h"

class EntityBase;

class __declspec(dllexport) EntityProperties
{
public:
	EntityProperties() : needsUpdate(false) {};
	EntityProperties(const EntityProperties &other);
	virtual ~EntityProperties();

	bool createProperty(EntityPropertiesBase *property, const std::string &group);
	bool deleteProperty(const std::string &name);
	bool propertyExists(const std::string &name);

	EntityPropertiesBase *getProperty(const std::string &name);

	void setNeedsUpdate(void) { needsUpdate = true; };
	bool anyPropertyNeedsUpdate(void) {return needsUpdate; };
	void checkWhetherUpdateNecessary(void);
	void forceResetUpdateForAllProperties();

	std::string getJSON(EntityBase *root, bool visibleOnly);
	void buildFromJSON(const std::string &prop);
	void checkMatchingProperties(EntityProperties &other);
	void readFromProperties(const EntityProperties &other, EntityBase *root);

	EntityProperties& operator=(const EntityProperties &other);

	void setAllPropertiesReadOnly(void);
	void setAllPropertiesNonProtected(void);

	std::list<EntityPropertiesBase*> getListOfAllProperties(void);
	std::list<EntityPropertiesBase *> getListOfPropertiesWhichNeedUpdate(void);
	std::list<EntityPropertiesDouble *> getListOfNumericalProperties(void);
	size_t getNumberOfProperties(void) { return properties.size(); }

	std::list<std::string> getListOfPropertiesForGroup(const std::string &group);

private:
	void deleteAllProperties(void);

	bool needsUpdate;
	std::map<std::string, EntityPropertiesBase *> properties;
	std::list<EntityPropertiesBase *> propertiesList; // The storage in the list is redundant, but allows to keep the order of the properties
};
